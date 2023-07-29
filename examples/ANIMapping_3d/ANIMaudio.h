/*
  ___        _            
 / _ \      (_)
/ /_\ \_ __  _ _ __ ___
|  _  | '_ \| | '_ ` _ \
| | | | | | | | | | | | |
\_| |_/_| |_|_|_| |_| |_

by Clark Reinholtz, 
Audio input DSP

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#include "ANIMutils.h"


// Function for Selection sort, not the fastest but it is nice and simple https://www.geeksforgeeks.org/selection-sort/?ref=lbp
//since the array is passed by reference, no output is needed and this function works directly on the "sorted_arr"
// "arr" is left untouched
void selectionSort(const int arr[], int sorted_arr[], const int n)
{
    int i, j, min_idx;
    //copy original
    for (i = 0; i < n - 1; i++) {
      sorted_arr[i] = arr[i];
    }
 
    // One by one move boundary of
    // unsorted subarray
    for (i = 0; i < n - 1; i++) {
         // Find the minimum element in
        // unsorted array
        min_idx = i;
        for (j = i + 1; j < n; j++) {
            if (sorted_arr[j] < sorted_arr[min_idx]) min_idx = j;
        }
         // Swap the found minimum element
        // with the first element
        if (min_idx != i) {std::swap(sorted_arr[min_idx], sorted_arr[i]); // might need "using namespace std;" for this?

        }
    }
}
 


class FIR {

public:

  float signal; //lowpass fir 
  static const int nSamples = 1; // THIS is how you tune the filter!!!!!! // moving average with this many inputs  20 seems to work ok at 192 FPS
  int thisSample;
  float sum;
  float samples[nSamples];
  
  FIR() {
    thisSample = 0;
    sum=0;
    for (int i = 0; i< nSamples; i++){
      samples[i] = 0;
    }
  }

  void update(float v){
    //subtract out old one
    sum = sum - samples[thisSample];

    //add in new one
    samples[thisSample] = v;
    sum = sum+v;

    //inc index, simple round robbin
    thisSample = thisSample+1;
    if (thisSample >= nSamples) thisSample = 0;

    signal = sum / nSamples;
  }


}; 




class IIR {

public:

  float signal=0; //lowpass iir tuned to 
  float iir_weight = .99; // must be <1, weight of the previous samples, this (along with the sample rate) determines the cutoff frequency, more weight = lower cutoff


  void update(float v){
    signal = (signal * iir_weight) + (v * (1.0-iir_weight));
  }

  void setWeight(float w) {
    iir_weight = w;
    if (iir_weight > 1.0) iir_weight = 1.0;
  }

  void setAverage(float a) {
    signal = a;
  }


}; 





class ANIMaudio {

public:


  bool verbose = true;
  float global_bpm = 115.0;
  float energy_level = 0.0;

  float raw_signal;
  unsigned long raw_time_ms;
  const float raw_min = 0;
  const float raw_max = 1023;
  const float raw_ac_couple = raw_max/2; // this needs to be the "0" level of your aplifier!!

  const float scaled_min = -1.0;
  const float scaled_max = 1.0;
  float scaled_signal;

  float abs_signal;
  float signal;
  IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume
  IIR iir_lowpass; //lowpass iir tuned to a  slow update time for measuring volume bass kicks
  IIR iir_bias; //lowpass iir tuned to a  slow update time for dc offset correction
  FIR lp;


  bool beat_detected;
  bool beat_detected_dbg;
  float beat_last;
  bool beat_armed = true;
  const float beat_delta_min = 150; //essentially a debounce in miliseconds, 100 milliseconds limits to 1/16 notes @ 120 bpm
  const float beat_multiplier_min = 2.0; //must hit moving average * multiplier to be considered a beat
  const float beat_volume_min = 0.5/20.0; //must hit moving average * multiplier to be considered a beat
  const float beat_hysteresis = beat_multiplier_min * 0.5;
  float hyst_count; 
  int beat_debounce_count;

  static const int bpm_num_samples = 21; // use odd to prevent DC offset
  static const int bpm_median_sample = bpm_num_samples/2;
  int bpm_current_sample = 0; //round robbin index, increments from 0 to bpm_num_samples-1 and wraps around
  float bpm_deltas[bpm_num_samples]; //keep values from the latest beat intevals
  float bpm_sorted_deltas[bpm_num_samples]; //keep values from the latest beat intevals (this list is sorted in ascending order)
  float bpm_median_delta; //average (median) time between beats in miliseconds.  this is the period of the pulse in millis
  float bpm; //the estimated bpm, collected by using the median.  this is the frequency of the pulse in millis

  int audioPin = 19;

  ANIMaudio() {
    this->init();
  }

  void init() {
    raw_signal = raw_ac_couple;
    raw_time_ms = 0;
    scaled_signal = 0;
    abs_signal = 0;

    iir_volume.setWeight(.995);
    iir_volume.setAverage(0.0);
    //.95 works for beat detect with 1.5 mult
    iir_lowpass.setWeight(.99999999999999); //.95 works for beat detect with 1.5 mult
    iir_lowpass.setAverage(0.0);
    iir_bias.setWeight(.999);
    iir_bias.setAverage(511);

    //findings 7/26
    /*
    volume .995 works well at 192 FPS
      takes less than second but still dips between beats
    */

    beat_detected = false;
    beat_detected_dbg = false;
    beat_last = 0;
    beat_debounce_count = 0;
    for (int i = 0; i < bpm_num_samples; i++) { bpm_deltas[i] = 500; }
    bpm = 120;

    this->update();
    
  }
  void debug(){
        
    EVERY_N_MILLIS(50) if (verbose){
      //Serial.print(" audio.raw_signal: ");
      //Serial.print(audio.raw_signal);
      //Serial.print(",");

      //Serial.print(" audio.scaled_signal: ");
      //Serial.print(audio.scaled_signal);
      //Serial.print(",");
      
      Serial.print("ref:");
      Serial.print(beat_multiplier_min);
      Serial.print(",");
  
      Serial.print("abs_signal:");
      Serial.print(abs_signal);
      Serial.print(",");

      /*
      Serial.print("iir_vol_weight:");
      Serial.print(iir_volume.iir_weight);
      Serial.print(",");

      Serial.print("iir_lowpass_weight:");
      Serial.print(iir_lowpass.iir_weight);
      Serial.print(",");
      */
      

      Serial.print("iir_volume:");
      Serial.print(iir_volume.signal);
      Serial.print(",");
/*
      Serial.print("iir_lowpass:");
      Serial.print(iir_lowpass.signal);
      Serial.print(",");
*//*
      Serial.print("Fir_lowpass:");
      Serial.print(lp.signal);
      Serial.print(",");*/

      Serial.print("hyst_count:");
      Serial.print(hyst_count);
      Serial.print(",");

      Serial.print("ratio:");
      Serial.print(max(0,abs_signal/iir_volume.signal/beat_multiplier_min));
      Serial.print(",");

      Serial.print("ratio:");
      Serial.print(max(0,abs_signal/iir_volume.signal/beat_hysteresis));
      Serial.print(",");

      Serial.print("beat:"); 
      int b=0;
      if (beat_detected_dbg){
        beat_detected_dbg = false;
        b=1;
      }
      Serial.print(b);
      Serial.print(",");

      Serial.print("armed:"); 
      b=0;
      if (beat_armed){
        b=1;
      }
      Serial.print(b);
      
      Serial.println();


    }
  }


  void peakDetect(){
    raw_time_ms = millis();
    //beat detected if its been a while since the last beat and the current energy is much higher than the average
    float delta = raw_time_ms - beat_last;

    if( (beat_armed == true) && (delta > beat_delta_min) && (iir_volume.signal > beat_volume_min) && (abs_signal > iir_volume.signal * beat_multiplier_min)){
      beat_last = raw_time_ms;
      beat_detected = true;
      beat_detected_dbg = true;
      beat_armed = false;
      hyst_count = 0;
    } else {
      //add hysterisis to beat thingy
      if (abs_signal < iir_volume.signal * beat_hysteresis && hyst_count <= 2.0) {
        hyst_count = hyst_count + .1;
        if (hyst_count > 1.0) beat_armed = true;
      }
      beat_detected = false;
    }
  }


