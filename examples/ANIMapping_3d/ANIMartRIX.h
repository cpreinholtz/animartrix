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

//#include <SmartMatrix.h>
#include <FastLED.h>
#include "custom_palettes.h"
// from AeroKeiths repo ColorUtilsHsi
#include <ColorUtilsHsi.h>
#include <plane3d.h>
#include <sphere3d.h>
#define HSI_SCALE 1.0
#define RGB_SCALE_FACTORS {255.0, 255.0, 255.0}

#define num_oscillators 10

#ifndef NUM_LEDS
  #define NUM_LEDS 50
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
  #define zind 1
#endif
extern float ledMap[NUM_LEDS][3]; // TODO, make this better...


#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))



/////////////////////////////////////////////////////////////////////////////


struct render_parameters {

  float center_x = (sqrt(NUM_LEDS)/2) - 0.5;   // center of the matrix
  float center_y = (sqrt(NUM_LEDS)/2) - 0.5;
  float center_z = 0;
  float dist, angle, anglephi;                
  float scale_x = 0.1;                  // smaller values = zoom in
  float scale_y = 0.1;
  float scale_z = 0.1;       
  float offset_x, offset_y, offset_z;     
  float z;  
  float low_limit  = 0;                 // getting contrast by highering the black point
  float high_limit = 1;                                            
};

render_parameters animation;     // all animation parameters in one place
struct oscillators {

  float master_speed;            // global transition speed
  float offset[num_oscillators]; // oscillators can be shifted by a time offset
  float ratio[num_oscillators];  // speed ratios for the individual oscillators                                  
};

oscillators timings;             // all speed settings in one place

struct modulators {  

  float linear[num_oscillators];        // returns 0 to FLT_MAX
  float radial[num_oscillators];        // returns 0 to 2*PI
  float directional[num_oscillators];   // returns -1 to 1
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

  float spread_x, spread_y, spread_z, xmin, xmax, ymin, ymax, zmin, zmax, center_x, center_y, center_z, maxD;

  #define radial_filter_radius 12.0;      // 23.0 on 32x32, use 11 for 16x16, for mapping, try like 130% of your max radias

  CRGB* buffer; 
  bool  serpentine;

  // TODO set sizes
  float polar_theta[NUM_LEDS];        // look-up table for polar angles
  float spherical_phi[NUM_LEDS];
  float distance[NUM_LEDS];           // look-up table for polar distances TODO

  unsigned long a, b, c;                  // for time measurements
  float global_scale = 1.0;

  uint16_t BeatsPerMinute; //u8.8 fixed point

  float show1, show2, show3, show4, show5, show6, show7, show8, show9, show0;

  ANIMartRIX() {}

  ANIMartRIX(struct CRGB *data) {
    this->init(data);
  }

  ANIMartRIX(struct CRGB *data, float scale){  //use num leds as width, and set height to 1, serpintine false
    this->init(data, scale);
  }

      

  void init(struct CRGB *data, float scale = 1.0) {
    this->buffer = data;
    this->serpentine = serpentine;
    setBpm(82.0);
    setGlobalScale(scale);
    render_spherical_lookup_table();

  }

  void setGlobalScale(float setTo){
    if (setTo <=0) setTo = 0.0000001;
    global_scale = setTo;
  }

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

    double runtime = millis() * timings.master_speed;  // global anaimation speed

