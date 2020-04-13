#include <Wire.h>
#include <Adafruit_PWMServoDriver.h>

int opticalSensorPin = A3;
Adafruit_PWMServoDriver pwm = Adafruit_PWMServoDriver();
const float v_in_max = 4;
const float v_in_min = 1;
const float v_on = 2;
const float v_switch = 3;
const float v_switch_rate = 12;

float normalizeVoltage(float volt){
  return (volt- v_in_min)/(v_in_max-v_in_min);
}

float v_switch_norm = normalizeVoltage(v_switch);

float coeffs[16][4] = {
  // Duty Range, Duty Offset, Switch Rate, Switch Offset
  // LED_1 RGBA (SIDE)
  {0.35, 0.26, v_switch_rate, v_switch_norm},
  {0, 0.84, 0, 0},
  {0.17, 0.1, -v_switch_rate, v_switch_norm},
  {0.07, 0.87, v_switch_rate, v_switch_norm},
  // LED_2 RGBA (CENTER)
  {0.22, 0.26, v_switch_rate, v_switch_norm},
  {0, 0.84, 0, 0},
  {0.17, 0.13, -v_switch_rate, v_switch_norm},
  {0.07, 0.8, v_switch_rate, v_switch_norm},
  // LED_3 RGBA (CENTER)
  {0.22, 0.26, v_switch_rate, v_switch_norm},
  {0, 0.84, 0, 0},
  {0.17, 0.13, -v_switch_rate, v_switch_norm},
  {0.07, 0.8, v_switch_rate, v_switch_norm},
  // LED_4 RGBA (SIDE)
  {0.35, 0.26, v_switch_rate, v_switch_norm},
  {0, 0.84, 0, 0},
  {0.17, 0.1, -v_switch_rate, v_switch_norm},
  {0.07, 0.87, v_switch_rate, v_switch_norm}
};

int offTimes[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float sensorReadings[36] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int sensorReadingsInd = 0;

void setup() {
    // Serial.begin(9600);
    pwm.begin();
    pwm.setPWMFreq(1000);
}

void loop() {
  // Drive each PWM in a 'wave'
  float sensorVoltageNormalized = getConditionedOpticalSensorReading();
  writePWMDutyCycles(sensorVoltageNormalized);
  delay(10000);
}

// Returns optical sensor reading as normalized voltage
float getOpticalSensorReading(){
  float rawReading = (float)analogRead(opticalSensorPin);
  float normalizedReading = normalizeVoltage(rawReading);
  if(normalizedReading < 0) return 0;
  else if(normalizedReading > 1) return 1;
  else return normalizedReading;
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
  return normalizeVoltage(readingsSum/36.0);
}

void writePWMDutyCycles(float sensorVoltageNormalized) {
  for(int i = 0; i < 16; ++i){
    float offTime = 0;
    if (sensorVoltageNormalized > v_switch_norm){
      offTime = (coeffs[i][0]*1.0/(1.0+exp(-coeffs[i][2]*(sensorVoltageNormalized-coeffs[i][3]))) + coeffs[i][1])*4095.0;  
    }
    pwm.setPWM(i,0,offTime);
  }
}
