/*
  ___        _            
 / _ \      (_)
/ /_\ \_ __  _ _ __ ___
|  _  | '_ \| | '_ ` _ \
| | | | | | | | | | | | |
\_| |_/_| |_|_|_| |_| |_

by Clark Reinholtz, 
IMU input DSP

VO.42 beta version
 
This code is licensed under a Creative Commons Attribution 
License CC BY-NC 3.0

*/

//#include <SmartMatrix.h>
#pragma once
#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <vector3d.h>
#include "ANIMutils.h"


Adafruit_ICM20948 icm;



class ANIMimu {

public:
  bool verbose = false;

  //actual sensor objects from adafruit library
  volatile sensors_event_t accel;
  volatile sensors_event_t gyro;
  volatile sensors_event_t mag;
  volatile sensors_event_t temp;

  //timings
  unsigned long lastUpdateMillis;
  float deltaUpdateSeconds;

  //acc members
  Vector3d accRaw;
  float accIirWeight = 0.7; // higher = slower lowpass more weight to iir... lower = more weight to raw.  must be  >0 <1
  Vector3d accIir; // lowpassed acc data
  float accGForce; // magnitude of ACC measurments

  Vector3d gyrRotationRaw;
  Vector3d gyrPosition;
  Vector3d magRaw;

  float accTrust ; // higher = more weight to ACC, lower = more weight to gyro in our complimentary filter
  Vector3d filteredPosition;




  //    IMU orientation VS pixel map 
  //   if you are standing, looking at your art align the IMU with how you mapped your pixels. 
  //   Look for the x-> silkscreen on the IMU pcb
  //    __________________
  //   |                  |
  //   |       +x-> right | +y^ up towords the sky
  //   |       | |        |
  //   |__________________|
  //                         +z @ IN or away from you


  ANIMimu() {
    this->init();
    
  }

  void init() {
    lastUpdateMillis = millis();
    gyrPosition.set(9.8,0,0);
    filteredPosition.set(0,0,9.8);
  }

  bool begin(){
    if(!icm.begin_I2C()) {
      Serial.println("Failed to find ICM20948 chip");
      return false;
    } else {
      Serial.println("ICM20948 Found!");
      return true;
    }
  }

  void debug(){
    EVERY_N_MILLIS(50) if (verbose){

      Serial.print("iirx:");
      Serial.print(accIir.mX);
      Serial.print(",iiry:");
      Serial.print(accIir.mY);
      Serial.print(",iirz:");
      Serial.print(accIir.mZ);              
    
/*
      Serial.print(",AccelX:");
      Serial.print(accel.acceleration.x);
      Serial.print("AccelX:");
      Serial.print(accel.acceleration.x);
      Serial.print(",AccelY:");
      Serial.print(accel.acceleration.y);
      Serial.print(",AccelZ:");
      Serial.print(accel.acceleration.z); 
      Serial.print(",accGForce:");
      Serial.print(accGForce);

      Serial.print(",accTrust:");
      Serial.print(accTrust);
  

*/

      //Serial.print(",GyroX:");
      //Serial.print(gyro.gyro.x);
      //Serial.print(",GyroY:");
      //Serial.print(gyro.gyro.y);

      //Serial.print(",GyroZ:");
      //Serial.print(gyro.gyro.z);

      Serial.print(",igyrx:");
      Serial.print(gyrPosition.mX);
      Serial.print(",igyry:");
      Serial.print(gyrPosition.mY);
      Serial.print(",igyrz:");
      Serial.print(gyrPosition.mZ);   
      

      /*


      Serial.print(",Mag X:");
      Serial.print(mag.magnetic.x);
      Serial.print(",Y:");
      Serial.print(mag.magnetic.y);
      Serial.print(",Z:");
      Serial.print(mag.magnetic.z);
      */


      Serial.println();
    }
  }

  void update(){
    EVERY_N_MILLIS(10){


      icm.getEvent(&accel, &gyro, &temp, &mag);


      //update loop timings
      deltaUpdateSeconds = ((float) (millis() - lastUpdateMillis))/1000.0;
      lastUpdateMillis = millis();

      //some elements of this brough to you by:
      //others I just made up, and look at that, it works alright
      //todo https://www.youtube.com/watch?v=0rlvvYgmTvI

      //set the acc
      accRaw.set((accel.acceleration.x), (accel.acceleration.y), (-accel.acceleration.z)); // scale acc by -9.8 to get the "down" vector in G's
      accIir = (accIir * accIirWeight) + (accRaw * (1-accIirWeight));
      accGForce = accIir.length(); // acc units is already in G's (9.8 m/s)
      //accTrust = map_float(absf(accGForce-1), 0, 5, .5, .05);//trust is high trust (.5) if exactly 1g, and low (.05) if <.5 g or >1.5g because the object is in motion and acc will be noisy
      
      magRaw.set(mag.magnetic.x, mag.magnetic.y, mag.magnetic.z);//todo, calabrate magnotometer using magcal in examply code
      
      gyrRotationRaw.set(gyro.gyro.x, gyro.gyro.y, -gyro.gyro.z); //todo this should be FITERED position
      gyrPosition = filteredPosition;
      deltaUpdateSeconds = deltaUpdateSeconds;

      //rotate about y axis
      gyrPosition.roll(-deltaUpdateSeconds * gyrRotationRaw.mY);
      //rotate about x axis
      gyrPosition.pitch(-deltaUpdateSeconds * gyrRotationRaw.mX);
      //rotate about z axis
      gyrPosition.yaw(-deltaUpdateSeconds * gyrRotationRaw.mZ);

      accTrust = 0.15;
      filteredPosition = (gyrPosition * (1-accTrust))+ ( accIir * accTrust);
      

          //todo
      //downAcc 
      //east = down cross mag
      //north = east cross down


      debug();
    } // n millis
  }  


}; 

