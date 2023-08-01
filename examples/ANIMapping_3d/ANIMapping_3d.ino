/*
  ___        _            ___  ______
 / _ \      (_)          / _ \ | ___ \
/ /_\ \_ __  _ _ __ ___ / /_\ \| |_/ /
|  _  | '_ \| | '_ ` _ \|  _  ||  __/
| | | | | | | | | | | | | | | || |
\_| |_/_| |_|_|_| |_| |_\_| |_/\_|

by Clark Reinholtz, forked from Stefan Petrick and netmindz

Expand ANIMartTRIX to allow 3d pixel mapping and spherical coordinates

VO.4X beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#define NUM_LEDS  150                       // how many LED total,  must be defined before ANIMapping

///////////// only set ONE of these to true, set ALL others to false
//#define NONE_A false
//#define TEENSY_A true
//#define ESP_A false

#define USE_A true

#if USE_A
#include <Audio.h>
AudioInputI2S            i2s1;           //xy=698,360
AudioAnalyzeFFT256       fft256_1;       //xy=1152,492
AudioAmplifier           amp1;           //xy=470,93
AudioConnection          patchCord0(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, fft256_1, 0);
#else

#endif


////////////////

//#include <FastLED.h>
#include "ANIMartRIX.h" //TODO make <> when you copy files back to the right directory
#include "ANIMaudio.h" //TODO make <> when you copy files back to the right directory

//******************************************************************************************************************
//float ledMap[NUM_LEDS][3];

float ledMap[NUM_LEDS][3] = {

    // X       Y       Z
    // -LR+ -INOUT+ -DOWNUP+

    { 7.5 , 0, 13},
    { 7.25 , 0, 9},
    { 7.25 , 0, 5.5},
    { 7.5 , 0, 1.5},

    { 10 , 0, 0},
    { 11 , 0, 4},
    { 10.5 , 0, 7.5},
    { 10.5 , 0, 11},
    { 11 , 0, 14.5},

    { 11.5 , 2, 13},
    { 11.5 , 2, 9},
    { 11.5 , 2, 5},
    { 11.5 , 2, 1.5},

    { 11.5 , 4, 0},
    { 11.5 , 4.5, 4},
    { 12.5 , 4.25, 7.5},
    { 12 , 4, 11},
    { 11.5 , 5, 14.5},

    { 9 , 6, 12.5},
    { 9.5 , 6, 9},
    { 10 , 6, 5},
    { 9 , 6, 1.5},

    { 6.5 , 6, 1.5},
    { 7 , 7, 5}, 
    { 7.5 , 6.5, 9},
    { 7 , 6, 12.5},

    { 6 , 6, 14.5},
    { 4 , 6.5, 12.5},
    { 4.5 , 6, 9},
    { 5 , 6, 5},
    { 4 , 6, 1.5},

    { 1 , 6, 1.5},
    { 1.5 , 7, 5}, 
    { 1.5 , 6, 9},
    { 1 , 6, 12.5},

    { 0 , 5, 14},
    { 0 , 4, 12},
    { 0 , 4.5, 9},
    { 0 , 5, 5},
    { 0 , 4, 0},

    { -.25 , .75, 1.5},
    { -1 , 1, 5.5},
    { -.5 , 1, 9.5},
    { 0 , 1.25, 13},

    { 1, 0, 14},
    { 2, 0, 11.5},
    { 2, 0, 8},
    { 1.75, 0, 4},
    { 2.25, 0, 0},
    { 4.5, 0, 4},   ///// 50
    
    { 7.5 , 0, 13},
    { 7.25 , 0, 9},
    { 7.25 , 0, 5.5},
    { 7.5 , 0, 1.5},

    { 10 , 0, 0},
    { 11 , 0, 4},
    { 10.5 , 0, 7.5},
    { 10.5 , 0, 11},
    { 11 , 0, 14.5},

    { 11.5 , 2, 13},
    { 11.5 , 2, 9},
    { 11.5 , 2, 5},
    { 11.5 , 2, 1.5},

    { 11.5 , 4, 0},
    { 11.5 , 4.5, 4},
    { 12.5 , 4.25, 7.5},
    { 12 , 4, 11},
    { 11.5 , 5, 14.5},

    { 9 , 6, 12.5},
    { 9.5 , 6, 9},
    { 10 , 6, 5},
    { 9 , 6, 1.5},

    { 6.5 , 6, 1.5},
    { 7 , 7, 5}, 
    { 7.5 , 6.5, 9},
    { 7 , 6, 12.5},

    { 6 , 6, 14.5},
    { 4 , 6.5, 12.5},
    { 4.5 , 6, 9},
    { 5 , 6, 5},
    { 4 , 6, 1.5},

    { 1 , 6, 1.5},
    { 1.5 , 7, 5}, 
    { 1.5 , 6, 9},
    { 1 , 6, 12.5},

    { 0 , 5, 14},
    { 0 , 4, 12},
    { 0 , 4.5, 9},
    { 0 , 5, 5},
    { 0 , 4, 0},

    { -.25 , .75, 1.5},
    { -1 , 1, 5.5},
    { -.5 , 1, 9.5},
    { 0 , 1.25, 13},

    { 1, 0, 14},
    { 2, 0, 11.5},
    { 2, 0, 8},
    { 1.75, 0, 4},
    { 2.25, 0, 0},
    { 4.5, 0, 4},   ///// 100
    
        { 7.5 , 0, 13},
    { 7.25 , 0, 9},
    { 7.25 , 0, 5.5},
    { 7.5 , 0, 1.5},

    { 10 , 0, 0},
    { 11 , 0, 4},
    { 10.5 , 0, 7.5},
    { 10.5 , 0, 11},
    { 11 , 0, 14.5},

    { 11.5 , 2, 13},
    { 11.5 , 2, 9},
    { 11.5 , 2, 5},
    { 11.5 , 2, 1.5},

    { 11.5 , 4, 0},
    { 11.5 , 4.5, 4},
    { 12.5 , 4.25, 7.5},
    { 12 , 4, 11},
    { 11.5 , 5, 14.5},

    { 9 , 6, 12.5},
    { 9.5 , 6, 9},
    { 10 , 6, 5},
    { 9 , 6, 1.5},

    { 6.5 , 6, 1.5},
    { 7 , 7, 5}, 
    { 7.5 , 6.5, 9},
    { 7 , 6, 12.5},

    { 6 , 6, 14.5},
    { 4 , 6.5, 12.5},
    { 4.5 , 6, 9},
    { 5 , 6, 5},
    { 4 , 6, 1.5},

    { 1 , 6, 1.5},
    { 1.5 , 7, 5}, 
    { 1.5 , 6, 9},
    { 1 , 6, 12.5},

    { 0 , 5, 14},
    { 0 , 4, 12},
    { 0 , 4.5, 9},
    { 0 , 5, 5},
    { 0 , 4, 0},

    { -.25 , .75, 1.5},
    { -1 , 1, 5.5},
    { -.5 , 1, 9.5},
    { 0 , 1.25, 13},

    { 1, 0, 14},
    { 2, 0, 11.5},
    { 2, 0, 8},
    { 1.75, 0, 4},
    { 2.25, 0, 0},
    { 4.5, 0, 4},   ///// 150
};




//******************************************************************************************************************

CRGB leds[NUM_LEDS];               // framebuffer
ANIMartRIX art(leds);  //led buffer, global scale
ANIMaudio audio;  //





//******************************************************************************************************************

typedef void (*Pattern)();
//typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

int currentPattern = 0;



void Module_Experiment10(){art.Module_Experiment10();}
void Module_Experiment9(){art.Module_Experiment9();}
void SPARKLE_EDGES_MOD9(){art.SPARKLE_EDGES_MOD9();}
void Module_Experiment8(){art.Module_Experiment8();}
void Module_Experiment7(){art.Module_Experiment7();}
void Module_Experiment6(){art.Module_Experiment6();}
void Module_Experiment5(){art.Module_Experiment5();}
void Module_Experiment4(){art.Module_Experiment4();}
void Module_Experiment3(){art.Module_Experiment3();}
void Module_Experiment2(){art.Module_Experiment2();}
void Module_Experiment1(){art.Module_Experiment1();}
void Zoom2(){art.Zoom2();}
void Zoom(){art.Zoom();}
void Parametric_Water(){art.Parametric_Water();}
void Water(){art.Water();}
void Complex_Kaleido_6(){art.Complex_Kaleido_6();}
void Complex_Kaleido_5(){art.Complex_Kaleido_5();}
void Complex_Kaleido_4(){art.Complex_Kaleido_4();}
void Complex_Kaleido_3(){art.Complex_Kaleido_3();}
void Complex_Kaleido_2(){art.Complex_Kaleido_2();}
void Complex_Kaleido(){art.Complex_Kaleido();}
void SM10(){art.SM10();}
void SM9(){art.SM9();}
void SM8(){art.SM8();}
void SM6(){art.SM6();}
void SM5(){art.SM5();}
void SM4(){art.SM4();}
void SM3(){art.SM3();}
void SM2(){art.SM2();}
void SM1(){art.SM1();}
void Big_Caleido(){art.Big_Caleido();}
void RGB_Blobs5(){art.RGB_Blobs5();}
void RGB_Blobs4(){art.RGB_Blobs4();}
void RGB_Blobs3(){art.RGB_Blobs3();}
void RGB_Blobs2(){art.RGB_Blobs2();}
void RGB_Blobs(){art.RGB_Blobs();}
void Polar_Waves(){art.Polar_Waves();}
void Slow_Fade(){art.Slow_Fade();}
void Hot_Blob(){art.Hot_Blob();}
void Spiralus2(){art.Spiralus2();}
void Spiralus(){art.Spiralus();}
void Yves(){art.Yves();}
void Scaledemo1(){art.Scaledemo1();}
void Scaledemo2(){art.Scaledemo2();}
void Caleido3(){art.Caleido3();}
void Caleido2(){art.Caleido2();}
void Caleido1(){art.Caleido1();}
void Lava1(){art.Lava1();}
void Distance_Experiment(){art.Distance_Experiment();}
void Center_Field(){art.Center_Field();}
void Waves(){art.Waves();}
void Chasing_Spirals(){art.Chasing_Spirals();}
void Rotating_Blob(){art.Rotating_Blob();}
void Rings(){art.Rings();}


void PlaneRotation1(){art.PlaneRotation1();}
void GrowingSpheres(){art.GrowingSpheres();}
void Chasing_Spirals_Hsi(){art.Chasing_Spirals_Hsi();}
void demoBpm(){art.demoBpm();}
void Module_Experiment11_Hsi(){art.Module_Experiment11_Hsi();}
void Module_Experiment9_Hsi(){art.Module_Experiment9_Hsi();}

PatternAndNameList gPatterns = {

  {Complex_Kaleido_5,"Complex_Kaleido_5"},
  {GrowingSpheres, "GrowingSpheres"},
  {PlaneRotation1, "PlaneRotation1"},
  {Chasing_Spirals_Hsi, "Chasing_Spirals_Hsi"},
  //{demoBpm, "demoBpm"}, //ugly
  {Module_Experiment11_Hsi, "Module_Experiment11_Hsi"},
  {Module_Experiment9_Hsi, "Module_Experiment9_Hsi"},


  {Rings, "Rings"},
  {Module_Experiment10,"Module_Experiment10"},
  {Module_Experiment9,"Module_Experiment9"}, // FAV swipes!
  {SPARKLE_EDGES_MOD9,"SPARKLE_EDGES_MOD9"}, // FAV swipes! with sweet edges
  {Module_Experiment8,"Module_Experiment8"},
  {Module_Experiment7,"Module_Experiment7"},
  {Module_Experiment6,"Module_Experiment6"},
  {Module_Experiment5,"Module_Experiment5"},
  {Module_Experiment4,"Module_Experiment4"},
  {Module_Experiment3,"Module_Experiment3"},//FAV yellow blob in red
  {Module_Experiment2,"Module_Experiment2"},//FAV
  {Module_Experiment1,"Module_Experiment1"},
  {Parametric_Water,"Parametric_Water"},
  {Water,"Water"},
  
  {Complex_Kaleido_6,"Complex_Kaleido_6"}, ///great at 30 secs!
  {Complex_Kaleido_4,"Complex_Kaleido_4"},//good
  {Complex_Kaleido_3, "Complex_Kaleido_3"},
  {Complex_Kaleido_2,"Complex_Kaleido_2"},
  {Complex_Kaleido, "Complex_Kaleido"},
  
  {SM10,"SM10"},
  {SM9,"SM9"},
  {SM8,"SM8"},//fun strobe
  //{SM7,"SM7"}, //todo why is this commented out?
  {SM6,"SM6"},
  {SM5,"SM5"},
  {SM4,"SM4"},
  {SM3,"SM3"},
  {SM2,"SM2"},
  {SM1,"SM1"},
  {Big_Caleido,"Big_Caleido"},
  {RGB_Blobs5,"RGB_Blobs5"},
  {RGB_Blobs4,"RGB_Blobs4"},
  {RGB_Blobs3,"RGB_Blobs3"},
  {RGB_Blobs2,"RGB_Blobs2"},
  {RGB_Blobs,"RGB_Blobs"},
  {Polar_Waves,"Polar_Waves"},
  {Slow_Fade,"Slow_Fade"},
  {Zoom2, "Zoom2"}, 
  {Zoom,"Zoom"},
  {Hot_Blob,"Hot_Blob"},
  {Spiralus2,"Spiralus2"},
  {Spiralus,"Spiralus"},
  {Yves,"Yves"},
  {Scaledemo1,"Scaledemo1"},///active and fun
  {Scaledemo2,"Scaledemo2"},///active and fun
  {Lava1,"Lava1"},//broken
  
  {Caleido3,"Caleido3"},
  {Caleido2,"Caleido2"},
  {Caleido1,"Caleido1"}, 
  
  {Distance_Experiment,"Distance_Experiment"},
  {Center_Field,"Center_Field"},
  {Waves,"Waves"},//broken
  {Chasing_Spirals,"Chasing_Spirals"},
  {Rotating_Blob,"Rotating_Blob"}
};



/*

//TODO





bugfixes: 

  test 400 leds with ESP vs teensy
  low limit, high limit?  set in get ready?
  maybey its a good idea to introduce randomness when selecting a new pattern?
  make music modes and toggleability
  hsi_sanity_check use constants you sucker, even in utils



  --Scaledemo1 still weird
  -- made scaldemo2




*/

