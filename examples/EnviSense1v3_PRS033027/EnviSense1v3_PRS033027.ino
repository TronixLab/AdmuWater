#include "EnviSense.h"

#define DEVICE_NAME "UC PROJECT DANUM"
#define MANUFACTURER "EnviSense Inc."
#define HARDWARE_VERSION "v3.0"
#define SENSOR_LOCATION "Cordillera Region"
#define COORDINATES "Lat: null, Long: null"
#define SWL "null meters below ground"
#define DASHBOARD "https://docs.google.com/spreadsheets/d/1J62nsckv25UBsj0EPw4nxL4yBIWSuUkeA_nimYvnJJA/edit#gid=0"

#define EZODO_ADR 97    // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98   // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR 99    // default I2C address number for EZO pH circuit board
#define EZOEC_ADR 100   // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102  // default I2C address number for EZO RTD circuit board

#define SWL_ADC_PIN 34
#define ADC_ZERO_DEPTH 173
#define DEPTH_SENSOR_SLOPE 38.666667F
#define DTG 18.00F

#define INTERVAL 120000

EnviSense UC;  // create EnviSense functions object to be call

String computerData = "";   // a String to hold incoming datalog from computer
bool event = false;         // whether the string is completed
bool BTconnection = false;  // whether the Bluetooth client is connected

String GOOGLE_SCRIPT_ID = "AKfycbx78mYXnuL5ygu8A3ORZnl9TWeccnddMbbiRKGVZXiP0h4RHEpY";    // Google script auth
String THINGSPEAK_AUTH = "Z4OX2LWYJDKAS83F";                                             // ThingSpeak secret code
String FIREBASE_HOST = "envisenseuc-default-rtdb.asia-southeast1.firebasedatabase.app";  // Google firebase project http link
String FIREBASE_AUTH = "uwPQgOIusWpyNcSaMZbZZSqpQAU3MC7HoYc7Ewub";                       // Google firebase secret code
String FIREBASE_PATH = "/UC-Bagiuo";                                                     // path where the datalog to be store
String DEVICE_ID = "ENV002";                                                             // define device ID id of the sensor unit
String apn = "internet";                                                                 // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                                                                       // APN-Username, not set by default
String apn_p = "";                                                                       // APN-Password, not set by default

