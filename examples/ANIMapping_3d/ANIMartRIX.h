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
//#include <SmartMatrix.h>
#include <FastLED.h>
#include "custom_palettes.h"
#include "ANIMutils.h"
#include <plane3d.h>
#include <sphere3d.h>
#include "ANIMaudio.h"

#if ART_TEENSY
#define num_oscillators 12
#else
#define num_oscillators 9
#endif

#ifndef NUM_LEDS
  //#define NUM_LEDS 100
#endif

//change the x, y, z index of your map here
// maps are defined as [led][axis] where axis is x y or z position,  
//this just allows you to change which axis number: 0 1 or 2 corrosponds to what variable x, y, or z
//I recomend making X and Y LeftRight and DownUp on the face viewed from your primary viewing angle, and Z being render_spherical_lookup_table
#ifndef xind
  #define xind 0
#endif
#ifndef yind
  #define yind 2
#endif
#ifndef zind
  //#define zind 1
#endif
extern float ledMap[NUM_LEDS][3]; //TODO, make this better...


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



/////////////////////////////////////////////////////////////////////////////


struct render_parameters {


  float dist, angle, anglephi;                
  float scale_x = 0.1;                  // smaller values = zoom in
  float scale_y = 0.1;
  float scale_z = 0.1;       
  float offset_x, offset_y, offset_z;     
#if ART_TEENSY
  modableF low_limit;                 // getting contrast by highering the black point
  modableF high_limit;                                            
#else
  float low_limit  = 0;                 // map(render) from low to 0  decrease me for more high values
  float high_limit = 1;                 // map(render) from high to 1 lower me for more high values
#endif
};

render_parameters animation;     // all animation parameters in one place
struct oscillators {

  float master_speed;            // global transition speed
  float offset[num_oscillators]; // oscillators can be shifted by a time offset
  float ratio[num_oscillators];  // speed ratios for the individual oscillators                                  
};

oscillators timings;             // all speed settings in one place

struct modulators {  
  unsigned long lastMillis=0;
  float ramp[num_oscillators];        // returns 0 to FLT_MAX
  float ramp_no_offset[num_oscillators];        // returns 0 to FLT_MAX
  float saw[num_oscillators];        // returns 0 to 2*PI
  //float tri[num_oscillators];        // returns 0 to PI double frequency
  float sine[num_oscillators];   // returns -1 to 1 in a sin waveform
  float noise_angle[num_oscillators];   // returns 0 to 2*PI
};

modulators move;                 // all oscillator based movers and shifters at one place

rgbF pixel;
hsiF pixel_hsi;

static const byte pNoise[] = {   151,160,137,91,90, 15,131, 13,201,95,96,
53,194,233, 7,225,140,36,103,30,69,142, 8,99,37,240,21,10,23,190, 6,
148,247,120,234,75, 0,26,197,62,94,252,219,203,117, 35,11,32,57,177,
33,88,237,149,56,87,174,20,125,136,171,168,68,175,74,165,71,134,139,
48,27,166, 77,146,158,231,83,111,229,122, 60,211,133,230,220,105,92,
41,55,46,245,40,244,102,143,54,65,25,63,161, 1,216,80,73,209,76,132,
187,208, 89, 18,169,200,196,135,130,116,188,159, 86,164,100,109,198,
173,186, 3,64,52,217,226,250,124,123,5,202,38,147,118,126,255,82,85,
212,207,206, 59,227, 47,16,58,17,182,189, 28,42,223,183,170,213,119,
248,152,2,44,154,163,70,221,153,101,155,167,43,172, 9,129,22,39,253,
19,98,108,110,79,113,224,232,178,185,112,104,218,246, 97,228,251,34,
242,193,238,210,144,12,191,179,162,241,81,51,145,235,249,14,239,107,
49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,
150,254,138,236,205, 93,222,114, 67,29,24, 72,243,141,128,195,78,66,
215,61,156,180
};


//MUST define NUM_LEDS
class ANIMartRIX {

public:


  ANIMartRIX() {}

  ANIMartRIX(struct CRGB *data) {
    this->init(data);
  }

  // mapping numbers
  float spread_x, spread_y, spread_z, xmin, xmax, ymin, ymax, zmin, zmax, center_x, center_y, center_z, maxD;

  float radial_filter_radius = 12.0;      // 23.0 on 32x32, use 11 for 16x16, for mapping, try like 130% of your max radias todo make this scale with spread

  //TODO set sizes better, not just using a define?
  float polar_theta[NUM_LEDS];        // look-up table for polar angles
  float spherical_phi[NUM_LEDS];
  float distance[NUM_LEDS];           // look-up table for polar distances TODO

  //misc 
  CRGB* buffer; 
  unsigned long a, b, c;                  // for time measurements
  float show1, show2, show3, show4, show5, show6, show7, show8, show9, show0;
  const int audioPin = 19;
  volatile float audioSum = 0;
  volatile float audioSamples = 0;
  Vector3d upVector;



  // modables for external modulation  
  modableF global_intensity; //todo name globa hue?
  modableF global_bpm; // todo why do i have two of these?


  //modables only for teensy
#if ART_TEENSY
  modableF center_xm, center_ym, center_zm;
  modableF gHue; //todo name globa hue?  
  modableF global_scale_x, global_scale_y, global_scale_z;
  modableF roll;
  modableF pitch;
#else
  float gHue = 0;
  float global_scale_x, global_scale_y, global_scale_z;

#endif

  ANIMaudio * audio;

  //!initialize this object
  //scale, smaller numbers = zoom in = larger blobs
  void init(struct CRGB *data) {
    this->buffer = data;
    upVector.set(0,1,0);
    render_spherical_lookup_table();

    //init modables  

#if ART_TEENSY
    
    animation.low_limit.setMinMax(-2,.5);
    animation.low_limit = 0;
    //animation.low_limit.envelope.isLfo = true;
    //animation.low_limit.envelope.shape = envConst;
    //animation.low_limit.envelope.setMax(0);

    animation.high_limit.setMinMax(.5,1.2);
    animation.high_limit = 1;
    //animation.high_limit.envelope.isLfo = true;
    //animation.high_limit.envelope.shape = envConst;
    //animation.high_limit.envelope.setMax(0);



    global_scale_x.setMinMax(0.25, 1.2); //todo scale with size etc???
    global_scale_y.setMinMax(0.25, 1.2);
    global_scale_z.setMinMax(0.25, 1.2);
    global_scale_x.envelope.shape = envTriangle;
    global_scale_y.envelope.shape = envTriangle;
    global_scale_z.envelope.shape = envTriangle; 
    global_scale_x.envelope.setAttackDecay(100,100);
    global_scale_z.envelope.setAttackDecay(100,100);
    global_scale_z.envelope.setAttackDecay(100,100);

    gHue.edge = edgeWrap;
    //minmax default 0:1 
    //gHue.envelope.setMax(1.0);// todo test out - min max....
    gHue.envelope.setAttackDecay(10,2000);
    gHue.envelope.setMax(0.1);
    gHue.envelope.shape = envExponential;    

    roll.setMinMax(0,2*PI);
    roll.edge = edgeWrap;
    roll.envelope.setAttackDecay(500,100);
    roll.envelope.setMax(2*PI);
    roll.envelope.shape = envExponential;


    pitch.setMinMax(0,PI);
    pitch.edge = edgeMirror;
    pitch.envelope.setAttackDecay(500,100);
    pitch.envelope.setMax(PI);
    pitch.envelope.shape = envExponential;




    center_xm.setMinMax(xmin, xmax);
    center_ym.setMinMax(ymin, ymax);
    center_zm.setMinMax(zmin, zmax);

    center_xm = (spread_x)/2.0 + xmin;
    center_ym = (spread_y)/2.0 + ymin;
    center_zm = (spread_z)/2.0 + zmin;

    //edgeclip fine

    center_xm.envelope.setAttackDecay(2000,2000);
    center_ym.envelope.setAttackDecay(2000,2000);
    center_zm.envelope.setAttackDecay(2000,2000);

    center_xm.envelope.setMax(xmax);
    center_ym.envelope.setMax(ymax);
    center_zm.envelope.setMax(zmax);


    center_xm.envelope.shape = envSine;
    center_ym.envelope.shape = envSine;
    center_zm.envelope.shape = envSine;



#endif


    float max_spread = max(max(spread_x,spread_y),spread_z);
    global_scale_x = 1.0/ max_spread*7.0; // this should end up ~1
    global_scale_y = 1.0/ max_spread*7.0; //todo make these scale with pixel spacing
    global_scale_z = 1.0/ max_spread*7.0;

    Serial.print("gscalx: ");
    Serial.println(global_scale_x.getBase());
    Serial.print("gscaly: ");
    Serial.println(global_scale_y.getBase());
    Serial.print("gscalz: ");
    Serial.println(global_scale_z.getBase());

    //global_intensity; //default, should be overwritten in top level
    //global_intensity.envelope.setMinMax(1.0);
    global_intensity.envelope.setAttackDecay(10,400);
    global_intensity.envelope.shape = envExponential;
    //global_intensity.envelope.shape = envTriangle;


    global_bpm.setMinMax(60, 200);
    global_bpm.envelope.setMax(50);
    global_bpm.envelope.setAttackDecay(10,2000);
    global_bpm.envelope.shape = envTriangle;
    global_bpm = 115.0;

    Serial.println("global_bpm");
    Serial.println(global_bpm.getBase());
  }

  //void setGlobalScale(float setTo){
  //  if (setTo <=0) setTo = 0.0000001;
  //  global_scale = setTo;
  //}

  // Dynamic darkening methods:

  float subtract(float &a, float&b) {

    return a - b;
  }


  float multiply(float &a, float&b) {

    return a * b / 255.f;
  }


  // makes low brightness darker
  // sets the black point high = more contrast 
  // animation.low_limit should be 0 for best results
  float colorburn(float &a, float&b) {  

    return (1-((1-a/255.f) / (b/255.f)))*255.f;
  }


  // Dynamic brightening methods

  float add(float &a, float&b) {

    return a + b;
  }


  // makes bright even brighter
  // reduces contrast
  float screen(float &a, float&b) {

    return (1 - (1 - a/255.f) * (1 - b/255.f))*255.f;
  }


  float colordodge(float &a, float&b) {  

    return (a/(255.f-b)) * 255.f;
  }

  /////////////////////////////////////////////////////////////////////////
  //PNOISE
  /////////////////////////////////////////////////////////////////////////
  /*
   Ken Perlins improved noise   -  http://mrl.nyu.edu/~perlin/noise/
   C-port:  http://www.fundza.com/c4serious/noise/perlin/perlin.html
   by Malcolm Kesson;   arduino port by Peter Chiochetti, Sep 2007 :
   -  make permutation constant byte, obsoletes init(), lookup % 256
  */

  float fade(float t){ return t * t * t * (t * (t * 6 - 15) + 10); }
  float lerp(float t, float a, float b){ return a + t * (b - a); }
  float grad(int hash, float x, float y, float z)
  {
  int    h = hash & 15;          /* CONVERT LO 4 BITS OF HASH CODE */
  float  u = h < 8 ? x : y,      /* INTO 12 GRADIENT DIRECTIONS.   */
            v = h < 4 ? y : h==12||h==14 ? x : z;
  return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
  }

  #define P(x) pNoise[(x) & 255]

