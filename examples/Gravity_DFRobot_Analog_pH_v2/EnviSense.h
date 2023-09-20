#ifndef EnviSense_h
#define EnviSense_h

#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#define ReceivedBufferLength 10

class EnviSense {
public:
  EnviSense(void);
  // calibrate the Gravity Analog pH
  // parameters:
  // voltage     : Voltage value
  // temperature : Ambient temperature
  // calibration commands : enterph -> enter the PH calibration mode
  //                        calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
  //                        exitph  -> save the calibrated parameters and exit from PH calibration mode
  void calibrateGravitypH(float voltage, float temperature, char* cmd);
  void calibrateGravitypH(float voltage, float temperature);

  // read Gravity analog pH sensor
  // convert voltage to pH with temperature compensation
  // parameters:
  // voltage     : Voltage value
  // temperature : Ambient temperature
  // return the pH value (float)
  float getGravitypH(float voltage, float temperature);

  // initialization of the Gravity analog pH Sensor
  void initGravitypH();

private:
  float _phValue;
  float _acidVoltage;
  float _neutralVoltage;
  float _voltage;
  float _temperature;

  char _cmdReceivedBuffer[ReceivedBufferLength];  
  byte _cmdReceivedBufferIndex;

private:
  boolean cmdSerialDataAvailable();
  void phCalibration(byte mode);  
  byte cmdParse(const char* cmd);
  byte cmdParse();
};

#endif