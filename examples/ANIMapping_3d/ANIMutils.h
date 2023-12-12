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
const float PI4 = 4*PI;
const float PI8 = 8*PI;
const float PIOVER2 = PI/2;
const float PIOVER2T3 = 3*PI/2;

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

typedef enum {
  envTriangle,
  envExponential,
  envSine,
  envConst
} envelopShape;

typedef enum {
  stateIdle,
  stateAttack,
  stateDecay,  
} envelopState;

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

/*
float max(float a, float b){
  if (a>b) return a;
  else return b;
}
*/




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
  hAdj = hsi.h * PI2;    // convert h to radians
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

#if ART_TEENSY
  float gamma = 1.01;
  // Step 3: Apply Gamma correction
  r = powf(r, gamma);
  g = powf(g, gamma);
  b = powf(b, gamma);
#endif

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


  //nsides 2, side_distance = PI
  //if my bound is 1, side_distance = PI, ref center = PI

  //nsides 3, side_distance = 2PI/3, bound dist = 4PI/3
  //if my bound is 1, ref center = 1*4PI/3 - 2PI/3 = 2PI/3 = 2/3  PI
  //if my bound is 2, ref center = 2*4PI/3 - 2PI/3 = 6PI/3 = 2PI  , odd sides dont really work...

  //nsides 4, side_distance = PI/2, 
  //if my bound is 1, ref center = PI/2
  //if my bound is 2, side_distance = PI/2, ref center = 3PI/2

  //nsides 6, side_distance = PI/3, bound dist = 2PI/3
  //if my bound is 1, ref center = 1*2PI/3 - PI/3 = PI/3
  //if my bound is 2, ref center = 2*2PI/3 - PI/3 = 3PI/3
  //if my bound is 3, ref center = 3*2PI/3 - PI/3 = 5PI/3

  float reflection_center = mybound*(single_reflection_distance) - side_distance;

  //the abs value always returns positive distance from reflection center
  //result is a triangle wave starts at side_distance when x=start_at, 
  //result=0 when x=start_at+side_distance, 
  //then result increases back up to side_distance as x increases to start_at+2*side_distance
  return (absf(reflection_center - x));
}




//! evelope class
class envelopeF {  
public:
  envelopShape shape = envExponential;
  bool verbose = false;  
  bool isLfo = false;
  
private:

  bool wasLfo = false;
  float low = 0.0;
  float high = 1.0;

  float attackBaseMillis = 400.0; // in millis
  float decayBaseMillis = 400.0; // in millis
  float attackMillis = 400.0; // in millis
  float decayMillis = 400.0; // in millis
  unsigned long start_millis = 0;
  float elapsedMillis = 200;

  float signal = 0.0;//!this is the base input signal
  float start_signal = 0.0;
  int state = stateIdle;

  float theta = 0;
  float speedMultiplier = 1.0;


  



  void updateState(){
    if (state == stateAttack){ 
      elapsedMillis = millis() - start_millis;
      if (elapsedMillis > attackMillis) state = stateDecay;

    } else if (state == stateDecay) {
      elapsedMillis = millis() - start_millis - attackMillis;
      if (elapsedMillis > decayMillis) {
        //repeat
        if (isLfo) trigger();
        //done go to idle
        else {
          state = stateIdle;
          isLfo = wasLfo;
        }
      }

    } else {
      //dont care: idle = stay idle
    }
  }

  void updateTriangle(){
    if (state == stateAttack){ 
      // attack phase, signal varies from start_signal to high while elapsedMillis <= attackMillis
      //float x = elapsedMillis;
      //float m = (high-start_signal)/attackMillis;//rise/ run
      //float b = start_signal;
      //y = mx +b
      signal = min(elapsedMillis/attackMillis * (high-start_signal)+start_signal, high); 
    } else if (state == stateDecay) {
      // decay phase, signal varies from high to low while elapsedMillis > attackMillis
      signal = max(elapsedMillis/decayMillis * (low-high) + high, low);
    } else {
      signal = low;
      state = stateIdle;
    }
  }

