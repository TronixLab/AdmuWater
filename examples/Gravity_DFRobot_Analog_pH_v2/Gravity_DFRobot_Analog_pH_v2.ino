/*
 * file DFRobot_PH.ino
 * @ https://github.com/DFRobot/DFRobot_PH
 *
 * This is the sample code for Gravity: Analog pH Sensor / Meter Kit V2, SKU:SEN0161-V2
 * In order to guarantee precision, a temperature sensor such as DS18B20 is needed, to execute automatic temperature compensation.
 * You can send commands in the serial monitor to execute the calibration.
 * Serial Commands:
 *   enterph -> enter the calibration mode
 *   calph   -> calibrate with the standard buffer solution, two buffer solutions(4.0 and 7.0) will be automaticlly recognized
 *   exitph  -> save the calibrated parameters and exit from calibration mode
 *
 * Copyright   [DFRobot](http://www.dfrobot.com), 2018
 * Copyright   GNU Lesser General Public License
 *
 * version  V1.0
 * date  2018-04
 */

#include "EnviSense.h"
#include <EEPROM.h>

#define PH_PIN A0
float voltage, phValue, temperature = 25;

EnviSense UC;  // create EnviSense functions object to be call

void setup() {
  Serial.begin(115200);
  UC.initGravitypH();
}

void loop() {
  static unsigned long timepoint = millis();
  if (millis() - timepoint > 1000U) {  //time interval: 1s
    timepoint = millis();
    //temperature = readTemperature();         // read your temperature sensor to execute temperature compensation
    voltage = analogRead(PH_PIN) / 1024.0 * 5000;  // read the voltage
    phValue = UC.getGravitypH(voltage, temperature);     // convert voltage to pH with temperature compensation
    Serial.print("temperature:");
    Serial.print(temperature, 1);
    Serial.print("^C  pH:");
    Serial.println(phValue, 2);
  }

  UC.calibrateGravitypH(voltage, temperature);  // calibration process by Serail CMD
}

float readTemperature() {
  //add your code here to get the temperature from your temperature sensor
}