void setup() {
  Serial.begin(9600);
  delay(3000);

  UC.gprsInit(apn, apn_u, apn_p);
  delay(3000);

  UC.scan(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  delay(3000);

  UC.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  computerData.reserve(200);
  delay(3000);

  UC.initSDCard("envslog.txt", "Timestamp, Batt, pH, DO, SAT, TempC, EC, TDS, SAL, SG, Depth, SWL");
  delay(3000);
}

void loop() {
  if (event) {  // print the string when a commands arrives
    if (computerData == "envs -help\r") {
      Serial.println();
      Serial.println(F("*********************************************************************************************************"));
      Serial.println(F("Welcome to EnviSense help utility!\n"));
      Serial.println(F("You may enter the following commands:"));
      Serial.println(F("envs -info           // show device/product information."));
      Serial.println(F("envs cal ph-low      // pH two point calibration at lowpoint (pH 4)."));
      Serial.println(F("envs cal ph-mid      // pH single point calibration at midpoint (pH 7)."));
      Serial.println(F("envs cal ph-high     // pH three point calibration at highpoint (pH 10)."));
      Serial.println(F("envs cal ph-clear    // pH delete calibration datalog."));
      Serial.println(F("envs cal do-atm      // DO (Disolve Oxygen) calibrate to atmospheric oxygen levels."));
      Serial.println(F("envs cal do-zero     // DO (Disolve Oxygen) calibrate device to 0 dissolved oxygen."));
      Serial.println(F("envs cal do-clear    // DO (Disolve Oxygen) delete calibration datalog."));
      Serial.println(F("envs cal ec-low      // EC (Electrical Conductivity) low end calibration at 12880uS/cm."));
      Serial.println(F("envs cal ec-high     // EC (Electrical Conductivity) high end calibration at 80000uS/cm."));
      Serial.println(F("envs cal ec-dry      // EC (Electrical Conductivity) dry calibration."));
      Serial.println(F("envs cal ec-clear    // EC (Electrical Conductivity) delete calibration datalog."));
      Serial.println(F("envs cal temp-boil   // Temperature calibration at boiling point (100°C)."));
      Serial.println(F("envs cal temp-room   // Temperature calibration at room temperature (25°C)."));
      Serial.println(F("envs cal temp-clear  // Temperature delete calibration datalog"));
      Serial.println(F("*********************************************************************************************************"));
      Serial.println();
    } else if (computerData == "envs -info\r") {
      Serial.println();
      Serial.println(F("*********************************************************************************************************"));
      Serial.println(F("ADMU water project provides portal to monitoring groundwater quality and quantity across the Philippines."));
      Serial.println(F("Go to the https://envisense.com/ to discover more!\n"));
      Serial.print(F("Device name:"));
      Serial.println(DEVICE_NAME);
      Serial.print(F("Manufacturer:"));
      Serial.println(MANUFACTURER);
      Serial.print(F("Hardware revision:"));
      Serial.println(HARDWARE_VERSION);
      Serial.print(F("Sensor location:"));
      Serial.println(SENSOR_LOCATION);
      Serial.print(F("Coordinates:"));
      Serial.println(COORDINATES);
      Serial.print(F("Static water level:"));
      Serial.println(SWL);
      Serial.print(F("Monitoring dashboard:"));
      Serial.println(DASHBOARD);
      Serial.println(F("*********************************************************************************************************"));
      Serial.println();
    } else if (computerData == "envs cal ph-low\r") {
      UC.AtlasEZO(EZOPH_ADR, "Cal,low,4", 900);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("pH-low cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ph-mid\r") {
      UC.AtlasEZO(EZOPH_ADR, "Cal,mid,7", 900);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("pH-mid cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ph-high\r") {
      UC.AtlasEZO(EZOPH_ADR, "Cal,high,10", 900);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("pH-high cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ph-clear\r") {
      UC.AtlasEZO(EZOPH_ADR, "Cal,clear", 300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("pH-cal clear OK.\n\n"));
      }
    } else if (computerData == "envs cal do-atm\r") {
      UC.AtlasEZO(EZODO_ADR, "Cal", 1300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("DO-atm cal OK.\n\n"));
      }
    } else if (computerData == "envs cal do-zero\r") {
      UC.AtlasEZO(EZODO_ADR, "Cal,0", 1300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("DO-zero cal OK.\n\n"));
        // This is necessary, add this line to maintain percent saturation (SAT) reading
        UC.AtlasEZO(EZODO_ADR, "O,%,1", 300);
      }
    } else if (computerData == "envs cal do-clear\r") {
      UC.AtlasEZO(EZODO_ADR, "Cal,clear", 300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("DO-clear cal OK.\n\n"));
        // This is necessary, add this line to maintain percent saturation (SAT) reading
        UC.AtlasEZO(EZODO_ADR, "O,%,1", 300);
      }
    } else if (computerData == "envs cal ec-low\r") {
      UC.AtlasEZO(EZOEC_ADR, "Cal,low,12880", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("EC-low cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ec-high\r") {
      UC.AtlasEZO(EZOEC_ADR, "Cal,high,80000", 900);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("EC-high cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ec-dry\r") {
      UC.AtlasEZO(EZOEC_ADR, "Cal,dry", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("EC-dry cal OK.\n\n"));
      }
    } else if (computerData == "envs cal ec-clear\r") {
      UC.AtlasEZO(EZOEC_ADR, "Cal,clear", 300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("EC-clear cal OK.\n\n"));
      }
    } else if (computerData == "envs cal temp-boil\r") {
      UC.AtlasEZO(EZORTD_ADR, "Cal,100", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("Temp-boil cal OK.\n\n"));
      }
    } else if (computerData == "envs cal temp-room\r") {
      UC.AtlasEZO(EZORTD_ADR, "Cal,25", 600);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("Temp-room cal OK.\n\n"));
      }
    } else if (computerData == "envs cal temp-clear\r") {
      UC.AtlasEZO(EZORTD_ADR, "Cal,clear", 300);
      if (UC.getStatus() == EnviSense::SUCCESS) {
        Serial.print(F("Temp-clear cal OK.\n\n"));
      }
    } else {
      Serial.print(F("Wrong command syntax! Please try again.\n\n"));
    }
    computerData = "";  // clear the input string for new readings
    event = false;      // toggle event status for new reading
  }

  static unsigned long timer = millis();
  if (millis() - timer > INTERVAL) {
    timer = millis();

    float ph_val, do_val, temp_val, sg_val, sal_val, sat_val;
    unsigned long int ec_val, tds_val;

    Serial.println("Reading Atlas EZO ciruit board...");
    // read Atlas Scientific EZO-pH sensor board
    UC.AtlasEZO(EZOPH_ADR, "R", 815);
    if (UC.getStatus() == EnviSense::SUCCESS) {
      ph_val = UC.getPH();
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
    UC.AtlasEZO(EZOPH_ADR, "sleep", 250);
    UC.AtlasEZO(EZORTD_ADR, "sleep", 250);
    UC.AtlasEZO(EZOEC_ADR, "sleep", 250);

    float batt_volt_val, depth_val, swl_val;

    // read battery voltage
    batt_volt_val = UC.getBattVolts();

    // read water depth
    depth_val = UC.getDepth(ADC_ZERO_DEPTH, DEPTH_SENSOR_SLOPE);

    // read static water level
    swl_val = UC.getSWL(DTG);

    // get local time from GSM module
    String timestamp = UC.gprsGetLocalTime();
    if (!timestamp.equals("")) Serial.println("GPRS time stamp: " + timestamp);

    // print sensor readings
    Serial.print(F("\n"));
    Serial.print(F("Readings: "));
    Serial.print(F("Timestamp:"));
    Serial.print(timestamp);
    Serial.print(F(", "));
    Serial.print(F("Batt:"));
    Serial.print(batt_volt_val);
    Serial.print(F("V, "));
    Serial.print(F("PH:"));
    Serial.print(ph_val);
    Serial.print(F(", "));
    Serial.print(F("Temp:"));
    Serial.print(temp_val);
    Serial.print(F("�C, "));
    Serial.print(F("EC:"));
    Serial.print(ec_val);
    Serial.print(F("uS/cm, "));
    Serial.print(F("TDS:"));
    Serial.print(tds_val);
    Serial.print(F("ppm, "));
    Serial.print(F("Salinity:"));
    Serial.print(sal_val);
    Serial.print(F("ppt, "));
    Serial.print(F("SG:"));
    Serial.print(sg_val);
    Serial.print(F(", "));
    Serial.print(F("Depth:"));
    Serial.print(depth_val);
    Serial.print(F("cm, "));
    Serial.print(F("SWL:"));
    Serial.print(swl_val);
    Serial.print(F("\n\n"));

    String datalog = timestamp + "," + String(batt_volt_val) + "," + String(ph_val) + "," + String(do_val) + "," + String(sat_val) + "," + String(temp_val) + "," + String(ec_val) + "," + String(tds_val) + "," + String(sal_val) + "," + String(sg_val) + "," + String(depth_val) + "," + String(swl_val);
    Serial.println("data log: " + datalog + "\n");

    UC.logData("envslog.txt", datalog);

    String GSheet_payload = String("id=") + String(DEVICE_ID) + "&action=addItem&ph=" + String(ph_val) + "&ec=" + String(ec_val) + "&tds=" + String(tds_val) + "&sal=" + String(sal_val) + "&swl=" + String(swl_val) + "&temp=" + String(temp_val) + "&battlvl=" + String(batt_volt_val);
    UC.gprsGoogleSheet(GOOGLE_SCRIPT_ID, GSheet_payload);
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