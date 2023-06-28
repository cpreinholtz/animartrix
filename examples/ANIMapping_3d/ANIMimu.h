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

#include <Adafruit_ICM20948.h>
#include <Adafruit_ICM20X.h>
#include <vector3d.h>


#define ICM_CS 10
// For software-SPI mode we need SCK/MOSI/MISO pins
#define ICM_SCK 13
#define ICM_MISO 12
#define ICM_MOSI 11





//helper functions todo move to a new place?
float map_float(float x, float in_min, float in_max, float out_min, float out_max) { 
  
  float result = (x-in_min) * (out_max-out_min) / (in_max-in_min) + out_min;
  if (result < out_min) result = out_min;
  if( result > out_max) result = out_max;

  return result; 
}









class ANIMimu {

public:
  bool debug = false;

  Adafruit_ICM20948 icm;
  Adafruit_Sensor *icm_temp, *icm_accel, *icm_gyro, *icm_mag;

  sensors_event_t accel;
  sensors_event_t gyro;
  sensors_event_t temp;
  sensors_event_t mag;

  unsigned long lastUpdateMillis;
  float deltaUpdateSeconds;

  //assume a 9 dof IMU
  Vector3d accRaw;
  Vector3d gyrRotationRaw;
  Vector3d gyrPositionRaw;
  Vector3d magRaw;

  Vector3d filteredPosition;



  unsigned long lastUpdateMillis;

  ANIMimu(bool verbose=false) {
    this->init(verbose);
  }

  void init(bool verbose=false) {
    debug=verbose;
    lastUpdateMillis = millis();

    if (!icm.begin_I2C()) {
      // if (!icm.begin_SPI(ICM_CS)) {
      // if (!icm.begin_SPI(ICM_CS, ICM_SCK, ICM_MISO, ICM_MOSI)) {
      Serial.println("Failed to find ICM20948 chip");
    } else {
      Serial.println("ICM20948 Found!");
      icm_temp = icm.getTemperatureSensor();
      icm_temp->printSensorDetails();
      icm_accel = icm.getAccelerometerSensor();
      icm_accel->printSensorDetails();
      icm_gyro = icm.getGyroSensor();
      icm_gyro->printSensorDetails();
      icm_mag = icm.getMagnetometerSensor();
      icm_mag->printSensorDetails();
    }

  }


  void update(Vector3d a,Vector3d g, Vector3d m){
    
    //get sensor data
    icm_accel->getEvent(&accel);
    icm_gyro->getEvent(&gyro);
    icm_mag->getEvent(&mag);

    //update loop timings
    deltaUpdateSeconds = ((float) (millis() - lastUpdateMillis))/1000.0;
    lastUpdateMillis = millis();


    //todo https://www.youtube.com/watch?v=0rlvvYgmTvI
    //https://www.mathworks.com/help/fusion/ug/Estimating-Orientation-Using-Inertial-Sensor-Fusion-and-MPU-9250.html;jsessionid=9938735d9bc2285ad4f23e5ca508?s_eid=PSM_15028

    //set the raw acc and mag vectors
    accRaw.set(accel.acceleration.x, accel.acceleration.y, accel.acceleration.z);
    magRaw.set(mag.magnetic.x, mag.magnetic.y, mag.magnetic.z);
    //todo, calabrate magnotometer using magcal in examply code


    //accTrust: map the magnitude of acceleration - 1g to a trust number
    //trust is high trust (.5) if exactly 1g, and low (.05) if <.5 g or >1.5g because the object is in motion and acc will be noisy
    accTrust = map_float(myabs(accRaw.length()-9.8), 0, 5, .5, .05);



    //set gry rotation using sensor data
    gyrRotationRaw.set(gyro.gyro.x, gyro.gyro.y, gyro.gyro.z);
    //integrate in rad/s to get gyro radians, but merge with filtered position.  pitch roll and yaw are all valid
    gyrPositionRaw = filteredPosition + (deltaUpdateSeconds * gyrRotationRaw);
    








    dbgSensor();
  }  






  void dbgSensor(){
    if(debug){
      /* Display the results (acceleration is measured in m/s^2) */
      Serial.print("\t\tAccel X: ");
      Serial.print(accel.acceleration.x);
      Serial.print(" \tY: ");
      Serial.print(accel.acceleration.y);
      Serial.print(" \tZ: ");
      Serial.print(accel.acceleration.z);
      Serial.println(" m/s^2 ");

      /* Display the results (rotation is measured in rad/s) */
      Serial.print("\t\tGyro X: ");
      Serial.print(gyro.gyro.x);
      Serial.print(" \tY: ");
      Serial.print(gyro.gyro.y);
      Serial.print(" \tZ: ");
      Serial.print(gyro.gyro.z);
      Serial.println(" radians/s ");
      Serial.println();

      Serial.print("\t\tMag X: ");
      Serial.print(mag.magnetic.x);
      Serial.print(" \tY: ");
      Serial.print(mag.magnetic.y);
      Serial.print(" \tZ: ");
      Serial.print(mag.magnetic.z);
      Serial.println(" uT");
    }
  }

}; 





