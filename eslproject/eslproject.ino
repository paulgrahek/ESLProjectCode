#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

int opticalSensorPin = A3;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
int offTimes[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float sensorReadings[36] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int sensorReadingsInd = 0;
float sensorReading = 1; 

void setup() {
    // Serial.begin(9600);
    pwm.begin();
    pwm.setPWMFreq(500);
}

void loop() {
  // Drive each PWM in a 'wave'
  getConditionedOpticalSensorReading();
  getPWMDutyCycleOffTimes();
  writePWMDutyCycles();
  delay(10000);
}

// TODO: Account for imperfect "1" from on state, ensure values returned vary 1 to 0 with "off" and "on" states, respectively
float getOpticalSensorReading(){
  return 1.0-(float)analogRead(opticalSensorPin)/1023.0;
}

float getConditionedOpticalSensorReading(){
  // Returns Optical Sensor Reading normalized between 0 and 1
  sensorReadings[sensorReadingsInd] = getOpticalSensorReading();
  sensorReadingsInd += 1;
  if(sensorReadingsInd == 36){
    sensorReadingsInd = 0;
  }
  
  float readingsSum = 0;
  for(int i = 0; i < 36; ++i){
    readingsSum += sensorReadings[36];
  }
  return readingsSum/36.0;
}

void getPWMDutyCycleOffTimes() {
  for(int i = 0; i < 16; ++i){
    offTimes[i] = 1.0/(1.0+exp(-sensorReading))*4095.0;
  }
}

void writePWMDutyCycles(){
  for(int i = 0; i < 16; ++i){
    pwm.setPWM(i,0,offTimes[i]);
  }
}
