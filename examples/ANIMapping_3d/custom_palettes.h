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
#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))


////////////////////////////////////////////////////////////////////////
//My Palette Choices
////////////////////////////////////////////////////////////////////////
// gradient palette explanation here https://github.com/FastLED/FastLED/wiki/Gradient-color-palettes
//picking colors here https://www.peko-step.com/en/tool/hsvrgb_en.html


//LAST INDEX MUST BE 255!!!!!!!
DEFINE_GRADIENT_PALETTE( blueie_gp ) {
  0,    0,    0,    0, //black
  32,  66,   255,  198, //aqua
  180,  45,   188,  196, //teal
  255,  118,  192,  196 }; //saturated purple


DEFINE_GRADIENT_PALETTE( gator_gp ) {
  0,    0,    0,    0, //black
  15,  255,   76,  0, //orange
  100,  255,   0,  187, //pink
  255,  203,  0,  255 }; //deep purple
//CRGBPalette16 gator_gp = gator_gpd;


DEFINE_GRADIENT_PALETTE( scuba_gp ) {
  0,    0,    0,    0, //black
  15,  0,   130,  211, //light blue
  99,  221,   90,  162, //magenta
  180,  255,   163,  175, //salmon
  255,  243,  255,  252 }; //sat yellow / white
//CRGBPalette16 scuba_gp = scuba_gpd;



////////////////////////////////////////////////////////////////////////
//List of Palette
////////////////////////////////////////////////////////////////////////
typedef struct {
  CRGBPalette16 palette;
  String name;
} PaletteAndName;
typedef PaletteAndName PaletteAndNameList[];


PaletteAndNameList gPalettes = {
  {blueie_gp, "blueie_gp"},
  {gator_gp, "gator_gp"},
  {scuba_gp, "scuba_gp"}
};
const int gPaletteCount = ARRAY_SIZE(gPalettes);



////////////////////////////////////////////////////////////////////////
//Navigation of list
////////////////////////////////////////////////////////////////////////
int currentPalette = 0;
void incrementPalette(int inc = 1){
  currentPalette = currentPalette + inc;
  if (currentPalette >= gPaletteCount) currentPalette = 0;
  if (currentPalette < 0) currentPalette = currentPalette-1;

  Serial.print("Incrementing currentPalette to: "); Serial.print(currentPalette); Serial.print(" "); Serial.println(gPalettes[currentPalette].name);
}

void randomPalette(){
  currentPalette = random(gPaletteCount);
  //if (currentPalette >= gPaletteCount) currentPalette = 0; // not needed with proper random?
  Serial.print("Setting currentPalette to: "); Serial.print(currentPalette); Serial.print(" "); Serial.println(gPalettes[currentPalette].name);
}