    for (int i = 0; i < num_oscillators; i++) {
      
      move.linear[i]      = (runtime + timings.offset[i]) * timings.ratio[i];     // continously rising offsets, returns              0 to max_float
      
      move.radial[i]      = fmodf(move.linear[i], 2 * PI);                        // angle offsets for continous rotation, returns    0 to 2 * PI
      
      move.directional[i] = sinf(move.radial[i]);                                 // directional offsets or factors, returns         -1 to 1
      
      move.noise_angle[i] = PI * (1 + pnoise(move.linear[i], 0, 0));              // noise based angle offset, returns                0 to 2 * PI
      
    }
  }


  void run_default_oscillators(){

    timings.master_speed = 0.005;    // master speed

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

    calculate_oscillators(timings);  
  }


  // Convert the 2 polar coordinates back to cartesian ones & also apply all 3d transitions.
  // Calculate the noise value at this point based on the 5 dimensional manipulation of 
  // the underlaying coordinates.

  /////////////////////////////////////////////////////////////////////////
  //Rendering
  /////////////////////////////////////////////////////////////////////////

  float render_value(render_parameters &animation, float scaleHigh = 255.0) {
    //EVERY_N_SECONDS(1){Serial.println("derrived class render");}

    // convert **SPHERICAL** coordinates back to cartesian ones
    //this is really the only difference from base class
    float newx = (animation.offset_x + animation.center_x - (animation.dist * sinf(animation.anglephi) * cosf(animation.angle))) * animation.scale_x * global_scale;
    float newy = (animation.offset_y + animation.center_y - (animation.dist * sinf(animation.anglephi) * sinf(animation.angle))) * animation.scale_y * global_scale;
    float newz = (animation.offset_z + animation.center_z - (animation.dist * cosf(animation.anglephi))) * animation.scale_z * global_scale;

    // render noisevalue at this new cartesian point
    //uint16_t raw_noise_field_value =inoise16(newx, newy, newz);
    float raw_noise_field_value = pnoise(newx, newy, newz);

    // A) enhance histogram (improve contrast) by setting the black and white point (low & high_limit)
    // B) scale the result to a 0-255 range (assuming you want 8 bit color depth per rgb chanel)
    // Here happens the contrast boosting & the brightness mapping

    if (raw_noise_field_value < animation.low_limit)  raw_noise_field_value =  animation.low_limit;
    if (raw_noise_field_value > animation.high_limit) raw_noise_field_value = animation.high_limit;

    float scaled_noise_value = map_float(raw_noise_field_value, animation.low_limit, animation.high_limit, 0, scaleHigh);

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
      if(ledMap[n][xind] > xmin) xmin = ledMap[n][xind];
      if(ledMap[n][xind] < xmax) xmax = ledMap[n][xind];
      if(ledMap[n][yind] > ymin) ymin = ledMap[n][yind];
      if(ledMap[n][yind] < ymax) ymax = ledMap[n][yind];
      if(ledMap[n][zind] > zmin) zmin = ledMap[n][zind];
      if(ledMap[n][zind] < zmax) zmax = ledMap[n][zind];
    }
    spread_x = xmax-xmin;
    spread_y = ymax-ymin;
    spread_z = zmax-zmin;

    center_x = (spread_x)/2.0 + xmin;
    center_y = (spread_y)/2.0 + ymin;
    center_z = (spread_z)/2.0 + zmin;
    
    Serial.print("x center: ");Serial.println(center_x);
    Serial.print("y center: ");Serial.println(center_y);
    Serial.print("z center: ");Serial.println(center_z); Serial.println();


    maxD = 0.0;
    for (int n = 0; n < NUM_LEDS; n++) {
        float dx = ledMap[n][xind] - center_x;
        float dy = ledMap[n][yind] - center_y;
        float dz = ledMap[n][zind] - center_z;
        
        distance[n] = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        polar_theta[n] = atan2f(dy,dx); //todo these extra indecies are wasted
        spherical_phi[n] = acosf(dz / distance[n]);

        if (maxD < distance[n]) maxD = distance[n];
        Serial.print("x: ");Serial.println(dx);
        Serial.print("y: ");Serial.println(dy);
        Serial.print("z: ");Serial.println(dz); 

        Serial.print("d: ");Serial.println(distance[n]); // note this index must match what you gave the base class template,  I'm using Y
        Serial.print("t: ");Serial.println(polar_theta[n]);
        Serial.print("p: ");Serial.println(spherical_phi[n]); Serial.println();
        
        
    }
    Serial.print("max mapped distance: "); Serial.println(maxD);
    Serial.print("reccomended radias filter: "); Serial.println(maxD*1.3);
  }



  /////////////////////////////////////////////////////////////////////////
  //Utilities, map, sanity check
  /////////////////////////////////////////////////////////////////////////

  // float mapping maintaining 32 bit precision
  // we keep values with high resolution for potential later usage

  float map_float(float x, float in_min, float in_max, float out_min, float out_max) { 
    
    float result = (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
    if (result < out_min) result = out_min;
    if( result > out_max) result = out_max;

    return result; 
  }



  // Avoid any possible color flicker by forcing the raw RGB values to be 0-255.
  // This enables to play freely with random equations for the colormapping
  // without causing flicker by accidentally missing the valid target range.

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
  }

  float color_sanity_check(float &color) {
        // discard everything above the valid 8 bit colordepth 0-255 range
        if (color   > 255)   color = 255;
        return color;
  }

  uint8_t float_to_uint8_t(float color){
    color = color_sanity_check(color);
    return ((uint8_t) color);
  }


  hsiF hsi_sanity_check(hsiF &pixel) {

      // rescue data if possible, return absolute value
      //if (pixel.red < 0)     pixel.red = fabsf(pixel.red);
      //if (pixel.green < 0) pixel.green = fabsf(pixel.green);
      //if (pixel.blue < 0)   pixel.blue = fabsf(pixel.blue);
      
      if (pixel.h   > 1.0)   pixel.h = 1.0;
      if (pixel.s > 1.0) pixel.s = 1.0;
      if (pixel.i  > 1.0)  pixel.i = 1.0;

      return pixel;
  }






  void get_ready() {  // wait until new buffer is ready, measure time
    // TODO: make callback
    // a = micros(); 
    // while(backgroundLayer.isSwapPending());
    // b = micros(); 
  }


  CRGB setPixelColor(rgbF pixel) {
    return CRGB(pixel.r, pixel.g, pixel.b);
  }



  // show current framerate and rendered pixels per second
  void report_performance() {
   
    int fps = FastLED.getFPS();                 // frames per second
    int kpps = (fps * NUM_LEDS) / 1000;   // kilopixel per second
  
    Serial.print(kpps); Serial.print(" kpps ... ");
    Serial.print(fps); Serial.print(" fps @ ");
    Serial.print(NUM_LEDS); Serial.println(" LEDs ... ");
  }

  // Show the current framerate, rendered pixels per second,
  // rendering time & time spend to push the data to the leds.
  // in the serial monitor.
  void report_performance2() {
    
    float calc  = b - a;                         // waiting time
    float push  = c - b;                         // rendering time
    float total = c - a;                         // time per frame
    int fps  = 1000000 / total;                // frames per second
    int kpps = (fps * NUM_LEDS) / 1000;   // kilopixel per second

    Serial.print(fps);                         Serial.print(" fps  ");
    Serial.print(kpps);                        Serial.print(" kpps @");
    Serial.print(NUM_LEDS);                 Serial.print(" LEDs  ");  
    Serial.print(round(total));                Serial.print(" µs per frame  waiting: ");
    Serial.print(round((calc * 100) / total)); Serial.print("%  rendering: ");
    Serial.print(round((push * 100) / total)); Serial.print("%  (");
    Serial.print(round(calc));                 Serial.print(" + ");
    Serial.print(round(push));                 Serial.print(" µs)  Core-temp: ");
    // TODO Serial.print( tempmonGetTemp() );
              Serial.println(" °C");
   
  }


  /////////////////////////////////////////////////////////////////////////////////////////////////////
  // Effects, written for animapping...

  void setBpm(float bpm){
    if (bpm > 255.0) bpm = 255.0;
    else if (bpm < 0.0) bpm = 0.0;

    Serial.println("desired BeatsPerMinute: ");
    Serial.println(bpm);
    bpm = bpm*256.0;
    Serial.println("scaled BeatsPerMinute fixed point: ");
    Serial.println(bpm);
    BeatsPerMinute = (uint16_t) bpm; //u8.8 fixed point
    Serial.println("BeatsPerMinute: ");
    Serial.println(BeatsPerMinute);
  }

  void demoBpm()
  {
    static uint8_t gHue = 0;
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    CRGBPalette16 palette = HeatColors_p    ;
    uint16_t beat = beatsin88( BeatsPerMinute, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
      buffer[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    }
  }

  void demoBpm2()
  {
    static uint8_t gHue = 0;
    EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
    CRGBPalette16 palette = PartyColors_p;
    uint8_t beat = beatsin8( 62, 64, 255);
    for( int i = 0; i < NUM_LEDS; i++) { //9948
      buffer[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
    }
  }


  void Module_Experiment11_Hsi() { 
    get_ready();

    //nscale might provide smoother fade?
    fadeToBlackBy( buffer, NUM_LEDS, 1);

    

    timings.master_speed = 0.03;    // master speed 0.031

    float w = 0.3;

    timings.ratio[0] = 0.1;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.011;
    timings.ratio[2] = 0.013;
    timings.ratio[3] = 0.33*w;
    timings.ratio[4] = 0.36*w;            // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[5] = 0.38*w; 
    timings.ratio[6] = 0.0003;  

    calculate_oscillators(timings); 
    float cutoff = 0.5;
    float scale = cutoff *4;
    
    for (int n = 0; n < NUM_LEDS; n++) { 


      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + move.radial[1];
      animation.z          = 5;
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.linear[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation, scale);


      pixel_hsi.h    = show1-cutoff;
      pixel_hsi.s = 1.0;
      
      if(show1<cutoff) {
        if (random16(1000) > 1000-50.0*show1){
          pixel_hsi.h = random16(100) / 100.0;
          pixel_hsi.s = 0.0 + random16(50) / 100.0;
          pixel_hsi.i = 0.30;
          pixel_hsi = hsi_sanity_check(pixel_hsi);
          pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
          pixel = rgb_sanity_check(pixel);
          buffer[n] = setPixelColor(pixel);
        } else {
          pixel_hsi.i = 0.0;
          pixel_hsi = hsi_sanity_check(pixel_hsi);
          pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
          pixel = rgb_sanity_check(pixel);
          buffer[n] += setPixelColor(pixel);
        }
      //not black, use this
      } else {
        pixel_hsi.i = 0.3;
        pixel_hsi = hsi_sanity_check(pixel_hsi);
        pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
        pixel = rgb_sanity_check(pixel);
        buffer[n] = setPixelColor(pixel);
      }
    }
    //this prevents the fade from happening too fast
    FastLED.delay(6);
    
  }


  void Module_Experiment9_Hsi() {
    get_ready();

    timings.master_speed = 0.03;    // master speed 0.031

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
      animation.angle      = polar_theta[n] + move.radial[1];
      animation.z          = 5;
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.linear[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation, 2.0);

      pixel_hsi.h    = show1;
      pixel_hsi.s = 1.0;
      pixel_hsi.i = 0.3;

    
      pixel_hsi = hsi_sanity_check(pixel_hsi);

      pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
      pixel = rgb_sanity_check(pixel);


      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Chasing_Spirals_Hsi() {

    get_ready(); 
    static Plane3d myPlane;

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.5;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.16;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;


    
    calculate_oscillators(timings);     // get linear movers and oscillators going



    myPlane.yaw(.001 * move.noise_angle[0]);
    myPlane.setRefpoint(center_x+spread_x*move.directional[1]+move.noise_angle[0]/2.0, center_y+spread_y*move.directional[2]/2.0, center_z);
    
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = polar_theta[n];//4 * polar_theta[n] +  move.radial[0] ;//- distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_z    = 0.1;  
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.offset_x   = move.linear[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        animation.z          = 0;
        float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[1];
        //float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[2];
        //float show3          = render_value(animation);

        // colormapping
        float radius = radial_filter_radius;
        float radial_filter = (radius - distance[n]) / radius;



        pixel_hsi.i = 0.5;//6.0/255.0 *show1 * radial_filter;
        pixel_hsi.s = 1;

        float d = myPlane.distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        d=fmodf(d,maxD);
        pixel_hsi.h = map_float(d, -maxD/2, maxD/2, 0, 1);

        pixel_hsi = hsi_sanity_check(pixel_hsi);

        pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
        
        //pixel.r   = 3*show1 * radial_filter;
        //pixel.g=0;
        //pixel.b=0;

        //pixel.g = show2 * radial_filter / 2;
        //pixel.b  = show3 * radial_filter / 4;


        pixel = rgb_sanity_check(pixel);
        buffer[n] = setPixelColor(pixel);
      }
   
  }




  void PlaneRotation1() { ///REALLLY cool sparklesssss

    get_ready(); 
    static Plane3d myPlane;

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.5;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.16;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;


    
    calculate_oscillators(timings);     // get linear movers and oscillators going



    myPlane.yaw(.001 * move.noise_angle[0]);
    myPlane.setRefpoint(center_x+spread_x*move.directional[1]+move.noise_angle[0]/2.0, center_y+spread_y*move.directional[2]/2.0, center_z);
    
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = polar_theta[n];//4 * polar_theta[n] +  move.radial[0] ;//- distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_z    = 0.1;  
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.offset_x   = move.linear[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        animation.z          = 0;
        float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[1];
        //float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[2];
        //float show3          = render_value(animation);

        // colormapping
        float radius = radial_filter_radius;
        float radial_filter = (radius - distance[n]) / radius;



        pixel_hsi.i = 0.5;//6.0/255.0 *show1 * radial_filter;
        pixel_hsi.s = 1;

        float d = myPlane.distance(ledMap[n][xind],ledMap[n][yind],ledMap[n][zind]);
        d=fmodf(d,maxD/2);
        pixel_hsi.h = map_float(d, -maxD/2, maxD/2, 0, 1);

        pixel_hsi = hsi_sanity_check(pixel_hsi);

        pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
        
        //pixel.r   = 3*show1 * radial_filter;
        //pixel.g=0;
        //pixel.b=0;

        //pixel.g = show2 * radial_filter / 2;
        //pixel.b  = show3 * radial_filter / 4;


        pixel = rgb_sanity_check(pixel);
        buffer[n] = setPixelColor(pixel);
      }
   
  }




  void GrowingSpheres() { ///REALLLY cool sparklesssss

    get_ready(); 
    const int num_spheres = 4;
    static Sphere3d spheres[num_spheres]; // 4 spheres
    static bool firstTime=true;
    const float h[num_spheres] = {.7,.04,.9,.5};
    const float resetRadias = maxD*1.1;//maxD*2.5;

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.5;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.16;
    
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
        //spheres[s].setRefpoint(center_x + spread_x*move.directional[1], center_y + spread_y*move.directional[2], center_z + spread_z*move.directional[0]);
        spheres[s].setRadias(0.0);
      }
    }

    //check last, reorder list


    EVERY_N_MILLISECONDS(1000){
      Serial.println("spheres[s].mRadias;");
      for (int s = 0; s <num_spheres; s++) {
        Serial.print(spheres[s].mRadias);
        Serial.print("\t");
      }
      Serial.println("");
    }

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
      

      pixel_hsi.i = 0.5;//6.0/255.0 *show1 * radial_filter;
      pixel_hsi.s = 1;

      pixel_hsi = hsi_sanity_check(pixel_hsi);

      pixel = Hsi2Rgb(pixel_hsi, DEFAULT_GAMMA, RGB_SCALE_FACTORS);
      
      pixel = rgb_sanity_check(pixel);
      //if (minD < .2) 
      buffer[n] = setPixelColor(pixel);
      //else buffer[n] += setPixelColor(pixel);
      
      
      //buffer[n] = setPixelColor(pixel);
    }
  }

  /////////////////////////////////////////////////////////////////////////////////////////////////////
  //Original effects
  void Rotating_Blob() {

    get_ready(); 
    

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.1;         // higher values = faster transitions
    timings.ratio[1] = 0.03;
    timings.ratio[2] = 0.03;
    timings.ratio[3] = 0.03;
    
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

    
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        
        // describe and render animation layers
        animation.scale_x    = 0.05;
        animation.scale_y    = 0.05;
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        animation.offset_z   = 100;
        animation.angle      = polar_theta[n] +  move.radial[0];
        
        animation.dist       = distance[n];
        animation.z          = move.linear[0];
        animation.low_limit  = -1;
        float show1          = render_value(animation);
        
        animation.angle      = polar_theta[n] - move.radial[1] + show1/512.0;
        animation.dist       = distance[n] * show1/255.0;
        animation.low_limit  = 0;
        animation.z          = move.linear[1];
        float show2          = render_value(animation);

        animation.angle      = polar_theta[n] - move.radial[2] + show1/512.0;
        animation.dist       = distance[n] * show1/220.0;
        animation.z          = move.linear[2];
        float show3          = render_value(animation);

        animation.angle      = polar_theta[n] - move.radial[3] + show1/512.0;
        animation.dist       = distance[n] * show1/200.0;
        animation.z          = move.linear[3];
        float show4          = render_value(animation);

        // colormapping
        pixel.r   = (show2+show4)/2;
        pixel.g = show3 / 6;
        pixel.b  = 0;

        pixel = rgb_sanity_check(pixel);

        buffer[n] = setPixelColor(pixel);
      }
   
  }






  void Chasing_Spirals() {

    get_ready(); 
    

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.1;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.16;
    
    timings.offset[1] = 10;
    timings.offset[2] = 20;
    timings.offset[3] = 30;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        // describe and render animation layers
        animation.angle      = 3 * polar_theta[n] +  move.radial[0] - distance[n]/3;
        animation.dist       = distance[n];
        animation.scale_z    = 0.1;  
        animation.scale_y    = 0.1;
        animation.scale_x    = 0.1;
        animation.offset_x   = move.linear[0];
        animation.offset_y   = 0;
        animation.offset_z   = 0;
        animation.z          = 0;
        float show1          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[1] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[1];
        float show2          = render_value(animation);

        animation.angle      = 3 * polar_theta[n] +  move.radial[2] - distance[n]/3;
        animation.dist       = distance[n];
        animation.offset_x   = move.linear[2];
        float show3          = render_value(animation);

        // colormapping
        float radius = radial_filter_radius;
        float radial_filter = (radius - distance[n]) / radius;

        pixel.r   = 3*show1 * radial_filter;
        pixel.g = show2 * radial_filter / 2;
        pixel.b  = show3 * radial_filter / 4;

        pixel = rgb_sanity_check(pixel);

        buffer[n] = setPixelColor(pixel);
      }
   
  }


  void Waves() {

    get_ready(); 
    

    timings.master_speed = 0.1;    // speed ratios for the oscillators
    timings.ratio[0] = 2;         // higher values = faster transitions
    timings.ratio[1] = 2.1;
    timings.ratio[2] = 1.2;
    
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
      animation.z          = 2*distance[n] - move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n];
      animation.dist       = distance[n];
      animation.z          = 2*distance[n] - move.linear[1];
      float show2          = render_value(animation);

  
      // colormapping
      pixel.r   = show1;
      pixel.g = 0;
      pixel.b  = show2;

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Center_Field() {

    get_ready(); 
    

    timings.master_speed = 0.01;    // speed ratios for the oscillators
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
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.1;
      animation.dist       = 5*sqrtf(distance[n]);
      animation.offset_y   = move.linear[0];
      animation.offset_x   = 0;
      animation.z          = 0;
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.1;
      animation.dist       = 4*sqrtf(distance[n]);
      animation.offset_y   = move.linear[0];
      animation.offset_x   = 0;
      animation.z          = 0;
      float show2          = render_value(animation);

     

  
      // colormapping
      pixel.r   = show1;
      pixel.g = show2;
      pixel.b  = 0;

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
   

  }

  void Distance_Experiment() {


    get_ready(); 
    

    timings.master_speed = 0.01;    // speed ratios for the oscillators
    timings.ratio[0] = 0.2;         // higher values = faster transitions
    timings.ratio[1] = 0.13;
    timings.ratio[2] = 0.012;
    
    timings.offset[1] = 100;
    timings.offset[2] = 200;
    timings.offset[3] = 300;
    
    calculate_oscillators(timings);     // get linear movers and oscillators going

      
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
  
      // describe and render animation layers
      animation.dist       = powf(distance[n], 0.5);
      animation.angle      = polar_theta[n] + move.radial[0];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.1;
      animation.offset_y   = move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 0;
      float show1          = render_value(animation);

      animation.dist       = powf(distance[n], 0.6);
      animation.angle      = polar_theta[n] + move.noise_angle[2];
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.scale_z    = 0.1;
      animation.offset_y   = move.linear[1];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 0;
      float show2          = render_value(animation);
      
      // colormapping
      pixel.r   = show1+show2;
      pixel.g = show2;
      pixel.b  = 0;

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
   

  }


  void Caleido1() {

    get_ready(); 
    

    timings.master_speed = 0.003;    // speed ratios for the oscillators
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
      animation.dist       = distance[n] * (2 + move.directional[0]) / 3;
      animation.angle      = 3 * polar_theta[n] + 3 * move.noise_angle[0] + move.radial[4];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = move.linear[0];
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[1]) / 3;
      animation.angle      = 4 * polar_theta[n] + 3 * move.noise_angle[1] + move.radial[4];
      animation.offset_x   = 2 * move.linear[1];
      animation.z          = move.linear[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[2]) / 3;
      animation.angle      = 5 * polar_theta[n] + 3 * move.noise_angle[2] + move.radial[4];
      animation.offset_y   = 2 * move.linear[2];
      animation.z          = move.linear[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[3]) / 3;
      animation.angle      = 4 * polar_theta[n] + 3 * move.noise_angle[3] + move.radial[4];
      animation.offset_x   = 2 * move.linear[3];
      animation.z          = move.linear[3];
      float show4          = render_value(animation);
      
      // colormapping
      pixel.r   = show1;
      pixel.g = show3 * distance[n] / 10;
      pixel.b  = (show2 + show4) / 2;

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Caleido2() {

    get_ready(); 
    

    timings.master_speed = 0.002;    // speed ratios for the oscillators
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
      animation.dist       = distance[n] * (2 + move.directional[0]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[0] + move.radial[4];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 0;//move.linear[0]; TODO
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[1]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[1] + move.radial[4];
      animation.offset_x   = 2 * move.linear[1];
      animation.z          = move.linear[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[2]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[2] + move.radial[4];
      animation.offset_y   = 2 * move.linear[2];
      animation.z          = move.linear[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[3]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[3] + move.radial[4];
      animation.offset_x   = 2 * move.linear[3];
      animation.z          = move.linear[3];
      float show4          = render_value(animation);
      
      // colormapping
      pixel.r   = show1;
      pixel.g = show3 * distance[n] / 10;
      pixel.b  = (show2 + show4) / 2;


      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Caleido3() {

    get_ready(); 
    

    timings.master_speed = 0.004;    // speed ratios for the oscillators
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
      animation.dist       = distance[n] * (2 + move.directional[0]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[0] + move.radial[4];
      animation.scale_x    = 0.1;// + (move.directional[0] + 2)/100;
      animation.scale_y    = 0.1;// + (move.directional[1] + 2)/100;
      animation.scale_z    = 0.1;
      animation.offset_y   = 2 * move.linear[0];
      animation.offset_x   = 2 * move.linear[1];
      animation.offset_z   = 0;
      animation.z          = move.linear[0];
      float show1          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[1]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[1] + move.radial[4];
      animation.offset_x   = 2 * move.linear[1];
      animation.offset_y   = show1 / 20.0;
      animation.z          = move.linear[1];
      float show2          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[2]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[2] + move.radial[4];
      animation.offset_y   = 2 * move.linear[2];
      animation.offset_x   = show2 / 20.0;
      animation.z          = move.linear[2];
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (2 + move.directional[3]) / 3;
      animation.angle      = 2 * polar_theta[n] + 3 * move.noise_angle[3] + move.radial[4];
      animation.offset_x   = 2 * move.linear[3];
      animation.offset_y   = show3 / 20.0;
      animation.z          = move.linear[3];
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

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
  }

  void Lava1() {

    get_ready(); 
    

    timings.master_speed = 0.0015;    // speed ratios for the oscillators
    timings.ratio[0] = 4;         // higher values = faster transitions
    timings.ratio[1] = 1;
    timings.ratio[2] = 1;
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
      animation.dist       = distance[n] * 0.8;
      animation.angle      = polar_theta[n];
      animation.scale_x    = 0.15;// + (move.directional[0] + 2)/100;
      animation.scale_y    = 0.12;// + (move.directional[1] + 2)/100;
      animation.scale_z    = 0.01;
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 30;
      float show1          = render_value(animation);

      animation.offset_y   = -move.linear[1];
      animation.scale_x    = 0.15;// + (move.directional[0] + 2)/100;
      animation.scale_y    = 0.12;// + (move.directional[1] + 2)/100;
      animation.offset_x   = show1 / 100;
      animation.offset_y   += show1/100;
     
      float show2          = render_value(animation);

      animation.offset_y   = -move.linear[2];
      animation.scale_x    = 0.15;// + (move.directional[0] + 2)/100;
      animation.scale_y    = 0.12;// + (move.directional[1] + 2)/100;
      animation.offset_x   = show2 / 100;
      animation.offset_y   += show2/100;
     
      float show3         = render_value(animation);

      // colormapping
      float linear = (ledMap[n][yind])/(spread_y-1.f);  // radial mask

      pixel.r = linear*show2;
      pixel.g = 0.1*linear*(show2-show3);
      
      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
  }

  void Scaledemo1() {

    get_ready(); 
        

    timings.master_speed = 0.00003;    // speed ratios for the oscillators
    timings.ratio[0] = 4;         // higher values = faster transitions
    timings.ratio[1] = 3.2;
    timings.ratio[2] = 10;
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
      animation.dist       = 0.3*distance[n] * 0.8;
      animation.angle      = 3*polar_theta[n] + move.radial[2];
      animation.scale_x    = 0.1 + (move.noise_angle[0])/10;
      animation.scale_y    = 0.1 + (move.noise_angle[1])/10;// + (move.directional[1] + 2)/100;
      animation.scale_z    = 0.01;
      animation.offset_y   = 0;
      animation.offset_x   = 0;
      animation.offset_z   = 100*move.linear[0];
      animation.z          = 30;
      float show1          = render_value(animation);

      animation.angle      = 3;
      float show2          = render_value(animation);

      float dist = 1;//(10-distance[n])/ 10;
      pixel.r = show1*dist;
      pixel.g = (show1-show2)*dist*0.3;
      pixel.b = (show2-show1)*dist;

      if (distance[n] > 16) {
         pixel.r = 0;
         pixel.g = 0;
         pixel.b = 0;

      }
      
      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
  }


  void Yves() {

    get_ready(); 
        

    a = micros();                   // for time measurement in report_performance()

    timings.master_speed = 0.001;    // speed ratios for the oscillators
    timings.ratio[0] = 3;         // higher values = faster transitions
    timings.ratio[1] = 2;
    timings.ratio[2] = 1;
    timings.ratio[3] = 0.13;
    timings.ratio[4] = 0.15;
    timings.ratio[5] = 0.03;
    timings.ratio[6] = 0.025;
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
      animation.angle      = polar_theta[n] + 2*PI + move.noise_angle[5];
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.08;
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2*PI + move.noise_angle[6];;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.08;
      animation.offset_y   = -move.linear[1];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = 0;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + show1/100 + move.noise_angle[3] + move.noise_angle[4];
      animation.dist       = distance[n] + show2/50;
      animation.offset_y   = -move.linear[2];

      animation.offset_y   += show1/100;
      animation.offset_x   += show2/100;

      float show3          = render_value(animation);

      animation.offset_y   = 0;
      animation.offset_x   = 0;

      float show4          = render_value(animation);
      
     
      pixel.r   = show3;
      pixel.g = show3*show4/255;
      pixel.b  = 0;
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Spiralus() {

    get_ready(); 
        

    timings.master_speed = 0.0011;    // speed ratios for the oscillators
    timings.ratio[0] = 1.5;         // higher values = faster transitions
    timings.ratio[1] = 2.3;
    timings.ratio[2] = 3;
    timings.ratio[3] = 0.05;
    timings.ratio[4] = 0.2;
    timings.ratio[5] = 0.03;
    timings.ratio[6] = 0.025;
    timings.ratio[7] = 0.021;
    timings.ratio[8] = 0.027;
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
      animation.angle      = 2*polar_theta[n] + move.noise_angle[5] + move.directional[3] * move.noise_angle[6]* animation.dist/10;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.02;
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = move.linear[1];
      float show1          = render_value(animation);

      animation.angle      = 2*polar_theta[n] + move.noise_angle[7] + move.directional[5] * move.noise_angle[8]* animation.dist/10;
      animation.offset_y   = -move.linear[1];
      animation.z          = move.linear[2];
            
      float show2          = render_value(animation);

      animation.angle      = 2*polar_theta[n] + move.noise_angle[6] + move.directional[6] * move.noise_angle[7]* animation.dist/10;
      animation.offset_y   = move.linear[2];
      animation.z          = move.linear[0];
      float show3          = render_value(animation);
      
      float f =  1;
     
      pixel.r   = f*(show1+show2);
      pixel.g = f*(show1-show2);
      pixel.b  = f*(show3-show1);
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void Spiralus2() {

    get_ready(); 
      

    timings.master_speed = 0.0015;    // speed ratios for the oscillators
    timings.ratio[0] = 1.5;         // higher values = faster transitions
    timings.ratio[1] = 2.3;
    timings.ratio[2] = 3;
    timings.ratio[3] = 0.05;
    timings.ratio[4] = 0.2;
    timings.ratio[5] = 0.05;
    timings.ratio[6] = 0.055;
    timings.ratio[7] = 0.06;
    timings.ratio[8] = 0.027;
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
      animation.angle      = 5*polar_theta[n] + move.noise_angle[5] + move.directional[3] * move.noise_angle[6]* animation.dist/10;
      animation.scale_x    = 0.08;
      animation.scale_y    = 0.08;
      animation.scale_z    = 0.02;
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      animation.z          = move.linear[1];
      float show1          = render_value(animation);

      animation.angle      = 6*polar_theta[n] + move.noise_angle[7] + move.directional[5] * move.noise_angle[8]* animation.dist/10;
      animation.offset_y   = -move.linear[1];
      animation.z          = move.linear[2];
            
      float show2          = render_value(animation);

      animation.angle      = 6*polar_theta[n] + move.noise_angle[6] + move.directional[6] * move.noise_angle[7]* animation.dist/10;
      animation.offset_y   = move.linear[2];
      animation.z          = move.linear[0];
      animation.dist       = distance[n] *0.8;
      float show3          = render_value(animation);
      
      
      float f =  1;//(24-distance[n])/24;
     
      pixel.r   = f*(show1+show2);
      pixel.g = f*(show1-show2);
      pixel.b  = f*(show3-show1);
      
      pixel = rgb_sanity_check(pixel);
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
      
      animation.scale_x    = 0.07 + move.directional[0]*0.002;
      animation.scale_y    = 0.07;
      
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      
      animation.z          = 0;
      animation.low_limit  = -1;
      float show1          = render_value(animation);

      animation.offset_y   = -move.linear[1];
      float show3          = render_value(animation);

      animation.offset_x   = show3/20;
      animation.offset_y   = -move.linear[0]/2 + show1/70;
      animation.low_limit  = 0;
      float show2          = render_value(animation);

      animation.offset_x   = show3/20;
      animation.offset_y   = -move.linear[0]/2 + show1/70;
      animation.z          = 100;
      float show4          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-animation.dist)/animation.dist;

      float linear = (ledMap[n][yind]+1)/(spread_y-1.f);
      
      pixel.r   = radial  * show2;
      pixel.g   = linear * radial* 0.3* (show2-show4);
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
 
  }

  void Zoom() { // nice one
    
    get_ready();

    

    run_default_oscillators();
    timings.master_speed = 0.003;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n] * distance[n])/2;
      animation.angle      = polar_theta[n];
      
      animation.scale_x    = 0.005;
      animation.scale_y    = 0.005;
      
      animation.offset_y   = -10*move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      
      animation.z          = 0;
      animation.low_limit  = 0;
      float show1          = render_value(animation);

  
      float linear = 1;//(y+1)/(num_y-1.f);
      
      pixel.r   = show1*linear;
      pixel.g   = 0;
      
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Zoom2() { // nice one
    
    get_ready();

    

    run_default_oscillators();
    timings.master_speed = 0.003;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n] * distance[n])/2;
      animation.angle      = polar_theta[n];
      
      animation.scale_x    = 0.005;
      animation.scale_y    = 0.005;
      
      animation.offset_y   = -10*move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0.1*move.linear[0];
      
      animation.z          = 0;
      animation.low_limit  = 0;
      float show1          = render_value(animation);

  
      //float linear = 1;//(y+1)/(num_y-1.f);
      
      pixel.r   = show1;
      pixel.b   = 40-show1;
      
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Slow_Fade() { // nice one

    get_ready();

                      

    run_default_oscillators();
    timings.master_speed = 0.00005;
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = sqrtf(distance[n]) * 0.7 * (move.directional[0] + 1.5);
      animation.angle      = polar_theta[n] - move.radial[0] + distance[n] / 5;
      
      animation.scale_x    = 0.11;
      animation.scale_y    = 0.11;
      
      animation.offset_y   = -50 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_z   = 0;
      
      animation.z          = move.linear[0];
      animation.low_limit  = -0.1;
      animation.high_limit = 1;
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
      
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Polar_Waves() { // nice one

    get_ready();

    

                      

    timings.master_speed = 0.5;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) { 
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = (distance[n]);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.radial[0];
      animation.z          = (animation.dist * 1.5)-10 * move.linear[0];
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_x   = move.linear[0];
      
      float show1          = render_value(animation);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.radial[1];
      animation.z          = (animation.dist * 1.5)-10 * move.linear[1];
      animation.offset_x   = move.linear[1];

      float show2          = render_value(animation);
      animation.angle      = polar_theta[n] - animation.dist * 0.1 + move.radial[2];
      animation.z          = (animation.dist * 1.5)-10 * move.linear[2];
      animation.offset_x   = move.linear[2];

      float show3          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * show1;
      pixel.g  = radial * show2;
      pixel.b   = radial * show3;
      
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs() { // nice one

    get_ready(); 
                         

    timings.master_speed = 0.2;    // master speed

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
      animation.angle      = polar_theta[n] + move.radial[0] + move.noise_angle[0]+ move.noise_angle[3];
      animation.z          = (sqrtf(animation.dist));// - 10 * move.linear[0];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.offset_z   = 10;
      animation.offset_x   = 10*move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[1]+ move.noise_angle[1]+ move.noise_angle[4];
      animation.offset_x   = 11*move.linear[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[2]+ move.noise_angle[2]+ move.noise_angle[5];
      animation.offset_x   = 12*move.linear[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * show1;
      pixel.g  = radial * show2;
      pixel.b   = radial * show3;
     
      
      
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
    
  }


  void RGB_Blobs2() { // nice one

    get_ready(); 
                        

    timings.master_speed = 0.12;    // master speed

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
      animation.angle      = polar_theta[n] + move.radial[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.z          = (sqrtf(animation.dist));// - 10 * move.linear[0];
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.offset_z   = 10;
      animation.offset_x   = 10*move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 11*move.linear[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 12*move.linear[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1-show3);
      pixel.g  = radial * (show2-show1);
      pixel.b   = radial * (show3-show2);
     
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs3() { // nice one

    get_ready();

                     

    timings.master_speed = 0.12;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.radial[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.z          = (sqrtf(animation.dist));// - 10 * move.linear[0];
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = 10;
      animation.offset_x   = 10*move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 11*move.linear[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 12*move.linear[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;
      pixel.g  = radial * (show2+show1)*0.5 * ledMap[n][yind]/spread_y;
      pixel.b   = radial * (show3+show2)*0.5 * ledMap[n][xind]/spread_x;
     
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs4() { // nice one

    get_ready();

    

                       

    timings.master_speed = 0.02;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.radial[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.z          = 3+sqrtf(animation.dist);
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.offset_z   = 10;
      animation.offset_x   = 50 * move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 50 * move.linear[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 50 * move.linear[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = 23;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;
      pixel.g  = radial * (show2+show1)*0.5 * ledMap[n][yind]/spread_y;
      pixel.b   = radial * (show3+show2)*0.5 * ledMap[n][xind]/spread_x;
     
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
  }

  void RGB_Blobs5() { // nice one

    get_ready();

    

                     

    timings.master_speed = 0.02;    // master speed

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.0031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] + move.noise_angle[4];
      animation.angle      = polar_theta[n] + move.radial[0] + move.noise_angle[0]+ move.noise_angle[3] + move.noise_angle[1];
      animation.z          = 3+sqrtf(animation.dist);
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 10;
      animation.offset_x   = 50 * move.linear[0];
      float show1          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[1]+ move.noise_angle[1]+ move.noise_angle[4] + move.noise_angle[2];
      animation.offset_x   = 50 * move.linear[1];
      animation.offset_z   = 100;
      float show2          = render_value(animation);

      animation.angle      = polar_theta[n] + move.radial[2]+ move.noise_angle[2]+ move.noise_angle[5]+ move.noise_angle[3];
      animation.offset_x   = 50 * move.linear[2];
      animation.offset_z   = 300;
      float show3          = render_value(animation);
      
      float radius = 23;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial * (show1+show3)*0.5 * animation.dist/5;
      pixel.g  = radial * (show2+show1)*0.5 * ledMap[n][yind]/spread_y;
      pixel.b   = radial * (show3+show2)*0.5 * ledMap[n][xind]/spread_x;
     
      pixel = rgb_sanity_check(pixel);
   
      buffer[n] = setPixelColor(pixel);
    }

  }

  void Big_Caleido() { // nice one

    get_ready();

                    

    timings.master_speed = 0.02;    // master speed

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
      animation.z          = 5;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 50 * move.linear[0];
      animation.offset_x   = 50 * move.noise_angle[0];
      animation.offset_y   = 50 * move.noise_angle[1];
      float show1          = render_value(animation);

      animation.angle      =6 * polar_theta[n] + 5*move.noise_angle[1] + animation.dist * 0.15;
      animation.z          = 5;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 50 * move.linear[1];
      animation.offset_x   = 50 * move.noise_angle[1];
      animation.offset_y   = 50 * move.noise_angle[2];
      float show2          = render_value(animation);

      animation.angle      = 5;
      animation.z          = 5;
      animation.scale_x    = 0.10;
      animation.scale_y    = 0.10;
      animation.offset_z   = 10 * move.linear[2];
      animation.offset_x   = 10 * move.noise_angle[2];
      animation.offset_y   = 10 * move.noise_angle[3];
      float show3          = render_value(animation);

      animation.angle      = 15;
      animation.z          = 15;
      animation.scale_x    = 0.10;
      animation.scale_y    = 0.10;
      animation.offset_z   = 10 * move.linear[3];
      animation.offset_x   = 10 * move.noise_angle[3];
      animation.offset_y   = 10 * move.noise_angle[4];
      float show4          = render_value(animation);

      animation.angle      = 2;
      animation.z          = 15;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_z   = 10 * move.linear[4];
      animation.offset_x   = 10 * move.noise_angle[4];
      animation.offset_y   = 10 * move.noise_angle[5];
      float show5          = render_value(animation);

      
      pixel.r    = show1-show4;
      pixel.g  = show2-show5;
      pixel.b   = show3-show2+show1;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
    //show_frame();
  }

  void SM1() { // nice one

    get_ready();

    

    timings.master_speed = 0.02;    // master speed

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
      animation.z          = 5;
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.offset_z   = 50 * move.linear[0];
      animation.offset_x   = 150 * move.directional[0];
      animation.offset_y   = 150 * move.directional[1];
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 4*move.noise_angle[1];
      animation.z          = 15;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_z   = 50 * move.linear[1];
      animation.offset_x   = 150 * move.directional[1];
      animation.offset_y   = 150 * move.directional[2];
      float show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[2];
      animation.z          = 25;
      animation.scale_x    = 0.1;
      animation.scale_y    = 0.1;
      animation.offset_z   = 50 * move.linear[2];
      animation.offset_x   = 150 * move.directional[2];
      animation.offset_y   = 150 * move.directional[3];
      float show3          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[3];
      animation.z          = 35;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_z   = 50 * move.linear[3];
      animation.offset_x   = 150 * move.directional[3];
      animation.offset_y   = 150 * move.directional[4];
      float show4          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 5*move.noise_angle[4];
      animation.z          = 45;
      animation.scale_x    = 0.2;
      animation.scale_y    = 0.2;
      animation.offset_z   = 50 * move.linear[4];
      animation.offset_x   = 150 * move.directional[4];
      animation.offset_y   = 150 * move.directional[5];
      float show5          = render_value(animation);

     

      pixel.r    = show1+show2;
      pixel.g  = show3+show4;
      pixel.b   = show5;
     
      pixel = rgb_sanity_check(pixel);
      buffer[n] = setPixelColor(pixel);
    }
    //show_frame();
  }

  void SM2() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0036;
    timings.ratio[5] = 0.0039;
    
    calculate_oscillators(timings); 

      
      for (int n = 0; n < NUM_LEDS; n++) {
        animation.anglephi   = spherical_phi[n]; //todo, move this later
        
        animation.dist       = distance[n] * (move.directional[0]);
        animation.angle      = polar_theta[n] + move.radial[0];
        animation.z          = 5;
        animation.scale_x    = 0.09;
        animation.scale_y    = 0.09;
        animation.offset_z   = 5 * move.linear[0];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show1          = render_value(animation);

        animation.dist       = distance[n]* move.directional[1];
        animation.angle      = polar_theta[n] + move.radial[1];
        animation.z          = 50;
        animation.scale_x    = 0.07;
        animation.scale_y    = 0.07;
        animation.offset_z   = 5 * move.linear[1];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show2          = render_value(animation);
        
        animation.dist       = distance[n]* move.directional[2];
        animation.angle      = polar_theta[n] + move.radial[2];
        animation.z          = 500;
        animation.scale_x    = 0.05;
        animation.scale_y    = 0.05;
        animation.offset_z   = 5 * move.linear[2];
        animation.offset_x   = 0;
        animation.offset_y   = 0;
        float show3          = render_value(animation);
       
       

        pixel.r    = show1;
        pixel.g  = show2;
        pixel.b   = show3;
       
        pixel = rgb_sanity_check(pixel);
        
        buffer[n] = setPixelColor(pixel);
      }
   //show_frame();
  }

  void SM3() { 

    get_ready();

    

    timings.master_speed = 0.02;    // master speed

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
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.linear[0];;
      animation.low_limit  = -1;
      animation.high_limit = 1;
      show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 500;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.linear[0];;
      animation.low_limit  = -1;
      animation.high_limit = 1;
      show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/20;
      animation.offset_y   = -4 * move.linear[0] + show2/20;
      animation.low_limit  = 0;
      animation.high_limit = 1;
      show3          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/18;
      animation.offset_y   = -4 * move.linear[0] + show2/18;
      animation.low_limit  = 0;
      animation.high_limit = 1;
      show4          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 500+show1/19;
      animation.offset_y   = -4 * move.linear[0] + show2/19;
      animation.low_limit  = 0.3;
      animation.high_limit = 1;
      show5          = render_value(animation);

      pixel.r    = show4;
      pixel.g  = show3;
      pixel.b   = show5;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
   
  }

  void SM4() { 

    get_ready();

    

    timings.master_speed = 0.02;    // master speed

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
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -20 * move.linear[0];;
      animation.low_limit  = 0;
      animation.high_limit = 1;
      show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 500;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.offset_y   = -40 * move.linear[0];;
      animation.low_limit  = 0;
      animation.high_limit = 1;
      show2          = render_value(animation);

      pixel.r    = add(show2, show1);
      pixel.g  = 0;
      pixel.b   = colordodge(show2, show1);
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);

    }
   
  }

  void SM5() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later
      
      animation.dist       = distance[n] * (move.directional[0]);
      animation.angle      = polar_theta[n] + move.radial[0];
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 5 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n]* move.directional[1];
      animation.angle      = polar_theta[n] + move.radial[1];
      animation.z          = 50;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 5 * move.linear[1];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show2          = render_value(animation);
      
      animation.dist       = distance[n]* move.directional[2];
      animation.angle      = polar_theta[n] + move.radial[2];
      animation.z          = 500;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 5 * move.linear[2];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (move.directional[3]);
      animation.angle      = polar_theta[n] + move.radial[3];
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 5 * move.linear[3];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show4          = render_value(animation);

      animation.dist       = distance[n]* move.directional[4];
      animation.angle      = polar_theta[n] + move.radial[4];
      animation.z          = 50;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 5 * move.linear[4];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show5          = render_value(animation);
      
      animation.dist       = distance[n]* move.directional[5];
      animation.angle      = polar_theta[n] + move.radial[5];
      animation.z          = 500;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 5 * move.linear[5];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show6          = render_value(animation);

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
     
      pixel.r    = radial * add(show1,show4);
      pixel.g  = radial * colordodge(show2,show5);
      pixel.b   = radial * screen(show3,show6);
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM6() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.0059;
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 0.7; // zoom factor
      
      animation.dist       = distance[n] * (move.directional[0]) * s;
      animation.angle      = polar_theta[n] + move.radial[0];
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 5 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n]* move.directional[1] * s;
      animation.angle      = polar_theta[n] + move.radial[1];
      animation.z          = 50;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 5 * move.linear[1];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show2          = render_value(animation);
      
      animation.dist       = distance[n]* move.directional[2] * s;
      animation.angle      = polar_theta[n] + move.radial[2];
      animation.z          = 500;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 5 * move.linear[2];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show3          = render_value(animation);

      animation.dist       = distance[n] * (move.directional[3]) * s;
      animation.angle      = polar_theta[n] + move.radial[3];
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 5 * move.linear[3];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show4          = render_value(animation);

      animation.dist       = distance[n]* move.directional[4] * s;
      animation.angle      = polar_theta[n] + move.radial[4];
      animation.z          = 50;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 5 * move.linear[4];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      float show5          = render_value(animation);
      
      animation.dist       = distance[n]* move.directional[5] * s;
      animation.angle      = polar_theta[n] + move.radial[5];
      animation.z          = 500;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 5 * move.linear[5];
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
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM8() { 

    get_ready();

    

    timings.master_speed = 0.06;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.0053;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.0056;
    timings.ratio[5] = 0.01;    // original Reddit post had no radial movement!
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      //float s = 0.7; // zoom factor
      
      animation.dist       = distance[n];
      animation.angle      = 2;
      animation.z          = 5;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_z   = 0;
      animation.offset_y   = 50 * move.linear[0];
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      float show1          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 2;
      animation.z          = 150;
      animation.offset_x   = -50 * move.linear[0];     
      float show2          = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.z          = 550;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_x   = 0;
      animation.offset_y   = -50 * move.linear[1];
      float show4          = render_value(animation);      

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.z          = 1250;
      animation.scale_x    = 0.15;
      animation.scale_y    = 0.15;
      animation.offset_x   = 0;
      animation.offset_y   = 50 * move.linear[1];
      float show5          = render_value(animation);      
      
     

      //float radius = radial_filter_radius;   // radius of a radial brightness filter
      //float radial = (radius-distance[n])/distance[n];

      show3 = add(show1, show2);
      show6 = screen(show4, show5);
      //show9 = screen(show3, show6);
     
      pixel.r    = show3;
      pixel.g  = 0;
      pixel.b   = show6;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void SM9() { 

    get_ready();

    

    timings.master_speed = 0.005;    // master speed

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
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_y   = -30 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_y   = -30 * move.linear[1];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show2                = render_value(animation);

      animation.dist       = distance[n];// + show1/64;
      animation.angle      = polar_theta[n] + 2 + (show1 / 255) * PI;
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_y   = -10 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2 +(show2 / 255) * PI;;
      animation.z          = 5;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_y   = -20 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show5, show3);

      float sy2 =spread_y*2.0;

      float linear1 = ledMap[n][yind] / sy2;
      float linear2 = (sy2-ledMap[n][yind]) / sy2;

      pixel.r    = show5 * linear1;
      pixel.g  = 0;
      pixel.b   = show6 * linear2;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void SM10() { 

    get_ready();

    

    timings.master_speed = 0.006;    // 0.006

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
      animation.z          = 5;
      animation.scale_x    = 0.09 * scale;
      animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -30 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.09 * scale;
      animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -30 * move.linear[1];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = -1;
      show2                = render_value(animation);

      animation.dist       = distance[n];// + show1/64;
      animation.angle      = polar_theta[n] + 2 + (show1 / 255) * PI;
      animation.z          = 5;
      animation.scale_x    = 0.09 * scale;
      animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -10 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = polar_theta[n] + 2 +(show2 / 255) * PI;;
      animation.z          = 5;
      animation.scale_x    = 0.09 * scale;
      animation.scale_y    = 0.09 * scale;
      animation.offset_y   = -20 * move.linear[0];
      animation.offset_z   = 0;
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show5, show3);

      //float linear1 = y / 32.f;
      //float linear2 = (32-y) / 32.f;

      pixel.r    = (show5+show6)/2;
      pixel.g  = (show5-50)+(show6/16);
      pixel.b   = 0;//show6;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido() { 



    get_ready();

    

    timings.master_speed = 0.009;    // master speed

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
      animation.angle      = 5 * polar_theta[n] + 10 * move.radial[0] + animation.dist /2;
      animation.z          = 5;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.linear[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.radial[1] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.07;
      animation.scale_y    = 0.07;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.linear[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.radial[2] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.05;
      animation.scale_y    = 0.05;
      animation.offset_z   = 0;
      animation.offset_x   = -40 * move.linear[2];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.radial[3] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.09;
      animation.scale_y    = 0.09;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.linear[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show2, show3);

      //float linear1 = y / 32.f;
      //float linear2 = (32-y) / 32.f;

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial*(show1+show2);
      pixel.g  = 0.3*radial*show6;//(radial*(show1))*0.3f;
      pixel.b   = radial*show5;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_2() { 



    get_ready();

    

    timings.master_speed = 0.009;    // master speed

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
      animation.angle      = 5 * polar_theta[n] + 10 * move.radial[0] + animation.dist /2;
      animation.z          = 5;
      animation.scale_x    = 0.07 * size;
      animation.scale_y    = 0.07 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.linear[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.radial[1] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.07 * size;
      animation.scale_y    = 0.07 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -30 * move.linear[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.radial[2] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.05 * size;
      animation.scale_y    = 0.05 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -40 * move.linear[2];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.radial[3] + animation.dist /2;
      animation.z          = 500;
      animation.scale_x    = 0.09 * size;
      animation.scale_y    = 0.09 * size;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.linear[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3);
      show6 = colordodge(show2, show3);

      //float linear1 = y / 32.f;
      //float linear2 = (32-y) / 32.f;

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      pixel.r    = radial*(show1+show2);
      pixel.g  = 0.3*radial*show6;//(radial*(show1))*0.3f;
      pixel.b   = radial*show5;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Complex_Kaleido_3() { 



    get_ready();

    

    timings.master_speed = 0.001;    // master speed

    timings.ratio[0] = 0.025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.027;
    timings.ratio[2] = 0.031;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[4] = 0.037;
    timings.ratio[5] = 0.038;
    timings.ratio[5] = 0.041;
    
    calculate_oscillators(timings); 

    float size = 0.4 + move.directional[0]*0.1;

    float q = 2;

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 10 * move.radial[0] + animation.dist / (((move.directional[0] + 3)*2)) + move.noise_angle[0]*q;
      animation.z          = 5;
      animation.scale_x    = 0.08 * size * (move.directional[0]+1.5);
      animation.scale_y    = 0.07 * size;
      animation.offset_z   = -10 * move.linear[0];
      animation.offset_x   = -30 * move.linear[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 10 * move.radial[1] + animation.dist / (((move.directional[1] + 3)*2))+ move.noise_angle[1]*q;
      animation.z          = 500;
      animation.scale_x    = 0.07 * size * (move.directional[1]+1.1);
      animation.scale_y    = 0.07 * size * (move.directional[2]+1.3);;
      animation.offset_z   = -12 * move.linear[1];;
      animation.offset_x   = -31 * move.linear[1];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = -5 * polar_theta[n] + 12 * move.radial[2] + animation.dist /(((move.directional[3] + 3)*2))+ move.noise_angle[2]*q;
      animation.z          = 500;
      animation.scale_x    = 0.05 * size * (move.directional[3]+1.5);;
      animation.scale_y    = 0.05 * size * (move.directional[4]+1.5);;
      animation.offset_z   = -12 * move.linear[3];
      animation.offset_x   = -40 * move.linear[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.radial[3] + animation.dist /(((move.directional[5] + 3)*2))+ move.noise_angle[3]*q;
      animation.z          = 500;
      animation.scale_x    = 0.09 * size * (move.directional[5]+1.5);;;
      animation.scale_y    = 0.09 * size * (move.directional[6]+1.5);;;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.linear[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      show5 = screen(show4, show3)-show2;
      show6 = colordodge(show4, show1);

      show7 = multiply(show1, show2);
      
      float linear1 = ledMap[n][yind] / (spread_y*2);
      //float linear2 = (32-y) / 32.f;

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];

      show7 = multiply(show1, show2) * linear1*2;
      show8 = subtract(show7, show5);

      //pixel.r    = radial*(show1+show2);
      pixel.g  = 0.2*show8;//(radial*(show1))*0.3f;
      pixel.b   = show5 * radial;
      pixel.r    = (1*show1 + 1*show2) - show7/2;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_4() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 0.01 in the video

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

      float s = 1 +  move.directional[6]*0.3;

      animation.dist       = distance[n] * s;
      animation.angle      = 5 * polar_theta[n] + 1 * move.radial[0] - animation.dist / (3+move.directional[0]*0.5);
      animation.z          = 5;
      animation.scale_x    = 0.08 * size + (move.directional[0]*0.01);
      animation.scale_y    = 0.07 * size + (move.directional[1]*0.01);
      animation.offset_z   = -10 * move.linear[0];
      animation.offset_x   = 0;//-30 * move.linear[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n] * s;
      animation.angle      = 5 * polar_theta[n] + 1 * move.radial[1] + animation.dist / (3+move.directional[1]*0.5);
      animation.z          = 50;
      animation.scale_x    = 0.08 * size + (move.directional[1]*0.01);
      animation.scale_y    = 0.07 * size + (move.directional[2]*0.01);
      animation.offset_z   = -10 * move.linear[1];
      animation.offset_x   = 0;//-30 * move.linear[0];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 1;
      animation.z          = 500;
      animation.scale_x    = 0.2 * size ;
      animation.scale_y    = 0.2 * size ;
      animation.offset_z   = 0;//-12 * move.linear[3];
      animation.offset_y   = +7 * move.linear[3] +  move.noise_angle[3];
      animation.offset_x   = 0;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      
      animation.dist       = distance[n];
      animation.angle      = 5 * polar_theta[n] + 12 * move.radial[3] + animation.dist /(((move.directional[5] + 3)*2))+ move.noise_angle[3]*q;
      animation.z          = 500;
      animation.scale_x    = 0.09 * size * (move.directional[5]+1.5);;;
      animation.scale_y    = 0.09 * size * (move.directional[6]+1.5);;;
      animation.offset_z   = 0;
      animation.offset_x   = -35 * move.linear[3];
      animation.offset_y   = 0;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      //show5 = screen(show4, show3)-show2;
      //show6 = colordodge(show4, show1);

      //show7 = multiply(show1, show2);
      /*
      float linear1 = y / 32.f;
      float linear2 = (32-y) / 32.f;
      */

      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
      /*
      show7 = multiply(show1, show2) * linear1*2;
      show8 = subtract(show7, show5);
      */

      show5 = ((show1 + show2)) - show3;
      if (show5>255) show5=255;
      if (show5<0) show5=0;

      show6 = colordodge(show1, show2);

      pixel.r    = show5 * radial;
      pixel.b   = (64-show5-show3) * radial;
      pixel.g  = 0.5*(show6);
      //pixel.b   = show5 * radial;
      //pixel.r    = (1*show1 + 1*show2) - show7/2;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Complex_Kaleido_5() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed 

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

      float s = 1 +  move.directional[6]*0.8;

      animation.dist       = distance[n] * s;
      animation.angle      = move.radial[6] + 2 * move.directional[5] * polar_theta[n]  - animation.dist / 3;
      animation.z          = 5;
      animation.scale_x    = 0.08 * size ;
      animation.scale_y    = 0.07 * size ;
      animation.offset_z   = -10 * move.linear[0];
      animation.offset_x   = 0;
      animation.offset_y   = 0;
      animation.low_limit  = -0.5;
      show1                = render_value(animation);

     
      float radius = radial_filter_radius;   // radius of a radial brightness filter
      float radial = (radius-distance[n])/distance[n];
      //12-10/10,  12-1/10,  brighter in center
     
     uint8_t color = float_to_uint8_t(show1 * radial);
     uint8_t brightness = 200;
     TBlendType    currentBlending = LINEARBLEND;


      buffer[n] = ColorFromPalette( currentPalette, color, brightness, currentBlending);
    }
  }

  void Complex_Kaleido_6() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 

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
      animation.angle      = 16 * polar_theta[n] + 16*move.radial[0];
      animation.z          = 5;
      animation.scale_x    = 0.06 ;
      animation.scale_y    = 0.06 ;
      animation.offset_z   = -10 * move.linear[0];
      animation.offset_y   = 10 * move.noise_angle[0];
      animation.offset_x   = 10 * move.noise_angle[4];
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n];
      animation.angle      = 16 * polar_theta[n] + 16*move.radial[1];
      animation.z          = 500;
      animation.scale_x    = 0.06 ;
      animation.scale_y    = 0.06 ;
      animation.offset_z   = -10 * move.linear[1];
      animation.offset_y   = 10 * move.noise_angle[1];
      animation.offset_x   = 10 * move.noise_angle[3];
      animation.low_limit  = 0;
      show2                = render_value(animation);

     
      //float radius = radial_filter_radius;   // radius of a radial brightness filter
      //float radial = (radius-distance[n])/distance[n];
     
      pixel.r    = show1;
      pixel.b   = show2;
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Water() { 

    get_ready();

    

    timings.master_speed = 0.037;    // master speed 

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

      animation.dist       = distance[n] + 4*sinf(move.directional[5]*PI+(float)ledMap[n][xind]/2) + 4 * cosf(move.directional[6]*PI+float(ledMap[n][yind])/2);
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.06 ;
      animation.scale_y    = 0.06 ;
      animation.offset_z   = -10 * move.linear[0];
      animation.offset_y   = 10;
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = (10+move.directional[0]) * sinf(-move.radial[5]+move.radial[0]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (10+move.directional[1]) * sinf(-move.radial[5]+move.radial[1]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 500;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = (10+move.directional[2]) * sinf(-move.radial[5]+move.radial[2]+(distance[n] / (3)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 500;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[2];
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
      
     
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Parametric_Water() { 

    get_ready();

    

    timings.master_speed = 0.003;    // master speed 

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
      float f = 10 + 2* move.directional[0];

      animation.dist       = (f+move.directional[0]) * sinf(-move.radial[5]+move.radial[0]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (f+move.directional[1]) * sinf(-move.radial[5]+move.radial[1]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 500;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[1];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      animation.dist       = (f+move.directional[2]) * sinf(-move.radial[5]+move.radial[2]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 5000;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show4                = render_value(animation);

      animation.dist       = (f+move.directional[3]) * sinf(-move.radial[5]+move.radial[3]+(distance[n] / (s)));
      animation.angle      = 1 * polar_theta[n];
      animation.z          = 2000;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[3];
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
      
      
     
     
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment1() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] + 20 * move.directional[0];
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      pixel.b    = show1;
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment2() { 

    get_ready();

    

    timings.master_speed = 0.02;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] - ( 16 + move.directional[0] * 16);
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);
      
      pixel.r    = show1;
      pixel.g  = show1 - 80;
      pixel.b   = show1 - 150;
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment3() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.033;           // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      animation.dist       = distance[n] - (12 + move.directional[3]*4);
      animation.angle      = move.noise_angle[0] + move.noise_angle[1] + polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 ;
      animation.scale_y    = 0.1 ;
      animation.offset_z   = -10 ;
      animation.offset_y   = 20 * move.linear[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);
      
      pixel.r    = show1;
      pixel.g  = show1 - 80;
      pixel.b   = show1 - 150;
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }


  void Module_Experiment4() { 

    get_ready();

    

    timings.master_speed = 0.031;    // master speed 

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
      animation.z          = 5;
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.offset_z   = 0.1*move.linear[2] ;
      animation.offset_y   = -20 * move.linear[2];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = (distance[n] * distance[n])*0.8;
      animation.angle      = polar_theta[n];
      animation.z          = 50;
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.offset_z   = 0.1*move.linear[3] ;
      animation.offset_y   = -20 * move.linear[3];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = (distance[n] * distance[n])*0.9;
      animation.angle      = polar_theta[n];
      animation.z          = 5000;
      animation.scale_x    = 0.004 * s;
      animation.scale_y    = 0.003 * s;
      animation.offset_z   = 0.1*move.linear[4] ;
      animation.offset_y   = -20 * move.linear[4];
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
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment5() { 

    get_ready();

    

    timings.master_speed = 0.031;    // master speed 

    timings.ratio[0] = 0.0025;           // speed ratios for the oscillators, higher values = faster transitions
    timings.ratio[1] = 0.0027;
    timings.ratio[2] = 0.029;
    timings.ratio[3] = 0.33;
    timings.ratio[4] = 0.036;            // speed ratios for the oscillators, higher values = faster transitions
    
    calculate_oscillators(timings); 

    
    for (int n = 0; n < NUM_LEDS; n++) {
      animation.anglephi   = spherical_phi[n]; //todo, move this later

      float s = 1.5;

      animation.dist       = distance[n] + sinf(0.5*distance[n]-move.radial[3]);
      animation.angle      = polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[0] ;
      animation.offset_y   = -20 * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      
      
      pixel.r    = show1;
      
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment6() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 0.031

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

      animation.dist       = distance[n] + sinf(0.25*distance[n]-move.radial[3]);
      animation.angle      = polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[0] ;
      animation.offset_y   = -20 * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = distance[n] + sinf(0.24*distance[n]-move.radial[4]);
      animation.angle      = polar_theta[n];
      animation.z          = 10;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[1] ;
      animation.offset_y   = -20 * move.linear[1];
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
      
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment7() { 

    get_ready();

    

    timings.master_speed = 0.005;    // master speed 0.031

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

      animation.dist       = 2+distance[n] + 2*sinf(0.25*distance[n]-move.radial[3]);
      animation.angle      = polar_theta[n];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 10*move.linear[0] ;
      animation.offset_y   = -20 * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 2+distance[n] + 2*sinf(0.24*distance[n]-move.radial[4]);
      animation.angle      = polar_theta[n];
      animation.z          = 10;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[1] ;
      animation.offset_y   = -20 * move.linear[1];
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
      
      
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment8() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 0.031

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

      animation.dist       = 3+distance[n] + 3*sinf(0.25*distance[n]-move.radial[3]);
      animation.angle      = polar_theta[n] + move.noise_angle[0] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 10*move.linear[0] ;
      animation.offset_y   = -5 * r * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 4+distance[n] + 4*sinf(0.24*distance[n]-move.radial[4]);
      animation.angle      = polar_theta[n] + move.noise_angle[1] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[1] ;
      animation.offset_y   = -5 * r * move.linear[1];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = 5+distance[n] + 5*sinf(0.23*distance[n]-move.radial[5]);
      animation.angle      = polar_theta[n] + move.noise_angle[2] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[2] ;
      animation.offset_y   = -5 * r * move.linear[2];
      animation.offset_x   = 1000;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      show4 = colordodge(show1, show2);

      float rad = sinf(PI/2+distance[n]/14); // better radial filter?!

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */
      
      pixel.r    = rad * ((show1 + show2) + show3);
      pixel.g  = (((show2 + show3)*0.8)-90)*rad;
      pixel.b   = show4 * 0.2;
      
      
      
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment9() { 

    get_ready();

    

    timings.master_speed = 0.03;    // master speed 0.031

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
      animation.angle      = polar_theta[n] + move.radial[1];
      animation.z          = 5;
      animation.scale_x    = 0.001;
      animation.scale_y    = 0.1;
      animation.scale_z    = 0.1;
      animation.offset_y   = -10*move.linear[0];
      animation.offset_x   = 20;
      animation.offset_z   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      pixel.r    = 10*show1;
    
      pixel = rgb_sanity_check(pixel);
      
      buffer[n] = setPixelColor(pixel);
    }
  }

  void Module_Experiment10() { 

    get_ready();

    

    timings.master_speed = 0.01;    // master speed 0.031

    float w = 1;

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

      animation.dist       = 3+distance[n] + 3*sinf(0.25*distance[n]-move.radial[3]);
      animation.angle      = polar_theta[n] + move.noise_angle[0] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 10*move.linear[0] ;
      animation.offset_y   = -5 * r * move.linear[0];
      animation.offset_x   = 10;
      animation.low_limit  = 0;
      show1                = render_value(animation);

      animation.dist       = 4+distance[n] + 4*sinf(0.24*distance[n]-move.radial[4]);
      animation.angle      = polar_theta[n] + move.noise_angle[1] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[1] ;
      animation.offset_y   = -5 * r * move.linear[1];
      animation.offset_x   = 100;
      animation.low_limit  = 0;
      show2                = render_value(animation);

      animation.dist       = 5+distance[n] + 5*sinf(0.23*distance[n]-move.radial[5]);
      animation.angle      = polar_theta[n] + move.noise_angle[2] + move.noise_angle[6];
      animation.z          = 5;
      animation.scale_x    = 0.1 * s;
      animation.scale_y    = 0.1 * s;
      animation.offset_z   = 0.1*move.linear[2] ;
      animation.offset_y   = -5 * r * move.linear[2];
      animation.offset_x   = 1000;
      animation.low_limit  = 0;
      show3                = render_value(animation);

      show4 = colordodge(show1, show2);

      float rad = sinf(PI/2+distance[n]/14); // better radial filter?!

      
      /*
      pixel.r    = show1;
      pixel.g  = show1 * 0.3;
      pixel.b   = show2-show1;
      */
      
       CHSV(rad * ((show1 + show2) + show3), 255, 255);
      
      
      
      
      pixel = rgb_sanity_check(pixel);

      byte a = millis()/100;
      
      buffer[n] = CRGB( CHSV(((a + show1 + show2) + show3), 255, 255));
    }
  }



  void Rings() {

    get_ready(); 
    

    timings.master_speed = 0.01;    // speed ratios for the oscillators
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
      animation.scale_z    = 1;
      animation.dist       = distance[n];
      animation.offset_y   = -move.linear[0];
      animation.offset_x   = 0;
      float show1          = render_value(animation);

       // describe and render animation layers
      animation.angle      = 10;
      
      animation.dist       = distance[n];
      animation.offset_y   = -move.linear[1];
      float show2          = render_value(animation);

       // describe and render animation layers
      animation.angle      = 12;
      
      animation.dist       = distance[n];
      animation.offset_y   = -move.linear[2];
      float show3          = render_value(animation);

      // colormapping
      pixel.r   = show1;
      pixel.g = show2 / 4;
      pixel.b  = show3 / 4;

      pixel = rgb_sanity_check(pixel);

      buffer[n] = setPixelColor(pixel);
    }
  }


}; 
