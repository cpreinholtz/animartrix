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

//These SHOULD be overwritten accordingly in map

//switch between teensy and esp architecture
#define ART_TEENSY false

//turn on and off IMU and AUDIO processing
#define USE_IMU false
#define USE_AUDIO false


//TEENSY + IIS MIC
#define ART_WAG false
#define ART_BF false
//TEENSY + analog mic
#define ART_CUBE false
#define ART_WALL false

//ESP + ANALOG MIC
#define ART_VEST false





//!!!! ONLY INCLUDE ONE MAP
#include "MapWag.h"
//#include "MapBf.h"
//#include "MapCube.h"
//#include "MapWall.h"
//#include "MapKVest.h"
//#include "MapCVest.h"
//#include "MapProtoVest.h"
//!!!! ONLY INCLUDE ONE MAP





#if USE_AUDIO
#if ART_TEENSY

#if ART_WAG
#include <Audio.h>
//include this^&@
AudioInputI2S2            i2s1;           //xy=698,360
AudioAnalyzeFFT256       fft256_1;       //xy=1152,492
AudioAmplifier           amp1;           //xy=470,93
AudioConnection          patchCord0(i2s1, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, fft256_1, 0);

//ART_CUBE 
#else
#include <Audio.h>
//include this^&@
AudioInputI2S2            audioSource;           //xy=698,360
AudioAnalyzeFFT256       fft256_1;       //xy=1152,492
AudioAmplifier           amp1;           //xy=470,93
AudioConnection          patchCord0(audioSource, 0, amp1, 0);
AudioConnection          patchCord2(amp1, 0, fft256_1, 0);

#endif

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

bool verbose2 = false;

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



