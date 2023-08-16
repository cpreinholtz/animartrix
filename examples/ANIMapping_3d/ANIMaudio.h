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
#pragma once
#include "ANIMutils.h"
#include "FFT.h"

const int samples = 256;
const float sampleFrequency = 20000.0;
const int sampling_period_us = int(1000000*(1.0/sampleFrequency)); 


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


  bool verbose = false;// beat detection
  bool verbose2 = false; // FFT
  float global_bpm = 115.0;
  float energy_level = 0.0;

  float raw_signal;
  unsigned long raw_time_ms;
  //const float raw_min = 0;
  //const float raw_max = 1023;
  //const float raw_ac_couple = raw_max/2; // this needs to be the "0" level of your aplifier!!
  float rawOffset = 2048;

  const float scaled_min = -1.0;
  const float scaled_max = 1.0;
  float scaled_signal;

  float abs_signal;
  float signal;
  IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume
  IIR iir_lowpass; //lowpass iir tuned to a  slow update time for measuring volume bass kicks
  IIR iir_bias; //lowpass iir tuned to a  slow update time for dc offset correction
  FIR lp;

  float sig;
  float comp;


  bool beat_detected;
  bool beat_detected_dbg;
  float beat_last;
  bool beat_armed = true;
  const float beat_delta_min = 60; //essentially a debounce in miliseconds, 100 milliseconds limits to 1/16 notes @ 120 bpm
  float beat_multiplier_min = 2.0; //must hit moving average * multiplier to be considered a beat
  float beat_volume_min = 0.005; //must hit moving average * multiplier to be considered a beat
  const float beat_hysteresis = beat_multiplier_min * 0.5;
  float hyst_count; 
  float hyst_arm = 0.6;

  static const int bpm_num_samples = 21; // use odd to prevent DC offset
  static const int bpm_median_sample = bpm_num_samples/2;
  int bpm_current_sample = 0; //round robbin index, increments from 0 to bpm_num_samples-1 and wraps around
  float bpm_deltas[bpm_num_samples]; //keep values from the latest beat intevals
  float bpm_sorted_deltas[bpm_num_samples]; //keep values from the latest beat intevals (this list is sorted in ascending order)
  float bpm_median_delta; //average (median) time between beats in miliseconds.  this is the period of the pulse in millis
  float bpm; //the estimated bpm, collected by using the median.  this is the frequency of the pulse in millis

  int audioPin = 19;

  bool locked = false;

  ANIMaudio() {
    this->init();
  }

  void init() {
    //raw_signal = raw_ac_couple;
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
    for (int i = 0; i < bpm_num_samples; i++) { bpm_deltas[i] = 500; }
    bpm = 120;

    this->update();
    
  }
  void debug(){
        
    EVERY_N_MILLIS(50) if (verbose){

      Serial.print("sig:");
      Serial.print(sig);
      Serial.print(",");

      Serial.print("comp:");
      Serial.print(comp);
      Serial.print(",");

      Serial.print("hyst_count:");
      Serial.print(hyst_count);
      Serial.print(",");

      Serial.print("iirv:");
      Serial.print(iir_volume.signal);
      Serial.print(",");

      Serial.print("ratio:");
      Serial.print(max(float(0.0),sig/comp/beat_multiplier_min));
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

  int lockedCount = 0;
  unsigned long beat_avg = 100; // millis


  void forceBeat(){
    beat_detected = true;
    beat_detected_dbg = true;
    beat_armed = false;
    hyst_count = 0;
  }
  void clearBeat(){
    //add hysterisis to beat thingy
    if (abs_signal < iir_volume.signal * beat_hysteresis ) {
      if  (hyst_count < 2.0 ) hyst_count = hyst_count + .1;
      if (hyst_count > hyst_arm) beat_armed = true; /// set armed HERE if hysterisis conditions satisfied  // 6 llops at ratio < hyst
    }
    beat_detected = false;
  }

  void assessLock(){
    if (lockedCount > 10) locked = true;
    else {
      locked=false;
    }
  }

  void peakFound(){
    if (lockedCount < 20) lockedCount++;
    beat_avg = ((raw_time_ms - beat_last) + beat_avg) / 2;
    beat_last = raw_time_ms;
    assessLock();
    forceBeat();
  }

  void peakMissed(){
    if (lockedCount >0 ) lockedCount--;
    assessLock();
    if (locked && raw_time_ms > beat_avg + 20){
      forceBeat();
      Serial.println("false beat");
    } else {
      clearBeat();
    }
  }



  bool peakReady(){
    raw_time_ms = millis();
    //beat detected if its been a while since the last beat and the current energy is much higher than the average
    float delta = raw_time_ms - beat_last;
    if( (beat_armed == true) && (delta > beat_delta_min) && (iir_volume.signal > beat_volume_min)){
      return true;
    }
    else { 
      return false;
    }
  }

  void peakDetect(float a, float b){
    sig = a;
    comp = b;
    if(peakReady() && a > b * beat_multiplier_min){
      peakFound();
    } else {
      peakMissed();
    }
    this->debug();// all roads lead to rome
  }

  void peakDetect(){
    peakDetect(abs_signal,iir_volume.signal);
  }




//!update by passing in nothing, assumes scaled signal is ready
  void updateScaled(){
    //take the abs value of that to get the current energy
    abs_signal = (scaled_signal >= 0.0) ? scaled_signal : - scaled_signal;
    // update filters
    iir_volume.update(abs_signal);// todo unused
    peakDetect();
  }

  //!update by passing in a scaled signal
  void updateScaled(float audiolevel){
    //scale from -1 to 1 and remove DC bias
    scaled_signal = audiolevel;
    updateScaled();    
  }



  //!update by passing in a raw signal ( 0 to 1024, like an analog read)
  /*
  void update(float audiolevel){
    //record the current time
    raw_signal = audiolevel;
    scaled_signal = map_float( raw_signal, raw_min, raw_max, scaled_min, scaled_max);
    updateScaled();
  }*/

  float fft_input[samples];
  float fft_output[samples];
  char print_buf[300];
  fft_config_t *real_fft_plan = fft_init(samples, FFT_REAL, FFT_FORWARD, fft_input, fft_output);


  float max_magnitude = 0;
  float fundamental_freq = 0;



  float scaler = 2.0/samples/1000.0;

  //!update by reading raw signal from pin // TODO OBE??? might be used for analog mics
  void update(){
    EVERY_N_MILLIS(5){
      long int t1 = micros();
      long int total=0;
      for(int i=0; i<samples; i++) {
          unsigned long microseconds = micros();    //Overflows after around 70 minutes!
          raw_signal = analogRead(A0);
          real_fft_plan->input[i] = float(raw_signal) - rawOffset;

          while(micros() - microseconds < sampling_period_us){}
          total += micros() - microseconds;
      }

      fft_execute(real_fft_plan);
      int j=1;

      float m = absf(real_fft_plan->output[2*j]) + absf(real_fft_plan->output[2*j+1] );
      float f = j*sampleFrequency/samples;
      float a = m*scaler;

      float ma=0.0;
      for ( j=1; j<5;j++){
          ma += absf(real_fft_plan->output[2*j]) + absf(real_fft_plan->output[2*j+1]);
      }
      ma = ma *scaler/ 4.0;
      iir_volume.update(ma);
      peakDetect(a,ma);

      //updateScaled(a);

      if (verbose2) {
        EVERY_N_MILLIS(100){
          long int t2 = micros();
          Serial.print("TimetakenMiliS:");Serial.print((t2-t1)*1.0/1000);Serial.print(",");
          Serial.print("total:");Serial.print(total);Serial.print(",");

          for ( j=0; j<5;j++){
              m = absf(real_fft_plan->output[2*j]) + absf(real_fft_plan->output[2*j+1]);
              f = j*sampleFrequency/samples;
              a = m*2.0/samples/10000.0;
              Serial.print("f");Serial.print(int(f));Serial.print(":");Serial.print(a);Serial.print(",");
          }
          Serial.println("");
        }
      }
    }
  }

  //! returns lowpass.signal, should be between 0 and 1.0
  //float getLp(){
  //  return iir_lowpass.signal;
  //}




}; 
