/*
  ___        _            ___  ______ _____    _      
 / _ \      (_)          / _ \ | ___ \_   _|  (_)     
/ /_\ \_ __  _ _ __ ___ / /_\ \| |_/ / | |_ __ ___  __
|  _  | '_ \| | '_ ` _ \|  _  ||    /  | | '__| \ \/ /
| | | | | | | | | | | | | | | || |\ \  | | |  | |>  < 
\_| |_/_| |_|_|_| |_| |_\_| |_/\_| \_| \_/_|  |_/_/\_\

by Stefan Petrick 2023.
modified by Clark Reinholtz, forked from Stefan Petrick and netmindz
to allow 3d pixel mapping and spherical coordinates

High quality LED animations for your next project.

This is a Shader and 5D Coordinate Mapper made for realtime 
rendering of generative animations & artistic dynamic visuals.

This is also a modular animation synthesizer with waveform 
generators, oscillators, filters, modulators, noise generators, 
compressors... and much more.

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/
#pragma once
#include <FastLED.h>
#include "ANIMutils.h"


////////////////////////////////////////////////////////////////////////
//My Palette Choices
////////////////////////////////////////////////////////////////////////
// gradient palette explanation here https://github.com/FastLED/FastLED/wiki/Gradient-color-palettes
//picking colors here https://www.peko-step.com/en/tool/hsvrgb_en.html

//LAST INDEX MUST BE 255!!!!!!!

////////////////////////////////////////////////////////////////////////
//random
DEFINE_GRADIENT_PALETTE( blueie_gp ) {
  0,    0,    0,    0, //black
  80,  66,   255,  198, //aqua
  160,  118,  192,  196, //saturated purple
  255,  255,   100,  100 //redish saturated
  }; 


DEFINE_GRADIENT_PALETTE( gator_gp ) {
  0,    0,    0,    0, //black
  80,  255,   76,  0, //orange
  160,  255,   0,  187, //pink
  255,  203,  0,  255 }; //deep purple
//CRGBPalette16 gator_gp = gator_gpd;


DEFINE_GRADIENT_PALETTE( scuba_gp ) {
  0,    0,    0,    0, //black
  60,  0,   130,  211, //light blue
  120,  221,   90,  162, //magenta
  180,  255,   163,  175, //salmon
  255,  243,  255,  252 }; //sat yellow / white
//CRGBPalette16 scuba_gp = scuba_gpd;


//yellow
//pink
//blue

///////////////////////////////////////////////////////////////////////
//traid //https://color.adobe.com/create/color-wheel
DEFINE_GRADIENT_PALETTE( tmg_gp ) {
  0,    0,    0,    0, //black
  80,36,224, 219,// teal
  160,224,13,180, //magenta
  255,230,187,0 //gold
};

DEFINE_GRADIENT_PALETTE( gop_gp ) {
  0,    0,    0,    0, //black
80,22,224,89,//green
160,224,135,0,//orange
255,123,22,224//purple
};

///////////////////////////////////////////////////////////////////////
//split complimentary //https://color.adobe.com/create/color-wheel
DEFINE_GRADIENT_PALETTE( bpg_gp ) {
  0,    0,    0,    0, //black
80,11,106,224,//blue
160,146,11,224,//purple
255,224,190,1//gold
};
DEFINE_GRADIENT_PALETTE( grb_gp ) {
  0,    0,    0,    0, //black
80,11,224,109,//green
160,11,107,224,//blue
255,225,85,0//red
};
///////////////////////////////////////////////////////////////////////
//square //https://color.adobe.com/create/color-wheel
DEFINE_GRADIENT_PALETTE( pbgg_gp ) {
  0,    0,    0,    0, //black
60,224,22,136,//pink
120,0,86,224,//blue
180,20,224,11,//green 
255,224,158,22//gold
};
DEFINE_GRADIENT_PALETTE( bggo_gp ) {
  0,    0,    0,    0, //black
60,19,0,224,//blue
120,11,224,110,//green
180,224,194,29,//gold
255,224,41,22//burnt orange
};



///////////////////////////////////////////////////////////////////////
//double ups

DEFINE_GRADIENT_PALETTE( grpdouble_gp ) {
  0,    0,    0,    0, //black
  40,     9,229,0,//green
  80,189,0,230,//purple
  120,     9,229,0,//green
  160,    0,    0,    0, //black
  200,     9,229,0,//green
  230,189,0,230,//purple
  255,230,30,21//red
};


DEFINE_GRADIENT_PALETTE( rgpdouble_gp ) {
  0,    0,    0,    0, //black
  40,230,0,0,//red
  80,     9,229,20,//green
  120,    0,    0,    0, //black
  200,     9,229,0,//green
  230,189,0,230,//purple
  255,230,30,21//red
};

DEFINE_GRADIENT_PALETTE( chaosdouble_gp ) {
  0,    0,    0,    0, //black
  60,255,0,255,//redblue
  100,     0,0,255,//blue
  140,     0,255,255,//greenblue
  160,    0,    0,    0, //black
  200,     255,255,0,//greenred
  255,230,30,21//red
};



///////////////////////////////////////////////////////////////////////
//two color
//peach ffa781
//maroon 5b0e2d





////////////////////////////////////////////////////////////////////////
//List of Palette
////////////////////////////////////////////////////////////////////////
typedef struct {
  CRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];

PaletteAndNameList gPalettes = {  
  {chaosdouble_gp,"chaosdouble_gp"},
  {rgpdouble_gp,"rgpdouble_gp"},
  {grpdouble_gp,"grpdouble_gp"},
  {blueie_gp, "blueie_gp"},
  {gator_gp, "gator_gp"},
  {tmg_gp, "tmg_gp"},
  {gop_gp, "gop_gp"},
  {bpg_gp, "bpg_gp"},
  {grb_gp, "grb_gp"},
  {pbgg_gp, "pbgg_gp"},
  {bggo_gp, "bggo_gp"}
};//customs




////////////////////////////////////////////////////////////////////////
//Navigation of list
////////////////////////////////////////////////////////////////////////
const int gCustomPaletteCount = ARRAY_SIZE(gPalettes);
const int gTotalPaletteCount = gCustomPaletteCount + 8;//this is the number of cases in the switch below
int currentPaletteIndex = 0;
CRGBPalette16 currentPalette  = gPalettes[0].palette;



void sanityCheckP(){
  if (currentPaletteIndex >= gTotalPaletteCount) currentPaletteIndex = 0;
  if (currentPaletteIndex < 0) currentPaletteIndex = gTotalPaletteCount-1;
}

void setCurrentPalette(){
  sanityCheckP();
  String s = "";
  if (currentPaletteIndex < gCustomPaletteCount) {
    currentPalette = gPalettes[currentPaletteIndex].palette;
    s = gPalettes[currentPaletteIndex].name;
  }
  else {
    switch (currentPaletteIndex-gCustomPaletteCount){
      case 0:
        currentPalette = LavaColors_p;
        s = "LavaColors_p";
        break;
      case 1:
        currentPalette = ForestColors_p;
        s = "ForestColors_p";
        break;
      case 2:
        currentPalette = RainbowColors_p;
        s = "RainbowColors_p";
        break;
      case 3:
        currentPalette = RainbowStripeColors_p;
        s = "RainbowStripeColors_p";
        break;
      case 4:
        currentPalette = PartyColors_p;
        s = "PartyColors_p";
        break;
      case 5:
        currentPalette = HeatColors_p;
        s = "HeatColors_p";
        break;
      case 6:
        currentPalette = CloudColors_p;
        s = "CloudColors_p";
        break;
      case 7:
        currentPalette = CloudColors_p;
        s = "OceanColors_p";
        break;
      //update gTotalPaletteCount before adding more
      default:
        currentPalette = CloudColors_p;
        s = "sanity check failed using CloudColors_p";
        break;
    }
  }
  Serial.print(currentPaletteIndex); Serial.print(": Setting pallet to: "); Serial.println(s); 
}


void incrementPalette(int inc = 1){
  currentPaletteIndex = currentPaletteIndex + inc;
  setCurrentPalette();
}

void randomPalette(){
  currentPaletteIndex = random(gTotalPaletteCount);
  setCurrentPalette();
}