// p to 0 = stop random
//b n or g23 to change
PatternAndNameList gPatterns = {
  //{TestMap, "TestMap"},
#if ART_CUBE
  //{Scaledemo1,"Scaledemo1"},///active and fun
  //{Scaledemo2,"Scaledemo2"},///active and fun
#endif

#if USE_IMU
  {PlaneCounterRotation1, "PlaneCounterRotation1"},

  {Module_Experiment10,"Module_Experiment10"},//todo  again a bit chatoc on cube
  {Module_Experiment9,"Module_Experiment9"}, //todo  FAV swipes!
  {SPARKLE_EDGES_MOD9,"SPARKLE_EDGES_MOD9"}, //todo  FAV swipes! with sweet edges, bit chaotic on the cude?

  //THIS IS CROKEN ON CUBE AND BREAKS OTHER PATTERNS?{Chasing_Spirals_Hsi, "Chasing_Spirals_Hsi"},//todo fix on cube!!!!
  //{GrowingSpheres, "GrowingSpheres"}, //BROKEN ON CUBE AND BREAKS OTHERS!!!!!!! {PlaneRotation1, "PlaneRotation1"},//todo fix on cube!!!!

#endif
  
//{Module_Experiment1,"Module_Experiment1"},
  {SM9,"SM9"}, 
  {Caleido1,"Caleido1"}, 
  {Complex_Kaleido_5,"Complex_Kaleido_5"},


  {Module_Experiment11_Hsi, "Module_Experiment11_Hsi"},
  {Module_Experiment9_Hsi, "Module_Experiment9_Hsi"},


  {Rings, "Rings"},
  {Module_Experiment8,"Module_Experiment8"}, // todo speed up
  {Module_Experiment7,"Module_Experiment7"},
#if not ART_WALL
  {Module_Experiment6,"Module_Experiment6"},
#endif
  {Module_Experiment5,"Module_Experiment5"},//incredible
  //{Module_Experiment4,"Module_Experiment4"},//pretty cool
#if not ART_WALL
  {Module_Experiment3,"Module_Experiment3"},//FAV yellow blob in red
#endif
  {Module_Experiment2,"Module_Experiment2"},//FAV

  {Parametric_Water,"Parametric_Water"},
#if not ART_WALL
  {Water,"Water"},
#endif
  
  {Complex_Kaleido_6,"Complex_Kaleido_6"},
  {Complex_Kaleido_4,"Complex_Kaleido_4"},
  //{Complex_Kaleido_3, "Complex_Kaleido_3"},
  {Complex_Kaleido_2,"Complex_Kaleido_2"},//todo figure out what is making the raindrop effect, need to do more!!!!!!
  {Complex_Kaleido, "Complex_Kaleido"},
  
  {SM10,"SM10"},

  {SM8,"SM8"},
  //{SM7,"SM7"}, //todo why is this commented out?
  {SM6,"SM6"},
  {SM5,"SM5"},//hass the red blob issue
  {SM4,"SM4"},
#if not ART_WALL
  {SM3,"SM3"},
#endif
  {SM2,"SM2"},
  {SM1,"SM1"},
  {Big_Caleido,"Big_Caleido"},
  {RGB_Blobs5,"RGB_Blobs5"},//todo needs better offsets?
  {RGB_Blobs4,"RGB_Blobs4"},//todo needs better offsets? not cool on cube
  {RGB_Blobs3,"RGB_Blobs3"},//has the red blob issue
  {RGB_Blobs2,"RGB_Blobs2"},
  {RGB_Blobs,"RGB_Blobs"},//has the red blob issue
  {Polar_Waves,"Polar_Waves"},//cool, has the red blob issue
  //{Slow_Fade,"Slow_Fade"},
#if ART_TEENSY
  {Zoom2, "Zoom2"}, //broken on esp, todo speed up on cube?
  {Zoom,"Zoom"}, //broken on esp
  {Hot_Blob,"Hot_Blob"}, //broken on esp,
#endif

  {Spiralus2,"Spiralus2"},//todo fix on cube?
#if not ART_WALL
  {Spiralus,"Spiralus"},//todo fix on cube?
#endif
  //{Yves,"Yves"},//bit darkwady?

  {Lava1,"Lava1"},
  
  {Caleido3,"Caleido3"},
  {Caleido2,"Caleido2"},
  {Caleido1,"Caleido1"},
  
  //{Distance_Experiment,"Distance_Experiment"},
  //{Center_Field,"Center_Field"},
  {Waves,"Waves"},
  {Chasing_Spirals,"Chasing_Spirals"},//todo make more like this!!!!!!
  {Rotating_Blob,"Rotating_Blob"}//todo fix on cube!!!!,
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
float multiir=0;
void setPattern(int setTo){
  //need to do this periodically or else the ramps get out of hand
  //every 100 secs should do
  EVERY_N_MILLIS(100000){art.clearRamps();Serial.println("clearing ramps");}

  currentPattern = setTo;
  if (currentPattern >= gPatternCount) currentPattern = 0;
  if (currentPattern < 0) currentPattern = gPatternCount-1;
  animation.low_limit_offset=0;
  multiir=0;
  Serial.print("Incrementing pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
  Serial.print("Setting pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
}

void clearPattern(){
  setPattern(0);
}

void incrementPattern(int inc = 1){
  setPattern(currentPattern + inc);
}

void randomPattern(){
  setPattern(random(gPatternCount));
  //state =2;
}


void setMusicMod(int i){
      audioModBeatDestPtr->envelope.clear();
      if (i==0) audioModBeatDestPtr = &dummy_mod;      
      else if (i==1) audioModBeatDestPtr = &art.global_intensity;
      else if (i==2) audioModBeatDestPtr = &art.global_bpm;
#if ART_TEENSY
      else if (i==3) audioModBeatDestPtr = &art.global_scale_x;//todo, globa scal x y z can be a bit intense????
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
  setMusicMod(random(2,im));
#else
  int im = 3;
  setMusicMod(random(2,im));
#endif
  
}






//******************************************************************************************************************
#if ART_TEENSY

float vBat = 12.8;
const float vBatMin = 11.7;
bool batteryCharged = true;
bool batteryChargedOverride = true;

void assessVdiv(){
#if ART_WAG
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
#endif
}


float mult=1;

int state =0 ;
void copyBuffer(){
  int thisPixel = 0;
  if (batteryCharged or batteryChargedOverride){

    float total = 0;
    float max=0;
    for (int ring = 0 ; ring < nRings; ring++){
      for (int pixel = 0; pixel < nPixelsPerRing[ring]; pixel++){
        float r = constrain_float(leds[thisPixel].r*mult,0,255);
        float g = constrain_float(leds[thisPixel].g*mult,0,255);
        float b = constrain_float(leds[thisPixel].b*mult,0,255);

        int color = (((int)(r))<<16) | ((int)(g)<<8) | ((int)(b));
        //int color = (((int)(leds[thisPixel].r))<<16) | (((int)(leds[thisPixel].g))<<8)| (((int)(leds[thisPixel].b)));
        oleds.setPixel(pixel+ring*nMaxPixels, color);
        thisPixel++;

        total+= r+g+b;
      }
      max +=nPixelsPerRing[ring];
    }
    //we want the total brightness > N*max always
    float desired = 0.1*765.0*max*art.global_intensity.getBase();
    //make mult IIR and constrain from 1 to high
      float ratio=0;
    if (state ==0) {

      ratio=(total-desired)/desired;
      multiir = ratio*.015 + multiir*.985;
      multiir = constrain_float(multiir,-5.0,5.0);
      mult = 1;
      //animation.low_limit_offset = constrain_float(multiir,-2,.3);
    }
    else {
      multiir = mult;
    }
    //if(state ==2) animation.low_limit_offset =0;
    
    //mult = constrain_float(mult,1.0,10000.0);
    EVERY_N_MILLIS(50){
      if(verbose2){
      Serial.print("ratiodot:"); Serial.print(ratio);Serial.print(",");
      Serial.print("low_limit_offset:"); Serial.print(animation.low_limit_offset);Serial.print(",");
      Serial.print("mult:"); Serial.print(mult);Serial.print(",");
      Serial.print("multiir:"); Serial.print(multiir);
      Serial.println();
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


#if ART_BF
  art.global_intensity.setMinMax(0.7, 0.9);//MIN MUST be >0// MAX MUST be <=1

#elif ART_WAG
  art.global_intensity.setMinMax(0.2, 0.6);//MIN MUST be >0// MAX MUST be <=1

#elif ART_CUBE
  art.global_intensity.setMinMax(0.5, 0.9);//MIN MUST be >0// MAX MUST be <=1
  digitalWrite(13,1);

#elif ART_WALL
  art.global_intensity.setMinMax(0.4, 0.9);//MIN MUST be >0// MAX MUST be <=1
  digitalWrite(13,1);

#elif ART_VEST
  art.global_intensity.setMinMax(0.4, 1.0);//MIN MUST be >0// MAX MUST be <=1

#else
  art.global_intensity.setMinMax(0.4, 0.8);//MIN MUST be >0// MAX MUST be <=1

#endif
  art.global_intensity.setBaseToMiddle();
  art.global_scale_x.setBaseToMiddle();
  art.global_scale_y.setBaseToMiddle();
  art.global_scale_z.setBaseToMiddle();
  art.gHue.setBaseToMiddle();

#if ART_TEENSY
  //randomSeed(max(analogRead(A1),1));
#else
  randomSeed(analogRead(A0));
  //FastLED.addLeds<APA102, 7, 14, BGR, DATA_RATE_MHZ(8)>(leds, NUM_LED);   
  FastLED.addLeds<WS2811, LED_PIN, GRB>(leds, NUM_LEDS);
  //FastLED.setMaxPowerInVoltsAndMilliamps( 5, 2000); // optional current limiting [5V, 2000mA]  todo
  FastLED.setBrightness(255); // this is OVERWRITTEN!!!!!! see art.global_intensity  //todo set to 255 in final production???
#endif

  if ((play and doRandom) or playAll){
    randomPattern();
    randomMusicMod();
  }

  //art.setGlobalScale(0.5); 

#if USE_AUDIO
#if ART_TEENSY
  fft256_1.averageTogether(2); //runs at 300Hz+, lets slow that down to ~ 200 hz
  AudioMemory(50);
  //filter1.frequency(30); // filter out DC & extremely low frequencies
  amp1.gain(20);        // amplify sign to useful range
  audio.beat_multiplier_min = 1.9;
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
      //randomPattern();
      state = 2;
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
  //assessVdiv();
#endif

  //check for all black animations, if so change the pattern
  darkWadCheck();

  art.markEndOfShow();
}


unsigned long ttime = 0;
void addLife(){


  ///////////////////////////////////////////////////////
  //add free runing patterns, hues, music reactivity, and colors
  if (playAll){
    //change pattern
    int chg = 90;
    EVERY_N_SECONDS(chg) {
      state =-1;
    }

    switch(state){
      case 0:
        break;
      case -1:
        ttime = millis();
        state = 1;
      case 1:
        mult-= (float(millis()-ttime)/300.0);
        ttime = millis();
        if (mult<=0) {
          state = 2;
          mult=0;
        }
        break;
      case 2:
        randomPattern();
        mult=0;
        incrementPalette();
        randomMusicMod();
        animation.low_limit_offset=0;
        ttime = millis();
        state = 3;
        break;
      case 3:
        mult+=(float(millis()-ttime)/2000.0);
        ttime= millis();
        if (mult >= 1) {
          state = 0;
          mult=1;
        }
        break;
      default:
        state=0;
        break;
    }



    //change hue shift
    art.gHue += .0001;
  ///////////////////////////////////////////////////////
  //add only user specified modulation
  } else {
    if (hueDrift) art.gHue += .0001; // todo make this scale with FPS, put into show current patter to make immune to FPS changes
    state =0;
    //changing paterns
    if (play){
        if (doRandom) {EVERY_N_SECONDS(45) randomPattern();}
        else {EVERY_N_SECONDS(45) incrementPattern();}
    }
  }

  
#if ART_TEENSY
  if (doModulation){
    const static int prob = 100;
    const static int spd_low = 50;
    const static int spd_high = 200;
    float val, speed;

    //Change X / Y / Z centers
    /*    */


    //EVERY_N_MILLIS(62542) {
    if (art.center_xm.isIdle()){
      val = float(random(-prob,prob))/float(prob);
      speed = float(random(spd_low,spd_high))/float(100);
      art.center_xm.trigger(val,speed);
      Serial.println(val);
    }
    art.center_xm.update();
    
    
    //EVERY_N_MILLIS(42833){
    if (art.center_ym.isIdle()){
      val = float(random(-prob,prob))/float(prob);
      speed = float(random(spd_low,spd_high))/float(100);
      art.center_ym.trigger(val, speed);
    }
    art.center_ym.update();


    //EVERY_N_MILLIS(74489){
    if (art.center_zm.isIdle()){
      val = float(random(-prob,prob))/float(prob);
      speed = float(random(spd_low,spd_high))/float(100);
      art.center_zm.trigger(val, speed);
    }
    art.center_zm.update();
    
    art.re_render_spherical_lookup_table();

    //base is set in animation, can only change envelope
    animation.low_limit.trigger(move.noise_range[10]/2);
    animation.low_limit.update();

    animation.high_limit.trigger(move.noise_range[11]/2);    //animation.high_limit.trigger( move.sine[11]);
    animation.high_limit.update();


    //envelope used in audio, only change base
    //todo this might be waaaaay too chaotic, test slowly, perhaps add switches and modes? calm things down?
    art.global_scale_x = art.global_scale_x.getMiddle() + art.global_scale_x.getQuarterSpread() * (move.noise_range[9]);
    art.global_scale_y = art.global_scale_y.getMiddle() + art.global_scale_y.getQuarterSpread() * (move.noise_range[10]*move.noise_range[11]);
    art.global_scale_z = art.global_scale_z.getMiddle() + art.global_scale_z.getQuarterSpread() * (move.noise_range[10] + move.noise_range[11]);
    art.global_intensity = art.global_intensity.getMiddle() + art.global_intensity.getQuarterSpread() * (move.noise_range[9] * move.noise_range[10] - move.noise_range[11] );;
    art.global_bpm = 80 + art.global_bpm.getQuarterSpread() * absf(move.noise_range[9] * move.noise_range[10]);

    EVERY_N_MILLIS(50){
      if(verbose2){
        //Serial.print("center_xme:"); Serial.print(art.center_xm.envelope.getMax()); Serial.print(",");
        //Serial.println();


        //Serial.print("roll:"); Serial.print(art.roll); Serial.print(",");
        //Serial.print("pitch:"); Serial.print(art.pitch);Serial.print(",");

        //Serial.print("center_xm:"); Serial.print(art.center_zm); Serial.print(",");
        //Serial.print("center_ym:"); Serial.print(art.center_zm);Serial.print(",");
        //Serial.print("center_zm:"); Serial.print(art.center_zm);Serial.print(",");

        //Serial.print("global_scale_x:"); Serial.print(art.global_scale_x); Serial.print(",");
        //Serial.print("global_scale_y:"); Serial.print(art.global_scale_y);Serial.print(",");
        //Serial.print("global_scale_z:"); Serial.print(art.global_scale_z);Serial.print(",");

        //Serial.print("globalbpm:"); Serial.print(art.global_bpm);Serial.print(",");
       // Serial.print("highlimit:"); Serial.print(animation.high_limit);Serial.print(",");
        //Serial.print("lowlimit:"); Serial.print(animation.low_limit);Serial.print(",");

        //Serial.println();
      } 
    }
  }
#endif

}



void updateIMU(){


#if USE_IMU
  imu.update();
  art.upVector = imu.filteredPosition;

#elif ART_TEENSY
  const static int prob = 100;
  const static int spd_low = 50;
  const static int spd_high = 250;
  float val, speed;

  //todo test these, honetly might be cool to increase the envelope min max and or add more modulation
  if (art.roll.isIdle()){
    val = float(random(-prob,prob))/float(prob);
    speed = float(random(spd_low,spd_high))/float(100);

    art.roll.trigger(val,speed);

  }
  art.roll.update();


  if (art.pitch.isIdle()){
    val = float(random(-prob,prob))/float(prob);
    speed = float(random(spd_low,spd_high))/float(100);
    art.pitch.trigger(val,speed);
  }  
  art.pitch.update();


#else

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
        audioModBeatDestPtr->trigger(audio.ratio*.75);
        art.global_intensity.trigger(audio.ratio);
      } // beat
    } // fft available
#else
  //audio.update();
  if (audio.beat_detected_poll && musicReactive) {
    Serial.print(audio.ratio_poll);Serial.println(" beat");
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
#if ART_TEENSY
    } else if (incomingByte == 'x'){
      batteryChargedOverride = not batteryChargedOverride;
      Serial.print("Setting batteryChargedOverride shift to"); Serial.println(batteryChargedOverride);
#endif
    } else if (incomingByte == 'm'){
      musicReactive = not musicReactive;
      Serial.print("Setting musicReactive shift to"); Serial.println(musicReactive);
    } else if (incomingByte == 'b'){
      incrementPattern(-1); 
    } else if (incomingByte == 'X'){
      doDarkCheck = not doDarkCheck;
      Serial.print("Setting doDarkCheck to"); Serial.println(doDarkCheck);
    } else if (incomingByte == 'O'){
      audio.verbose = not audio.verbose;
      Serial.print("Setting audio.verbose shift to"); Serial.println(audio.verbose);
    } else if (incomingByte == '['){
      audio.verbose2 = not audio.verbose2;
      Serial.print("Setting audio.verbose2 shift to"); Serial.println(audio.verbose2);
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
      Serial.print("Setting hue shift to"); Serial.println(art.gHue);
    } else if (incomingByte == 'H'){
      art.gHue = 0;
      Serial.print("Setting hue shift to"); Serial.println(art.gHue);
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
