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

#if ART_VEST
#include "FFT.h"
//used for Vest FFT only
const int samples = 256;
const float sampleFrequency = 15000.0;
const int sampling_period_us = int(1000000*(1.0/sampleFrequency));
const int fft_save_bins = 5;
#endif




class FIR {

public:

  float signal;
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

  IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume

  float sig; // input signal, usually FFT BIN 0
  float comp; // 
  float ratio;
  float ratio_poll;

  //use peaks to determine beats
  bool beat_detected = false;
  bool beat_detected_poll = false;
  bool beat_detected_dbg = false;  
  float beat_multiplier_min = 2.0; //must hit moving average * multiplier to be considered a beat

  //detect peaks
  unsigned long peak_last = 0;
  bool peak_armed = true;// implies all other conditions met (hysterisis, debounce)
  const unsigned int peak_delta_min = 200; //essentially a debounce in miliseconds, 100 milliseconds limits to 1/16 notes @ 120 bpm
  float peak_volume_min = 0.005; //iir_volume must be > this to be a peak

  //ok so i wrote hyst kinda weird, but when a peak is detected hyst_count is set to 0, 
  //it needs to rise above hyst_arm in order to arm the next peak,  
  //hyst count increases by 1 only when ratio is below peak_hyst_low
  float peak_hyst_low = 0.8; // count increases when ratio than this
  int peak_hyst_count=0; //current hyst value, ranges from 0 to 20...
  int peak_hyst_arm = 6;// arms if hyst is greater than this


  ANIMaudio() {
    this->init();
  }

  void init() {
    //raw_signal = raw_ac_couple;

    iir_volume.setWeight(.995);
    iir_volume.setAverage(0.0);

  }

  void debug(){
#if ART_VEST
    //print fft bins
    EVERY_N_MILLIS(50) if (verbose2){
      for (int j=0; j<5;j++){
          float m = fft_magnitude[j];
          float f = (j+1)*sampleFrequency/samples;
          Serial.print("f");Serial.print(int(f));Serial.print(":");Serial.print(m);Serial.print(",");
      }
      Serial.println("");
    }
#endif
        
    EVERY_N_MILLIS(50) if (verbose){

      Serial.print("sig:");
      Serial.print(sig);
      Serial.print(",");

      //Serial.print("comp:");
      //Serial.print(comp);
      //Serial.print(",");

      Serial.print("peak_hyst_count:");
      if (peak_hyst_count < peak_hyst_arm) Serial.print(float(peak_hyst_count)/10.0);
      else Serial.print(float(peak_hyst_arm)/10.0);
      Serial.print(",");

      Serial.print("iirv:");
      Serial.print(iir_volume.signal);
      Serial.print(",");

      Serial.print("ratio:");
      Serial.print(ratio);
      Serial.print(",");

      Serial.print("ratiop:");
      Serial.print(ratio_poll);
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
      if (peak_armed){
        b=1;
      }
      Serial.print(b);
      
      Serial.println();


    }
  }


  void setBeat(){
    beat_detected = true;
    beat_detected_dbg = true;
    if (beat_detected_poll == false){
      ratio_poll = ratio;
      beat_detected_poll = true;
    }    
    peak_armed = false;
    peak_hyst_count = 0;
  }

  void clearBeat(){
    //inc hyst_count whenever signal is below the hysteresis level
    if (ratio < peak_hyst_low ) {
      peak_hyst_count = peak_hyst_count + 1;
      if (peak_hyst_count > peak_hyst_arm) peak_armed = true; /// set armed HERE if hysterisis conditions satisfied  // 6 llops at ratio < hyst
    }
    beat_detected = false;
  }

  void peakFound(){
    peak_last = millis();
    setBeat();

  }

  void peakMissed(){
    clearBeat();
            //Serial.println("peakFound");//CPR

  }




  /////////////////////////////////////////////////////////////
  bool peakReady(){
    //beat detected if its been a while since the last beat and the current energy is much higher than the average
    unsigned long  delta = millis() - peak_last;
    if( (peak_armed == true) && (delta > peak_delta_min)){
      return true;
    }
    else { 
      return true;
    }
  }

  //////////////////////////////////////////
  //given  no inputs peak dect assumses sig and comp are ready, see below for how sig and comp are set
  void peakDetect(){    
    ratio = sig/comp/beat_multiplier_min;
    if(peakReady() && ratio > 1.0){
      peakFound();
    } else {
      peakMissed();
    }
    this->debug();// all roads lead to rome
  }

  //given one signal, use that for the IIF volume, and iir volume as comp
  void peakDetect(float a){
    sig = a;    
    iir_volume.update(a);
    comp = iir_volume.signal;
    peakDetect();
  }


  //!update by passing in a raw signal ( 0 to 1024, like an analog read)
  /*
  void update(float audiolevel){
    //record the current time
    raw_signal = audiolevel;
    scaled_signal = map_float( raw_signal, raw_min, raw_max, scaled_min, scaled_max);
    updateScaled();
  }*/

  ////////////////////////////////////////////////////////////////////////////////
  //this is the FFT for the vests  todo make this a define switch
#if ART_VEST
  float fft_input[samples];
  float fft_output[samples];
  float fft_magnitude[fft_save_bins];
  

  fft_config_t *real_fft_plan = fft_init(samples, FFT_REAL, FFT_FORWARD, fft_input, fft_output);

  //used to scale down magnitudes
  float scaler = 2.0/samples/1000.0;

  //!update by reading raw signal from pin, perform FFT, then peak detect
  void update(){
    //SAMPLE 256 times in a row
    for(int i=0; i<samples; i++) {
        unsigned long microseconds = micros();    //Overflows after around 70 minutes!
        real_fft_plan->input[i] = float(analogRead(A0)-2048);

        while(micros() - microseconds < sampling_period_us){}
    }

    //DO FFT on those samples, //TODO try using robins LIB
    fft_execute(real_fft_plan);

    //calculate amplitude for desired n bins
    for ( int j=1; j<fft_save_bins+1;j++){
        // get magnitude by pyth therum
        fft_magnitude[j-1] = scaler * sqrtf((real_fft_plan->output[2*j] * real_fft_plan->output[2*j] ) + (real_fft_plan->output[2*j+1]*real_fft_plan->output[2*j+1]));
    }

    peakDetect(fft_magnitude[0]);

  }
#endif

}; 