int gPatternCount = ARRAY_SIZE(gPatterns);
void clearPattern(){
  currentPattern = 0;
  Serial.print("Setting pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
}


void incrementPattern(int inc = 1){
  currentPattern = currentPattern + inc;
  if (currentPattern >= gPatternCount) currentPattern = 0;
  if (currentPattern < 0) currentPattern = gPatternCount-1;

  Serial.print("Incrementing pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
}

void randomPattern(){
  currentPattern = random(gPatternCount);
  if (currentPattern >= gPatternCount) currentPattern = 0;
  Serial.print("Setting pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
}





//******************************************************************************************************************



void showCurrentPattern(){
  gPatterns[currentPattern].pattern();  
  art.markStartOfShow();
  FastLED.show();
  art.markEndOfShow();
}


//******************************************************************************************************************

modableF* audioModBeatDestPtr = &art.global_intensity;

//******************************************************************************************************************
void setup() {

  art.global_intensity.setMinMax(0.2, 0.8);//MIN MUST be >0// MAX MUST be <=1
  //art.global_intensity.setBaseToMiddle();
  //art.global_scale_x.setBaseToMiddle();
  //art.global_scale_y.setBaseToMiddle();
  //art.global_scale_z.setBaseToMiddle();
  //art.gHue.setBaseToMiddle();
  
  //FastLED.addLeds<APA102, 7, 14, BGR, DATA_RATE_MHZ(8)>(leds, NUM_LED);   
  FastLED.addLeds<WS2811, 2, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, 2000); // optional current limiting [5V, 2000mA] 
  FastLED.setBrightness(255); // this is OVERWRITTEN!!!!!! see art.global_intensity  //todo set to 255 in final production???
  Serial.begin(115200);                 // check serial monitor for current fps count
  //art.setGlobalScale(0.5); 

#if USE_A
  fft256_1.averageTogether(2); //runs at 300Hz+, lets slow that down to ~ 200 hz
  AudioMemory(50);
  //filter1.frequency(30); // filter out DC & extremely low frequencies
  amp1.gain(8.5);        // amplify sign to useful range
#endif


}


//*******************************************************************************************************************


bool verbose = false;
bool verbose2 = false;
bool play = false;
bool doRandom = true;
bool musicReactive = true;
bool hueDrift = false;
int cnt = 32;



void loop() {
  //changing paterns
  if (play){
      if (doRandom) {EVERY_N_SECONDS(30) randomPattern();}
      else {EVERY_N_SECONDS(30) incrementPattern();}
  }
  // report
  if(verbose){
    EVERY_N_MILLIS(500) {
      art.report_performance();   // check serial monitor for report 
      /*
      Serial.print("tsum: ");
      Serial.print(art.audioSum);
      Serial.print("tsam: ");
      Serial.println(art.audioSamples);
      */
    }
  }

//*******************************************************************************************************************
  EVERY_N_MILLIS(5) {
    if (hueDrift) art.gHue += .0003; // todo make this scale with FPS, put into show current patter to make immune to FPS changes
    showCurrentPattern(); // 200 FPS max
    audioModBeatDestPtr->update();
    //audio.update(art.pollAudio());
#if USE_A
      //EVERY_N_MILLIS(5) Serial.println("trying audio");
      if (fft256_1.available()) {
        //EVERY_N_MILLIS(5) Serial.println("fft audio");
        float b0 = fft256_1.read(0);
        audio.updateScaled(b0);
        if (audio.beat_detected && musicReactive) {
          //art.global_intensity += audio.abs_signal*5;//todo make this proportional to ratio, not abs_signal?
          //*audioModBeatDestPtr += audio.abs_signal*5;//todo make this proportional to ratio, not abs_signal?
          audioModBeatDestPtr->trigger(audio.abs_signal*5);//todo make this proportional to ratio, not abs_signal?
          //EVERY_N_MILLIS(5) Serial.println("beat audio");
        } // beat
      } // fft available
#else
      EVERY_N_MILLIS(1000) {
        audioModBeatDestPtr->trigger(.9);//todo make this proportional to ratio, not abs_signal?
        //Serial.println("beat");
      }
#endif
  } // EVERY_N_MILLIS(5)
//*******************************************************************************************************************
  // testing interface, user input
  if (Serial.available() > 0) {
    // read the incoming byte:
    int incomingByte = Serial.read();

    if (incomingByte == 'd'){
      verbose = not verbose;
      Serial.print("Setting top verbose shift to"); Serial.println(verbose);
    } else if (incomingByte == 'D'){
      verbose2 = not verbose2;
      Serial.print("Setting top verbose2 shift to"); Serial.println(verbose2);
    } else if(incomingByte == 'P'){
      play = not play;    
      Serial.print("Setting play to"); Serial.println(play);
    } else if(incomingByte == 'a'){
      int i = Serial.parseInt();
      //audioModBeatDestPtr->disa;
      if (i==1) audioModBeatDestPtr = &art.global_intensity;
      if (i==2) audioModBeatDestPtr = &art.global_scale_x;
      if (i==3) audioModBeatDestPtr = &art.global_scale_y;
      if (i==4) audioModBeatDestPtr = &art.global_scale_z;
      if (i==5) audioModBeatDestPtr = &art.gHue;
    } else if(incomingByte == 'c'){
      incrementPalette();
    } else if (incomingByte == 'n'){
      incrementPattern();
    } else if (incomingByte == 'm'){
      musicReactive = not musicReactive;
      Serial.print("Setting musicReactive shift to"); Serial.println(musicReactive);
    } else if (incomingByte == 'b'){
      incrementPattern(-1); 
    } else if (incomingByte == 'p'){
      audio.verbose = not audio.verbose;
      Serial.print("Setting audio.verbose shift to"); Serial.println(audio.verbose);
    } else if (incomingByte == 'o'){
      audioModBeatDestPtr->envelope.verbose = not audioModBeatDestPtr->envelope.verbose;
      Serial.print("Setting amod.verbose shift to"); Serial.println(audioModBeatDestPtr->envelope.verbose);
    } else if (incomingByte == 'g'){
      int i = Serial.parseInt();
      clearPattern();
      incrementPattern(i);
    } else if (incomingByte == 'v'){
      float i = Serial.parseFloat();
      audio.iir_volume.setWeight(float(i));
      Serial.print("Setting audio.iir_volume. to"); Serial.println(audio.iir_volume.iir_weight);
    } else if (incomingByte == 'l'){
      float i = Serial.parseFloat();
      audio.iir_lowpass.setWeight(float(i));
      Serial.print("Setting audio.iir_lowpass. to"); Serial.println(audio.iir_lowpass.iir_weight);
    } else if (incomingByte == 'B'){
      clearPattern();
    } else if (incomingByte == 'z'){
      hueDrift = not hueDrift;
      Serial.print("Setting hueDrift to"); Serial.println(hueDrift);
    } else if (incomingByte == 'h'){
      art.gHue += .1;
      Serial.print("Setting hue shift to"); Serial.println(art.gHue.getBase());
    } else if (incomingByte == 'H'){
      art.gHue = 0;
      Serial.print("Setting hue shift to"); Serial.println(art.gHue.getBase());
    } else if (incomingByte == 'r'){
      doRandom = not doRandom;
      if (doRandom && play==false) play = true;
      Serial.print("Setting doRandom to"); Serial.println(doRandom);
      Serial.print("Setting play to"); Serial.println(play);
    }
  }


} 
