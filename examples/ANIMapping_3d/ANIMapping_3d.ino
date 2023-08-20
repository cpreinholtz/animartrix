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

//These SHOULD be overwritten accordinly in map
//TEENSY + IIS MIC
#define ART_WAG false
#define ART_BF false
#define ART_TEENSY false
//ESP + ANALOG MIC
#define ART_VEST false

//!!!! ONLY INCLUDE ONE MAP
#include "MapWag.h"
//#include "MapKVest.h"
//#include "MapCVest.h"
//#include "MapProtoVest.h"
//!!!! ONLY INCLUDE ONE MAP


#define USE_AUDIO true
#define USE_IMU true
// todo set these in map??^^


#if USE_AUDIO
#if ART_TEENSY
#include <Audio.h>
//include this^&@
AudioInputI2S2            i2s1;           //xy=698,360
AudioAnalyzeFFT256       fft256_1;       //xy=1152,492
AudioAmplifier           amp1;           //xy=470,93
AudioConnection          patchCord0(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, fft256_1, 0);
#else
//esp archictecture using fft

#endif
#endif


////////////////

//#include <FastLED.h>
#include "ANIMartRIX.h" //TODO make <> when you copy files back to the right directory
#include "ANIMaudio.h" //TODO make <> when you copy files back to the right directory
#if USE_IMU
#include "ANIMimu.h" //TODO make <> when you copy files back to the right directory
#endif

//******************************************************************************************************************
//float ledMap[NUM_LEDS][3];




//******************************************************************************************************************

CRGB leds[NUM_LEDS];               // framebuffer
ANIMartRIX art(leds);  //led buffer, global scale

ANIMaudio audio;  //
#if USE_IMU
ANIMimu imu;
#endif


modableF* audioModBeatDestPtr = &art.global_intensity;
modableF dummy_mod;


//******************************************************************************************************************

typedef void (*Pattern)();
//typedef Pattern PatternList[];
typedef struct {
  Pattern pattern;
  String name;
} PatternAndName;
typedef PatternAndName PatternAndNameList[];

int currentPattern = 0;


void TestMap(){art.TestMap();}
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


void PlaneCounterRotation1(){art.PlaneCounterRotation1();}
void PlaneRotation1(){art.PlaneRotation1();}
void GrowingSpheres(){art.GrowingSpheres();}
void Chasing_Spirals_Hsi(){art.Chasing_Spirals_Hsi();}
void Module_Experiment11_Hsi(){art.Module_Experiment11_Hsi();}
void Module_Experiment9_Hsi(){art.Module_Experiment9_Hsi();}