class ANIMaudio {

public:



  float global_bpm = 115.0;
  float energy_level = 0.0;

  int raw_signal;
  unsigned long raw_time_ms;
  float raw_min = 0;
  float raw_max = 1023;
  float raw_ac_couple = raw_max/2; // this needs to be the "0" level of your aplifier!!

  float scaled_min = -1.0;
  float scaled_max = 1.0;
  float scaled_signal;

  float abs_signal;

  IIR iir_volume; //lowpass iir tuned to a VERY slow update time for measuring volume
  IIR iir_lowpass; //lowpass iir tuned to a  slow update time for measuring volume bass kicks



  bool beat_detected;
  float beat_last;
  float beat_delta_min = 100; //essentially a debounce in miliseconds, 100 milliseconds limits to 1/16 notes @ 120 bpm
  float beat_multiplier_min = 1.5; //must hit moving average * multiplier to be considered a beat

  static const int bpm_num_samples = 21; // use odd to prevent DC offset
  static const int bpm_median_sample = bpm_num_samples/2;
  int bpm_current_sample = 0; //round robbin index, increments from 0 to bpm_num_samples-1 and wraps around
  float bpm_deltas[bpm_num_samples]; //keep values from the latest beat intevals
  float bpm_sorted_deltas[bpm_num_samples]; //keep values from the latest beat intevals (this list is sorted in ascending order)
  float bpm_median_delta; //average (median) time between beats in miliseconds.  this is the period of the pulse in millis
  float bpm; //the estimated bpm, collected by using the median.  this is the frequency of the pulse in millis

  int audioPin = 1;

  ANIMaudio() {
    this->init();
  }

  void init() {
    

    //To the extent that time constants mean anything in discrete time constant would be
    //-(sample time) * ln(1-weight)

    //assume sampling 100 times per second, sample time =.1
    //wt = .999; 5tau = 5 * -.1* ln(.001); 5tau = 3.4538 seconds
    //wt = .99; 5tau = 5 * -.1* ln(.01); 5tau = 2.3 seconds
    //wt = .9; 5tau = 5 * -.1* ln(.1); 5tau = 1.1 seconds
    //wt = .75; 5tau = 5 * -.1* ln(.25); 5tau = 0.69 seconds
    //wt = .5; 5tau = 5 * -.1* ln(.5); 5tau = 0.3 seconds
    iir_volume.setWeight(.99);
    iir_lowpass.setWeight(.5);
  }


  void update(){
    //read the analog pin and record the current time
    raw_signal = analogRead(audioPin);
    raw_time_ms = millis();
    
    //scale from -1 to 1 and remove DC bias
    scaled_signal = map_float(((float) raw_signal) - raw_ac_couple), raw_min, raw_max, scaled_min, scaled_max);
    
    //take the abs value of that to get the current energy
    abs_signal = (scaled_signal >= 0.0) ? scaled_signal : - scaled_signal;
    
    // update filters
    iir_volume.update(abs_signal);
    iir_lowpass.update(abs_signal);

    //beat detected if its been a while since the last beat and the current energy is much higher than the average
    float delta = raw_time_ms - beat_last;
    if(delta > beat_delta_min && iir_lowpass.signal > iir_volume.signal * beat_multiplier_min){
      beat_last = raw_time_ms;
      beat_detected = true;

      //put this delta into the buffer
      bpm_deltas[bpm_current_sample] = delta;

      //bpm_deltas is a round robin buffer, increment bpm_current_sample index and wrap around if needed
      bpm_current_sample ++;
      if (bpm_current_sample >= bpm_num_samples) bpm_current_sample = 0;

      //sort then for median
      selectionSort(bpm_deltas, bpm_sorted_deltas, bpm_num_samples);
      //todo really this isnt so good, because many 8th notes will skew the quarter notes,  
      //really you should somehow detect if samples are multiples of one another within some margin, 
      //and throw out the smaller ones, that should leave you with only quarters?
      //honestly the whole bpm thing is totally uneeded anyways
      bpm_median_delta = bpm_sorted_deltas[bpm_median_sample];
      bpm = 60000/bpm_median_delta;// to convert millis per beat to bpm, invert to get beats per milli, then multiply by 60000 millis per 1 second


    } else {
      beat_detected = false;
    }




  }




}; 
