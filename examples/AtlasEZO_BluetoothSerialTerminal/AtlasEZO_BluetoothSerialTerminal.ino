#include <AdmuWater.h>
#include "BluetoothSerial.h"

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

#if !defined(CONFIG_BT_SPP_ENABLED)
#error Serial Bluetooth not available or not enabled. It is only available for the ESP32 chip.
#endif

#define DEVICE_NAME       "PhiGO Pampanga-Gueco Balibago Elementary School"
#define MANUFACTURER      "Ateneo de Manila University"
#define HARDWARE_VERSION  "v2.0"
#define SENSOR_LOCATION   "Gueco Balibago Elementary School, HG Lopez St., Balibago, Angeles, Pampanga"
#define COORDINATES       "Lat: 15.168946, Long: 120.596543"
#define SWL               "7.86 meters below ground"
#define DASHBOARD         "https://admuwater.com/phigo/dashboard"

#define EZODO_ADR  97                               // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98                               // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99                               // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100                              // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102                              // default I2C address number for EZO RTD circuit board

AdmuWater PhiGO;

BluetoothSerial SerialBT;

String computerData = "";                           // a String to hold incoming data from computer
bool event = false;                                 // whether the string is completed
bool BTconnection = false;                          // whether the Bluetooth client is connected

void setup() {
  Serial.begin(115200);

  if (!SerialBT.begin("GMP-PhiGO NCR-Marikina")) {  // Initialize ESP32 Bluetooth module
    Serial.println("An error occurred initializing Bluetooth!");
  } else {
    Serial.println("Bluetooth initialized successful.");
  }

  SerialBT.register_callback(callback);             // add Bluetooth client status call back function

  PhiGO.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  computerData.reserve(200);                        // reserve 200 bytes for the input string
}

