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

#ifndef NUM_LEDS
#define NUM_LEDS 100
#endif


extern float ledMap[NUM_LEDS][3]; // TODO, make this better...

class ANIMapping : public ANIMartRIX {

  public:

  ANIMapping() {}



  ANIMapping(struct CRGB *data, float scale = 1.0) : ANIMartRIX(NUM_LEDS, 1, data, false){  //use num leds as width, and set height to 1, serpintine false
    render_spherical_lookup_table();
    setGlobalScale(scale);
  }



  // given a static polar origin we can precalculate 
  // the polar coordinates

  void render_spherical_lookup_table() {
    //TODO make sure this overwriets base class
    Serial.println("polar lookup overwritten in derrived class");
    //change the index of your map here
    // maps are defined as [led][axis] where axis is x y or z position,  
    //this just allows you to change which axis number: 0 1 or 2 corrosponds to what variable x, y, or z
    //I recomend making X and Y LeftRight and DownUp on the face viewed from your primary viewing angle, and Z being render_spherical_lookup_table
    int xind=0; int yind=2; int zind = 1;

    //initialize min / max
    float xmin = ledMap[0][xind];
    float xmax = xmin;
    float ymin = ledMap[0][yind];
    float ymax = ymin;
    float zmin = ledMap[0][zind];
    float zmax = zmin;

    //min and max mapping used for finding center
    for (int n = 1; n < NUM_LEDS; n++) {
      if(ledMap[n][xind] > xmin) xmin = ledMap[n][xind];
      if(ledMap[n][xind] < xmax) xmax = ledMap[n][xind];
      if(ledMap[n][yind] > ymin) ymin = ledMap[n][yind];
      if(ledMap[n][yind] < ymax) ymax = ledMap[n][yind];
      if(ledMap[n][zind] > zmin) zmin = ledMap[n][zind];
      if(ledMap[n][zind] < zmax) zmax = ledMap[n][zind];
    }

    for (int n = 0; n < NUM_LEDS; n++) {
        float dx = ledMap[n][0] - ((xmax - xmin)/2.0 + xmin);
        float dy = ledMap[n][1] - ((ymax - ymin)/2.0 + ymin);
        float dz = ledMap[n][2] - ((zmax - zmin)/2.0 + zmin);

        distance[n][0] = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        polar_theta[n][0] = atan2f(dy,dx); //todo these extra indecies are wasted
        spherical_phi[n] = acosf(dz / distance[n][0]);
    }
  }



}; 
