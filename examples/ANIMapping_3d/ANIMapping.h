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

//#include <SmartMatrix.h>
#define USE_3D_MAP
#include "ANIMartRIX.h" //TODO make <> when you copy files back to the right directory

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//#define USE_3D_MAP true  //this turns off some unneeded polar things if you are using a 3d map TODO...

extern float ledMap[NUM_LEDS][3]; // TODO, make this better...

template <std::size_t LedArraySize>
class ANIMapping : public ANIMartRIX <1,LedArraySize> {  //derived from the matrix clas but set y size to 1 dimension, using Y because it is the most 

  public:
  
  
  //template <std::size_t LedArraySize>
  ANIMapping(): ANIMartRIX<1,LedArraySize>(){}


  ANIMapping(struct CRGB *data, float scale = 1.0): ANIMartRIX<1,LedArraySize>(data, false){  //use num leds as width, and set height to 1, serpintine false
    render_spherical_lookup_table();
    ANIMartRIX<1,LedArraySize>::setGlobalScale(scale);
  }



  // given a static polar origin we can precalculate 
  // the spherical coordinates
  // this overwrites the polar lookup table, should be called after the base class constructor
  //template <std::size_t LedArraySize>
  void render_spherical_lookup_table() {
    //change the x, y, z index of your map here
    // maps are defined as [led][axis] where axis is x y or z position,  
    //this just allows you to change which axis number: 0 1 or 2 corrosponds to what variable x, y, or z
    //I recomend making X and Y LeftRight and DownUp on the face viewed from your primary viewing angle, and Z being render_spherical_lookup_table
    int xind=0; int yind=2; int zind = 1;

    //initialize min / max for finding center of each axis
    float xmin = ledMap[0][xind];
    float xmax = xmin;
    float ymin = ledMap[0][yind];
    float ymax = ymin;
    float zmin = ledMap[0][zind];
    float zmax = zmin;

    //min and max mapping used for finding center
    for (int n = 1; n < (int) LedArraySize; n++) {
      if(ledMap[n][xind] > xmin) xmin = ledMap[n][xind];
      if(ledMap[n][xind] < xmax) xmax = ledMap[n][xind];
      if(ledMap[n][yind] > ymin) ymin = ledMap[n][yind];
      if(ledMap[n][yind] < ymax) ymax = ledMap[n][yind];
      if(ledMap[n][zind] > zmin) zmin = ledMap[n][zind];
      if(ledMap[n][zind] < zmax) zmax = ledMap[n][zind];
    }

    float cx = (xmax - xmin)/2.0 + xmin;
    float cy = (ymax - ymin)/2.0 + ymin;
    float cz = (zmax - zmin)/2.0 + zmin;
    
    Serial.print("x center: ");Serial.println(cx);
    Serial.print("y center: ");Serial.println(cy);
    Serial.print("z center: ");Serial.println(cz); Serial.println();

    float maxD = 0.0;
    for (int n = 0; n < NUM_LEDS; n++) {
        float dx = ledMap[n][xind] - cx;
        float dy = ledMap[n][yind] - cy;
        float dz = ledMap[n][zind] - cz;
        
        ANIMartRIX<1,LedArraySize>::distance[0][n] = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        ANIMartRIX<1,LedArraySize>::polar_theta[0][n] = atan2f(dy,dx); //todo these extra indecies are wasted
        ANIMartRIX<1,LedArraySize>::spherical_phi[0] = acosf(dz / ANIMartRIX<1,LedArraySize>::distance[n][0]);

        if (maxD < ANIMartRIX<1,LedArraySize>::distance[n][0]) maxD = ANIMartRIX<1,LedArraySize>::distance[0][n];
        Serial.print("x: ");Serial.println(dx);
        Serial.print("y: ");Serial.println(dy);
        Serial.print("z: ");Serial.println(dz); 

        Serial.print("d: ");Serial.println(ANIMartRIX<1,LedArraySize>::distance[0][n]); // note this index must match what you gave the base class template,  I'm using Y
        Serial.print("t: ");Serial.println(ANIMartRIX<1,LedArraySize>::polar_theta[0][n]);
        Serial.print("p: ");Serial.println(ANIMartRIX<1,LedArraySize>::spherical_phi[n]); Serial.println();
        
        
    }
    Serial.print("max mapped distance: "); Serial.println(maxD);
    Serial.print("reccomended radias filter: "); Serial.println(maxD*1.3);
  }

  // show current framerate and rendered pixels per second
  void report_performance() {
   
    int fps = FastLED.getFPS();                 // frames per second
    int kpps = (fps * LedArraySize) / 1000;   // kilopixel per second
  
    Serial.print(kpps); Serial.print(" kpps ... ");
    Serial.print(fps); Serial.print(" fps @ ");
    Serial.print(LedArraySize); Serial.println(" LEDs ... ");
  }



}; 
