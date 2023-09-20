#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EnviSense.h"
#include <EEPROM.h>

#define EEPROM_write(address, p) \
  { \
    int i = 0; \
    byte *pp = (byte *)&(p); \
    for (; i < sizeof(p); i++) EEPROM.write(address + i, pp[i]); \
  }

#define EEPROM_read(address, p) \
  { \
    int i = 0; \
    byte *pp = (byte *)&(p); \
    for (; i < sizeof(p); i++) pp[i] = EEPROM.read(address + i); \
  }

#define PHVALUEADDR 0x00  //the start address of the pH calibration parameters stored in the EEPROM


EnviSense::EnviSense(void) {
  this->_temperature = 25.0;
  this->_phValue = 7.0;
  this->_acidVoltage = 2032.44;    //buffer solution 4.0 at 25C
  this->_neutralVoltage = 1500.0;  //buffer solution 7.0 at 25C
  this->_voltage = 1500.0;
}

void EnviSense::initGravitypH() {
  // load the neutral (pH = 7.0)voltage of the pH board from the EEPROM
  EEPROM_read(PHVALUEADDR, this->_neutralVoltage);

  if (EEPROM.read(PHVALUEADDR) == 0xFF && EEPROM.read(PHVALUEADDR + 1) == 0xFF && EEPROM.read(PHVALUEADDR + 2) == 0xFF && EEPROM.read(PHVALUEADDR + 3) == 0xFF) {
    this->_neutralVoltage = 1500.0;
    // new EEPROM, write typical voltage
    EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
  }

  // load the acid (pH = 4.0) voltage of the pH board from the EEPROM
  EEPROM_read(PHVALUEADDR + 4, this->_acidVoltage);

  if (EEPROM.read(PHVALUEADDR + 4) == 0xFF && EEPROM.read(PHVALUEADDR + 5) == 0xFF && EEPROM.read(PHVALUEADDR + 6) == 0xFF && EEPROM.read(PHVALUEADDR + 7) == 0xFF) {
    this->_acidVoltage = 2032.44;
    // new EEPROM, write typical voltage
    EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
  }
}

float EnviSense::getGravitypH(float voltage, float temperature) {
  // based on two point calibration (neutralVoltage, 7.0), (acidVoltage, 4.0) --> pH = m*x + b
  float slope = (7.0 - 4.0) / ((this->_neutralVoltage - 1500.0) / 3.0 - (this->_acidVoltage - 1500.0) / 3.0);
  float intercept = 7.0 - slope * (this->_neutralVoltage - 1500.0) / 3.0;
  this->_phValue = slope * (voltage - 1500.0) / 3.0 + intercept;
  return _phValue;
}

void EnviSense::calibrateGravitypH(float voltage, float temperature, char *cmd) {
  this->_voltage = voltage;
  this->_temperature = temperature;
  String sCmd = String(cmd);
  sCmd.toUpperCase();
  // if received Serial CMD from the serial monitor, enter into the calibration mode
  phCalibration(cmdParse(sCmd.c_str()));
}

void EnviSense::calibrateGravitypH(float voltage, float temperature) {
  this->_voltage = voltage;
  this->_temperature = temperature;
  // if received Serial CMD from the serial monitor, enter into the calibration mode
  if (cmdSerialDataAvailable() > 0) {
    phCalibration(cmdParse());
  }
}

boolean EnviSense::cmdSerialDataAvailable() {
  char cmdReceivedChar;
  static unsigned long cmdReceivedTimeOut = millis();
  while (Serial.available() > 0) {
    if (millis() - cmdReceivedTimeOut > 500U) {
      this->_cmdReceivedBufferIndex = 0;
      memset(this->_cmdReceivedBuffer, 0, (ReceivedBufferLength));
    }
    cmdReceivedTimeOut = millis();
    cmdReceivedChar = Serial.read();
    if (cmdReceivedChar == '\r' || this->_cmdReceivedBufferIndex == ReceivedBufferLength - 1) {
      this->_cmdReceivedBufferIndex = 0;
      strupr(this->_cmdReceivedBuffer);
      return true;
    } else {
      this->_cmdReceivedBuffer[this->_cmdReceivedBufferIndex] = cmdReceivedChar;
      this->_cmdReceivedBufferIndex++;
    }
  }
  return false;
}

byte EnviSense::cmdParse(const char *cmd) {
  byte modeIndex = 0;
  if (strstr(cmd, "ENTERPH") != NULL) {
    modeIndex = 1;
  } else if (strstr(cmd, "EXITPH") != NULL) {
    modeIndex = 3;
  } else if (strstr(cmd, "CALPH") != NULL) {
    modeIndex = 2;
  }
  return modeIndex;
}

byte EnviSense::cmdParse() {
  byte modeIndex = 0;
  if (strstr(this->_cmdReceivedBuffer, "ENTERPH") != NULL) {
    modeIndex = 1;
  } else if (strstr(this->_cmdReceivedBuffer, "EXITPH") != NULL) {
    modeIndex = 3;
  } else if (strstr(this->_cmdReceivedBuffer, "CALPH") != NULL) {
    modeIndex = 2;
  }
  return modeIndex;
}

void EnviSense::phCalibration(byte mode) {
  char *receivedBufferPtr;
  static boolean phCalibrationFinish = 0;
  static boolean enterCalibrationFlag = 0;
  switch (mode) {
    case 0:
      if (enterCalibrationFlag) {
        Serial.println(F(">>>Command Error<<<"));
      }
      break;

    case 1:
      enterCalibrationFlag = 1;
      phCalibrationFinish = 0;
      Serial.println();
      Serial.println(F(">>>Enter PH Calibration Mode<<<"));
      Serial.println(F(">>>Please put the probe into the 4.0 or 7.0 standard buffer solution<<<"));
      Serial.println();
      break;

    case 2:
      if (enterCalibrationFlag) {
        if ((this->_voltage > 1322) && (this->_voltage < 1678)) {  // buffer solution:7.0{
          Serial.println();
          Serial.print(F(">>>Buffer Solution:7.0"));
          this->_neutralVoltage = this->_voltage;
          Serial.println(F(",Send EXITPH to Save and Exit<<<"));
          Serial.println();
          phCalibrationFinish = 1;
        } else if ((this->_voltage > 1854) && (this->_voltage < 2210)) {  //buffer solution:4.0
          Serial.println();
          Serial.print(F(">>>Buffer Solution:4.0"));
          this->_acidVoltage = this->_voltage;
          Serial.println(F(",Send EXITPH to Save and Exit<<<"));
          Serial.println();
          phCalibrationFinish = 1;
        } else {
          Serial.println();
          Serial.print(F(">>>Buffer Solution Error Try Again<<<"));
          Serial.println();  // not buffer solution or faulty operation
          phCalibrationFinish = 0;
        }
      }
      break;

    case 3:
      if (enterCalibrationFlag) {
        Serial.println();
        if (phCalibrationFinish) {
          if ((this->_voltage > 1322) && (this->_voltage < 1678)) {

            EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
          } else if ((this->_voltage > 1854) && (this->_voltage < 2210)) {
            EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
          }
          Serial.print(F(">>>Calibration Successful"));
        } else {
          Serial.print(F(">>>Calibration Failed"));
        }
        Serial.println(F(",Exit PH Calibration Mode<<<"));
        Serial.println();
        phCalibrationFinish = 0;
        enterCalibrationFlag = 0;
      }
      break;
  }
}