PatternAndNameList gPatterns = {
  //{TestMap, "TestMap"},

#if USE_IMU
  {PlaneCounterRotation1, "PlaneCounterRotation1"},
#endif
  
  {Module_Experiment1,"Module_Experiment1"},
  {SM9,"SM9"},  
  {Chasing_Spirals_Hsi, "Chasing_Spirals_Hsi"},
  {Caleido1,"Caleido1"}, 
  {Complex_Kaleido_5,"Complex_Kaleido_5"},
  //{GrowingSpheres, "GrowingSpheres"},
  {PlaneRotation1, "PlaneRotation1"},


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

  {Parametric_Water,"Parametric_Water"},
  {Water,"Water"},
  
  {Complex_Kaleido_6,"Complex_Kaleido_6"}, ///great at 30 secs!
  {Complex_Kaleido_4,"Complex_Kaleido_4"},//good
  //{Complex_Kaleido_3, "Complex_Kaleido_3"},
  {Complex_Kaleido_2,"Complex_Kaleido_2"},
  {Complex_Kaleido, "Complex_Kaleido"},
  
  {SM10,"SM10"},

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
  //{Slow_Fade,"Slow_Fade"},
  {Zoom2, "Zoom2"}, 
  {Zoom,"Zoom"},
  {Hot_Blob,"Hot_Blob"},
  {Spiralus2,"Spiralus2"},
  {Spiralus,"Spiralus"},
  {Yves,"Yves"},
  //{Scaledemo1,"Scaledemo1"},///active and fun
  //{Scaledemo2,"Scaledemo2"},///active and fun
  {Lava1,"Lava1"},
  
  {Caleido3,"Caleido3"},
  {Caleido2,"Caleido2"},
  {Caleido1,"Caleido1"}, 
  
  //{Distance_Experiment,"Distance_Experiment"},
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



void setMusicMod(int i){
      if (i==0) audioModBeatDestPtr = &dummy_mod;      
      else if (i==1) audioModBeatDestPtr = &art.global_intensity;
      else if (i==2) audioModBeatDestPtr = &art.global_bpm;
#if ART_TEENSY
      else if (i==3) audioModBeatDestPtr = &art.global_scale_x;
      else if (i==4) audioModBeatDestPtr = &art.global_scale_y;
      else if (i==5) audioModBeatDestPtr = &art.global_scale_z;
      else if (i==6) audioModBeatDestPtr = &art.gHue;
#endif
      else audioModBeatDestPtr = &dummy_mod; 
      Serial.println("setting music mod to");
      Serial.println(i);
}

void randomMusicMod(){
#if ART_TEENSY
  int im = 7;
#else
  int im = 3;
#endif
  setMusicMod(random(im));
}






//******************************************************************************************************************
#if ART_TEENSY

float vBat = 12.8;
const float vBatMin = 11.7;
bool batteryCharged = true;
bool batteryChargedOverride = false;

void assessVdiv(){
  EVERY_N_MILLIS(100){
    //full scale = 3.3v @ 1024
    //vAdc = 3.3 * analogRead / 1024
    //vAdc = vbat * 82 / 4282 ( real rdiv is more like 4720.0 / 82.0 from empirical testing)
    //vbat = vAdc * 4284 / 82
    int sum = 0;
    float n = 20;
    for (int i=0; i < n; i++) sum += analogRead(15);
    n = float(sum) / n;
    //Serial.println(n);
    
    vBat = 0.9 * vBat + (3.3 * n / 1024.0 * 4720.0 / 82.0) * .1;

    if (batteryCharged and vBat < vBatMin){
      batteryCharged = false;
      Serial.println("Battery too low!!! Turning lights off.");
    }   
  }
}



void copyBuffer(){
  int thisPixel = 0;
  if (batteryCharged or batteryChargedOverride){
    for (int ring = 0 ; ring < nRings; ring++){
      for (int pixel = 0; pixel < nPixelsPerRing[ring]; pixel++){
        int color = (((int)leds[thisPixel].r)<<16) | (((int)leds[thisPixel].g)<<8)| (((int)leds[thisPixel].b));
        oleds.setPixel(pixel+ring*nMaxPixels, color);
        thisPixel++;
      }
    }
  } else {
    int color = 0;
    for (int ring = 0 ; ring < nRings; ring++){
      for (int pixel = 0; pixel < nPixelsPerRing[ring]; pixel++){
        oleds.setPixel(pixel+ring*nMaxPixels, color);
        thisPixel++;
      }
    }
  }

  oleds.show();
}


#endif




//******************************************************************************************************************

bool playAll = true;

bool verbose = false;
bool verbose2 = false;
bool play = false;

bool doModulation = true;
bool doRandom = true;
bool musicReactive = true;
bool hueDrift = true;
bool doDarkCheck = true;



//******************************************************************************************************************
void setup() {
  Serial.begin(115200);                 // check serial monitor for current fps count
  delay(100);

  art.global_intensity.setMinMax(0.2, 0.6);//MIN MUST be >0// MAX MUST be <=1
  //art.global_intensity.setBaseToMiddle();
  //art.global_scale_x.setBaseToMiddle();
  //art.global_scale_y.setBaseToMiddle();
  //art.global_scale_z.setBaseToMiddle();
  //art.gHue.setBaseToMiddle();
#if ART_TEENSY
  randomSeed(max(analogRead(A1),1));
#else
  randomSeed(analogRead(A0));
  //FastLED.addLeds<APA102, 7, 14, BGR, DATA_RATE_MHZ(8)>(leds, NUM_LED);   
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  //FastLED.setMaxPowerInVoltsAndMilliamps( 5, 2000); // optional current limiting [5V, 2000mA]  todo
  FastLED.setBrightness(255); // this is OVERWRITTEN!!!!!! see art.global_intensity  //todo set to 255 in final production???
#endif

  if ((play and doRandom) or playAll){
    randomPattern();
  }

  //art.setGlobalScale(0.5); 

#if USE_AUDIO
#if ART_TEENSY
  fft256_1.averageTogether(2); //runs at 300Hz+, lets slow that down to ~ 200 hz
  AudioMemory(50);
  //filter1.frequency(30); // filter out DC & extremely low frequencies
  amp1.gain(85);        // amplify sign to useful range
  audio.beat_multiplier_min = 2.2;
  audio.peak_hyst_arm = 10;
  audio.peak_volume_min = 0.1;  //SET TO .1
  audio.iir_volume.setWeight(.995);
#else
  audio.beat_multiplier_min = 2.0;
  audio.peak_hyst_arm = 1;
  audio.peak_volume_min = 0.2;
  audio.iir_volume.setWeight(0.973); // my polling is 20 HZ, teensy is 150, sloooww this down
  art.audio = & audio;
#endif
#endif

#if USE_IMU
  while(1){
    if (imu.begin()) 
      break;
  }
#endif

#if ART_TEENSY
  oleds.begin();
  for (int ring = 0 ; ring < nRings; ring++){
    if (nPixelsPerRing[ring] > nMaxPixels) {
      Serial.println("need to increase nMaxPixels to match max(nPixelsPerRing) in MapWag.h") ;
      while(1) delay(10);
    }
  }
#endif

}


//*******************************************************************************************************************


bool isDark = false;

unsigned long darkTime = 0; //start of total darkness

void darkWadCheck(){
  if (doDarkCheck){
    // if we were not dark last time, update timer to now  // else leave timer 
    if (not isDark) darkTime = millis();

    //do dark check
    isDark = true;
    for (int i=0; i< NUM_LEDS; i++){
      //leds[i].r = 0;
      //leds[i].b = 0;
      //leds[i].g = 0;
      if (leds[i].r > 0 or leds[i].b > 0 or leds[i].g > 0) {
        isDark = false;
        break;
      }
    }

    // if we are dark now and have been for a long while, change the pattern
    if (isDark and millis() - darkTime > 3000){
      randomPattern();
      Serial.println("force random pattern");
      isDark = false;
    }
  }
}


void showCurrentPattern(){
  //plat the current pattern through the art
  gPatterns[currentPattern].pattern();  
  art.markStartOfShow();

  //show buffer
#if ART_VEST
    FastLED.show();
#endif
#if ART_TEENSY
  copyBuffer();
  assessVdiv();
#endif

  //check for all black animations, if so change the pattern
  darkWadCheck();

  art.markEndOfShow();
}

void addLife(){

  ///////////////////////////////////////////////////////
  //add all sorts of modulation
  if (playAll){
    //change pattern
    int chg = 60;
    EVERY_N_SECONDS(chg) {
      randomPattern();
      incrementPalette();
    }
    //change music reactivity
    EVERY_N_SECONDS(chg*2) randomMusicMod();

    //change hue shift
    art.gHue += .0001;
  }

  if (doModulation){

#if ART_TEENSY
    int prob = 100;

    //Change X / Y / Z centers
    /*    */

    float val = float(random(-prob,prob))/float(prob);

    EVERY_N_MILLIS(62542) {
      art.center_xm.trigger(val);
      Serial.println(val);
    }
    art.center_xm.update();
    
    val = float(random(-prob,prob))/float(prob);
    EVERY_N_MILLIS(42833) art.center_ym.trigger(val);
    art.center_ym.update();

    val = float(random(-prob,prob))/float(prob);
    EVERY_N_MILLIS(74489) art.center_zm.trigger(val);
    art.center_zm.update();
    
    art.re_render_spherical_lookup_table();

    //set in animation, can only change envelope

    animation.low_limit.trigger(.3 * (move.noise_angle[10]-PI));
    animation.low_limit.update();

    //TODO TEST THESE!!!
    animation.high_limit.trigger( .3 * (move.noise_angle[11]-PI));
    //animation.high_limit.trigger( move.sine[11]);
    animation.high_limit.update();

    EVERY_N_MILLIS(25){
      if(verbose2){
        //
        //Serial.print("center_xme:"); Serial.print(art.center_xm.envelope.getMax()); Serial.print(",");
        //Serial.print("center_xm:"); Serial.print(art.center_xm.getEnvelope()); Serial.print(",");
        //art.center_xm.envelope.verbose=true;
        //art.center_xm.envelope.debug();        
        //art.global_scale_x.envelope.verbose=true;
        //art.global_scale_x.envelope.debug();
        /*
        Serial.print("center_ym:"); Serial.print(art.center_ym.getEnvelope()); Serial.print(",");
        Serial.print("center_zm:"); Serial.print(art.center_zm.getEnvelope()); Serial.print(",");
        Serial.print("low_limit:"); Serial.print(animation.low_limit.getEnvelope()); Serial.print(",");
        Serial.print("high_limit:"); Serial.print(animation.high_limit.getEnvelope()); Serial.print(",");
        Serial.println();*/
      } else   art.center_xm.envelope.verbose=false;

      
      /*

      Serial.print("10 ramp, sine noise");
      Serial.print(move.ramp[10]);
      Serial.print("   ");
      Serial.print(move.sine[10]);
      Serial.print("   ");
      Serial.println(move.noise_angle[10]);

      Serial.print("11 ramp, sine noise");
      Serial.print(move.ramp[11]);
      Serial.print("   ");
      Serial.print(move.sine[11]);
      Serial.print("   ");
      Serial.println(move.noise_angle[11]);

      Serial.print("2 ramp, sine noise");
      Serial.print(move.ramp[2]);
      Serial.print("   ");
      Serial.print(move.sine[2]);
      Serial.print("   ");
      Serial.println(move.noise_angle[2]);*/
    }


#endif
    


  ///////////////////////////////////////////////////////
  //add only user specified modulation
  } else {
    if (hueDrift) art.gHue += .0001; // todo make this scale with FPS, put into show current patter to make immune to FPS changes

    //changing paterns
    if (play){
        if (doRandom) {}
        else {EVERY_N_SECONDS(45) incrementPattern();}
    }
  }
}



void updateIMU(){
#if USE_IMU
  imu.update();
  art.upVector = imu.filteredPosition;
#endif
}



void updateAudio(){
#if USE_AUDIO
#if ART_TEENSY
  if (fft256_1.available()) {
      //EVERY_N_MILLIS(5) Serial.println("fft audio");
      float b0 = fft256_1.read(0);
      audio.peakDetect(b0);
      if (audio.beat_detected && musicReactive) {
        //Serial.println("beat");
        audioModBeatDestPtr->trigger(audio.ratio/3.0);
      } // beat
    } // fft available
#else
  //audio.update();
  if (audio.beat_detected_poll && musicReactive) {
    //Serial.println("beat");
    audioModBeatDestPtr->trigger(audio.ratio_poll);
    audio.beat_detected_poll = false;
  } // beat
#endif
#endif
  audioModBeatDestPtr->update();
}


void updateSerial(){
  // report
  if(verbose){
    EVERY_N_MILLIS(500) {
      Serial.print("vBat:"); Serial.print(vBat); Serial.print(",");
      art.report_performance();   // check serial monitor for report 
    }
  }

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
    } else if(incomingByte == 'p'){
      playAll = not playAll;    
      Serial.print("Setting playAll to"); Serial.println(playAll);
    }else if(incomingByte == 'P'){
      play = not play;    
      Serial.print("Setting play to"); Serial.println(play);
    } else if(incomingByte == 'a'){
      int a = Serial.parseInt();
      setMusicMod(a);
      Serial.print("Setting misic mod shift to"); Serial.println(a);
    } else if(incomingByte == 'c'){
      incrementPalette();
    } else if (incomingByte == 'n'){
      incrementPattern();
    } else if (incomingByte == 'x'){
      batteryChargedOverride = not batteryChargedOverride;
      Serial.print("Setting batteryChargedOverride shift to"); Serial.println(batteryChargedOverride);
    } else if (incomingByte == 'm'){
      musicReactive = not musicReactive;
      Serial.print("Setting musicReactive shift to"); Serial.println(musicReactive);
    } else if (incomingByte == 'b'){
      incrementPattern(-1); 
    } else if (incomingByte == 'O'){
      audio.verbose = not audio.verbose;
      Serial.print("Setting audio.verbose shift to"); Serial.println(audio.verbose);
    } else if (incomingByte == '['){
      audio.verbose2 = not audio.verbose2;
      Serial.print("Setting audio.verbose shift to"); Serial.println(audio.verbose2);
    }else if (incomingByte == 'o'){
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


void loop() {

//*******************************************************************************************************************
  EVERY_N_MILLIS(5) {
    addLife();
    showCurrentPattern(); // 200 FPS max
    updateIMU();
    updateAudio();
    updateSerial();
  } // EVERY_N_MILLIS(5)
//*******************************************************************************************************************
  


} 