  void updateExponential(){
    if (state == stateAttack){ 
      // attack phase, signal varies from start_signal to high while elapsedMillis <= attackMillis
      float tau = attackMillis / 5;
      signal = start_signal + (high-start_signal) * (1 - exp(-elapsedMillis/tau));
    } else if (state == stateDecay) {
      // decay phase, signal varies from high to low while elapsedMillis > attackMillis
      float tau = decayMillis / 5;
      signal = high + (low-high) * (1 - exp(-elapsedMillis/tau));
    } else {
      signal = low;
    }
  }

  void updateSine(){
    if (state == stateAttack){
      if (isLfo) theta =  PIOVER2 * elapsedMillis / attackMillis;
      else theta = min(PIOVER2, PIOVER2 * elapsedMillis / attackMillis);
      //dc offset for start signal
      //amplitude high-start signal
      //phase shift 0
      //total angular distance in attack PI/2
      signal = start_signal + (high-start_signal) * sinf(theta);

    } else if (state == stateDecay){
      if (isLfo) theta = PIOVER2 +  PIOVER2T3 * elapsedMillis / (decayMillis);
      //dc offset for low
      //amplitude high-low
      //phase shift PI/2
      //max phase 2 PI
      //total angular distance in decay 3PI/2
      else theta = min(PI2, PIOVER2 + PIOVER2T3 * elapsedMillis / (decayMillis));
      signal = low + (high-low) * sinf(theta);

    } else {
      signal = low;
      theta = 0;
    }
  }

  void updateConst(){
    if (state == stateAttack or state == stateDecay){
      signal = high;
      //EVERY_N_MILLIS(100){ Serial.print("const set to");Serial.println(signal);}

    } else {
      signal = low;
      theta = 0;
    }
  }
public:

  //getters / const functions///////////////////////////
  void debug(){        

      Serial.print("elapsedSeconds:");
      Serial.print(elapsedMillis/1000);
      Serial.print(",");
      Serial.print("state:");
      Serial.print(state);
      Serial.print(",");
      Serial.print("signal:");
      Serial.print(signal);
      Serial.print(",");
      Serial.print("start_signal:");
      Serial.print(start_signal);
      Serial.print(",");
      Serial.print("high:");
      Serial.print(high);
      Serial.print(",");
      Serial.print("low:");
      Serial.flush();
      Serial.print(low);
      Serial.print(",");
      Serial.print("theta:");
      Serial.print(theta);
      Serial.print(",");
      Serial.print("wasLfo:");
      Serial.print(wasLfo);
      Serial.print(",");
      Serial.print("isLfo:");
      Serial.print(isLfo);
      Serial.print(",");

      Serial.print("ref:");
      Serial.print(3);      
      Serial.println();

  }



  float getSignal() const {
    return signal;
  }

  bool isIdle() const {
    return (state == stateIdle);
  }

  float getMax() const {
    return high;
  }


  // setters / utility functions///////////////////////////
  void clear(){
    state = stateIdle;
    signal = 0.0;
  }

  void setAttackDecay(float aMillis, float dMillis){
     float m = 0.0;
     attackBaseMillis = max(aMillis, m);
     decayBaseMillis = max(dMillis, m);
  }

  void setMax(float m){
     high=m;
     //if( high < low) Serial.println("dude, max should probably be > low in evelope setMax");
     //Serial.println(high);
  }


  //! start a new envelop
  void trigger(float speed=1.0){
    start_millis = millis();
    start_signal = signal;
    state = stateAttack;
    wasLfo = isLfo;
    speedMultiplier = max(speed,0.);
    attackMillis = attackBaseMillis / speedMultiplier;// higher speed = shorter attack decay
    decayMillis = decayBaseMillis / speedMultiplier;
    //update();
  }

  void update(){
    updateState();
    //todo make a switch for modes triangle, exponential, etc
    if (shape == envTriangle ) updateTriangle();
    else if (shape == envExponential ) updateExponential();
    else if (shape == envSine ) updateSine();
    else if (shape == envConst ) updateConst();

    EVERY_N_MILLIS(50) if (verbose) debug();
  }

  void stopLfoNextCylce(){
    wasLfo = isLfo;
    isLfo = false;
  }



};





