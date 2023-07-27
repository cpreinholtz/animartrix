/*
  ___        _            
 / _ \      (_)
/ /_\ \_ __  _ _ __ ___
|  _  | '_ \| | '_ ` _ \
| | | | | | | | | | | | |
\_| |_/_| |_|_|_| |_| |_

by Clark Reinholtz, 
HSI library, float operations, etc

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

#pragma once

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))
const float PI2 = 2*PI;


////////////////////////////////////////////////////////////////////////////
// Data structures
////////////////////////////////////////////////////////////////////////////

//! Struct used to represent HSI colors using floats.  h, s, i value ranges from 0 - 1. 
struct hsiF {  
  float h;
  float s;
  float i;
};


//! Struct used to represent RGB colors using floats. r, b, g  value ranges from 0 - 1. 
struct rgbF {  
  float r;
  float g;
  float b;
};


typedef enum {
  edgeClip,
  edgeWrap,
  edgeMirror
} modEdge;


////////////////////////////////////////////////////////////////////////////
// floating point utilities
////////////////////////////////////////////////////////////////////////////
//!constrain, includes edges
float constrain_float(float &x, float out_min, float out_max){
  if (x < out_min) x = out_min;
  if( x > out_max) x = out_max;
  return x;
}


//helper functions todo move to a new place?
float map_float(float x, float in_min, float in_max, float out_min, float out_max) { 
  
  float result = (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
  result = constrain_float(result,out_min,out_max);
  return result; 
}


//abs value for floats
float absf(float x) { 
  if (x<0) return -x;
  else return x;
}





////////////////////////////////////////////////////////////////////////////
// data conversions
////////////////////////////////////////////////////////////////////////////

//this function rightfully belongs to AeroKeith, I modified only slightly to remove gamma correction and intensity scaling
rgbF Hsi2Rgb(hsiF hsi) {
  const float rad60 = 1.0472;     // 60 degrees in radians
  const float rad120 = 2.0944;    // 120 degrees in radians
  const float rad240 = 4.1888;    // 240 degrees in radians
  float hAdj;   // hue (0 - 1) converted to radians ( 0 - 2*PI) and adjusted to range 0 - rad120
  float r, g, b;   // temps for computed return value

  hsi.h = constrain_float(hsi.h, 0, 1);   // ensure that all components of hsi are in range 0-1
  hsi.s = constrain_float(hsi.s, 0, 1);
  hsi.i = constrain_float(hsi.i, 0, 255);

    // Step 1: Compute RGB assuming full intensity (I == 1)
  hAdj = hsi.h * 2 * PI;    // convert h to radians
  if (hAdj <= rad120) {   // if h <= 120 degrees

    r = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    b = (1 - hsi.s) / 3;
    g = 1 - (r + b);
    
  }
  else if (hAdj <= rad240) {    // if (120 < h <= 240) degrees
    hAdj -= rad120;   // adjust down to range 0 - rad120
    g = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    r = (1 - hsi.s) / 3;
    b = 1 - (r + g);
    
  }
  else {    // if (240 < h <= 360) degrees
    hAdj -= rad240;   // adjust down to range 0 - rad120
    b = (1 + (hsi.s * cos(hAdj))/cos(rad60 - hAdj)) / 3;
    g = (1 - hsi.s) / 3;
    r = 1 - (b + g);
  }

  // Step 2: Scale RGB values by intensity I
  r *= hsi.i;
  g *= hsi.i;
  b *= hsi.i;

  return {r, g, b};
}



//https://www.had2know.org/technology/hsi-rgb-color-converter-equations.html
hsiF Rgb2Hsi(rgbF rgb)
{
  float h,s,i;
  rgb.r = constrain_float(rgb.r,0,255);
  rgb.b = constrain_float(rgb.b,0,255);
  rgb.g = constrain_float(rgb.g,0,255);
  i = (rgb.r+rgb.g+rgb.b) / 3.0;
  if (i==0.0) {
    s = 0.0;
    h = 0.0;
  }
  else{
    s = 1.0 - (min(rgb.r,min(rgb.g,rgb.b))/i);

    //H = cos-1[ (R - ½G - ½B)/√(R² + G² + B² - RG - RB - GB) ]            if G ≥ B, or
    //H = 360 - cos-1[ (R - ½G - ½B)/√(R² + G² + B² - RG - RB - GB) ]    if B > G,
    h = acosf((rgb.r-.5*(rgb.g+rgb.b))/sqrtf(rgb.r*rgb.r + rgb.g*rgb.g + rgb.b*rgb.b - rgb.r*rgb.g - rgb.r*rgb.b - rgb.g*rgb.b));
    if (rgb.b>rgb.g) h = PI2 - h;
    h=h/PI2;
  }

  return {h, s, i};
}


////////////////////////////////////////////////////////////////////////////
// synthesis
////////////////////////////////////////////////////////////////////////////

/*!mirror_float
 * duplicate x over a number of sides, but flip odd sides so the result is continuous
 * n_sides should be even for smooth reflection
 *
*/
float mirror_float(float x, float side_distance, int n_sides=2, float start_at = 0.0){
  //the total distance of our mirror, if you think about a 2 sided circle with PI as distance, single_reflection_distance is time for full reflection (2 sides)
  float total_distance = side_distance * n_sides;
  float single_reflection_distance = side_distance + side_distance;

  //subtract out offset
  x=absf(x-start_at);

  //make sure x-start_at is within our total mirror distance
  x = fmodf(x,total_distance);


  //now figure out bounds, bounds are 2 side_distance becuase that is a period of semetric reflection
  float upperbound = start_at;
  int mybound = 0;
  while (x>upperbound){
    mybound = mybound+1;
    upperbound = upperbound + single_reflection_distance;
  }


  //nsides 2, side_distance = pi
  //if my bound is 1, side_distance = pi, ref center = pi

  //nsides 3, side_distance = 2pi/3, bound dist = 4pi/3
  //if my bound is 1, ref center = 1*4pi/3 - 2pi/3 = 2pi/3 = 2/3  pi
  //if my bound is 2, ref center = 2*4pi/3 - 2pi/3 = 6pi/3 = 2Pi  , odd sides dont really work...

  //nsides 4, side_distance = pi/2, 
  //if my bound is 1, ref center = pi/2
  //if my bound is 2, side_distance = pi/2, ref center = 3pi/2

  //nsides 6, side_distance = pi/3, bound dist = 2pi/3
  //if my bound is 1, ref center = 1*2pi/3 - pi/3 = pi/3
  //if my bound is 2, ref center = 2*2pi/3 - pi/3 = 3pi/3
  //if my bound is 3, ref center = 3*2pi/3 - pi/3 = 5pi/3

  float reflection_center = mybound*(single_reflection_distance) - side_distance;

  //the abs value always returns positive distance from reflection center
  //result is a triangle wave starts at side_distance when x=start_at, 
  //result=0 when x=start_at+side_distance, 
  //then result increases back up to side_distance as x increases to start_at+2*side_distance
  return (absf(reflection_center - x));
}


//! Struct used to represent RGB colors using floats. r, b, g  value ranges from 0 - 1. 
class modableF {  
public:
  float base = 0.0;
  modEdge edge = edgeClip;
  float min = 0.0;
  float max = 1.0;

  float modulate(float m){
    float result = m + base;
    if (edge == edgeClip) return constrain_float(result, min, max);//edgeClip = clipped like an op-amp hitting the rails
    else if (edge == edgeWrap) return (fmodf(result - min, max - min) + min); //edgeWrap = sawtooth
    else return mirror_float(result,max-min, 2, min); //edgeMirror = triangle //float mirror_float(float x, float side_distance, int n_sides=2, float start_at = 0.0){

  }

  void inc(float m){
    base = modulate(m);
  }


};



