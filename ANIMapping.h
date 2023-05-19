/*
  ___        _            ___  ______
 / _ \      (_)          / _ \ | ___ \
/ /_\ \_ __  _ _ __ ___ / /_\ \| |_/ /
|  _  | '_ \| | '_ ` _ \|  _  ||  __/
| | | | | | | | | | | | | | | || |
\_| |_/_| |_|_|_| |_| |_\_| |_/\_|

by Clark Reinholtz

Expand ANIMartTRIX to allow 3d pixel mapping and spherical coordinates

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

//#include <SmartMatrix.h>
#include <ANIMartRIX.h>

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

//#define USE_3D_MAP true  //this turns off some unneeded polar things if you are using a 3d map TODO...

#ifndef NUM_LEDS
#define NUM_LEDS 100
#endif




class ANIMapping : ANIMartRIX {

  public:

  // TODO set sizes
  float spherical_phi[NUM_LEDS];        // look-up table for spherical angles


  ANIMapping() {}

  float map[NUM_LEDS][3];

  ANIMapping(float * mapPtr, struct CRGB *data) : ANIMartRIX(NUM_LEDS, 1, data, false){  //use num leds as width, and set height to 1, serpintine false
    this-> map = mapPtr; //this sets map to the address of the mapPtr
    render_polar_lookup_table();
  }





  float render_value(render_parameters &animation) {
    EVERY_N_SECONDS(1){Serial.println("derrived class render")}

    // convert **SPHERICAL** coordinates back to cartesian ones
    //this is really the only difference from base class
    float newx = (animation.offset_x + animation.center_x - (newdist * sinf(animation.anglephi) * cosf(animation.angle))) * animation.scale_x;
    float newy = (animation.offset_y + animation.center_y - (newdist * sinf(animation.anglephi) * sinf(animation.angle))) * animation.scale_y;
    float newz = (animation.offset_z + animation.center_z - (newdist * cosf(animation.anglephi))) * animation.scale_z;

    // render noisevalue at this new cartesian point
    //uint16_t raw_noise_field_value =inoise16(newx, newy, newz);
    float raw_noise_field_value = pnoise(newx, newy, newz);

    // A) enhance histogram (improve contrast) by setting the black and white point (low & high_limit)
    // B) scale the result to a 0-255 range (assuming you want 8 bit color depth per rgb chanel)
    // Here happens the contrast boosting & the brightness mapping

    if (raw_noise_field_value < animation.low_limit)  raw_noise_field_value =  animation.low_limit;
    if (raw_noise_field_value > animation.high_limit) raw_noise_field_value = animation.high_limit;

    float scaled_noise_value = map_float(raw_noise_field_value, animation.low_limit, animation.high_limit, 0, 255);

    return scaled_noise_value;
  }


  // given a static polar origin we can precalculate 
  // the polar coordinates

  void render_polar_lookup_table() {
    //TODO make sure this overwriets base class
    println("polar lookup in derrived class")
    //change the index of your map here
    //I recomend making X and Y LeftRight and DownUp on the face viewed from your primary viewing angle, and Z being render_spherical_lookup_table
    int x_ind=0; int y_ind=2; int z_ind = 1;

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

        distance[n] = sqrt(pow(dx, 2) + pow(dy, 2) + pow(dz, 2));
        polar_theta[n] = atan2f(dy,dx);
        spherical_phi[n] = acosf(dz / lookup_distance[n]);
    }
  }



  typedef void (*Pattern)();
  typedef Pattern PatternList[];
  typedef struct {
    Pattern pattern;
    String name;
  } PatternAndName;
  typedef PatternAndName PatternAndNameList[];

  int currentPattern = 0;


  static const PatternAndNameList gPatterns = {
    {Module_Experiment10,"Module_Experiment10"},
    {Module_Experiment9,"Module_Experiment9"},
    {Module_Experiment8,"Module_Experiment8"},
    {Module_Experiment7,"Module_Experiment7"},
    {Module_Experiment6,"Module_Experiment6"},
    {Module_Experiment5,"Module_Experiment5"},
    {Module_Experiment4,"Module_Experiment4"},
    {Zoom2, "Zoom2"}, 
    {Module_Experiment3,"Module_Experiment3"},
    {Module_Experiment2,"Module_Experiment2"},
    {Module_Experiment1,"Module_Experiment1"},
    {Parametric_Water,"Parametric_Water"},
    {Water,"Water"},
    {Complex_Kaleido_6,"Complex_Kaleido_6"},
    {Complex_Kaleido_5,"Complex_Kaleido_5"},
    {Complex_Kaleido_4,"Complex_Kaleido_4"},
    {Complex_Kaleido_3, "Complex_Kaleido_3"},
    {Complex_Kaleido_2,"Complex_Kaleido_2"},
    {Complex_Kaleido, "Complex_Kaleido"},
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
    {Zoom,"Zoom"},
    {Hot_Blob,"Hot_Blob"},
    {Spiralus2,"Spiralus2"},
    {Spiralus,"Spiralus"},
    {Yves,"Yves"},
    {Scaledemo1,"Scaledemo1"},
    {Lava1,"Lava1"},
    {Caleido3,"Caleido3"},
    {Caleido2,"Caleido2"},
    {Caleido1,"Caleido1"},
    {Distance_Experiment,"Distance_Experiment"},
    {Center_Field,"Center_Field"},
    {Waves,"Waves"},
    {Chasing_Spirals,"Chasing_Spirals"},
    {Rotating_Blob,"Rotating_Blob"},
  };




  int gPatternCount = ARRAY_SIZE(gPatterns);

  void incrementPattern(){
    currentPattern = currentPattern + 1;
    if (currentPattern >= gPatternCount) currentPattern = 0;
  }
  void showCurrentPattern(){
    gPatterns[currentPattern]();
    FastLED.show();
  }

}; 