  float pnoise(float x, float y, float z) {
    
  int   X = (int)floorf(x) & 255,             /* FIND UNIT CUBE THAT */
        Y = (int)floorf(y) & 255,             /* CONTAINS POINT.     */
        Z = (int)floorf(z) & 255;
  x -= floorf(x);                             /* FIND RELATIVE X,Y,Z */
  y -= floorf(y);                             /* OF POINT IN CUBE.   */
  z -= floorf(z);
  float  u = fade(x),                         /* COMPUTE FADE CURVES */
         v = fade(y),                         /* FOR EACH OF X,Y,Z.  */
         w = fade(z);
  int  A = P(X)+Y, 
       AA = P(A)+Z, 
       AB = P(A+1)+Z,                         /* HASH COORDINATES OF */
       B = P(X+1)+Y, 
       BA = P(B)+Z, 
       BB = P(B+1)+Z;                         /* THE 8 CUBE CORNERS, */

  return lerp(w,lerp(v,lerp(u, grad(P(AA  ), x, y, z),    /* AND ADD */
                            grad(P(BA  ), x-1, y, z)),    /* BLENDED */
                lerp(u, grad(P(AB  ), x, y-1, z),         /* RESULTS */
                     grad(P(BB  ), x-1, y-1, z))),        /* FROM  8 */
              lerp(v, lerp(u, grad(P(AA+1), x, y, z-1),   /* CORNERS */
                   grad(P(BA+1), x-1, y, z-1)),           /* OF CUBE */
                lerp(u, grad(P(AB+1), x, y-1, z-1),
                     grad(P(BB+1), x-1, y-1, z-1))));
  }

  /////////////////////////////////////////////////////////////////////////
  //Oscilators
  /////////////////////////////////////////////////////////////////////////
  void calculate_oscillators(oscillators &timings) { 
    unsigned long thisMillis = millis();
    double runtime = (thisMillis - move.lastMillis) * timings.master_speed;  // global anaimation speed gives the frequency in millis of the ramp to increase by 1
    move.lastMillis = thisMillis;
    for (int i = 0; i < num_oscillators; i++) {
      
      move.ramp_no_offset[i]      = move.ramp_no_offset[i] +  runtime * timings.ratio[i];     // continously rising offsets, returns              0 to max_float, infinite ramp

      move.ramp[i]      = move.ramp_no_offset[i] + timings.offset[i] * timings.ratio[i];     // continously rising offsets, returns              0 + offset to max_float + offset, infinite ramp
      
      move.saw[i]      = fmodf(move.ramp[i], 2 * PI);                        // angle offsets for continous rotation, returns    0 to 2 * PI, sawtooth

      //move.tri[i]      = fabsf(move.saw[i] - PI);                        // angle offsets for continous rotation, returns    PI to 0 to PI, triangle double frequency
      
      move.sine[i] = sinf(move.saw[i]);                                 // directional offsets or factors, returns         -1 to 1, sin
      
      move.noise_angle[i] = PI * (1 + pnoise(move.ramp[i], 0, 0));              // noise based angle offset, returns                0 to 2 * PI, smooth noise
      
    }

    //LIB8STATIC uint8_t squarewave8( uint8_t in, uint8_t pulsewidth=128)
     //LIB8STATIC uint8_t cubicwave8(uint8_t in)

  //LIB8STATIC uint8_t quadwave8(uint8_t in)
  //706 {LIB8STATIC uint8_t triwave8(uint8_t in)
  //beatsin88 //sinf
  //beat 88// sawtooth
  //beat88( accum88 beats_per_minute_88, uint32_t timebase = 0)
  //LIB8STATIC uint16_t beatsin88( accum88 beats_per_minute_88, uint16_t lowest = 0, uint16_t highest = 65535,  uint32_t timebase = 0, uint16_t phase_offset = 0);
  }


  float bpmToSpeedMillis(modableF& bpm) const{
    //double runtime = millis() * timings.master_speed;  // global anaimation speed gives the frequency in millis of the ramp to increase by 1 per millisecond
    //so if master speed is 1000, ramp increases 1000 times per millisecond
    //if master speed is .1, it takes 10 millis to increase by ones

    //really master speed is beat per milli.
    //1m = 60000ms

    //(b/m) * (1m/ 60000ms) would give bpms
    //but I'm really interested in the time it takes to increase by 2*PI, so lets set "beat" to 2PI instead of 1
    return bpm.getEnvelope()/9549.29658551; //2PI*bpm/60000 gives the frequency in millis of the ramp to increase by 2PI
    //return bpm;
  }


  void run_default_oscillators(){

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.005;    // master speed

#if ART_TEENSY
    for (int i=0; i<9; i++ ){
      timings.ratio[0] = i;
      timings.offset[0] = 100*i;
    }
    for (int i=10; i < num_oscillators; i++ ){
      timings.ratio[0] = float(i)/15;
      timings.offset[0] = 100*i;
    }

#else

    timings.ratio[0] = 1;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 2;
    timings.ratio[2] = 3;
    timings.ratio[3] = 4;
    timings.ratio[4] = 5;
    timings.ratio[5] = 6;
    timings.ratio[6] = 7;
    timings.ratio[7] = 8;
    timings.ratio[8] = 9;
    timings.ratio[9] = 10;
    timings.ratio[9] = 10;

    
    timings.offset[0] = 000;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    timings.offset[7] = 700;
    timings.offset[8] = 800;
    timings.offset[9] = 900;
    //set_osc_offset();

#endif

    calculate_oscillators(timings);  
  }


  // Convert the 2 polar coordinates back to cartesian ones & also apply all 3d transitions.
  // Calculate the noise value at this point based on the 5 dimensional manipulation of 
  // the underlaying coordinates.

  /////////////////////////////////////////////////////////////////////////
  //Rendering
  /////////////////////////////////////////////////////////////////////////



  float render_value(render_parameters &animation, float scaleHigh = 255.0) {
    do_often();


    // convert **SPHERICAL** coordinates back to cartesian ones
    //this is really the only difference from base class

#if ART_TEENSY
    float newtheta = animation.angle + roll;
    float newphi = animation.anglephi + pitch; 
    newphi = animation.anglephi;
    newtheta = animation.angle ;


    float newx = (animation.offset_x + center_x - (animation.dist * sinf(newphi) * cosf(newtheta))) * animation.scale_x * global_scale_x;
    float newy = (animation.offset_y + center_y - (animation.dist * sinf(newphi) * sinf(newtheta))) * animation.scale_y * global_scale_y;
    float newz = (animation.offset_z + center_z - (animation.dist * cosf(newphi))) * animation.scale_z * global_scale_z;
#else
    float newx = (animation.offset_x + center_x - (animation.dist * sinf(animation.anglephi) * cosf(animation.angle))) * animation.scale_x * global_scale_x;
    float newy = (animation.offset_y + center_y - (animation.dist * sinf(animation.anglephi) * sinf(animation.angle))) * animation.scale_y * global_scale_y;
    float newz = (animation.offset_z + center_z - (animation.dist * cosf(animation.anglephi))) * animation.scale_z * global_scale_z;
#endif

    // render noisevalue at this new cartesian point
    //uint16_t raw_noise_field_value =inoise16(newx, newy, newz);
    float raw_noise_field_value = pnoise(newx, newy, newz);

    // A) enhance histogram (improve contrast) by setting the black and white point (low & high_limit)
    // B) scale the result to a 0-255 range (assuming you want 8 bit color depth per rgb chanel)
    // Here happens the contrast boosting & the brightness mapping

    if (raw_noise_field_value < animation.low_limit)  raw_noise_field_value =  animation.low_limit;
    if (raw_noise_field_value > animation.high_limit) raw_noise_field_value = animation.high_limit;
#if ART_TEENSY
    float scaled_noise_value = map_float(raw_noise_field_value, animation.low_limit.getEnvelope(), animation.high_limit.getEnvelope(), 0, scaleHigh);
#else
    float scaled_noise_value = map_float(raw_noise_field_value, animation.low_limit, animation.high_limit, 0, scaleHigh);
#endif
    return scaled_noise_value;
  }


  // given a static polar origin we can precalculate 
  // the spherical coordinates
  void render_spherical_lookup_table() {

    //initialize min / max for finding center of each axis
    xmin = ledMap[0][xind];
    xmax = xmin;
    ymin = ledMap[0][yind];
    ymax = ymin;
    zmin = ledMap[0][zind];
    zmax = zmin;

    //min and max mapping used for finding center
    for (int n = 1; n < (int) NUM_LEDS; n++) {
      if(ledMap[n][xind] < xmin) xmin = ledMap[n][xind];
      if(ledMap[n][xind] > xmax) xmax = ledMap[n][xind];
      if(ledMap[n][yind] < ymin) ymin = ledMap[n][yind];
      if(ledMap[n][yind] > ymax) ymax = ledMap[n][yind];
      if(ledMap[n][zind] < zmin) zmin = ledMap[n][zind];
      if(ledMap[n][zind] > zmax) zmax = ledMap[n][zind];
    }
    spread_x = xmax-xmin;
    spread_y = ymax-ymin;
    spread_z = zmax-zmin;

    center_x = (spread_x)/2.0 + xmin;
    center_y = (spread_y)/2.0 + ymin;
    center_z = (spread_z)/2.0 + zmin;
    
#if ART_TEENSY
    center_xm = (spread_x)/2.0 + xmin;
    center_ym = (spread_y)/2.0 + ymin;
    center_zm = (spread_z)/2.0 + zmin;
#endif
    
    Serial.print("x center: ");Serial.println(center_x);
    Serial.print("y center: ");Serial.println(center_y);
    Serial.print("z center: ");Serial.println(center_z); Serial.println();


    maxD = 0.0;
    for (int n = 0; n < NUM_LEDS; n++) {
        float dx = ledMap[n][xind] - center_x;
        float dy = ledMap[n][yind] - center_y;
        float dz = ledMap[n][zind] - center_z;
        
        distance[n] = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        polar_theta[n] = atan2f(dy,dx);
        spherical_phi[n] = acosf(dz / distance[n]);

        if (maxD < distance[n]) maxD = distance[n];
        /*
        Serial.print("x: ");Serial.println(dx);
        Serial.print("y: ");Serial.println(dy);
        Serial.print("z: ");Serial.println(dz); 

        Serial.print("d: ");Serial.println(distance[n]);
        Serial.print("t: ");Serial.println(polar_theta[n]);
        Serial.print("p: ");Serial.println(spherical_phi[n]); Serial.println();
        */

        
        
    }
    radial_filter_radius = maxD * 1.3;

    Serial.print("max mapped distance: "); Serial.println(maxD);
    Serial.print("reccomended radias filter: "); Serial.println(maxD*1.3);


    Serial.print("x spread: ");Serial.println(spread_x);
    Serial.print("y spread: ");Serial.println(spread_y);
    Serial.print("z spread: ");Serial.println(spread_z); Serial.println();

  }



  /////////////////////////////////////////////////////////////////////////
  //Color setting Utilities, map, sanity check
  /////////////////////////////////////////////////////////////////////////
  




