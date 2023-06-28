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

//#include <SmartMatrix.h>







//helper functions todo move to a new place?
float map_float(float x, float in_min, float in_max, float out_min, float out_max) { 
  
  float result = (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
  if (result < out_min) result = out_min;
  if( result > out_max) result = out_max;

  return result; 
}





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
 






class IIR {

public:

  float signal; //lowpass iir tuned to 
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



  float global_bpm = 115.0;
  float energy_level = 0.0;

  int raw_signal;
  unsigned long raw_time_ms;
  const float raw_min = 0;
  const float raw_max = 1023;
  const float raw_ac_couple = raw_max/2; // this needs to be the "0" level of your aplifier!!

  const float scaled_min = -1.0;
  const float scaled_max = 1.0;
  float scaled_signal;

  float abs_signal;

  IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume
  IIR iir_lowpass; //lowpass iir tuned to a  slow update time for measuring volume bass kicks
  IIR iir_bias; //lowpass iir tuned to a  slow update time for dc offset correction



  bool beat_detected;
  float beat_last;
  const float beat_delta_min = 100; //essentially a debounce in miliseconds, 100 milliseconds limits to 1/16 notes @ 120 bpm
  const float beat_multiplier_min = 1.3; //must hit moving average * multiplier to be considered a beat
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

    //To the extent that time constants mean anything in discrete time constant would be
    //-(sample time) * ln(1-weight)

    //assume sampling 100 times per second, sample time =.1
    //wt = .999; 5tau = 5 * -.1* ln(.001); 5tau = 3.4538 seconds
    //wt = .99; 5tau = 5 * -.1* ln(.01); 5tau = 2.3 seconds
    //wt = .9; 5tau = 5 * -.1* ln(.1); 5tau = 1.1 seconds
    //wt = .75; 5tau = 5 * -.1* ln(.25); 5tau = 0.69 seconds
    //wt = .5; 5tau = 5 * -.1* ln(.5); 5tau = 0.3 seconds
    iir_volume.setWeight(.99);
    iir_volume.setAverage(0.0);
    //.95 works for beat detect with 1.5 mult
    iir_lowpass.setWeight(.96); //.95 works for beat detect with 1.5 mult
    iir_lowpass.setAverage(0.0);
    iir_bias.setWeight(.999);
    iir_bias.setAverage(511);

    beat_detected = false;
    beat_last = 0;
    beat_debounce_count = 0;

    for (int i = 0; i < bpm_num_samples; i++) { bpm_deltas[i] = 500; }

    bpm = 120;

    this->update();
    
  }


  void update(){
    //read the analog pin and record the current time
    raw_signal = analogRead(audioPin);
    raw_time_ms = millis();

    
    //scale from -1 to 1 and remove DC bias
    scaled_signal = map_float(((float) raw_signal), raw_min, raw_max, scaled_min, scaled_max);
    
    //take the abs value of that to get the current energy
    abs_signal = (scaled_signal >= 0.0) ? scaled_signal : - scaled_signal;
    
    // update filters
    iir_volume.update(abs_signal);
    iir_lowpass.update(abs_signal);
    iir_bias.update(raw_signal);

    //beat detected if its been a while since the last beat and the current energy is much higher than the average
    float delta = raw_time_ms - beat_last;
    if(delta > beat_delta_min && iir_lowpass.signal > iir_volume.signal * beat_multiplier_min){
      beat_last = raw_time_ms;
      beat_detected = true;

      /*
      //put this delta into the buffer
      bpm_deltas[bpm_current_sample] = delta;

      //bpm_deltas is a round robin buffer, increment bpm_current_sample index and wrap around if needed
      bpm_current_sample ++;
      if (bpm_current_sample >= bpm_num_samples) bpm_current_sample = 0;

      //sort then for median
      //selectionSort(bpm_deltas, bpm_sorted_deltas, bpm_num_samples);
      //todo really this isnt so good, because many 8th notes will skew the quarter notes,  
      //really you should somehow detect if samples are multiples of one another within some margin, 
      //and throw out the smaller ones, that should leave you with only quarters?
      //honestly the whole bpm thing is totally uneeded anyways
      //bpm_median_delta = bpm_sorted_deltas[bpm_median_sample];
      bpm = 60000/bpm_median_delta;// to convert millis per beat to bpm, invert to get beats per milli, then multiply by 60000 millis per 1 second
      */

    } else {
      beat_detected = false;
    }




  }

  //! returns lowpass.signal, should be between 0 and 1.0
  float getLp(){
    return iir_lowpass.signal;
  }




}; 