//! moddable class has 2 control signals, high and low, these control VALUE output clipPIng and BASE level
//2 input signals BASE and MOD.  BASE is constrained to remain in the bounds of high and low, MOD is not constrained
//1 output signal VALUE, value = BASE + MOD, but is clipped to the bounds of high and low
class modableF {  
private:
  float low = 0.0;
  float high = 1.0;
  //these are really just used for convenience and are always a function of low and high
  float middle = 0.5; // low + (high-low)/2
  float spread = 1.0; // high-low
  float halfSpread = 0.5; // (high-low)/2
  float quarterSpread = 0.25; // (high-low)/4

  float base = 0.0;//!this is the base input signal




 
public:
  modEdge edge = edgeClip;
  envelopeF envelope;
  //getters///////////////////////////
  float getBase() const{return base;}
  float getEnvelope() const{return clip(base+envelope.getSignal());}
  float getOffset() const{return base - low;}
  float getSpread() const { return spread;}
  float getHalfSpread() const { return halfSpread;}
  float getQuarterSpread() const { return quarterSpread;}
  float getMiddle() const { return middle;}
  float getSpace() const { return high-(base+envelope.getSignal());}
  float getNegSpace() const { return (envelope.getSignal()+base)-low;}

  float isIdle() const {return envelope.isIdle();}
  //utility functions///////////////////////////
  float clip(float v) const{
    if (edge == edgeClip) v =  constrain_float(v, low, high);//edgeClip = clipped like an op-amp hitting the rails
    else if (edge == edgeWrap) v = (fmodf(v - low, high - low) + low); //edgeWrap = sawtooth
    else v = mirror_float(v,high-low, 2, low); //edgeMirror = triangle //float mirror_float(float x, float side_distance, int n_sides=2, float start_at = 0.0){
    return v;
  }


  //! input: value number ideally from -1/1 that we use to set envelope maximum proportional to the space we 
  void trigger(float value, float speed = 1.0){
      constrain_float(value,-1,1);
      if( edge == edgeClip ) {
        if (value > 0) envelope.setMax( getSpace() * value); // if we are clipPIng, get the ammount of space from max-base we have left, make env proportional to that
        else envelope.setMax( getNegSpace() * value); // if we are clipPIng, get the ammount of space from max-base we have left, make env proportional to that
      }
      else {
        base += envelope.getSignal();
        envelope.clear();
        envelope.setMax( getSpread() *value);// not clipPIng, get the total spread max-min, make env proportional to that
      }
      envelope.trigger(speed);
  }


  //! update the envelope
  void update(){
    envelope.update();
  }


  //SETTERS///////////////////////////
  void setMinMax(float mn, float mx) {
    low = mn;
    high = mx;
    spread = high - low;
    halfSpread = spread / 2;
    quarterSpread = halfSpread / 2;
    middle = low + halfSpread;
    if (high <= low) Serial.println("dude, max should probably be > min");
    base = clip(base); //just in case user set min max out of range of where base was
  }

  void setBaseToMiddle(){
    base = middle;
  }

  void setBaseToMin(){
    base = low;
  }

  //! operator= sets the BASE, and value, keeps mod the same...  follows edge rules for setting base and value
  modableF& operator=(const float b) {
    base = clip(b);
    return *this;
  }

  //! operator+= sets the BASE, and value, keeps mod the same...  follows edge rules for setting base and value
  modableF& operator+=(const float b) {
    base = clip(base + b);
    return *this;
  }

  operator float() const { return getEnvelope(); }

};



////ARITHMATIC OPERATIONS///////////////////////////
//! operator* [float = float *modable] returns float x value, does NOT follow edge rules
float operator*(const float &a, const modableF &b){
  return (b.getEnvelope())*a;
}

//! operator+ [float = float + modable] returns float x, does follow edge rules
float operator+(const float &a, const modableF &b){
  return b.clip(b.getEnvelope()+a);
}

float operator<(const float &a, const modableF &b){
  return (a<b.getEnvelope());
}
float operator>(const float &a, const modableF &b){
  return (a>b.getEnvelope());
}