  // Avoid any possible color flicker by forcing the raw RGB values to be 0-255.
  // This enables to play freely with random equations for the colormapping
  // without causing flicker by accidentally missing the valid target range.
  /*
  rgbF rgb_sanity_check(rgbF &pixel) {

        // rescue data if possible, return absolute value
        //if (pixel.r < 0)     pixel.r = fabsf(pixel.r);
        //if (pixel.g < 0) pixel.g = fabsf(pixel.g);
        //if (pixel.b < 0)   pixel.b = fabsf(pixel.b);
        
        // discard everything above the valid 8 bit colordepth 0-255 range
        if (pixel.r   > 255)   pixel.r = 255;
        if (pixel.g > 255) pixel.g = 255;
        if (pixel.b  > 255)  pixel.b = 255;

        return pixel;
  }*/


  uint8_t float_to_uint8_t(float color){
    if (color < 0.0) color = 0.0; // todo check behavioor of this??? 
    else if (color   > 255)   color = 255; // TODO added this in and now things look more boring???
    return ((uint8_t) color);
  }

/*
  hsiF hsi_sanity_check(hsiF &pixel) {

      // rescue data if possible, return absolute value
      //if (pixel.red < 0)     pixel.red = fabsf(pixel.red);
      //if (pixel.green < 0) pixel.green = fabsf(pixel.green);
      //if (pixel.blue < 0)   pixel.blue = fabsf(pixel.blue);
        if (pixel.h   > 1.0)   pixel.h = 1.0;
        if (pixel.s > 1.0) pixel.h = 1.0;
        if (pixel.i  > 255)  pixel.i = 255;
      return pixel;
  }*/


/*
  hsiF CHSV2Hsi(CHSV c){
    hsiF p;
    p.h = ((float)c.h)/255.0;
    p.s = ((float)c.s)/255.0;
    p.i = ((float)c.v);
    return p;
  }*/

  rgbF CRGB2Rgb(CRGB rgb){
    rgbF rgbf;
    rgbf.r = rgb.r;
    rgbf.g = rgb.g;
    rgbf.b = rgb.b;
    return rgbf;
  }



  //! hue shift given hsiF, return rgbF.  sanity checks performed in Hsi2Rgb
  rgbF hue_shift(hsiF hsi){
      hsi.h = hsi.h+gHue;  //todo PROTECT THIS
      return Hsi2Rgb(hsi);
  }
  //! hue shift given rgbF, convert to hsiF, perform shift, return rgbF.  sanity checks performed in Rgb2Hsi and Hsi2Rgb
  rgbF hue_shift(rgbF rgb){
      return hue_shift(Rgb2Hsi(rgb));
  }

  //!todo, this one is no good??, cant use set pixel color because of the double intensity thing???
  rgbF hue_shift(CRGB rgb){
      return hue_shift(CRGB2Rgb(rgb));//todo test this???
  }






  ///////////////////////////////////////////////////////////////////////
  //master setting of color
  //!setPixelColor takes in rgbF pixel, performs hue shift, multiplies by global intensity, and returns rounded resut as integer CRGB
  CRGB setPixelColor(hsiF phsi) {
    rgbF p = hue_shift(phsi);
    return CRGB(round(p.r*global_intensity), round(p.g*global_intensity), round(p.b*global_intensity));
  }  

  CRGB setPixelColor(rgbF p) {
    p = hue_shift(p);
    return CRGB(round(p.r*global_intensity), round(p.g*global_intensity), round(p.b*global_intensity));
  }

  //! take in uint8_t color, perform color from pallet and hue shift
  CRGB setPixelColor(uint8_t color) {
    CRGB pCRGB = ColorFromPalette(currentPalette, color);
    rgbF p = hue_shift(pCRGB);
    return CRGB(round(p.r*global_intensity), round(p.g*global_intensity), round(p.b*global_intensity));
  }


  ///////////////////////////////////////////////////////////////////////
  //timings / debug outputs

  void get_ready() {  // measure time etc
    markStartOfRender();
    do_often();
    animation.low_limit = 0;
    animation.high_limit = 1;

#if ART_TEENSY
    //center_x = center_xm.getEnvelope();
    //center_x = center_ym.getEnvelope();
    //center_x = center_zm.getEnvelope();
#endif

#if ART_WAG and USE_IMU
  upVector.makeUnitVector();
  roll = upVector.x*2*PI;
  pitch = upVector.y*PI;
#endif
  }

  void do_often(){
#if ART_VEST and USE_AUDIO
    EVERY_N_MILLIS(50) {
      audio->update();//really needed to figure out more regular polling
    }
#endif
 }

  void markStartOfRender(){
    a = micros();
  }
  void markStartOfShow(){
    b = micros();
  }  
  void markEndOfShow(){
    c = micros();
  }  
  unsigned long getRenderTime(){
    return b -a;
  }  
  unsigned long getShowTime(){
    return c - b;
  }
  unsigned long getTotalUpdateTime(){
    return c - a;
  }


  // show current framerate and rendered pixels per second
  void report_performance() {
   
    int fps = FastLED.getFPS();                 // frames per second
    //int kpps = (fps * NUM_LEDS) / 1000;   // kilopixel per second
  
    //Serial.print(kpps); Serial.print(" kpps ... ");
    Serial.print(getRenderTime()); Serial.print(" us RenderTime ... ");
    Serial.print(getShowTime()); Serial.print(" us ShowTime ... ");
    Serial.print(getTotalUpdateTime()); Serial.print(" TotalUpdateTime ... ");
    Serial.print(fps); Serial.print(" fps @ ");
    Serial.print(NUM_LEDS); Serial.println(" LEDs ... "); Serial.println();
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Effects, written for animapping...


  void TestMap() { // todo rename, brighness
    get_ready();     
    calculate_oscillators(timings);     // get linear movers and oscillators going
      for (int n = 0; n < NUM_LEDS; n++) {
        pixel_hsi.i = 0;
        pixel_hsi.h = 2;

        if (ledMap[n][xind] > center_x && ledMap[n][yind] > center_y){
          pixel_hsi.h = .17;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.i = 16;
        } 
        
        if (ledMap[n][xind] < center_x && ledMap[n][zind] > center_z){
          pixel_hsi.h = .5;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.i = 16;
        } 

        if (ledMap[n][xind] < center_x && ledMap[n][yind] < center_y){
          pixel_hsi.h = .8;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.i = 16;          
        }

        /* 
        else if (ledMap[n][xind] > 0){
          pixel_hsi.i = 4;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.h = .3;
        }
        else if (ledMap[n][yind] > 0){
          pixel_hsi.i = 4;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.h = .6;
        }

        else if (ledMap[n][zind] > 0){
          pixel_hsi.i = 10;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.h = .8;
        }

        else{
          pixel_hsi.i = 10;//6.0/255.0 *show1 * radial_filter; //todo fix all these
          pixel_hsi.h = .5;
        }  
        */

        pixel_hsi.s = 1;
        
        buffer[n] = setPixelColor(pixel_hsi);
      }   
  }


  void Chasing_Spirals_Hsi() { // todo rename, brighness

    get_ready(); 
    static Plane3d myPlane;

    timings.master_speed = bpmToSpeedMillis(global_bpm)/5;// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = .5;         // higher values = faster transitions
    timings.ratio[1] = 0.25;
    timings.ratio[2] = 1/3;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;


    
    calculate_oscillators(timings);     // get linear movers and oscillators going



    //myPlane.yaw(.001 * move.noise_angle[1]);

    myPlane.setRefpoint(center_x+spread_x*move.sine[1]+move.noise_angle[0]/2.0, center_y+spread_y*move.sine[2]*move.noise_angle[1]/2.0, center_z);

      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = polar_theta[n];//4 * polar_theta[n] +  move.saw[0] ;//- distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.scale_z    = 0.1;
        animation.offset_x   = move.ramp[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        //float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[1];
        //float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[2];
        //float show3          = render_value(animation);

        // colormapping
        //float radius = radial_filter_radius;
        //float radial_filter = (radius - distance[n]) / radius;





        float d = myPlane.distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        //d=fmodf(d,maxD);
        pixel_hsi.h = (map_float(d, -maxD/2, maxD/2, 0, 1));        
        pixel_hsi.i = 64;//6.0/255.0 *show1 * radial_filter; //todo fix all these
        pixel_hsi.s = 1;
        buffer[n] = setPixelColor(pixel_hsi);
      }
   
  }




  void Module_Experiment11_Hsi() { //todo brightness
    get_ready();

    //nscale might provide smoother fade?
    fadeToBlackBy( buffer, NUM_LEDS, 1);

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed 0.031

    timings.ratio[0] = 0.08;   //linear        // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.051;   //angle

    calculate_oscillators(timings); 
    //float cutoff = 0.5;
    //float scale = cutoff *4;
    
    for (int n = 0; n < NUM_LEDS; n++) { 


      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation, 1.0);
      pixel_hsi.h    = (show1);
      pixel_hsi.s = 1.0;      
      pixel_hsi.i = 255;
      buffer[n] = setPixelColor(pixel_hsi);
    }

    
  }


