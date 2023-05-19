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

#define NUM_LEDS  50                       // how many LED total,  must be defined before ANIMapping

#include <FastLED.h>
#include "ANIMapping.h" //TODO make <> when you copy files back to the right directory


//******************************************************************************************************************

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
    { 4.5, 0, 4}
    
};




//******************************************************************************************************************

CRGB leds[NUM_LEDS];               // framebuffer
ANIMapping art( leds, 0.50);  //led buffer, global scale




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
void Caleido3(){art.Caleido3();}
void Caleido2(){art.Caleido2();}
void Caleido1(){art.Caleido1();}
void Lava1(){art.Lava1();}
void Distance_Experiment(){art.Distance_Experiment();}
void Center_Field(){art.Center_Field();}
void Waves(){art.Waves();}
void Chasing_Spirals(){art.Chasing_Spirals();}
void Rotating_Blob(){art.Rotating_Blob();}

PatternAndNameList gPatterns = {
  /*
  {Module_Experiment10,"Module_Experiment10"},
  {Module_Experiment9,"Module_Experiment9"},
  {Module_Experiment8,"Module_Experiment8"},
  {Module_Experiment7,"Module_Experiment7"},
  {Module_Experiment6,"Module_Experiment6"},
  {Module_Experiment5,"Module_Experiment5"},
  {Module_Experiment4,"Module_Experiment4"},
  {Module_Experiment3,"Module_Experiment3"},
  {Module_Experiment2,"Module_Experiment2"},
  {Module_Experiment1,"Module_Experiment1"},
  {Parametric_Water,"Parametric_Water"},
  {Water,"Water"},
  */
  {Complex_Kaleido_6,"Complex_Kaleido_6"},
  {Complex_Kaleido_5,"Complex_Kaleido_5"},
  {Complex_Kaleido_4,"Complex_Kaleido_4"},
  {Complex_Kaleido_3, "Complex_Kaleido_3"},
  {Complex_Kaleido_2,"Complex_Kaleido_2"},
  {Complex_Kaleido, "Complex_Kaleido"},
  /*
  {SM10,"SM10"},
  {SM9,"SM9"},
  {SM8,"SM8"},
  // {SM7,"SM7"},
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
  {Scaledemo1,"Scaledemo1"},
  {Lava1,"Lava1"},
  */
  {Caleido3,"Caleido3"},
  {Caleido2,"Caleido2"},
  {Caleido1,"Caleido1"}, 
  /*
  {Distance_Experiment,"Distance_Experiment"},
  {Center_Field,"Center_Field"},
  {Waves,"Waves"},
  {Chasing_Spirals,"Chasing_Spirals"},
  {Rotating_Blob,"Rotating_Blob"},*/
};




int gPatternCount = ARRAY_SIZE(gPatterns);

void incrementPattern(){
  currentPattern = currentPattern + 1;
  if (currentPattern >= gPatternCount) currentPattern = 0;

  Serial.print("Incrementing pattern to: "); Serial.print(currentPattern); Serial.print(" "); Serial.println(gPatterns[currentPattern].name);
}
void showCurrentPattern(){
  gPatterns[currentPattern].pattern();
  FastLED.show();
}

//******************************************************************************************************************


void setup() {
  
  // FastLED.addLeds<NEOPIXEL, 13>(leds, NUM_LED);
  
  //FastLED.addLeds<APA102, 7, 14, BGR, DATA_RATE_MHZ(8)>(leds, NUM_LED);   
  FastLED.addLeds<WS2813, 2, GRB>(leds, NUM_LEDS);
  FastLED.setMaxPowerInVoltsAndMilliamps( 5, 2000); // optional current limiting [5V, 2000mA] 
  FastLED.setBrightness(128);
  Serial.begin(115200);                 // check serial monitor for current fps count
 
 // fill_rainbow(leds, NUM_LED, 0);
  //fill_solid(leds, NUM_LED, CRGB::Green);
  //FastLED.show();
}


//*******************************************************************************************************************

void loop() {


  showCurrentPattern();


  EVERY_N_SECONDS(10)  incrementPattern();
  //EVERY_N_MILLIS(500) art.report_performance();   // check serial monitor for report 
} 