void loop() {
  while (SerialBT.available()) {
    char inChar = (char)SerialBT.read();            // get the new byte
    computerData += inChar;                         // add it to the inputString
    if (inChar == '\r') {                           // Check if data reach end marker (carriage return '\r')
      computerData.toLowerCase();                   // get a lower-case version of a String
      event = true;                                 // if the incoming character is a carriage return, set a flag status
      break;                                        // Exit loop when received char data is end marker (new line '\r')
    }
  }

  if (event) {                                      // print the string when a commands arrives
    if (computerData == "admuwater -help\r") {
      Serial.println();
      Serial.println(F("*********************************************************************************************************"));
      Serial.println(F("Welcome to ADMU water help utility!\n"));
      Serial.println(F("You may enter the following commands:"));
      Serial.println(F("admuwater -info                       // show device/product information."));
      Serial.println(F("admuwater conf -net gprs              // config network type to GPRS."));
      Serial.println(F("admuwater conf -net wifi              // config network type to Wi-Fi."));
      Serial.println(F("admuwater cal -s ph -p low            // pH two point calibration at lowpoint (pH 4)."));
      Serial.println(F("admuwater cal -s ph -p mid            // pH single point calibration at midpoint (pH 7)."));
      Serial.println(F("admuwater cal -s ph -p high           // pH three point calibration at highpoint (pH 10)."));
      Serial.println(F("admuwater cal -s ph clear             // pH delete calibration data."));
      Serial.println(F("admuwater cal -s do -p atm            // DO (Disolve Oxygen) calibrate to atmospheric oxygen levels."));
      Serial.println(F("admuwater cal -s do -p zero           // DO (Disolve Oxygen) calibrate device to 0 dissolved oxygen."));
      Serial.println(F("admuwater cal -s do clear             // DO (Disolve Oxygen) delete calibration data."));
      Serial.println(F("admuwater cal -s ec -p low            // EC (Electrical Conductivity) low end calibration at 12880uS/cm."));
      Serial.println(F("admuwater cal -s ec -p high           // EC (Electrical Conductivity) high end calibration at 80000uS/cm."));
      Serial.println(F("admuwater cal -s ec -p dry            // EC (Electrical Conductivity) dry calibration."));
      Serial.println(F("admuwater cal -s ec clear             // EC (Electrical Conductivity) delete calibration data."));
      Serial.println(F("admuwater cal -s rtd -p boil          // Temperature calibration at boiling point (100°C)."));
      Serial.println(F("admuwater cal -s rtd clear            // Temperature delete calibration data"));
      Serial.println(F("*********************************************************************************************************"));
      Serial.println();
      if (BTconnection) {
        SerialBT.println();
        SerialBT.println(F("*********************************************************************************************************"));
        SerialBT.println(F("Welcome to ADMU water help utility!\n"));
        SerialBT.println(F("You may enter the following commands:"));
        SerialBT.println(F("admuwater -info                       // show device/product information."));
        SerialBT.println(F("admuwater conf -net gprs              // config network type to GPRS."));
        SerialBT.println(F("admuwater conf -net wifi              // config network type to Wi-Fi."));
        SerialBT.println(F("admuwater cal -s ph -p low            // pH two point calibration at lowpoint (pH 4)."));
        SerialBT.println(F("admuwater cal -s ph -p mid            // pH single point calibration at midpoint (pH 7)."));
        SerialBT.println(F("admuwater cal -s ph -p high           // pH three point calibration at highpoint (pH 10)."));
        SerialBT.println(F("admuwater cal -s ph clear             // pH delete calibration data."));
        SerialBT.println(F("admuwater cal -s do -p atm            // DO (Disolve Oxygen) calibrate to atmospheric oxygen levels."));
        SerialBT.println(F("admuwater cal -s do -p zero           // DO (Disolve Oxygen) calibrate device to 0 dissolved oxygen."));
        SerialBT.println(F("admuwater cal -s do clear             // DO (Disolve Oxygen) delete calibration data."));
        SerialBT.println(F("admuwater cal -s ec -p low            // EC (Electrical Conductivity) low end calibration at 12880uS/cm."));
        SerialBT.println(F("admuwater cal -s ec -p high           // EC (Electrical Conductivity) high end calibration at 80000uS/cm."));
        SerialBT.println(F("admuwater cal -s ec -p dry            // EC (Electrical Conductivity) dry calibration."));
        SerialBT.println(F("admuwater cal -s ec clear             // EC (Electrical Conductivity) delete calibration data."));
        SerialBT.println(F("admuwater cal -s rtd -p boil          // Temperature calibration at boiling point (100°C)."));
        SerialBT.println(F("admuwater cal -s rtd clear            // Temperature delete calibration data"));
        SerialBT.println(F("*********************************************************************************************************"));
        SerialBT.println();
      }
    }
    else if (computerData == "admuwater -info\r") {
      Serial.println();
      Serial.println(F("*********************************************************************************************************"));
      Serial.println(F("ADMU water project provides portal to monitoring groundwater quality and quantity across the Philippines."));
      Serial.println(F("Go to the https://admuwater.com/ to discover more!\n"));
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
      if (BTconnection) {
        SerialBT.println();
        SerialBT.println(F("*********************************************************************************************************"));
        SerialBT.println(F("ADMU water project provides portal to monitoring groundwater quality and quantity across the Philippines."));
        SerialBT.println(F("Go to the https://admuwater.com/ to discover more!\n"));
        SerialBT.print(F("Device name:"));
        SerialBT.println(DEVICE_NAME);
        SerialBT.print(F("Manufacturer:"));
        SerialBT.println(MANUFACTURER);
        SerialBT.print(F("Hardware revision:"));
        SerialBT.println(HARDWARE_VERSION);
        SerialBT.print(F("Sensor location:"));
        SerialBT.println(SENSOR_LOCATION);
        SerialBT.print(F("Coordinates:"));
        SerialBT.println(COORDINATES);
        SerialBT.print(F("Static water level:"));
        SerialBT.println(SWL);
        SerialBT.print(F("Monitoring dashboard:"));
        SerialBT.println(DASHBOARD);
        SerialBT.println(F("*********************************************************************************************************"));
        SerialBT.println();
      }
    }
    else if (computerData == "admuwater conf -net gprs\r") {
      Serial.print(F("Config network type to GPRS.\n\n"));
      if (BTconnection) SerialBT.print(F("Config network type to GPRS.\n\n"));
    }
    else if (computerData == "admuwater conf -net wifi\r") {
      Serial.print(F("Config network type to Wi-Fi.\n\n"));
      if (BTconnection) SerialBT.print(F("Config network type to Wi-Fi.\n\n"));
    }
    else if (computerData == "admuwater cal -s ph -p low\r") {
      PhiGO.AtlasEZO(EZOPH_ADR, "Cal,low,4", 900);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("pH low-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("pH low-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ph -p mid\r") {
      PhiGO.AtlasEZO(EZOPH_ADR, "Cal,mid,7", 900);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("pH mid-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("pH mid-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ph -p high\r") {
      PhiGO.AtlasEZO(EZOPH_ADR, "Cal,high,10", 900);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("pH high-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("pH high-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ph clear\r") {
      PhiGO.AtlasEZO(EZOPH_ADR, "Cal,clear", 300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("pH clear calibration data OK.\n\n"));
        if (BTconnection) SerialBT.print(F("pH clear calibration data OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s do -p atm\r") {
      PhiGO.AtlasEZO(EZODO_ADR, "Cal", 1300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("DO atmospheric oxygen level calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("DO atmospheric oxygen level calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s do -p zero\r") {
      PhiGO.AtlasEZO(EZODO_ADR, "Cal,0", 1300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("DO zero dissolved oxygen calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("DO zero dissolved oxygen calibration OK.\n\n"));
        // This is necessary, add this line to maintain percent saturation (SAT) reading
        PhiGO.AtlasEZO(EZODO_ADR, "O,%,1", 300);
      }
    }
    else if (computerData == "admuwater cal -s do clear\r") {
      PhiGO.AtlasEZO(EZODO_ADR, "Cal,clear", 300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("DO clear calibration data OK.\n\n"));
        if (BTconnection) SerialBT.print(F("DO clear calibration data OK.\n\n"));
        // This is necessary, add this line to maintain percent saturation (SAT) reading
        PhiGO.AtlasEZO(EZODO_ADR, "O,%,1", 300);
      }
    }
    else if (computerData == "admuwater cal -s ec -p low\r") {
      PhiGO.AtlasEZO(EZOEC_ADR, "Cal,low,12880", 600);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("EC low-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("EC low-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ec -p high\r") {
      PhiGO.AtlasEZO(EZOEC_ADR, "Cal,high,80000", 900);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("EC high-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("EC high-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ec -p dry\r") {
      PhiGO.AtlasEZO(EZOEC_ADR, "Cal,dry", 600);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("EC dry calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("EC dry calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s ec clear\r") {
      PhiGO.AtlasEZO(EZOEC_ADR, "Cal,clear", 300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("EC clear calibration data OK.\n\n"));
        if (BTconnection) SerialBT.print(F("EC clear calibration data OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s rtd -p boil\r") {
      PhiGO.AtlasEZO(EZORTD_ADR, "Cal,100", 600);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("RTD boiling-point calibration OK.\n\n"));
        if (BTconnection) SerialBT.print(F("RTD boiling-point calibration OK.\n\n"));
      }
    }
    else if (computerData == "admuwater cal -s rtd clear\r") {
      PhiGO.AtlasEZO(EZORTD_ADR, "Cal,clear", 300);
      if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
        Serial.print(F("RTD clear calibration data OK.\n\n"));
        if (BTconnection) SerialBT.print(F("RTD clear calibration data OK.\n\n"));
      }
    }
    else {
      Serial.print(F("Wrong command syntax! Please try again.\n\n"));
      if (BTconnection) SerialBT.print(F("Wrong command syntax! Please try again.\n\n"));
    }

    computerData = "";                              // clear the input string for new readings
    event = false;                                  // toggle event status for new reading
  }

  static unsigned long timer = millis();
  if (millis() - timer > 30000) {
    timer = millis();

    float ph_val, do_val, orp_val, temp_val, sg_val, sal_val, sat_val;
    unsigned long int ec_val, tds_val;

    Serial.println("Reading Atlas EZO ciruit board...");
    // read Atlas Scientific EZO-pH sensor board
    PhiGO.AtlasEZO(EZOPH_ADR, "R", 815);
    if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
      ph_val = PhiGO.getPH();
    }

    // read Atlas Scientific EZO-RTD sensor board
    PhiGO.AtlasEZO(EZORTD_ADR, "R", 600);
    if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
      temp_val = PhiGO.getRTD();
    }

    // read Atlas Scientific EZO-EC sensor board
    PhiGO.AtlasEZO(EZOEC_ADR, "R", 570);
    if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
      ec_val = PhiGO.getEC();
      tds_val = PhiGO.getTDS();
      sal_val = PhiGO.getSAL();
      sg_val = PhiGO.getSG();
    }

    // read Atlas Scientific EZO-DO sensor board
    PhiGO.AtlasEZO(EZODO_ADR, "R", 575);
    if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
      do_val = PhiGO.getDO();
      sat_val = PhiGO.getSAT();
    }

    // print sensor readings
    if (BTconnection) {
      SerialBT.print(F("\n"));
      SerialBT.print(F("PH:"));
      SerialBT.print(ph_val);
      SerialBT.print(F(", "));
      SerialBT.print(F("DO:"));
      SerialBT.print(do_val);
      SerialBT.print(F("mg/L, "));
      SerialBT.print(F("Saturation:"));
      SerialBT.print(sat_val);
      SerialBT.print(F("%, "));
      SerialBT.print(F("Temp:"));
      SerialBT.print(temp_val);
      SerialBT.print(F("°C, "));
      SerialBT.print(F("EC:"));
      SerialBT.print(ec_val);
      SerialBT.print(F("uS/cm, "));
      SerialBT.print(F("TDS:"));
      SerialBT.print(tds_val);
      SerialBT.print(F("ppm, "));
      SerialBT.print(F("Salinity:"));
      SerialBT.print(sal_val);
      SerialBT.print(F("ppt, "));
      SerialBT.print(F("SG:"));
      SerialBT.print(sg_val);
      SerialBT.print(F(", "));
      SerialBT.print(F("ORP:"));
      SerialBT.print(orp_val);
      SerialBT.print(F("mV"));
      SerialBT.print(F("\n"));
    }

    Serial.print(F("\n"));
    Serial.print(F("PH:"));
    Serial.print(ph_val);
    Serial.print(F(", "));
    Serial.print(F("DO:"));
    Serial.print(do_val);
    Serial.print(F("mg/L, "));
    Serial.print(F("Saturation:"));
    Serial.print(sat_val);
    Serial.print(F("%, "));
    Serial.print(F("Temp:"));
    Serial.print(temp_val);
    Serial.print(F("°C, "));
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
    Serial.print(F("ORP:"));
    Serial.print(orp_val);
    Serial.print(F("mV"));
    Serial.print(F("\n"));
  }
}

void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();              // get the new byte
    computerData += inChar;                         // add it to the inputString
    if (inChar == '\r') {                           // check if data reach end marker (carriage return '\r')
      computerData.toLowerCase();                   // get a lower-case version of a String
      event = true;                                 // if the incoming character is a carriage return, set a flag status
      break;                                        // exit loop when received char data is end marker (new line '\r')
    }
  }
}

void callback(esp_spp_cb_event_t event, esp_spp_cb_param_t *param) {
  if (event == ESP_SPP_SRV_OPEN_EVT) {              // check if Serial Bluetooth Terminal app connected
    BTconnection = true;                            // set flag for Bluetooth connection status
    Serial.println("Serial Bluetooth Terminal app Connected.");
  }
  if (event == ESP_SPP_CLOSE_EVT ) {                // check if Serial Bluetooth Terminal app disconnected
    BTconnection = false;                           // set flag for Bluetooth connection status
    Serial.println("Serial Bluetooth Terminal app disconnected!");
  }
}