  void Module_Experiment9_Hsi() { // todo brightness
    get_ready();

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed 0.031


    timings.ratio[0] = 0.08;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.051;

    calculate_oscillators(timings); 
    
    for (int n = 0; n < NUM_LEDS; n++) { 


      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation, 1.0);

      pixel_hsi.h    = (show1);
      pixel_hsi.s = 1.0;
      pixel_hsi.i = 255;   

      
      buffer[n] = setPixelColor(pixel_hsi);
    }
  }





  void PlaneRotation1() { ///REALLLY cool sparklesssss

    get_ready();
    static Plane3d myPlane;

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.5;         // higher values = faster transitions
    timings.ratio[1] = 0.25;
    timings.ratio[2] = 0.33;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;


    
    calculate_oscillators(timings);     // get linear movers and oscillators going



    myPlane.yaw(.001 * move.noise_angle[0]);

    myPlane.setRefpoint(center_x+spread_x*move.sine[1]+move.noise_angle[0]/2.0, center_y+spread_y*move.sine[2]/2.0, center_z);


      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = polar_theta[n];//4 * polar_theta[n] +  move.saw[0] ;//- distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_z    = 0.1;  
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.offset_x   = move.ramp[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        //float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[1];
        //float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[2];
        //float show3          = render_value(animation);

        // colormapping
        //float radius = radial_filter_radius;
        //float radial_filter = (radius - distance[n]) / radius;



        pixel_hsi.i = 255;//6.0/255.0 *show1 * radial_filter;
        pixel_hsi.s = 1;

        float d = myPlane.distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        d=fmodf(d,maxD/2);
        pixel_hsi.h = (map_float(d, -maxD/2, maxD/2, 0, 1));
        buffer[n] = setPixelColor(pixel_hsi);
      }
   
  }

  void PlaneCounterRotation1() {

    get_ready(); 
    static Plane3d myPlane;

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.05;         // higher values = faster transitions
    timings.ratio[1] = 0.025;
    timings.ratio[2] = 0.033;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
   
    calculate_oscillators(timings);     // get linear movers and oscillators going

    myPlane.setNormal(upVector);
    myPlane.setRefpoint(center_x+spread_x*move.sine[1]+move.noise_angle[0]/2.0, center_y+spread_y*move.sine[2]/2.0, center_z);
    myPlane.setRefpoint(center_x, center_y, center_z);
    
      for (int n = 0; n < NUM_LEDS; n++) {

        pixel_hsi.i = 255;//6.0/255.0 *show1 * radial_filter;
        pixel_hsi.s = 1;

        float d = myPlane.distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        //d=fmodf(d,maxD/2);
        if (d < 0 ){
          pixel_hsi.i = 0;
        } else {
          pixel_hsi.h = (map_float(d, -maxD/2, maxD/2, 0, 1));
          pixel_hsi.h = 0;
        }
        buffer[n] = setPixelColor(pixel_hsi);
      }
   
  }


  void GrowingSpheres() { ///REALLLY cool sparklesssss

    get_ready(); 
    const int num_spheres = 4;
    static Sphere3d spheres[num_spheres]; // 4 spheres
    static bool firstTime=true;
    const float h[num_spheres] = {.7,.04,.9,.5};
    const float resetRadias = maxD*1.1;//maxD*2.5;

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 1.0;         // higher values = faster transitions
    timings.ratio[1] = 0.33;
    timings.ratio[2] = 0.36;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
    calculate_oscillators(timings);     // get linear movers and oscillators going


    //if first time set centers and init radiases in ascending order
    if (firstTime){
      for (int s = 0; s < num_spheres; s++) {
        spheres[s].setRefpoint(center_x,center_y,center_z);
        spheres[s].setRadias(s*resetRadias/((float)num_spheres)); //this could break with too many spheres  if maxd*1.5 = 4, 0 1 2 3
      }
      firstTime = false; //only so this once

    }

    //grow all equally
    for (int s = 0; s < num_spheres; s++) {
      spheres[s].grow(.01);    
      if (spheres[s].mRadias > resetRadias) {
        //spheres[s].setRefpoint(center_x + spread_x*move.sine[1], center_y + spread_y*move.sine[2], center_z + spread_z*move.sine[0]);
        spheres[s].setRadias(0.0);
      }
    }

    //check last, reorder list




    //fadeToBlackBy( buffer, NUM_LEDS, 1);
    /////////
    //now loop through all leds
    for (int n = 0; n < NUM_LEDS; n++) {
      int mind = 0;
      float minD = -1000;
      for (int s = 0; s <num_spheres; s++) {
        float d = spheres[s].distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        //must be outside, but closest
        if (d >= 0 and (d<minD or minD<0)){
          minD = d;
          mind=s;
        } else if (d < 0 and minD<0 and d>minD){
          mind=s;
          minD=d;
        }
      }
      pixel_hsi.h = h[mind];
      

      pixel_hsi.i = 255;//6.0/255.0 *show1 * radial_filter;
      pixel_hsi.s = 1;


      buffer[n] = setPixelColor(pixel_hsi);
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Original effects
  void Rotating_Blob() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 1.;         // higher values = faster transitions
    timings.ratio[1] = 0.3;
    timings.ratio[2] = 0.31;
    timings.ratio[3] = 0.35;
    
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        
        // describe and render animation layers
        animation.scale_x    = 0.05;
        animation.scale_y    = 0.05;
        animation.scale_z    = 0.05;
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        animation.angle      = polar_theta[n] +  move.saw[0];
        
        animation.dist       = distance[n];
        animation.offset_z   = move.ramp[0];
        animation.low_limit  = -1;
        float show1          = render_value(animation);
        
        animation.angle      = polar_theta[n] - move.saw[1] + show1/512.0;
        animation.dist       = distance[n] * show1/255.0;
        animation.low_limit  = 0;
        animation.offset_z   = move.ramp[1];
        float show2          = render_value(animation);

        animation.angle      = polar_theta[n] - move.saw[2] + show1/512.0;
        animation.dist       = distance[n] * show1/220.0;
        animation.offset_z   = move.ramp[2];
        float show3          = render_value(animation);

        animation.angle      = polar_theta[n] - move.saw[3] + show1/512.0;
        animation.dist       = distance[n] * show1/200.0;
        animation.offset_z   = move.ramp[3];
        float show4          = render_value(animation);

        // colormapping
        pixel.r   = (show2+show4);
        pixel.g = show3 / 2;
        pixel.b  = 0; //todo


        buffer[n] = setPixelColor(pixel);
      }
   
  }






  void Chasing_Spirals() { // todo make this aperiodic

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 1;         // higher values = faster transitions
    timings.ratio[1] = 1/3;
    timings.ratio[2] = 0.5;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = 3 * polar_theta[n] +  move.saw[0] - distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_z    = 0.1;  
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.offset_x   = move.ramp[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        animation.offset_z   = 0;
        float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[1];
        float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.saw[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.ramp[2];
        float show3          = render_value(animation);

        // colormapping
        float radius = radial_filter_radius;
        float radial_filter = (radius - distance[n]) / radius;

        pixel.r   = 3*show1 * radial_filter;
        pixel.g = show2 * radial_filter / 2;
        pixel.b  = show3 * radial_filter / 4;


        buffer[n] = setPixelColor(pixel);
      }
   
  }


  void Waves() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.1;    // speed ratios for the oscillators
    timings.ratio[0] = .25;         // higher values = faster transitions
    timings.ratio[1] = .33;
    
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.dist       = distance[n];
      animation.offset_y   = 0;
      animation.offset_x   = 0;
      animation.offset_z   = 2*distance[n] - move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n];
      animation.dist       = distance[n];
      animation.offset_z   = 2*distance[n] - move.ramp[1];
      float show2          = render_value(animation);

  
      // colormapping
      pixel.r   = show1;
      pixel.g = 0;
      pixel.b  = show2;


      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Center_Field() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 1;         // higher values = faster transitions
    
    timings.offset[1] = 100;

    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      // describe and render animation layers
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.dist       = 5*sqrtf(distance[n]);
      animation.offset_y   = move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.dist       = 4*sqrtf(distance[n]);
      animation.offset_y   = move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show2          = render_value(animation);

     

  
      // colormapping
      pixel.r   = show1;
      pixel.g = show2;
      pixel.b  = 0;


      buffer[n] = setPixelColor(pixel);
    }
   

  }




  void Distance_Experiment() {


    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.2;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.032;
    
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = powf(distance[n], 0.5);
      animation.angle      = polar_theta[n] + move.saw[0];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_y   = move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show1          = render_value(animation);

      animation.dist       = powf(distance[n], 0.6);
      animation.angle      = polar_theta[n] + move.noise_angle[2];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_y   = move.ramp[1];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show2          = render_value(animation);
      
      // colormapping
      pixel.r   = show1+show2;
      pixel.g = show2;
      pixel.b  = 0; // todo


      buffer[n] = setPixelColor(pixel);
    }
   

  }


  void Caleido1() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.003;    // speed ratios for the oscillators
    timings.ratio[0] = 0.02;         // higher values = faster transitions
    timings.ratio[1] = 0.03;
    timings.ratio[2] = 0.04;
    timings.ratio[3] = 0.05;
    timings.ratio[4] = 0.66;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = distance[n] * (2 + move.sine[0]) / 3;
      animation.angle      = 3 * polar_theta[n] + 3 * move.noise_angle[0] + move.saw[4];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = move.ramp[0];
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[1]) / 3;
      animation.angle      = 4 * polar_theta[n] + 3 * move.noise_angle[1] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[1];
      animation.offset_z   = move.ramp[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[2]) / 3;
      animation.angle      = 5 * polar_theta[n] + 3 * move.noise_angle[2] + move.saw[4];
      animation.offset_y   = 2 * move.ramp[2];
      animation.offset_z   = move.ramp[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[3]) / 3;
      animation.angle      = 4 * polar_theta[n] + 3 * move.noise_angle[3] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[3];
      animation.offset_z   = move.ramp[3];
      float show4          = render_value(animation);
      
      // colormapping
      pixel.r   = show1;
      pixel.g = show3 * distance[n] / 10;
      pixel.b  = (show2 + show4) / 2;

 

      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Caleido2() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.002;    // speed ratios for the oscillators
    timings.ratio[0] = 0.02;         // higher values = faster transitions
    timings.ratio[1] = 0.03;
    timings.ratio[2] = 0.04;
    timings.ratio[3] = 0.05;
    timings.ratio[4] = 0.6;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = distance[n] * (2 + move.sine[0]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[0] + move.saw[4];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = move.ramp[0]; //TODO was 0????
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[1]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[1] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[1];
      animation.offset_z   = move.ramp[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[2]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[2] + move.saw[4];
      animation.offset_y   = 2 * move.ramp[2];
      animation.offset_z   = move.ramp[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[3]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[3] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[3];
      animation.offset_z   = move.ramp[3];
      float show4          = render_value(animation);
      
      // colormapping
      pixel.r   = show1;
      pixel.g = show3 * distance[n] / 10;
      pixel.b  = (show2 + show4) / 2;


      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Caleido3() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.004;    // speed ratios for the oscillators
    timings.ratio[0] = 0.02;         // higher values = faster transitions
    timings.ratio[1] = 0.03;
    timings.ratio[2] = 0.04;
    timings.ratio[3] = 0.05;
    timings.ratio[4] = 0.60;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = distance[n] * (2 + move.sine[0]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[0] + move.saw[4];
      animation.scale_x    = 0.1;// + (move.sine[0] + 2)/100;
      animation.scale_y    = 0.1;// + (move.sine[1] + 2)/100;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.ramp[0];
      animation.offset_x   = 2 * move.ramp[1];
      //animation.offset_z   = 0;
      animation.offset_z   = move.ramp[0];
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[1]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[1] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[1];
      animation.offset_y   = show1 / 20.0;
      animation.offset_z   = move.ramp[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[2]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[2] + move.saw[4];
      animation.offset_y   = 2 * move.ramp[2];
      animation.offset_x   = show2 / 20.0;
      animation.offset_z   = move.ramp[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.sine[3]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[3] + move.saw[4];
      animation.offset_x   = 2 * move.ramp[3];
      animation.offset_y   = show3 / 20.0;
      animation.offset_z   = move.ramp[3];
      float show4          = render_value(animation);
      
      // colormapping
      float radius = radial_filter_radius;  // radial mask

      pixel.r   = show1 * (ledMap[n][yind]+1) / spread_y;
      pixel.g = show3 * distance[n] / 10;
      pixel.b  = (show2 + show4) / 2;
      if (distance[n] > radius) {
        pixel.r = 0;
        pixel.g = 0;
        pixel.b = 0;
      }

      buffer[n] = setPixelColor(pixel);
    }
  }

  void Lava1() {

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.0015;    // speed ratios for the oscillators
    timings.ratio[0] = 4;         // higher values = faster transitions
    timings.ratio[1] = 1;
    timings.ratio[2] = 1;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = distance[n] * 0.8;
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.15;// + (move.sine[0] + 2)/100;
      animation.scale_y    = 0.12;// + (move.sine[1] + 2)/100;
      animation.scale_z    = 0.10;
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      //animation.offset_z   = 0;
      animation.offset_z   = 30;
      float show1          = render_value(animation);

      animation.offset_y   = -move.ramp[1];
      //animation.scale_x    = 0.15;// + (move.sine[0] + 2)/100;
      //animation.scale_y    = 0.12;// + (move.sine[1] + 2)/100;
      animation.offset_x   = show1 / 100;
      animation.offset_y   += show1/100;
     
      float show2          = render_value(animation);

      animation.offset_y   = -move.ramp[2];
      //animation.scale_x    = 0.15;// + (move.sine[0] + 2)/100;
      //animation.scale_y    = 0.12;// + (move.sine[1] + 2)/100;
      animation.offset_x   = show2 / 100;
      animation.offset_y   += show2/100;
     
      float show3         = render_value(animation);

      // colormapping
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-animation.dist)/animation.dist;


      pixel.r = 10+radial*show2;
      pixel.g = 0.1*radial*(show2-show3);
      pixel.b = 4;//3*move.noise_angle[1]; //todohue
      


      buffer[n] = setPixelColor(pixel);
    }
  }

  void Scaledemo1() { // todo copy and try to fix?

    get_ready(); 
        

    timings.master_speed = bpmToSpeedMillis(global_bpm)*.5;// was: 0.00003;    // speed ratios for the oscillators
    timings.ratio[0] = .02;         // higher values = faster transitions
    timings.ratio[1] = .008;
    timings.ratio[2] = .00125;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;

    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = 0.3*distance[n] * 0.8;
      animation.angle      = 3*polar_theta[n] + move.saw[2];
      animation.scale_x    = 0.1 + (move.noise_angle[0])/10;
      animation.scale_y    = 0.1 + (move.noise_angle[1])/10 +  (move.sine[1] + 2)/100;
      animation.scale_z    = 0.1 + (move.noise_angle[0]+move.noise_angle[1])/10;
      animation.offset_y   = 0; // todo make this more interesting ?
      animation.offset_x   = 0;
      animation.offset_z   = 10*move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = 3; // todo make this cooler? 
      float show2          = render_value(animation);

      float dist = 1;//(10-distance[n])/ 10;
      pixel.r = show1*dist;
      pixel.g = (show1-show2)*dist*0.7;
      pixel.b = (show2-show1)*dist;

      if (distance[n] > 16) { // todo make scaled version????
         pixel.r = 0;
         pixel.g = 0;
         pixel.b = 0;

      }
      


      buffer[n] = setPixelColor(pixel);
    }
  }


  void Scaledemo2() { // todo copy and try to fix?

    get_ready(); 
        

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.00003;    // speed ratios for the oscillators
    timings.ratio[0] = .005;         // higher values = faster transitions
    timings.ratio[1] = .001;
    timings.ratio[2] = .00125;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;

    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = 0.3*distance[n] * 0.8;
      animation.angle      = 3*polar_theta[n] + move.saw[2];
      animation.scale_x    = 0.1 + (move.noise_angle[0])/10;
      animation.scale_y    = 0.1 + (move.noise_angle[1])/10;// + (move.sine[1] + 2)/100;
      animation.scale_z    = 0.1 + (move.noise_angle[0]+move.noise_angle[1])/10;
      animation.offset_y   = 0;
      animation.offset_x   = 0;
      animation.offset_z   = 10*move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = 3;
      float show2          = render_value(animation);

      float dist = 1;//(10-distance[n])/ 10; // todo replace with radial?
      pixel.r = show1*dist;
      pixel.g = (show1-show2)*dist*0.76;
      pixel.b = (show2-show1)*dist;

      if (distance[n] > 16) { // todo make scaled version????
         pixel.r = 0;
         pixel.g = 0;
         pixel.b = 0;

      }
      


      buffer[n] = setPixelColor(pixel);
    }
  }



  void Yves() { 

    get_ready(); 
        

    timings.master_speed = bpmToSpeedMillis(global_bpm)/3;// was: 0.001;    // speed ratios for the oscillators
    timings.ratio[3] = 0.13;
    timings.ratio[4] = 0.15;
    timings.ratio[5] = 0.03;
    timings.ratio[6] = 0.025;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] ;
      animation.angle      = polar_theta[n] + 2*PI + move.noise_angle[5];
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.08;
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2*PI + move.noise_angle[6];;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.08;
      animation.offset_y   = -move.ramp[1];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + show1/100 + move.noise_angle[3] + move.noise_angle[4];
      animation.dist       = distance[n] + show2/50;
      animation.offset_y   = -move.ramp[2];

      animation.offset_y   += show1/100;
      animation.offset_x   += show2/100;

      float show3          = render_value(animation);

      animation.offset_y   = 0;
      animation.offset_x   = 0;

      float show4          = render_value(animation);
      
     
      pixel.r   = show3;
      pixel.g = show3*show4/255;
      pixel.b  = 0;// todo to dark
      

      buffer[n] = setPixelColor(pixel);
    }
  }

  void Spiralus() {

    get_ready(); 
        

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.0011;    // speed ratios for the oscillators
    timings.ratio[0] = .15;         // higher values = faster transitions
    timings.ratio[1] = .23;
    timings.ratio[2] = .3;
    timings.ratio[3] = 0.005;
    timings.ratio[4] = 0.02;
    timings.ratio[5] = 0.003;
    timings.ratio[6] = 0.0025;
    timings.ratio[7] = 0.0021;
    timings.ratio[8] = 0.0027;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] ;
      animation.angle      = 2*polar_theta[n] + move.noise_angle[5] + move.sine[3] * move.noise_angle[6]* animation.dist/10;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.07;
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = move.ramp[1];
      float show1          = render_value(animation);

      animation.angle      = 2*polar_theta[n] + move.noise_angle[7] + move.sine[5] * move.noise_angle[8]* animation.dist/10;
      animation.offset_y   = -move.ramp[1];
      animation.offset_z   = move.ramp[2];
            
      float show2          = render_value(animation);

      animation.angle      = 2*polar_theta[n] + move.noise_angle[6] + move.sine[6] * move.noise_angle[7]* animation.dist/10;
      animation.offset_y   = move.ramp[2];
      animation.offset_z   = move.ramp[0];
      float show3          = render_value(animation);
      
      float f =  1;
     
      pixel.r   = f*(show1+show2);
      pixel.g = f*(show1-show2);
      pixel.b  = f*(show3-show1);
      

      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Spiralus2() {

    get_ready(); 
      

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.0015;    // speed ratios for the oscillators
    timings.ratio[0] = .125;         // higher values = faster transitions
    timings.ratio[1] = .225;
    timings.ratio[2] = .3;
    timings.ratio[3] = 0.005;
    timings.ratio[4] = 0.02;
    timings.ratio[5] = 0.005;
    timings.ratio[6] = 0.0055;
    timings.ratio[7] = 0.006;
    timings.ratio[8] = 0.0027;
    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] ;
      animation.angle      = 2*polar_theta[n] + move.noise_angle[5] + move.sine[3] * move.noise_angle[6]* animation.dist/10;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.07;
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = move.ramp[1];
      float show1          = render_value(animation);

      animation.angle      = 6*polar_theta[n] + move.noise_angle[7] + move.sine[5] * move.noise_angle[8]* animation.dist/10;
      animation.offset_y   = -move.ramp[1];
      animation.offset_z   = move.ramp[2];
            
      float show2          = render_value(animation);

      animation.angle      = 6*polar_theta[n] + move.noise_angle[6] + move.sine[6] * move.noise_angle[7]* animation.dist/10;
      animation.offset_y   = move.ramp[2];
      animation.offset_z   = move.ramp[0];
      animation.dist       = distance[n] *0.8;
      float show3          = render_value(animation);
      
      
      float f =  1;//(24-distance[n])/24;
     
      pixel.r   = f*(show1+show2);
      pixel.g = f*(show1-show2);
      pixel.b  = f*(show3-show1);
      

      buffer[n] = setPixelColor(pixel);
    }
  }


  void Hot_Blob() { // nice one

    get_ready(); 
                    

    run_default_oscillators();

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] ;
      animation.angle      = polar_theta[n];
      
      animation.scale_x    = 0.07 + move.sine[0]*0.002;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.low_limit  = -1;
      float show1          = render_value(animation);

      animation.offset_y   = -move.ramp[1];
      float show3          = render_value(animation);

      animation.offset_x   = show3/20;
      animation.offset_y   = -move.ramp[0]/2 + show1/70;
      animation.low_limit  = 0;
      float show2          = render_value(animation);

      animation.offset_x   = show3/20;
      animation.offset_y   = -move.ramp[0]/2 + show1/70;
      animation.offset_z   = 100;
      float show4          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-animation.dist)/animation.dist;

      
      pixel.r   = radial  * show2;
      pixel.g   = radial* 0.3* (show2-show4);
      pixel.b = 15;
      

      buffer[n] = setPixelColor(pixel);
    }
 
  }

  void Zoom() { // nice one
    
    get_ready();

    

    run_default_oscillators();
    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.003;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n] * distance[n])/2;
      animation.angle      = polar_theta[n];
      
      animation.scale_x    = 0.005;
      animation.scale_y    = 0.005;
      animation.scale_z    = 0.005;
      
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      
      animation.low_limit  = 0;
      float show1          = render_value(animation);

  
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
      
      pixel.r   = show1*radial; //todo this is pretty boring, maybe make palette
      pixel.g   = 0;
      pixel.b = 15; 
      
      

      buffer[n] = setPixelColor(pixel);
    }
  }


  void Zoom2() { // nice one
    
    get_ready();

    

    run_default_oscillators();
    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.003;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n] * distance[n])/2;
      animation.angle      = polar_theta[n];
      
      animation.scale_x    = 0.005;
      animation.scale_y    = 0.005;
      animation.scale_z    = 0.005;
      
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0.1*move.ramp[0];
      
      animation.low_limit  = 0;
      float show1          = render_value(animation);

  
      //float linear = 1;//(y+1)/(num_y-1.f);
      
      pixel.r   = show1;
      pixel.b   = 40-show1;
      pixel.g = 15; 
      

      buffer[n] = setPixelColor(pixel);
    }
  }


  void Slow_Fade() { // nice one

    get_ready();

                      

    run_default_oscillators();
    timings.master_speed = bpmToSpeedMillis(global_bpm)/1000;// was: 0.00005;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = sqrtf(distance[n]) * 0.7 * (move.sine[0] + 1.5);
      animation.angle      = polar_theta[n] - move.saw[0] + distance[n] / 5;
      
      animation.scale_x    = 0.11;
      animation.scale_y    = 0.11;
      animation.scale_z    = 0.11;
      
      animation.offset_y   = -50 * move.ramp[0];
      animation.offset_x   = 0;
      
      animation.offset_z   = move.ramp[0];
      animation.low_limit  = -0.1;
      float show1          = render_value(animation);

      animation.dist       = animation.dist  * 1.1;
      animation.angle      += move.noise_angle[0] / 10;
      float show2          = render_value(animation);

      animation.dist       = animation.dist  * 1.1;
      animation.angle      += move.noise_angle[1] / 10;
      
      float show3          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];


    
      
      pixel.r    = radial * show1;
      pixel.g  = radial * (show1 - show2) / 6;
      pixel.b   = radial * (show1 - show3) / 5;
      
      

      buffer[n] = setPixelColor(pixel);
    }
  }

  void Polar_Waves() { // nice one

    get_ready();

    

                      

    timings.master_speed = bpmToSpeedMillis(global_bpm)/2;// was: 0.5;    // master speed

    timings.ratio[0] = 0.25;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.27;
    timings.ratio[2] = 0.31;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n]);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.saw[0];
      animation.offset_z   = (animation.dist * 1.5)-10 * move.ramp[0];
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_x   = move.ramp[0];
      
      float show1          = render_value(animation);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.saw[1];
      animation.offset_z   = (animation.dist * 1.5)-10 * move.ramp[1];
      animation.offset_x   = move.ramp[1];

      float show2          = render_value(animation);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.saw[2];
      animation.offset_z   = (animation.dist * 1.5)-10 * move.ramp[2];
      animation.offset_x   = move.ramp[2];

      float show3          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * show1;
      pixel.g  = radial * show2;
      pixel.b   = radial * show3;
      
      

      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs() { // nice one

    get_ready(); 
                         

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.2;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.036;
    timings.ratio[5] = 0.039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[0] + move.noise_angle[0]+ move.noise_angle[3];
      animation.offset_z   = (sqrtf(animation.dist));// - 10 * move.ramp[0];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_x   = 10*move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[1]+ move.noise_angle[1]+ move.noise_angle[4];
      animation.offset_x   = 11*move.ramp[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[2]+ move.noise_angle[2]+ move.noise_angle[5];
      animation.offset_x   = 12*move.ramp[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * show1;
      pixel.g  = radial * show2;
      pixel.b   = radial * show3;
     
      
      

      buffer[n] = setPixelColor(pixel);
    }
    
  }


  void RGB_Blobs2() { // nice one

    get_ready(); 
                        

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.12;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.036;
    timings.ratio[5] = 0.039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.offset_z   = (sqrtf(animation.dist));// - 10 * move.ramp[0];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_x   = 10*move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 11*move.ramp[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 12*move.ramp[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1-show3);
      pixel.g  = radial * (show2-show1);
      pixel.b   = radial * (show3-show2);
     

      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs3() { // nice one

    get_ready();

                     

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.12;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.036;
    timings.ratio[5] = 0.039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.saw[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.offset_z   = (sqrtf(animation.dist));// - 10 * move.ramp[0];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ; // todo scaling seems small
      animation.offset_x   = 10*move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 11*move.ramp[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 12*move.ramp[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;//todo why?
      pixel.g  = radial * (show2+show1)*0.5;
      pixel.b   = radial * (show3+show2)*0.5;
     

      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs4() { // nice one

    get_ready();

    

                       

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.036;
    timings.ratio[5] = 0.039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.saw[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.offset_z   = 3+sqrtf(animation.dist);
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_x   = 50 * move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 50 * move.ramp[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 50 * move.ramp[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = 23;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;
      pixel.g  = radial * (show2+show1)*0.5 ;//* ledMap[n][yind]/spread_y;
      pixel.b   = radial * (show3+show2)*0.5 ;//* ledMap[n][xind]/spread_x;
     

      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs5() { // nice one

    get_ready();

    

                     

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.036;
    timings.ratio[5] = 0.039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.saw[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.offset_z   = 3+sqrtf(animation.dist);
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_x   = 50 * move.ramp[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 50 * move.ramp[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.saw[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 50 * move.ramp[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = 23;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;
      pixel.g  = radial * (show2+show1)*0.5 ;//* ledMap[n][yind]/spread_y;
      pixel.b   = radial * (show3+show2)*0.5 ;//* ledMap[n][xind]/spread_x;
     

   
      buffer[n] = setPixelColor(pixel);
    }

  }

  void Big_Caleido() { // nice one

    get_ready();

                    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 5*move.noise_angle[0] + animation.dist * 0.1;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 50 * move.ramp[0];
      animation.offset_x   = 50 * move.noise_angle[0];
      animation.offset_y   = 50 * move.noise_angle[1];
      float show1          = render_value(animation);

      animation.angle      =6 * polar_theta[n] + 5*move.noise_angle[1] + animation.dist * 0.15;
      animation.offset_z   = 50 * move.ramp[1];
      animation.offset_x   = 50 * move.noise_angle[1];
      animation.offset_y   = 50 * move.noise_angle[2];
      float show2          = render_value(animation);

      animation.angle      = 5;
      animation.scale_x    = 0.10;
      animation.scale_y    = 0.10;
      animation.scale_z    = 0.10;
      animation.offset_z   = 10 * move.ramp[2];
      animation.offset_x   = 10 * move.noise_angle[2];
      animation.offset_y   = 10 * move.noise_angle[3];
      float show3          = render_value(animation);

      animation.angle      = 15;
      animation.offset_z   = 10 * move.ramp[3];
      animation.offset_x   = 10 * move.noise_angle[3];
      animation.offset_y   = 10 * move.noise_angle[4];
      float show4          = render_value(animation);

      animation.angle      = 2;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_z   = 10 * move.ramp[4];
      animation.offset_x   = 10 * move.noise_angle[4];
      animation.offset_y   = 10 * move.noise_angle[5];
      float show5          = render_value(animation);

      
      pixel.r    = show1-show4;
      pixel.g  = show2-show5;
      pixel.b   = 10+show3-show2+show1;
     

      
      buffer[n] = setPixelColor(pixel);
    }
    //show_frame();
  }

  void SM1() { // nice one

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    for (int n = 0; n < NUM_LEDS; n++) {
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[0];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_z   = 50 * move.ramp[0];
      animation.offset_x   = 150 * move.sine[0];
      animation.offset_y   = 150 * move.sine[1];
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 4*move.noise_angle[1];
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_z   = 50 * move.ramp[1];
      animation.offset_x   = 150 * move.sine[1];
      animation.offset_y   = 150 * move.sine[2];
      float show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[2];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_z   = 50 * move.ramp[2];
      animation.offset_x   = 150 * move.sine[2];
      animation.offset_y   = 150 * move.sine[3];
      float show3          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[3];
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_z   = 50 * move.ramp[3];
      animation.offset_x   = 150 * move.sine[3];
      animation.offset_y   = 150 * move.sine[4];
      float show4          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[4];
      animation.scale_x    = 0.2;
      animation.scale_y    = 0.2;
      animation.scale_z    = 0.2;
      animation.offset_z   = 50 * move.ramp[4];
      animation.offset_x   = 150 * move.sine[4];
      animation.offset_y   = 150 * move.sine[5];
      float show5          = render_value(animation);

     

      pixel.r    = show1+show2;
      pixel.g  = show3+show4;
      pixel.b   = show5;
     

      buffer[n] = setPixelColor(pixel);
    }
    //show_frame();
  }

  void SM2() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed

    timings.ratio[0] = 0.05;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.052;
    timings.ratio[2] = 0.066;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0064;
    timings.ratio[5] = 0.0080;
    
    calculate_oscillators(timings); 

      
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        
        animation.dist       = distance[n] * (move.sine[0]);
        animation.angle      = polar_theta[n] + move.saw[0];
        animation.scale_x    = 0.09;
        animation.scale_y    = 0.09;
        animation.scale_z    = 0.09;
        animation.offset_z   = 5 * move.ramp[0];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show1          = render_value(animation);

        animation.dist       = distance[n]* move.sine[1];
        animation.angle      = polar_theta[n] + move.saw[1];
        animation.scale_x    = 0.07;
        animation.scale_y    = 0.07;
        animation.scale_z    = 0.07;
        animation.offset_z   = 5 * move.ramp[1];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show2          = render_value(animation);
        
        animation.dist       = distance[n]* move.sine[2];
        animation.angle      = polar_theta[n] + move.saw[2];
        animation.scale_x    = 0.05;
        animation.scale_y    = 0.05;
        animation.scale_z    = 0.05;
        animation.offset_z   = 5 * move.ramp[2];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show3          = render_value(animation);
       
       

        pixel.r    = show1;
        pixel.g  = show2;
        pixel.b   = show3;
       

        
        buffer[n] = setPixelColor(pixel);
      }
   //show_frame();
  }

  void SM3() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.ramp[0];;
      animation.low_limit  = -1;
      show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.ramp[0];;
      animation.low_limit  = -1;
      show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/20;
      animation.offset_y   = -4 * move.ramp[0] + show2/20;
      animation.low_limit  = 0;
      show3          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/18;
      animation.offset_y   = -4 * move.ramp[0] + show2/18;
      animation.low_limit  = 0;
      show4          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/19;
      animation.offset_y   = -4 * move.ramp[0] + show2/19;
      animation.low_limit  = 0.3;
      show5          = render_value(animation);

      pixel.r    = show4;
      pixel.g  = show3;
      pixel.b   = show5;
     

      
      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void SM4() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.offset_z   = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.ramp[0];;
      animation.low_limit  = 0;
      show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.offset_z   = 500;
      //animation.scale_x    = 0.09;
      //animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -40 * move.ramp[0];;
      animation.low_limit  = 0;
      show2          = render_value(animation);

      pixel.r    = add(show2, show1);
      pixel.g  = 0;
      pixel.b   = colordodge(show2, show1);
     

      
      buffer[n] = setPixelColor(pixel);

    }
   
  }

  void SM5() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed

    timings.ratio[0] = 0.05;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.057;
    timings.ratio[2] = 0.061;
    timings.ratio[3] = 0.0103;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0106;
    timings.ratio[5] = 0.0109;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] * (move.sine[0]);
      animation.angle      = polar_theta[n] + move.saw[0];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 5 * move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n]* move.sine[1];
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_z   = 5 * move.ramp[1];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show2          = render_value(animation);
      
      animation.dist       = distance[n]* move.sine[2];
      animation.angle      = polar_theta[n] + move.saw[2];
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 5 * move.ramp[2];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (move.sine[3]);
      animation.angle      = polar_theta[n] + move.saw[3];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 5 * move.ramp[3];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show4          = render_value(animation);

      animation.dist       = distance[n]* move.sine[4];
      animation.angle      = polar_theta[n] + move.saw[4];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_z   = 5 * move.ramp[4];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show5          = render_value(animation);
      
      animation.dist       = distance[n]* move.sine[5];
      animation.angle      = polar_theta[n] + move.saw[5];
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 5 * move.ramp[5];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show6          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
     
      pixel.r    = radial * add(show1,show4);
      pixel.g  = radial * colordodge(show2,show5);
      pixel.b   = radial * screen(show3,show6);
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM6() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed

    timings.ratio[0] = 0.05;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.057;
    timings.ratio[2] = 0.061;
    timings.ratio[3] = 0.0103;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0106;
    timings.ratio[5] = 0.0109;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.7; // zoom factor
      
      animation.dist       = distance[n] * (move.sine[0]) * s;
      animation.angle      = polar_theta[n] + move.saw[0];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 5 * move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n]* move.sine[1] * s;
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_z   = 5 * move.ramp[1];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show2          = render_value(animation);
      
      animation.dist       = distance[n]* move.sine[2] * s;
      animation.angle      = polar_theta[n] + move.saw[2];
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 5 * move.ramp[2];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (move.sine[3]) * s;
      animation.angle      = polar_theta[n] + move.saw[3];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 5 * move.ramp[3];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show4          = render_value(animation);

      animation.dist       = distance[n]* move.sine[4] * s;
      animation.angle      = polar_theta[n] + move.saw[4];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_z   = 5 * move.ramp[4];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show5          = render_value(animation);
      
      animation.dist       = distance[n]* move.sine[5] * s;
      animation.angle      = polar_theta[n] + move.saw[5];
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 5 * move.ramp[5];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show6          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      show7 = screen(show1, show4);
      show8 = colordodge(show2, show5);
      show9 = screen(show3, show6);
     
      pixel.r    = radial * (show7 + show8);
      pixel.g  = 0;
      pixel.b   = radial * show9;
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM8() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.06;    // master speed

    timings.ratio[0] = 0.      ;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.27;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      //float s = 0.7; // zoom factor
      
      animation.dist       = distance[n];
      animation.angle      = 2;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_z   = 0;
      animation.offset_y   = 5 * move.ramp[0];
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 2;
      animation.offset_z   = 150;
      animation.offset_x   = -5 * move.ramp[0];     
      float show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.offset_z   = 550;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_x   = 0;
      animation.offset_y   = -5 * move.ramp[1];
      float show4          = render_value(animation);      

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.offset_z   = 1250;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.scale_z    = 0.15;
      animation.offset_x   = 0;
      animation.offset_y   = 5 * move.ramp[1];
      float show5          = render_value(animation);      
      
     

      //float radius = radial_filter_radius;   // radius of a radial brightness filter
      //float radial = (radius-distance[n])/distance[n];

      show3 = add(show1, show2);
      show6 = screen(show4, show5);
      //show9 = screen(show3, show6);
     
      pixel.r    = show3;
      pixel.g  = 0;
      pixel.b   = show6; // todo this one is SOOOO cool but too monochrome
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM9() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.005;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_y   = -30 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      //animation.scale_x    = 0.09;
      //animation.scale_y    = 0.09;
      animation.offset_y   = -30 * move.ramp[1];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show2                = render_value(animation);

      animation.dist       = distance[n];// + show1/64;
      animation.angle      = polar_theta[n] + 2 + (show1 / 255) * PI;
      //animation.scale_x    = 0.09;
      //animation.scale_y    = 0.09;
      animation.offset_y   = -10 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2 +(show2 / 255) * PI;;
      //animation.scale_x    = 0.09;
      //animation.scale_y    = 0.09;
      animation.offset_y   = -20 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show5, show3);

      //float sy2 =spread_y*2.0;


      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = show5 * radial;
      pixel.g  = 0;
      pixel.b   = show6 * radial; // todo again too monochrome
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void SM10() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.006;    // 0.006

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float scale = 0.6;
      
      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.09 * scale;
      animation.scale_y    = 0.09 * scale;
      animation.scale_z    = 0.09 * scale;
      animation.offset_y   = -30 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      //animation.scale_x    = 0.09 * scale;
      //animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -30 * move.ramp[1];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show2                = render_value(animation);

      animation.dist       = distance[n];// + show1/64;
      animation.angle      = polar_theta[n] + 2 + (show1 / 255) * PI;
      //animation.scale_x    = 0.09 * scale;
      //animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -10 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2 +(show2 / 255) * PI;;
      //animation.scale_x    = 0.09 * scale;
      //animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -20 * move.ramp[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show5, show3);


      pixel.r    = (show5+show6)/2;
      pixel.g  = (show5-50)+(show6/16);
      pixel.b   = show6;
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido() { 



    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.009;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    //float size = 1.5;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 10 * move.saw[0] + animation.dist /2;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.07;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.ramp[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.saw[1] + animation.dist /2;
      //animation.scale_x    = 0.07;
      //animation.scale_y    = 0.07;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.ramp[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.saw[2] + animation.dist /2;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.scale_z    = 0.05;
      animation.offset_z   = 0;
      animation.offset_x   = -40 * move.ramp[2];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.saw[3] + animation.dist /2; // todo scale the kaleido complexity  for different setups?
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.scale_z    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.ramp[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show2, show3);



      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial*(show1+show2);
      pixel.g  = 0.3*radial*show6;//(radial*(show1))*0.3f;
      pixel.b   = radial*show5;
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_2() { 



    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.009;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    float size = 0.5;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 10 * move.saw[0] + animation.dist /2;
      animation.scale_x    = 0.07 * size;
      animation.scale_y    = 0.07 * size;
      animation.scale_z    = 0.07 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.ramp[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.saw[1] + animation.dist /2;
      //animation.scale_x    = 0.07 * size;
      //animation.scale_y    = 0.07 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.ramp[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.saw[2] + animation.dist /2;
      animation.scale_x    = 0.05 * size;
      animation.scale_y    = 0.05 * size;
      animation.scale_z    = 0.05 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -40 * move.ramp[2];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.saw[3] + animation.dist /2;
      animation.scale_x    = 0.09 * size;
      animation.scale_y    = 0.09 * size;
      animation.scale_z    = 0.09 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.ramp[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show2, show3);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial*(show1+show2);
      pixel.g  = 0.3*radial*show6;//(radial*(show1))*0.3f;
      pixel.b   = radial*show5;
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Complex_Kaleido_3() { 



    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.001;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0037;
    timings.ratio[5] = 0.0038;
    timings.ratio[5] = 0.0041;
    
    calculate_oscillators(timings); 

    float size = 0.4 + move.sine[0]*0.1;

    float q = 2;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 10 * move.saw[0] + animation.dist / (((move.sine[0] + 3)*2)) + move.noise_angle[0]*q;
      animation.scale_x    = 0.08 * size * (move.sine[0]+1.5);
      animation.scale_y    = 0.07 * size;
      animation.scale_z    = 0.07 * size;
      animation.offset_z   = -10 * move.ramp[0];
      animation.offset_x   = -3.20 * move.ramp[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 10 * move.saw[1] + animation.dist / (((move.sine[1] + 3)*2))+ move.noise_angle[1]*q;
      animation.scale_x    = 0.07 * size * (move.sine[1]+1.1);
      animation.scale_y    = 0.07 * size * (move.sine[2]+1.3);
      animation.scale_z    = 0.07 * size * (move.sine[2]+1.5);
      animation.offset_z   = -12 * move.ramp[1];
      animation.offset_x   = -3.1 * move.ramp[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.saw[2] + animation.dist /(((move.sine[3] + 3)*2))+ move.noise_angle[2]*q;
      animation.scale_x    = 0.05 * size * (move.sine[3]+1.5);
      animation.scale_y    = 0.05 * size * (move.sine[4]+1.5);
      animation.scale_z    = 0.05 * size * (move.sine[4]+1.5);
      animation.offset_z   = -12 * move.ramp[3];
      animation.offset_x   = -4.0 * move.ramp[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.saw[3] + animation.dist /(((move.sine[5] + 3)*2))+ move.noise_angle[3]*q;
      animation.scale_x    = 0.09 * size * (move.sine[5]+1.5);
      animation.scale_y    = 0.09 * size * (move.sine[6]+1.5);
      animation.scale_z    = 0.09 * size * (move.sine[6]+1.5);
      animation.offset_z   = 0;
      animation.offset_x   = -3.5 * move.ramp[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3)-show2;
      show6 = colordodge(show4, show1);

      show7 = multiply(show1, show2);
      
      float linear1 = ledMap[n][yind] / (spread_y*2);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      show7 = multiply(show1, show2) * linear1*2; // todo should this be radial?  at least it should be normalized?
      show8 = subtract(show7, show5);

      //pixel.r    = radial*(show1+show2);
      pixel.g  = 0.2*show8;//(radial*(show1))*0.3f;
      pixel.b   = show5 * radial; // todo flickering.... attemting to slow by 10/2 (deleted 0 in osc added *2 in master timing)
      pixel.r    = (1*show1 + 1*show2) - show7/2; 
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_4() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // master speed 0.01 in the video

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.038;
    timings.ratio[6] = 0.041;
    
    calculate_oscillators(timings); 

    float size = 0.6;

    float q = 1;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 1 +  move.sine[6]*0.3;

      animation.dist       = distance[n] * s;
      animation.angle      = 5 * polar_theta[n] + 1 * move.saw[0] - animation.dist / (3+move.sine[0]*0.5);
      animation.offset_z   = 5;
      animation.scale_x    = 0.08 * size + (move.sine[0]*0.01);
      animation.scale_y    = 0.07 * size + (move.sine[1]*0.01);
      animation.scale_z    = 0.07 * size + (move.sine[1]*0.01);
      animation.offset_z   = -10 * move.ramp[0];
      animation.offset_x   = 0;//-30 * move.ramp[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n] * s;
      animation.angle      = 5 * polar_theta[n] + 1 * move.saw[1] + animation.dist / (3+move.sine[1]*0.5);
      animation.scale_x    = 0.08 * size + (move.sine[1]*0.01);
      animation.scale_y    = 0.07 * size + (move.sine[2]*0.01);
      animation.scale_z    = 0.07 * size + (move.sine[2]*0.01);
      animation.offset_z   = -10 * move.ramp[1];
      animation.offset_x   = 0;//-30 * move.ramp[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.scale_x    = 0.2 * size ;
      animation.scale_y    = 0.2 * size ;
      animation.scale_z    = 0.2 * size ;
      animation.offset_z   = 0;//-12 * move.ramp[3];
      animation.offset_y   = +7 * move.ramp[3] +  move.noise_angle[3];
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.saw[3] + animation.dist /(((move.sine[5] + 3)*2))+ move.noise_angle[3]*q;
      animation.scale_x    = 0.09 * size * (move.sine[5]+1.5);
      animation.scale_y    = 0.09 * size * (move.sine[6]+1.5);
      animation.scale_z    = 0.09 * size * (move.sine[6]+1.5);
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.ramp[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      //show5 = screen(show4, show3)-show2;
      //show6 = colordodge(show4, show1);

      //show7 = multiply(show1, show2);


      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];


      show5 = ((show1 + show2)) - show3;
      if (show5>255) show5=255;
      if (show5<0) show5=0;

      show6 = colordodge(show1, show2);

      pixel.r    = show5 * radial;
      pixel.b   = (64-show5-show3) * radial;
      pixel.g  = 0.5*(show6);

     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_5() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm)*2;// was: 0.03;    // master speed 

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.0038;
    timings.ratio[6] = 0.041;
    
    calculate_oscillators(timings); 

    float size = 0.6;

    //float q = 1;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 1 +  move.sine[6]*0.8;

      animation.dist       = distance[n] * s;
      animation.angle      = move.saw[6] + 2 * move.sine[5] * polar_theta[n]  - animation.dist / 3;
      animation.scale_x    = 0.08 * size ;
      animation.scale_y    = 0.07 * size ;
      animation.scale_z    = 0.07 * size ;
      animation.offset_z   = -10 * move.ramp[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      animation.low_limit  = -0.5;
      show1                = render_value(animation);

     
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
      //12-10/10,  12-1/10,  brighter in center
     
      uint8_t color = float_to_uint8_t(show1 * radial); //todo see if top clip messed this up?
      buffer[n] = setPixelColor(color);
    }
  }

  void Complex_Kaleido_6() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // master speed 

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.0038;
    timings.ratio[6] = 0.041;
    
    calculate_oscillators(timings); 

    

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = 16 * polar_theta[n] + 16*move.saw[0];
      animation.scale_x    = 0.06 ;
      animation.scale_y    = 0.06 ;
      animation.scale_z    = 0.06 ;
      animation.offset_z   = -10 * move.ramp[0];
      animation.offset_y   = 10 * move.noise_angle[0];
      animation.offset_x   = 10 * move.noise_angle[4];
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 4 * polar_theta[n] + 16*move.saw[1];
      //animation.scale_x    = 0.06 ;
      //animation.scale_y    = 0.06 ;
      animation.offset_z   = -10 * move.ramp[1];
      animation.offset_y   = 10 * move.noise_angle[1];
      animation.offset_x   = 10 * move.noise_angle[3];
      animation.low_limit  = 0;
      show2                = render_value(animation);

     
      //float radius = radial_filter_radius;   // radius of a radial brightness filter
      //float radial = (radius-distance[n])/distance[n];
     
      pixel.r    = show1;
      pixel.b   = show2;
      pixel.g   = 0; //todo
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Water() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.037;    // master speed 

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.1;
    timings.ratio[6] = 0.41;
    
    calculate_oscillators(timings); 


    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] + 4*sinf(move.sine[5]*PI+(float)ledMap[n][xind]/2) + 4 * cosf(move.sine[6]*PI+float(ledMap[n][yind])/2);
      animation.angle      = 1 * polar_theta[n];
      animation.scale_x    = 0.06 ;
      animation.scale_y    = 0.06 ;
      animation.scale_z    = 0.06 ;
      animation.offset_z   = -10 * move.ramp[0];
      animation.offset_y   = 10;
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = (10+move.sine[0]) * sinf(-move.saw[5]+move.saw[0]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (10+move.sine[1]) * sinf(-move.saw[5]+move.saw[1]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      //animation.scale_x    = 0.1 ;
      //animation.scale_y    = 0.1 ;
      //animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = (10+move.sine[2]) * sinf(-move.saw[5]+move.saw[2]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      //animation.scale_x    = 0.1 ;
      //animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show4                = render_value(animation);


      
     
      //float radius = radial_filter_radius;   // radius of a radial brightness filter
      //float radial = (radius-distance[n])/distance[n];
     
      //pixel.r    = show2;
      
      pixel.b    = (0.7*show2+0.6*show3+0.5*show4);
      pixel.r     = pixel.b-40;
      //pixel.r     = radial*show3;
      pixel.g     = 0;//0.9*radial*show4;
      
     
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Parametric_Water() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.003;    // master speed 

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.15;    // wave speed
    timings.ratio[6] = 0.41;
    
    calculate_oscillators(timings); 


    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 4;
      float f = 10 + 2* move.sine[0];

      animation.dist       = (f+move.sine[0]) * sinf(-move.saw[5]+move.saw[0]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (f+move.sine[1]) * sinf(-move.saw[5]+move.saw[1]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      //animation.scale_x    = 0.1 ;
      //animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = (f+move.sine[2]) * sinf(-move.saw[5]+move.saw[2]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      //animation.scale_x    = 0.1 ;
      //animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      animation.dist       = (f+move.sine[3]) * sinf(-move.saw[5]+move.saw[3]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      //animation.scale_x    = 0.1 ;
      //animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[3];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show5                = render_value(animation);

      show6 = screen(show4, show5);
      show7 = screen(show2, show3);


      
     
      float radius = 40;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/radius;
     
      //pixel.r    = show6;
      //pixel.b = show7;
      
      pixel.b    = (0.3*show6+0.7*show7)*radial;
      pixel.r     = pixel.b-40;

      
      
     
     

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment1() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] + 20 * move.sine[0];
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      uint8_t color = float_to_uint8_t(show1);
      buffer[n] = setPixelColor( color);
    }
  }

  void Module_Experiment2() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.02;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] - ( 16 + move.sine[0] * 16);
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);
      
      pixel.r    = show1;
      pixel.g  = show1 - 80;
      pixel.b   = show1 - 150;
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment3() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] - (12 + move.sine[3]*4);
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.scale_z    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);
      
      pixel.r    = show1;
      pixel.g  = show1 - 80;
      pixel.b   = show1 - 150;
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Module_Experiment4() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm)*3;// was: 0.031;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;
    timings.ratio[4] = 0.036;            // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.8;

      animation.dist       = (distance[n] * distance[n])*0.7;
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.scale_z    = 0.003 * s;
      animation.offset_z   = 0.1*move.ramp[2] ;
      animation.offset_y   = -20 * move.ramp[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = (distance[n] * distance[n])*0.8;
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.scale_z    = 0.003 * s;
      animation.offset_z   = 0.1*move.ramp[3] ;
      animation.offset_y   = -20 * move.ramp[3];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (distance[n] * distance[n])*0.9;
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.scale_z    = 0.003 * s;
      animation.offset_z   = 0.1*move.ramp[4] ;
      animation.offset_y   = -20 * move.ramp[4];
      animation.offset_x   = 1000;
      animation.low_limit  = 0;
      show3                = render_value(animation);


      // overlapping color mapping
      /*
      float r = show1;
      float g = show2-show1;
      float b = show3-show1-show2;
      */      
      
      pixel.r    = show1-show2-show3;
      pixel.b   = show2-show1-show3;
      pixel.g  = show3-show1-show2;
      //pixel.g  = b;
      //pixel.g  = show1 - 80;
      //pixel.b   = show1 - 150;
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment5() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.031;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[3] = 0.33;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 1.5;

      animation.dist       = distance[n] + sinf(0.5*distance[n]-move.saw[3]);
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[0] ;
      animation.offset_y   = -20 * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      uint8_t color = float_to_uint8_t(show1); //todo no hue shift
      buffer[n] = setPixelColor( color); // todo why is this soo much brighter than all others?  also very black?

    }
  }

  void Module_Experiment6() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // master speed 0.031

    float w = 0.7;

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.8;

      animation.dist       = distance[n] + sinf(0.25*distance[n]-move.saw[3]);
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[0] ;
      animation.offset_y   = -20 * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n] + sinf(0.24*distance[n]-move.saw[4]);
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[1] ;
      animation.offset_y   = -20 * move.ramp[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */

      pixel.r    = (show1 + show2);
      pixel.g  = ((show1 + show2)*0.6)-30;
      pixel.b   = 0;
      
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment7() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.005;    // master speed 0.031

    float w = 0.3;

    timings.ratio[0] = 0.01;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.7;

      animation.dist       = 2+distance[n] + 2*sinf(0.25*distance[n]-move.saw[3]);
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 10*move.ramp[0] ;
      animation.offset_y   = -20 * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 2+distance[n] + 2*sinf(0.24*distance[n]-move.saw[4]);
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[1] ;
      animation.offset_y   = -20 * move.ramp[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */
      
      pixel.r    = (show1 + show2);
      pixel.g  = ((show1 + show2)*0.6)-50;
      pixel.b   = 0;
      
      
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment8() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // master speed 0.031

    float w = 0.3;

    timings.ratio[0] = 0.01;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.013;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[5] = 0.38*w; 
    timings.ratio[6] = 0.0003;  // master rotation

    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    

    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.4; // scale
      float r = 1.5; // scroll speed

      animation.dist       = 3+distance[n] + 3*sinf(0.25*distance[n]-move.saw[3]);
      animation.angle      = polar_theta[n] + move.noise_angle[0] + move.noise_angle[6];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 10*move.ramp[0] ;
      animation.offset_y   = -5 * r * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 4+distance[n] + 4*sinf(0.24*distance[n]-move.saw[4]);
      animation.angle      = polar_theta[n] + move.noise_angle[1] + move.noise_angle[6];
      //animation.scale_x    = 0.1 * s;
      //animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[1] ;
      animation.offset_y   = -5 * r * move.ramp[1];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = 5+distance[n] + 5*sinf(0.23*distance[n]-move.saw[5]);
      animation.angle      = polar_theta[n] + move.noise_angle[2] + move.noise_angle[6];
      //animation.scale_x    = 0.1 * s;
      //animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[2] ;
      animation.offset_y   = -5 * r * move.ramp[2];
      animation.offset_x   = 1000;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      show4 = colordodge(show1, show2);

      float rad = sinf(PI/2+distance[n]/14); // todo better radial filter?!

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */
      
      pixel.r    = rad * ((show1 + show2) + show3);
      pixel.g  = (((show2 + show3)*0.8)-90)*rad;
      pixel.b   = show4 * 0.2;
      
      
      

      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment9() { 

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed 0.031
    float w = 0.3;

    timings.ratio[0] = 0.1;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.013;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[5] = 0.38*w; 
    timings.ratio[6] = 0.0003;  

    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      uint8_t color = float_to_uint8_t(show1); // todo no hue shift
      buffer[n] = setPixelColor( color); // todo hue shift????  todo, these all seem WWWAAAAY brighter than others

    }
  }

  void SPARKLE_EDGES_MOD9() {  //TODO why did this break???

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.03;    // master speed 0.031
    float w = 0.3;

    timings.ratio[0] = 0.1;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.013;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[5] = 0.38*w; 
    timings.ratio[6] = 0.0003;  

    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.saw[1];
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.ramp[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      uint8_t color = round(100*show1);
      buffer[n] = setPixelColor( color);

    }
  }


  void Module_Experiment10() {  // todo brightness

    get_ready();

    

    timings.master_speed = bpmToSpeedMillis(global_bpm)/9;// was: 0.01;    // master speed 0.031



    timings.ratio[0] = 0.01;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.013;
    timings.ratio[3] = 0.33;
    timings.ratio[4] = 0.36;            // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[5] = 0.38; 
    timings.ratio[6] = 0.0003;  // master rotation

    timings.offset[0] = 0;
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    timings.offset[4] = 400;
    timings.offset[5] = 500;
    timings.offset[6] = 600;
    

    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.4; // scale
      float r = 1.5; // scroll speed

      animation.dist       = 3+distance[n] + 3*sinf(0.25*distance[n]-move.saw[3]);
      animation.angle      = polar_theta[n] + move.noise_angle[0] + move.noise_angle[6];
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.scale_z    = 0.1 * s;
      animation.offset_z   = 10*move.ramp[0] ;
      animation.offset_y   = -5 * r * move.ramp[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 4+distance[n] + 4*sinf(0.24*distance[n]-move.saw[4]);
      animation.angle      = polar_theta[n] + move.noise_angle[1] + move.noise_angle[6];
      //animation.scale_x    = 0.1 * s;
      //animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[1] ;
      animation.offset_y   = -5 * r * move.ramp[1];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = 5+distance[n] + 5*sinf(0.23*distance[n]-move.saw[5]);
      animation.angle      = polar_theta[n] + move.noise_angle[2] + move.noise_angle[6];
      //animation.scale_x    = 0.1 * s;
      //animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.ramp[2] ;
      animation.offset_y   = -5 * r * move.ramp[2];
      animation.offset_x   = 1000;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      //show4 = colordodge(show1, show2);

      //float rad = sinf(PI/2+distance[n]/14); // better radial filter?!

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */

      byte a = millis()/100;
      pixel_hsi.s = 1.0;
      pixel_hsi.i = 255;
      pixel_hsi.h = (a + show1 + show2) + show3;

      
      buffer[n] = setPixelColor(pixel_hsi);
    }
  }



  void Rings() {// todo scale

    get_ready(); 
    

    timings.master_speed = bpmToSpeedMillis(global_bpm);// was: 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 1;         // higher values = faster transitions
    timings.ratio[1] = 1.1;
    timings.ratio[2] = 1.2;
    
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
     
      // describe and render animation layers
      animation.angle      = 5;
      animation.scale_x    = 0.2;
      animation.scale_y    = 0.2;
      animation.scale_z    = 0.3;
      animation.dist       = distance[n];
      animation.offset_y   = -move.ramp[0];
      animation.offset_x   = 0;
      float show1          = render_value(animation);

       // describe and render animation layers
      animation.angle      = 10;
      
      animation.dist       = distance[n];
      animation.offset_y   = -move.ramp[1];
      float show2          = render_value(animation);

       // describe and render animation layers
      animation.angle      = 12;
      
      animation.dist       = distance[n];
      animation.offset_y   = -move.ramp[2];
      float show3          = render_value(animation);

      // colormapping
      pixel.r   = show1;
      pixel.g = show2 / 4;
      pixel.b  = show3 / 4;



      buffer[n] = setPixelColor(pixel);
    }
  }


}; 
