/*
  ___        _            
 / _ \      (_)
/ /_\ \_ __  _ _ __ ___
|  _  | '_ \| | '_ ` _ \
| | | | | | | | | | | | |
\_| |_/_| |_|_|_| |_| |_

by Clark Reinholtz, 
This is the set of functions and objects specific to the wiggle waggle, a 4 foot diameter sphere with ws2815's

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once
#define ART_WALL1BY8 true
#define ART_WALL false
#define ART_TEENSY true

#define USE_IMU false
#define USE_AUDIO true

//******************************************************************************************************************
//python generated
//******************************************************************************************************************

#ifndef xind
  #define xind 0
#endif
#ifndef yind
  #define yind 1
#endif
#ifndef zind
  #define zind 2
#endif


#include "wall1by8.h"



//******************************************************************************************************************
//Octo stuff
//******************************************************************************************************************
#include <OctoWS2811.h>

// Any group of digital pins may be used
const int numPins = 8;

//top jack, bottom jack
//orange, blue, green, brown, orange, blue, green, brown
//defaults {2, 14, 7, 8, 6, 20, 21, 5};
//I USED {2, 14, 7, 8, 6, 20, 21,***9***}; on the waggle because I needed pin 5 for I2S
//byte pinList[numPins] = {2, 14, 7, 8, 6, 20, 21, 5};

//mounting the octo shifter on the teensy 4.1 (dont connect teensy 3.3v to octo pin 15~!!!)
//see blue notebook for explanation
//results in T34 = O14, T33 = O13... 
//orange, blue, green, brown, orange, blue, green, brown
//byte pinList[numPins] = {12,34,27,28,  26,40,41,25};

// wall left is green blue    brown orange
//pipe  left      1      2       3     4

// wall right is  orange brown    blue green
//pipe  right      5      6       7     8



//so order is Lgreen Lblue...
byte pinList[numPins] = {27,34,28,12,  26, 25,40,41};

//const int nMaxPixels = nMaxPixels; // set this to your MAX leds per strip
const int bytesPerLED = 3;  // change to 4 if using RGBW
DMAMEM int displayMemory[nMaxPixels * numPins * bytesPerLED / 4];
int drawingMemory[nMaxPixels * numPins * bytesPerLED / 4];
const int config = WS2811_GRB | WS2811_800kHz;


OctoWS2811 oleds(nMaxPixels, displayMemory, drawingMemory, config, numPins, pinList);

