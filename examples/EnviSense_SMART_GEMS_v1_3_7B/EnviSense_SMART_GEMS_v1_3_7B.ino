#include "EnviSense.h"
#include <EEPROM.h>

#define DEVICE_NAME "UC PROJECT DANUM"
#define MANUFACTURER "EnviSense Inc."
#define HARDWARE_VERSION "v3.0"
#define SENSOR_LOCATION "Cordillera Region"
#define COORDINATES "Lat: null, Long: null"
#define SWL "null meters below ground"
#define DASHBOARD "https://script.google.com/macros/s/AKfycbxQn4Jsx2OAYjLbNwix_UW-AQCEht3eLUIgMtkrl_kJjH3Lv-aNMAQ-N5N9eYS496l27A/exec?id=ENV002"

#define EZODO_ADR 97    // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98   // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR 99    // default I2C address number for EZO pH circuit board
#define EZOEC_ADR 100   // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102  // default I2C address number for EZO RTD circuit board

#define PH_ADC_PIN A0    // analog pH sensor connection to ADC pins
#define PH_PROBE_TYPE 1  // define type of pH probe 1 ---> Gravity Analog pH, 2 ---> Scientific Atlas Digital

#define DEPTH_PROBE_PIN A1  // analog depth sensor connection to ADC pins
#define MAX_DEPTH 20.0f     // define max depth specification of the sensor
#define OFFSET_DEPTH 0.0f   // offset for calibration
#define DTG_FACTOR 18.00f   // measured DTG as reference to calculate SWL

#define BATT_SENSOR_PIN A2  // analog votlage sensor connection to ADC pins

#define SEND_INTERVAL_ADDR 10  // the start address of the data transmission interval stored in the EEPROM
#define NETWORK_APN_ADDR 11    // the start address of the network APN stored in the EEPROM

unsigned long READING_INTERVAL_S = 10;   // default sending interval every 10 seconds
unsigned long SENDING_INTERVAL_S = 900;  // default sending interval every 15 minutes

unsigned long PREV_READING_INTERVAL_S = 0;  // store previous interval time
unsigned long PREV_SENDING_INTERVAL_S = 0;  // store previous interval time

EnviSense UC;  // create EnviSense functions object to be call

String computerData = "";   // a String to hold incoming datalog from computer
bool event = false;         // whether the string is completed
bool BTconnection = false;  // whether the Bluetooth client is connected
byte deviceMode = 0;        // store device mode status 1: calibration, 2: reading & transmission

String GOOGLE_SCRIPT_ID = "AKfycbx78mYXnuL5ygu8A3ORZnl9TWeccnddMbbiRKGVZXiP0h4RHEpY";    // Google script auth
String THINGSPEAK_AUTH = "Z4OX2LWYJDKAS83F";                                             // ThingSpeak secret code
String FIREBASE_HOST = "envisenseuc-default-rtdb.asia-southeast1.firebasedatabase.app";  // Google firebase project http link
String FIREBASE_AUTH = "uwPQgOIusWpyNcSaMZbZZSqpQAU3MC7HoYc7Ewub";                       // Google firebase secret code
String FIREBASE_PATH = "/UC-Bagiuo";                                                     // path where the datalog to be store
String DEVICE_ID = "ENV002";                                                             // define device ID id of the sensor unit
String apn = "internet";                                                                 // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                                                                       // APN-Username, not set by default
String apn_p = "";                                                                       // APN-Password, not set by default
String SIM_TYPE = "SMART";                                                               // store the type of network (default is SMART)
String time = "";                                                                        // store date and time from the modem

float adc_milliVolts_pH = 0.00;  // milli-volts measurement for analog pH

float ph_val, do_val, temp_val, sg_val, sal_val, sat_val;
float batt_volt_val, depth_val, swl_val;
unsigned long int ec_val, tds_val;
int simcode_val;
bool isConfig = false;