//!update by passing in nothing, assumes scaled signal is ready
  void updateScaled(){
    //record the current time
    

    //take the abs value of that to get the current energy
    abs_signal = (scaled_signal >= 0.0) ? scaled_signal : - scaled_signal;
    
    // update filters
    iir_volume.update(abs_signal);// todo unused
    //iir_lowpass.update(abs_signal);
    //lp.update(abs_signal);// todo unused???
    //iir_bias.update(raw_signal); // todo unused

    peakDetect();

    this->debug();
  }

  //!update by passing in a scaled signal
  void updateScaled(float audiolevel){
    //scale from -1 to 1 and remove DC bias
    scaled_signal = audiolevel;
    updateScaled();    
  }



  //!update by passing in a raw signal ( 0 to 1024, like an analog read)
  void update(float audiolevel){
    //record the current time
    raw_signal = audiolevel;
    scaled_signal = map_float( raw_signal, raw_min, raw_max, scaled_min, scaled_max);
    updateScaled();
  }

  //!update by reading raw signal from pin // TODO OBE??? might be used for analog mics
  void update(){
    //read the analog pin
    float avg = 0;
    int div = 5;
    for (int i=0; i< div; i++){
      avg = avg + analogRead(audioPin);
    }
    update(avg/div);
  }

  //! returns lowpass.signal, should be between 0 and 1.0
  //float getLp(){
  //  return iir_lowpass.signal;
  //}




}; 
