/*
  AdmuWater.cpp - Library for monitoring groundwater quality and quantity across the Philippines (https://admuwater.com/)
  Created by Paul Ryan A. Santiago, October 25, 2022.
  Released into the public domain.
*/

#include "AdmuWater.h"
#include "Arduino.h"
#include <Wire.h>

#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define  __DEBUG__        // Comment this out to disable prints and save program memory space
#ifdef  __DEBUG__
#define debug(...) Serial.print(__VA_ARGS__);
#else
#define debug(...)
#endif

// TTGO T-Call pins
#define MODEM_RST            5
#define MODEM_PWKEY          4
#define MODEM_POWER_ON       23
#define MODEM_TX             27
#define MODEM_RX             26

AdmuWater::AdmuWater(void) {}

void AdmuWater::scan(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr)
{
  // assign device I2C address
  this->EZODO_ADR = ezodo_adr;
  this->EZOORP_ADR = ezoorp_adr;
  this->EZOPH_ADR = ezoph_adr;
  this->EZOEC_ADR = ezoec_adr;
  this->EZORTD_ADR = ezortd_adr;

  // initialize I2C port
  Wire.begin();
  delay(1000);

  // variable for error, I2C address, and number of devices
  byte error, address;
  int nDevices = 0;

  // scan for availalable I2C devices
  debug(F("debug print: Scanning Scientific Atlas EZO circuit boards...\n"));
  delay(3000);

  for (address = 1; address < 127; address++ )
  {
    // look for available I2C address
    Wire.beginTransmission(address);

    // transmit find device command
    Wire.write("find");

    // use the return value of the Write.endTransmisstion to see
    // if a device did acknowledge to the address.
    error = Wire.endTransmission();

    // no error were found, check the avaialble Atlas Circuit I2C board
    if (error == 0)
    {
      if (address == this->EZODO_ADR) {
        debug(F("debug print: Scientific Atlas EZO DO circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      else if (address == this->EZOORP_ADR) {
        debug(F("debug print: Scientific Atlas EZO ORP circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      else if (address == this->EZOPH_ADR) {
        debug(F("debug print: Scientific Atlas EZO PH circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      else if (address == this->EZOEC_ADR) {
        debug(F("debug print: Scientific Atlas EZO EC circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      else if (address == this->EZORTD_ADR) {
        debug(F("debug print: Scientific Atlas EZO RTD circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      else {
        debug(F("debug print: Unknown devices found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      }
      // count the number of I2C devices found
      nDevices++;
    }
    // if error were found
    else if (error == 4)
    {
      debug(F("debug print: Unknown error at address 0x"));
      if (address < 16) debug(F("debug print: 0"));
      debug(address, HEX);
      debug("(");
      debug(address);
      debug(")\n");
    }
  }

  // check the number of Atlas I2C circuit board found
  if (nDevices == 0) debug(F("debug print: No I2C devices found!\n\n"));
  if (nDevices > 0 && nDevices < 5) debug(F("debug print: Some Atlas circuit board not found! Please check wiring / connect module.\n\n"));
  if (nDevices == 5) debug(F("debug print: Scanning completed.\n\n"));
  delay(3000);

  // Send found command for Atlas I2C circuit board
  for (address = 1; address < 127; address++ )
  {
    Wire.beginTransmission(address);
    Wire.write("found!");
    Wire.endTransmission();
  }
}

void AdmuWater::initAtlasEZO(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr)
{
  this->EZODO_ADR = ezodo_adr;
  this->EZOORP_ADR = ezoorp_adr;
  this->EZOPH_ADR = ezoph_adr;
  this->EZOEC_ADR = ezoec_adr;
  this->EZORTD_ADR = ezortd_adr;

  debug(F("debug print: Initializing Atlas probe..."));
  Wire.beginTransmission(EZOEC_ADR);              // call the circuit by its ID number
  Wire.write("K,0.1");                            // transmit the set K value command
  delay(300);
  Wire.write("T,25");                             // transmit the temperature compensation command
  delay(300);
  Wire.write("O,TDS,1");                          // transmit the enable total dissolved solids command
  delay(300);
  Wire.write("O,S,1");                            // transmit the enable salinity command
  delay(300);
  Wire.write("O,SG,1");                           // transmit the enable specific gravity command
  Wire.endTransmission();                         // end the I2C data transmission

  Wire.beginTransmission(EZODO_ADR);              // call the circuit by its ID number
  Wire.write("O,%,1");                            // transmit the enable percent saturation command
  delay(300);
  Wire.endTransmission();                         // end the I2C data transmission
  debug(F("done.\n"));
}

void AdmuWater::initSDCard() {
  if (!SD.begin()) {
    debug(F("debug print: Card Mount Failed! Things to check:\n"));
    debug(F("debug print: * is a card inserted?"));
    debug(F("debug print: * is your wiring correct?"));
    debug(F("debug print: * did you change the chipSelect pin to match your shield or module?"));
    return;
  }
  else {
    debug(F("debug print: Wiring is correct and a SD card found.\n"));
  }

  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    debug(F("debug print: No SD card attached.\n"));
    return;
  }

  debug(F("debug print: SD card type: "));

  if (cardType == CARD_MMC) {
    debug(F("MMC\n"));
  } else if (cardType == CARD_SD) {
    debug(F("SDSC\n"));
  } else if (cardType == CARD_SDHC) {
    debug(F("SDHC\n"));
  } else {
    debug(F("UNKNOWN\n"));
  }

  debug(F("debug print: Card size:  "));
  debug((uint64_t)SD.cardSize() / (1024 * 1024));
  debug(F("MB\n"));

  debug(F("debug print: Total bytes: "));
  debug((uint64_t)SD.totalBytes() / (1024 * 1024));
  debug(F("MB\n"));

  debug(F("debug print: Used bytes: "));
  debug((uint64_t)SD.usedBytes() / (1024 * 1024));
  debug(F("MB\n"));

  debug(F("debug print: Initializing SD card..."));
  File file = SD.open("/data.txt");               // If the data.txt file doesn't exist. Create a file on the SD card and write the data labels
  if (!file) {
    debug(F("File doesn't exist. Creating file..."));
    writeData("/data.txt", "Date, Time, Battery level, Charge status, pH, DO, %SAT, TempC, EC, TDS, SAL, SG, ORP, Depth, SWL \r\n");
  }
  else {
    debug(F("debug print: File already exists..."));
  }
  file.close();
  debug(F("debug print: done.\n"));
}

void AdmuWater::AtlasEZO(uint8_t address, const char* cmd, uint16_t time) {
  Wire.beginTransmission(address);                // call the circuit by its ID number
  Wire.write(cmd);                                // transmit the command that was sent through the serial port
  Wire.endTransmission();                         // end the I2C data transmission

  if (strcmp(cmd, "sleep") != 0) {                // if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data

    delay(time);                                  // wait the correct amount of time for the circuit to complete its instruction

    Wire.requestFrom(address, 32, 1);             // call the circuit and request 32 bytes (this could be too small, but it is the max i2c buffer size for an Arduino)
    byte code = Wire.read();                      // the first byte is the response code, we read this separately

    switch (code) {                               // switch case based on what the response code is
      case 1:                                     // decimal 1
        debug("debug print: Success\n");          // means the command was successful
        this->ezo_stat = SUCCESS;                 // assign status to variable
        break;                                    // exits the switch case
      case 2:                                     // decimal 2
        debug("debug print: Failed\n");           // means the command has failed
        this->ezo_stat = SYNTAX_ERROR;            // assign status to variable
        break;                                    // exits the switch case
      case 254:                                   // decimal 254
        debug("debug print: Pending\n");          // means the command has not yet been finished calculating
        this->ezo_stat = NOT_READY;               // assign status to variable
        break;                                    // exits the switch case
      case 255:                                   // decimal 255
        debug("debug print: No Data\n");          // means there is no further data to send
        this->ezo_stat = NO_DATA;                 // assign status to variable
        break;                                    // exits the switch case
    }

    while (Wire.available()) {                    // are there bytes to receive
      in_char = Wire.read();                      // receive a byte
      ezo_data[i] = in_char;                      // load this byte into our array
      i += 1;                                     // occur the counter for the array element
      if (in_char == 0) {                         // if we see that we have been sent a null command
        i = 0;                                    // reset the counter i to 0
        break;                                    // exit the while loop
      }
    }

    if (strcmp(cmd, "r") == 0 || strcmp(cmd, "R") == 0) {
      debug("debug print: EZO data: ");
      debug(ezo_data);                            // print the read data
      debug("\n");                                // this just makes the output easier to read by adding an extra blank line
    }
  }
}

enum AdmuWater::status AdmuWater::getStatus() {
  return this->ezo_stat;                          // return status value
}

float AdmuWater::getPH()
{
  debug(F("debug print: PH:"));
  debug(ezo_data);
  debug(F("\n"));

  ph_val = atof(ezo_data);                        // take the ezo value and convert it into floating point number
  return this->ph_val;                            // return floating point pH value
}

float AdmuWater::getORP()
{
  debug(F("debug print: ORP:"));
  debug(ezo_data);
  debug(F("\n"));

  this->orp_val = atof(ezo_data);                 // take the ezo value and convert it into floating point number
  return this->orp_val;                           // return floating point pH value
}

float AdmuWater::getDO()
{
  char *DO = strtok(ezo_data, ",");               // break up the CSV string into its 2 individual parts. DO,PERCENT SATURATION
  char *SAT = strtok(NULL, ",");                  // let's pars the string at each comma

  debug(F("debug print: DO:"));
  debug(DO);
  debug(F(", "));
  debug(F("SAT:"));
  debug(SAT);
  debug(F("\n"));

  if (strcmp(DO, "38.19") == 0 && strcmp(SAT, "420.0") == 0) {
    this->do_val = 0.00;                          // Assigned zero value when sensor probe is not connected
    this->sat_val = 0.00;                         // Assigned zero value when sensor probe is not connected
  }
  else if (strcmp(DO, "100000.00") == 0 && strcmp(SAT, "100000.0") == 0) {
    this->do_val = 0.00;                          // Assigned zero value when sensor probe is not connected
    this->sat_val = 0.00;                         // Assigned zero value when sensor probe is not connected
  }
  else {
    this->do_val = atof(DO);                      // take the DO value and convert it into floating point number
    this->sat_val = atof(SAT);                    // take the percent saturation value and convert it into floating point number
  }
  return this->do_val;                            // return floating point DO value
}

float AdmuWater::getSAT()
{
  return this->sat_val;                           // return floating point percent saturation value
}

float AdmuWater::getRTD()
{
  debug(F("debug print: RTD:"));
  debug(ezo_data);
  debug(F("\n"));

  if (strcmp(ezo_data, "-1023.000") == 0) {       // Check if RTD probe disconnected
    this->temp_val = 0.00;                        // Assigned zero value when sensor probe is not connected
  }
  else {
    this->temp_val = atof(ezo_data);              // take the temperature value and convert it into floating point number
  }
  return this->temp_val;                          // return floating point temperature value
}

float AdmuWater::getEC()
{
  char *EC = strtok(ezo_data, ",");               // break up the CSV string into its 2 individual parts. EC,TDS, SAL, SG
  char *TDS = strtok(NULL, ",");                  // let's pars the string at each comma
  char *SAL = strtok(NULL, ",");                  // let's pars the string at each comma
  char *SG = strtok(NULL, ",");                   // let's pars the string at each comma

  debug(F("debug print: EC:"));
  debug(EC);
  debug(F(", "));
  debug(F("TDS:"));
  debug(TDS);
  debug(F(", "));
  debug(F("SAL:"));
  debug(SAL);
  debug(F(", "));
  debug(F("SG:"));
  debug(SG);
  debug(F("\n"));

  if (strcmp(EC, "0.00") == 0) {                  // Check if EC probe disconnected
    this->sg_val = 0.00;                          // Assigned zero values when sensor probe is not connected
  }
  else {
    this->sg_val = atof(SG);                      // take the Specific Gravity value and convert it into floating point number
    this->ec_val = atol(EC);                      // take the Electrical Conductivity value and convert it into floating point number
    this->tds_val = atol(TDS);                    // take the Total Disolve Solids value and convert it into floating point number
    this->sal_val = atof(SAL);                    // take the Salinity value and convert it into floating point number
  }
  return this->ec_val;                            // return floating point electrical conductivity value
}

float AdmuWater::getTDS()
{
  return this->tds_val;                           // return floating point total dissolved solids value
}

float AdmuWater::getSAL()
{
  return this->sal_val;                           // return floating point salinity value
}

float AdmuWater::getSG()
{
  return this->sg_val;                            // return floating point specific gravity value
}

float AdmuWater::getBattVolts()
{
  // read raw ADC value at D35
  int adc1_raw_val = analogRead(35);

  // convert adc raw value to voltage reading
  float input_voltage = adc1_raw_val * (3.3 / 4095.0);

  // calculate the voltage of the source batery from voltage divider circuit
  this->battVolts_val = input_voltage / (R2 / (R1 + R2));

  // return the battey voltage reading
  return this->battVolts_val;
}

float AdmuWater::getBattPercent()
{
  // calculate the battery pertage level
  this->battPercent_val = ((this->battVolts_val - 10.5) / 2.5) * 100.0;

  // check the battery pertage level if in percent range values
  this->battPercent_val = constrain(this->battPercent_val, 0, 100);

  // return the battey percent reading
  return this->battPercent_val;
}

String AdmuWater::getBattStatus()
{
  // check the battery charge status
  if (battPercent_val <= 100.0 && battPercent_val >= 97.0) {
    this->battStatus_val = "Fully charge";
  }
  else if (battPercent_val < 97.0 && battPercent_val >= 75.0) {
    this->battStatus_val = "OK";
  }
  else if (battPercent_val < 75.0 && battPercent_val >= 25.0) {
    this->battStatus_val = "Low charge";
  }
  else if (battPercent_val < 25.0) {
    this->battStatus_val = "discharge";
  }

  // return the battey charge status
  return this->battStatus_val;
}

float AdmuWater::getDEPTH() {
  // read raw ADC value at D34
  int adc1_raw_val = analogRead(34);

  // convert adc raw value to voltage reading
  float input_voltage = adc1_raw_val * (3.3 / 4095.0);

  // calculate input current via ohms law (I = V/R) where R = 165 ohms, Vmax = 3.3V
  float input_current = input_voltage / 165.0;

  // calculate depth value vias lope line equation (y = mx + b) where m is the slope (0-20mA -> 4-30m), b = 4
  this->water_depth = 1.3 * input_current + 4;

  // return the water depth reading
  return this->water_depth;
}

float AdmuWater::getDepth(uint8_t adc_zero_depth, float depth_sensor_slope)
{
  // read raw ADC value at D34
  int adc1_raw_val = analogRead(34);

  // map ADC reading to 0 - 1023
  int adc1_map_val = adc1_raw_val / 4.0;

  // calculate the water depth
  this->water_depth = float(adc1_map_val - adc_zero_depth) / depth_sensor_slope;

  // check if calculated value is negative
  if (this->water_depth < 0) {
    this->water_depth = 0;
  }

  // return the water depth reading
  return this->water_depth;
}

float AdmuWater::getSWL(float dtg_factor)
{
  // calculate static water level (SWL)
  swl_val = dtg_factor - this->water_depth;

  // return the SWL reading
  return swl_val;
}

void writeFile(fs::FS &fs, const char* path, const char* message) {
  debug("Writing file: " + String(path) + "\n");

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    debug(F("debug print: Failed to open file for writing!\n"));
    return;
  }
  if (file.print(message)) {
    debug(F("debug print: File written.\n"));
  } else {
    debug(F("debug print: Write failed!\n"));
  }
  file.close();
}

void appendFile(fs::FS &fs, const char* path, const char* message) {
  debug("debug print: Appending to file: " + String(path) + "\n");

  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    debug(F("debug print: Failed to open file for appending!\n"));
    return;
  }
  if (file.print(message)) {
    debug(F("debug print: Message appended.\n"));
  } else {
    debug(F("debug print: Append failed!\n"));
  }
  file.close();
}

void AdmuWater::writeData(const char* path, const char* message) {
  writeFile(SD, path, message);
}

void AdmuWater::appendData(const char* path, const char* message) {
  appendFile(SD, path, message);
}

String modem_command(String at_command) {
  Serial1.println(at_command);
  delay(1000);

  while (Serial1.available() == 0);                                         // wait for first char
  unsigned long lastRead = millis();                                        // last time a char was available

  String response;
  while (millis() - lastRead < 10000) {                                     // get AT response within 10 seconds
    while (Serial1.available() != 0) {                                      // if response is available
      lastRead = millis();                                                  // update the lastRead timestamp
      response = Serial1.readStringUntil('\n');                             // read AT response
      debug(F("debug print: "));                                            // print AT response
      debug(response);
      debug(F("\n"));
      return response;                                                      // return the AT response value
    }

    if (response.startsWith("OK"))                                          // exit loop when receive the following AT response
      break;
    if (response.startsWith("ERROR"))
      break;
    if (response.startsWith("+CME ERROR: operation not allowed"))
      break;
  }
}

void AdmuWater::gprsInit(String apn, String username, String password) {
  // Set modem reset, enable, power pins
  pinMode(MODEM_PWKEY, OUTPUT);
  pinMode(MODEM_RST, OUTPUT);
  pinMode(MODEM_POWER_ON, OUTPUT);

  digitalWrite(MODEM_PWKEY, LOW);
  digitalWrite(MODEM_RST, HIGH);
  digitalWrite(MODEM_POWER_ON, HIGH);
  delay(1000);

  // Set GSM module baud rate and UART pins
  Serial1.begin(115200, SERIAL_8N1, MODEM_RX, MODEM_TX);
  delay(3000);

  debug(F("debug print: ------- CHECK SIM CHARD ------- \n"));
  modem_command("AT");                                                      // check gsm module
  modem_command("AT+CLTS=1;&W");                                            // enable auto network time sync
  modem_command("AT+CFUN=1,1");                                             // reboot the module
  modem_command("AT+CSQ");                                                  // check signal quality
  modem_command("AT+CCID");                                                 // check sim card number
  modem_command("AT+CREG?");                                                // check sim card registered to the network
  modem_command("ATI");                                                     // gets the sim card's name and number
  modem_command("AT+COPS?");                                                // check sim card if connected to the network

  debug(F("debug print: -------   CONFIG GPRS   ------- \n"));
  modem_command("AT+SAPBR=3,1,Contype,GPRS");                               // set connection to GPRS
  modem_command("AT+SAPBR=3,1,APN," + apn);                                 // set APN

  if (username != "") {
    modem_command("AT+SAPBR=3,1,USER," + username);                         // set APN username
  }
  if (password != "") {
    modem_command("AT+SAPBR=3,1,PWD," + password);                          // set APN password
  }

  modem_command("AT+CCLK?");                                                // get initial local time value
  modem_command("AT+SSLOPT=0,1");                                           // disables SLL certificate checking
}

void AdmuWater::gprsGoogleSheet(String google_script_id, String payload) {
  modem_command("AT+SAPBR=1,1");                                            // activate a GPRS context
  modem_command("AT+SAPBR=2,1");                                            // query the GPRS context
  modem_command("AT+HTTPINIT");                                             // initialize the HTTP service
  modem_command("AT+HTTPPARA=CID,1");                                       // set HTTP session parameters

  //https://script.google.com/macros/s/AKfycbx7tTKEAYH7EIymHOa0cQefjXvR4fWczm_SaOm5gwWOc6k0VGk5bX6KQ9toE_xgDeBB/exec?sensor=ESP32&value=777
  String URL = "https://script.google.com/macros/s/" + google_script_id + "/exec?" + payload;

  modem_command("AT+HTTPPARA=URL," + URL);                                  // set HTTP server
  modem_command("AT+HTTPSSL=1");                                            // enabling the HTTPS function
  modem_command("AT+HTTPACTION=0");                                         // HTTP get request
  modem_command("AT+HTTPREAD");                                             // read response from the HTTP server
  modem_command("AT+HTTPTERM");                                             // close HTTP request
  modem_command("AT+SAPBR=0,1");                                            // close GPRS
}

void AdmuWater::gprsThingSpeak(String thingspeak_http_auth, String payload) {
  modem_command("AT+SAPBR=1,1");                                            // activate a GPRS context
  modem_command("AT+SAPBR=2,1");                                            // query the GPRS context
  modem_command("AT+HTTPINIT");                                             // initialize the HTTP service
  modem_command("AT+HTTPPARA=CID,1");                                       // set HTTP session parameters

  // Create HTTP URL
  String URL = String("api.thingspeak.com/update?api_key=") + thingspeak_http_auth + payload;

  modem_command("AT+HTTPPARA=URL," + URL);                                  // set HTTP server
  modem_command("AT+HTTPSSL=0");                                            // disable the HTTPS function
  modem_command("AT+HTTPACTION=1");                                         // HTTP post request
  modem_command("AT+HTTPREAD");                                             // read data from the HTTP server
  modem_command("AT+HTTPTERM");                                             // close HTTP request
  modem_command("AT+SAPBR=0,1");                                            // close GPRS
}

void AdmuWater::gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload) {
  modem_command("AT+SAPBR=1,1");                                            // activate a GPRS context
  modem_command("AT+SAPBR=2,1");                                            // query the GPRS context
  modem_command("AT+HTTPINIT");                                             // initialize the HTTP service
  modem_command("AT+HTTPPARA=CID,1");                                       // set HTTP session parameters

  // Create HTTPS URL
  String URL = String("https://") + firebase_host + firebase_path + ".json?auth=" + firebase_auth;

  modem_command("AT+HTTPPARA=URL," + URL);                                  // set HTTP server
  modem_command("AT+HTTPPARA=CONTENT,application/json");                    // set HTTP payload type
  modem_command("AT+HTTPDATA=" + String(payload.length()) + ",100000");     // set HTTP payload size
  modem_command(payload);                                                   // set HTTP payload body content
  modem_command("AT+HTTPSSL=1");                                            // enabling the HTTPS function
  modem_command("AT+HTTPACTION=1");                                         // HTTP post request
  modem_command("AT+HTTPREAD");                                             // read data from the HTTP server
  modem_command("AT+HTTPTERM");                                             // close HTTP request
  modem_command("AT+SAPBR=0,1");                                            // close GPRS
}

String AdmuWater::gprsGetLocalTime() {
  String timestamp;
  String response = modem_command("AT+CCLK?");                              // get AT response
  if (response.startsWith("+CCLK")) {                                       // parse substring that contain timestamp only
    int idx_1, idx_2, idx_3;
    idx_1 = response.indexOf(":");
    idx_2 = response.indexOf("\"", idx_1 + 1);
    idx_3 = response.indexOf("+", idx_2 + 1);
    timestamp = response.substring(idx_2, idx_3);                           // parse timestamp from AT response
    timestamp.remove(0, 1);                                                 // remove unwanted character from parsed timestamp

    debug(F("debug print: "));                                              // print parsed timestamp
    debug(F("GPRS time stamp: "));
    debug(timestamp);
    debug(F("\n"));
    return timestamp;                                                       // return a parsed timestamp value
  }
}

float AdmuWater::getAveVal(float* array, uint8_t len) {
  unsigned long int sum = 0L ;                                              // assigned long
  for (int i = 0 ; i < len ; i++) {
    sum += array [i] ;                                                      // calculate for sum of an array
  }
  float ave = ((float) sum) / len ;                                         // calculate for average of an array
  return ave;                                                               // return average value from array
}