void setup() {
  Serial.begin(9600);
  delay(3000);

  if (isnan(EEPROM.read(NETWORK_APN_ADDR))) {
    apn = "internet";  // set apn to default network (SMART)
  } else {             // else network APN to previously saved configuration
    switch (EEPROM.read(NETWORK_APN_ADDR)) {
      case 1:
        apn = "internet.globe.com.ph";
        SIM_TYPE = "GLOBE";
        break;
      case 2:
        apn = "internet";
        SIM_TYPE = "SMART";
        break;
      case 3:
        apn = "http.globe.com.ph";
        SIM_TYPE = "TM";
        break;
      case 4:
        apn = "gomo.ph";
        SIM_TYPE = "GOMO";
        break;
      case 5:
        apn = "internet.dito.ph";
        SIM_TYPE = "DITO";
        break;
    }
  }

  UC.gprsInit(apn, apn_u, apn_p);
  delay(3000);

  UC.scan(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  delay(3000);

  UC.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  computerData.reserve(200);
  delay(3000);

  UC.initSDCard("envslog.txt", "Timestamp, ID, pH, DO, SAT, TempC, EC, TDS, SAL, SG, Depth, SWL, Batt");
  delay(3000);

  UC.initDepth(DEPTH_PROBE_PIN);
  delay(3000);

  if (PH_PROBE_TYPE == 1) UC.initGravitypH(), delay(3000);

  if (isnan(EEPROM.read(SEND_INTERVAL_ADDR))) {
    SENDING_INTERVAL_S = 900;  // set sending interval to default value (every 15 minutes)
  } else {
    SENDING_INTERVAL_S = EEPROM.read(SEND_INTERVAL_ADDR) * 100;  // set sending interval to previously saved configuration
  }
}

void loop() {
  unsigned long CURRENT_TIME_S = millis() / 1000;  // get the current run time in seconds

  if (event) {
    if (computerData == "envs -help\r") {
      Serial.print("\n");
      Serial.print(F("verbose: ***************************************************************************"));
      Serial.print("\n");
      Serial.print(F("verbose: Welcome to EnviSense help utility!"));
      Serial.print("\n");
      Serial.print(F("verbose: You may enter the following commands:"));
      Serial.print("\n");
      Serial.print(F("verbose: envs device-info     // show device/product information"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ph-4        // pH calibration at lowpoint (pH 4)"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ph-7        // pH calibration at midpoint (pH 7)"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ph-10       // pH calibration at highpoint (pH 10)"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ph-clear    // pH delete calibration datalog"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal do-atm      // DO calibration at atmospheric point"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal do-zero     // DO calibration at 0 value"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal do-clear    // DO delete calibration datalog"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-dry      // EC dry calibration"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-84       // EC calibration at 84uS"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-1413     // EC calibration at 1413uS"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-12880    // EC calibration at 12880 uS"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-80000    // EC calibration at 80000uS"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-150000   // EC calibration at 150000uS"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal ec-clear    // EC delete calibration datalog"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal temp-100    // Temperature calibration at boiling point"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal temp-25     // Temperature calibration at room temperature"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal temp-clear  // Temperature delete calibration datalog"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal orp-225     // ORP calibration at 225mV"));
      Serial.print("\n");
      Serial.print(F("verbose: envs cal orp-clear   // ORP delete calibration datalog"));
      Serial.print("\n");
      Serial.print(F("verbose: envs sendevery-15    // Set sending interval every 15 mins"));
      Serial.print("\n");
      Serial.print(F("verbose: envs sendevery-30    // Set sending interval every 30 mins"));
      Serial.print("\n");
      Serial.print(F("verbose: envs sendevery-45    // Set sending interval every 45 mins"));
      Serial.print("\n");
      Serial.print(F("verbose: envs sendevery-60    // Set sending interval every 60 mins"));
      Serial.print("\n");
      Serial.print(F("verbose: envs sendevery?      // Display the set sending interval"));
      Serial.print("\n");
      Serial.print(F("verbose: envs mode-cal        // Set device mode for calibration"));
      Serial.print("\n");
      Serial.print(F("verbose: envs mode-deploy     // Set device mode for deployment & data transmission"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn-globe       // Set network APN to GLOBE"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn-smart       // Set network APN to SMART"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn-tm          // Set network APN to TM"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn-gomo        // Set network APN to GOMO"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn-dito        // Set network APN to DITO"));
      Serial.print("\n");
      Serial.print(F("verbose: envs apn?            // Display the set network APN"));
      Serial.print("\n");
      Serial.print(F("verbose: ***************************************************************************"));
      Serial.print("\n");
    } else if (computerData == "envs device-info\r") {
      Serial.print("\n");
      Serial.print(F("verbose: ***************************************************************************"));
      Serial.print("\n");
      Serial.print(F("verbose: EnviSense Inc. R&D Solutions and Electronics Consultant"));
      Serial.print("\n");
      Serial.print(F("verbose: Go to the https://envisense.com/xxx to discover more!\n"));
      Serial.print("\n");
      Serial.print(F("verbose: Device name:"));
      Serial.print(DEVICE_NAME);
      Serial.print("\n");
      Serial.print(F("verbose: Manufacturer:"));
      Serial.print(MANUFACTURER);
      Serial.print("\n");
      Serial.print(F("verbose: Hardware revision:"));
      Serial.print(HARDWARE_VERSION);
      Serial.print("\n");
      Serial.print(F("verbose: Sensor location:"));
      Serial.print("\n");
      Serial.print(SENSOR_LOCATION);
      Serial.print("\n");
      Serial.print(F("verbose: Coordinates:"));
      Serial.print("\n");
      Serial.print(COORDINATES);
      Serial.print("\n");
      Serial.print(F("verbose: Static water level:"));
      Serial.print("\n");
      Serial.print(SWL);
      Serial.print("\n");
      Serial.print(F("verbose: API:"));
      Serial.print("\n");
      Serial.print(DASHBOARD);
      Serial.print(F("verbose: ***************************************************************************"));
      Serial.print(F("\n"));
      Serial.print(F("\n"));
    } else if (computerData == "envs mode-cal\r") {
      deviceMode = 1;    // set device mode to calibration
      isConfig = false;  // set config state to false
      Serial.print(F("verbose: Calibration device mode."));
      Serial.print(F("\n"));
      // enter Gravity analog pH calibration
      adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;
      if (PH_PROBE_TYPE == 1) UC.calibrateGravitypH(adc_milliVolts_pH, 25.0, "enterph");
    } else if (computerData == "envs mode-deploy\r") {
      deviceMode = 2;   // set device mode to deployment
      isConfig = true;  // set config state to true to trigger data transmission once
      Serial.print(F("verbose: Deployment device (reading and transmitting data) mode."));
      Serial.print(F("\n"));
      // save and exit Gravity analog pH calibration
      adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;
      if (PH_PROBE_TYPE == 1) UC.calibrateGravitypH(adc_milliVolts_pH, 25.0, "exitph");
    } else if (computerData == "envs cal ph-4\r") {
      if (deviceMode == 1) {
        switch (PH_PROBE_TYPE) {
          case 1:
            adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;  // read the voltage
            ph_val = UC.getGravitypH(adc_milliVolts_pH, 25.0);             // convert voltage to pH with temperature compensation @ 25°C
            UC.calibrateGravitypH(adc_milliVolts_pH, 25.0, "calph");       // calibrate pH at low-point (pH 4)
            break;
          case 2:
            UC.AtlasEZO(EZOPH_ADR, "Cal,low,4", 900);
            if (UC.getStatus() == EnviSense::SUCCESS) {
              Serial.print(F("\n"));
              Serial.print(F("verbose: pH-low cal OK."));
              Serial.print(F("\n"));
            }
            break;
          default:
            Serial.print(F("\n"));
            Serial.print(F("verbose: wrong pH probe code."));
            Serial.print(F("\n"));
            break;
        }
      }
    } else if (computerData == "envs cal ph-7\r") {
      if (deviceMode == 1) {
        switch (PH_PROBE_TYPE) {
          case 1:
            adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;  // read the voltage
            ph_val = UC.getGravitypH(adc_milliVolts_pH, 25.0);             // convert voltage to pH with temperature compensation @ 25°C
            UC.calibrateGravitypH(adc_milliVolts_pH, 25.0, "calph");       // calibrate pH at mid-point (pH 7)
            break;
          case 2:
            UC.AtlasEZO(EZOPH_ADR, "Cal,mid,7", 900);
            if (UC.getStatus() == EnviSense::SUCCESS) {
              Serial.print(F("\n"));
              Serial.print(F("verbose: pH-mid cal OK."));
              Serial.print(F("\n"));
            }
            break;
          default:
            Serial.print(F("\n"));
            Serial.print(F("verbose: wrong pH probe code."));
            Serial.print(F("\n"));
            break;
        }
      }
    } else if (computerData == "envs cal ph-10\r") {
      if (deviceMode == 1) {
        switch (PH_PROBE_TYPE) {
          case 1:
            break;
          case 2:
            UC.AtlasEZO(EZOPH_ADR, "Cal,high,10", 900);
            if (UC.getStatus() == EnviSense::SUCCESS) {
              Serial.print(F("\n"));
              Serial.print(F("verbose: pH-high cal OK."));
              Serial.print(F("\n"));
            }
            break;
          default:
            Serial.print(F("\n"));
            Serial.print(F("verbose: wrong pH probe code."));
            Serial.print(F("\n"));
            break;
        }
      }
    } else if (computerData == "envs cal ph-clear\r") {
      if (deviceMode == 1) {
        switch (PH_PROBE_TYPE) {
          case 1:
            break;
          case 2:
            UC.AtlasEZO(EZOPH_ADR, "Cal,clear", 300);
            if (UC.getStatus() == EnviSense::SUCCESS) {
              Serial.print(F("\n"));
              Serial.print(F("verbose: pH-cal clear OK."));
              Serial.print(F("\n"));
            }
            break;
          default:
            Serial.print(F("\n"));
            Serial.print(F("verbose: wrong pH probe code."));
            Serial.print(F("\n"));
            break;
        }
      }
    } else if (computerData == "envs cal do-atm\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZODO_ADR, "Cal", 1300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: DO-atm cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal do-zero\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZODO_ADR, "Cal,0", 1300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: DO-zero cal OK."));
          Serial.print(F("\n"));
          UC.AtlasEZO(EZODO_ADR, "O,%,1", 300);  // This is necessary, add this line to maintain percent saturation (SAT) reading
        }
      }
    } else if (computerData == "envs cal do-clear\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZODO_ADR, "Cal,clear", 300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: DO-cal clear OK."));
          Serial.print(F("\n"));
          UC.AtlasEZO(EZODO_ADR, "O,%,1", 300);  // This is necessary, add this line to maintain percent saturation (SAT) reading
        }
      }
    } else if (computerData == "envs cal ec-dry\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,dry", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-dry cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-84\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,low,84", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-84 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-1413\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,low,1413", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-1413 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-12880\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,high,12880", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-12880 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-80000\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,high,80000", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-80000 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-150000\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,high,150000", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-150000 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ec-clear\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOEC_ADR, "Cal,clear", 300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: EC-cal clear OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal temp-100\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZORTD_ADR, "Cal,100", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: Temp-100 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal temp-25\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZORTD_ADR, "Cal,25", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: Temp-25 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal temp-clear\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZORTD_ADR, "Cal,clear", 300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: Temp-cal clear OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal orp-225\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOORP_ADR, "Cal,25", 600);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: ORP-225 cal OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs cal ORP-clear\r") {
      if (deviceMode == 1) {
        UC.AtlasEZO(EZOORP_ADR, "Cal,clear", 300);
        if (UC.getStatus() == EnviSense::SUCCESS) {
          Serial.print(F("\n"));
          Serial.print(F("verbose: ORP-cal clear OK."));
          Serial.print(F("\n"));
        }
      }
    } else if (computerData == "envs sendevery-15\r") {
      SENDING_INTERVAL_S = 900;
      Serial.print(F("\n"));
      Serial.print(F("verbose: sending interval every 15 mins OK."));
      Serial.print(F("\n"));
      EEPROM.update(SEND_INTERVAL_ADDR, SENDING_INTERVAL_S / 100);
    } else if (computerData == "envs sendevery-30\r") {
      SENDING_INTERVAL_S = 1800;
      Serial.print(F("\n"));
      Serial.print(F("verbose: sending interval every 30 mins OK."));
      Serial.print(F("\n"));
      EEPROM.update(SEND_INTERVAL_ADDR, SENDING_INTERVAL_S / 100);
    } else if (computerData == "envs sendevery-45\r") {
      SENDING_INTERVAL_S = 2760;
      Serial.print(F("\n"));
      Serial.print(F("verbose: sending interval every 45 mins OK."));
      Serial.print(F("\n"));
      EEPROM.update(SEND_INTERVAL_ADDR, SENDING_INTERVAL_S / 100);
    } else if (computerData == "envs sendevery-60\r") {
      SENDING_INTERVAL_S = 3600;
      Serial.print(F("\n"));
      Serial.print(F("verbose: sending interval every 60 mins OK."));
      Serial.print(F("\n"));
      EEPROM.update(SEND_INTERVAL_ADDR, SENDING_INTERVAL_S / 100);
    } else if (computerData == "envs sendevery?\r") {
      if (isnan(EEPROM.read(SEND_INTERVAL_ADDR))) {
        SENDING_INTERVAL_S = 900;  // set sending interval to default value (every 15 minutes)
      } else {
        SENDING_INTERVAL_S = EEPROM.read(SEND_INTERVAL_ADDR) * 100;  // set sending interval to previously saved configuration
      }

      Serial.print(F("\n"));
      Serial.print(F("verbose: sending interval is every "));
      Serial.print(SENDING_INTERVAL_S / 60);
      Serial.print(F(" minutes."));
      Serial.print(F("\n"));
    } else if (computerData == "envs apn-globe\r") {
      apn = "internet.globe.com.ph";
      SIM_TYPE = "GLOBE";
      Serial.print(F("\n"));
      Serial.print(F("verbose: network access point name is set to GLOBE."));
      Serial.print(F("\n"));
      Serial.print(F("verbose: restart the hardware to reconfigure the network settings."));
      Serial.print(F("\n"));
      EEPROM.update(NETWORK_APN_ADDR, 1);
    } else if (computerData == "envs apn-smart\r") {
      apn = "internet";
      SIM_TYPE = "SMART";
      Serial.print(F("\n"));
      Serial.print(F("verbose: network access point name is set to SMART."));
      Serial.print(F("\n"));
      Serial.print(F("verbose: restart the hardware to reconfigure the network settings."));
      Serial.print(F("\n"));
      EEPROM.update(NETWORK_APN_ADDR, 2);
    } else if (computerData == "envs apn-tm\r") {
      apn = "http.globe.com.ph";
      SIM_TYPE = "TM";
      Serial.print(F("\n"));
      Serial.print(F("verbose: network access point name is set to TM."));
      Serial.print(F("\n"));
      Serial.print(F("verbose: restart the hardware to reconfigure the network settings."));
      Serial.print(F("\n"));
      EEPROM.update(NETWORK_APN_ADDR, 3);
    } else if (computerData == "envs apn-gomo\r") {
      apn = "gomo.ph";
      SIM_TYPE = "GOMO";
      Serial.print(F("\n"));
      Serial.print(F("verbose: network access point name is set to GOMO."));
      Serial.print(F("\n"));
      Serial.print(F("verbose: restart the hardware to reconfigure the network settings."));
      Serial.print(F("\n"));
      EEPROM.update(NETWORK_APN_ADDR, 4);
    } else if (computerData == "envs apn-dito\r") {
      apn = "internet.dito.ph";
      SIM_TYPE = "DITO";
      Serial.print(F("\n"));
      Serial.print(F("verbose: network access point name is set to DITO."));
      Serial.print(F("\n"));
      Serial.print(F("verbose: restart the hardware to reconfigure the network settings."));
      Serial.print(F("\n"));
      EEPROM.update(NETWORK_APN_ADDR, 5);
    } else if (computerData == "envs apn?\r") {
      Serial.print(F("\n"));
      Serial.print(F("verbose: the network APN is set to "));
      Serial.print(SIM_TYPE);
      Serial.print(F("."));
      Serial.print(F("\n"));
    } else {
      Serial.print(F("\n"));
      Serial.print(F("verbose: Wrong command syntax! Please try again."));
      Serial.print(F("\n"));
    }
    computerData = "";  // clear the input string for new readings
    event = false;      // toggle event status for new reading
  }

  if (deviceMode == 1) {
    if (CURRENT_TIME_S - PREV_READING_INTERVAL_S >= READING_INTERVAL_S) {
      // save the last time read sensor data
      PREV_READING_INTERVAL_S = CURRENT_TIME_S;

      Serial.print("verbose: Reading Atlas EZO ciruit board...");
      Serial.print("\n");

      switch (PH_PROBE_TYPE) {
        case 1:
          // read Gravity analog pH sensor board
          adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;  // read the voltage
          ph_val = UC.getGravitypH(adc_milliVolts_pH, 25.0);             // convert voltage to pH with temperature compensation @ 25°C
          break;
        case 2:
          // read Atlas Scientific EZO-pH sensor board
          UC.AtlasEZO(EZOPH_ADR, "R", 815);
          if (UC.getStatus() == EnviSense::SUCCESS) {
            ph_val = UC.getPH();
          }
          break;
        default:
          Serial.print(F("\n"));
          Serial.print(F("verbose: wrong pH probe code."));
          Serial.print(F("\n"));
          break;
      }

      // read Atlas Scientific EZO-RTD sensor board
      UC.AtlasEZO(EZORTD_ADR, "R", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        temp_val = UC.getRTD();
      }

      // read Atlas Scientific EZO-EC sensor board
      UC.AtlasEZO(EZOEC_ADR, "R", 570);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        ec_val = UC.getEC();
        tds_val = UC.getTDS();
        sal_val = UC.getSAL();
        sg_val = UC.getSG();
      }

      // after reading sensor datalog, enter sleep mode to conserve power consumption
      UC.AtlasEZO(EZOPH_ADR, "sleep", 100);
      UC.AtlasEZO(EZORTD_ADR, "sleep", 100);
      UC.AtlasEZO(EZOEC_ADR, "sleep", 100);

      // print sensor calibration readings
      Serial.print(F("PH:"));
      Serial.print(ph_val);
      Serial.print(F(", "));
      Serial.print(F("Temp:"));
      Serial.print(temp_val);
      Serial.print(F("°C, "));
      Serial.print(F("EC:"));
      Serial.print(ec_val);
      Serial.print(F("uS, "));
      Serial.print(F("\n"));
    }
  }

  if (deviceMode == 2) {
    if (CURRENT_TIME_S - PREV_SENDING_INTERVAL_S >= SENDING_INTERVAL_S || isConfig == true) {
      // save the last time read sensor data
      PREV_SENDING_INTERVAL_S = CURRENT_TIME_S;

      // reset deployment config once after first data transmission
      isConfig = false;

      switch (PH_PROBE_TYPE) {
        case 1:
          // read Gravity analog pH sensor board
          adc_milliVolts_pH = analogRead(PH_ADC_PIN) / 1023.0 * 5000.0;  // read the voltage
          ph_val = UC.getGravitypH(adc_milliVolts_pH, 25.0);             // convert voltage to pH with temperature compensation @ 25°C
          break;
        case 2:
          // read Atlas Scientific EZO-pH sensor board
          UC.AtlasEZO(EZOPH_ADR, "R", 815);
          if (UC.getStatus() == EnviSense::SUCCESS) {
            ph_val = UC.getPH();
          }
          break;
        default:
          Serial.print(F("\n"));
          Serial.print(F("verbose: wrong pH probe code."));
          Serial.print(F("\n"));
          break;
      }

      // read Atlas Scientific EZO-RTD sensor board
      UC.AtlasEZO(EZORTD_ADR, "R", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        temp_val = UC.getRTD();
      }

      // read Atlas Scientific EZO-EC sensor board
      UC.AtlasEZO(EZOEC_ADR, "R", 570);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        ec_val = UC.getEC();
        tds_val = UC.getTDS();
        sal_val = UC.getSAL();
        sg_val = UC.getSG();
      }

      // after reading sensor datalog, enter sleep mode to conserve power consumption
      UC.AtlasEZO(EZOPH_ADR, "sleep", 100);
      UC.AtlasEZO(EZORTD_ADR, "sleep", 100);
      UC.AtlasEZO(EZOEC_ADR, "sleep", 100);

      // read battery voltage
      batt_volt_val = UC.getBattVolts(BATT_SENSOR_PIN);

      // read water depth
      depth_val = UC.getDepth(DEPTH_PROBE_PIN, MAX_DEPTH, OFFSET_DEPTH);

      // read static water level
      swl_val = UC.getSWL(depth_val, DTG_FACTOR);

      // print sensor readings
      Serial.print(F("PH:"));
      Serial.print(ph_val);
      Serial.print(F(", "));
      Serial.print(F("Temp:"));
      Serial.print(temp_val);
      Serial.print(F("°C, "));
      Serial.print(F("EC:"));
      Serial.print(ec_val);
      Serial.print(F("uS, "));
      Serial.print(F("TDS:"));
      Serial.print(tds_val);
      Serial.print(F("ppm, "));
      Serial.print(F("Salinity:"));
      Serial.print(sal_val);
      Serial.print(F("ppt, "));
      Serial.print(F("SG:"));
      Serial.print(sg_val);
      Serial.print(F(", "));
      Serial.print(F("Batt:"));
      Serial.print(batt_volt_val);
      Serial.print(F(", "));
      Serial.print(F("Depth:"));
      Serial.print(depth_val);
      Serial.print(F("cm, "));
      Serial.print(F("SWL:"));
      Serial.print(swl_val);
      Serial.print(F("\n"));

      // get local time from modem
      String time_check = UC.gprsGetLocalTime();
      if (time_check != NULL) time = time_check;

      // log sensor readings to SD card
      String datalog = time + "," + String(DEVICE_ID) + "," + String(ph_val) + "," + String(do_val) + "," + String(sat_val) + "," + String(temp_val) + "," + String(ec_val) + "," + String(tds_val) + "," + String(sal_val) + "," + String(sg_val) + "," + String(depth_val) + "," + String(swl_val) + "," + String(batt_volt_val);
      UC.logData("envslog.txt", datalog);
      Serial.print(F("data log: "));
      Serial.print(datalog);
      Serial.print(F("\n"));

      // transmit sensor readings via GPRS/LTE network (NB-IoT)
      String GSheet_payload = String("id=") + String(DEVICE_ID) + "&action=addItem&ph=" + String(ph_val) + "&ec=" + String(ec_val) + "&tds=" + String(tds_val) + "&sal=" + String(sal_val) + "&swl=" + String(swl_val) + "&temp=" + String(temp_val) + "&battlvl=" + String(batt_volt_val);
      UC.gprsGoogleSheet(GOOGLE_SCRIPT_ID, GSheet_payload);
    }
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();  // get the new byte
    computerData += inChar;             // add it to the inputString
    delay(10);

    if (inChar == '\r') {          // check if datalog reach end marker (carriage return '\r')
      computerData.toLowerCase();  // get a lower-case version of a String
      event = true;                // if the incoming character is a carriage return, set a flag status
      break;                       // exit loop when received char datalog is end marker (new line '\r')
    }
  }
}