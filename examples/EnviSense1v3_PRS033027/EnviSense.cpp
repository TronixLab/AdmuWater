#include "EnviSense.h"
#include "Arduino.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>

#define __DEBUG__  // Comment this out to disable prints and save program memory space
#ifdef __DEBUG__
#define debug(...) Serial.print(__VA_ARGS__);
#else
#define debug(...)
#endif

#define SS_RX 10
#define SS_TX 11
SoftwareSerial mySerial(SS_RX, SS_TX);
// Connect the GSM/GPRS shield TX (D7) to Arduino pin 10 Software Serial RX.
// Connect the GSM/GPRS shield RX (D8) to Arduino pin 11 Software Serial TX.

File myFile;  // create a file object to store the data

EnviSense::EnviSense(void) {}

void EnviSense::scan(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr) {
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

  for (address = 1; address < 127; address++) {
    // look for available I2C address
    Wire.beginTransmission(address);

    // transmit find device command
    Wire.write("find");

    // use the return value of the Write.endTransmisstion to see
    // if a device did acknowledge to the address.
    error = Wire.endTransmission();

    // no error were found, check the avaialble Atlas Circuit I2C board
    if (error == 0) {
      if (address == this->EZODO_ADR) {
        debug(F("debug print: Scientific Atlas EZO DO circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      } else if (address == this->EZOORP_ADR) {
        debug(F("debug print: Scientific Atlas EZO ORP circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      } else if (address == this->EZOPH_ADR) {
        debug(F("debug print: Scientific Atlas EZO PH circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      } else if (address == this->EZOEC_ADR) {
        debug(F("debug print: Scientific Atlas EZO EC circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      } else if (address == this->EZORTD_ADR) {
        debug(F("debug print: Scientific Atlas EZO RTD circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug("(");
        debug(address);
        debug(")");
        debug(F("!\n"));
        delay(300);
      } else {
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
    else if (error == 4) {
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
  for (address = 1; address < 127; address++) {
    Wire.beginTransmission(address);
    Wire.write("found!");
    Wire.endTransmission();
  }
}

void EnviSense::initAtlasEZO(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr) {
  this->EZODO_ADR = ezodo_adr;
  this->EZOORP_ADR = ezoorp_adr;
  this->EZOPH_ADR = ezoph_adr;
  this->EZOEC_ADR = ezoec_adr;
  this->EZORTD_ADR = ezortd_adr;

  debug(F("debug print: Initializing Atlas probe...\n"));
  Wire.beginTransmission(EZOEC_ADR);  // call the circuit by its ID number
  Wire.write("K,0.1");                // transmit the set K value command
  delay(300);
  Wire.write("T,25");  // transmit the temperature compensation command
  delay(300);
  Wire.write("O,TDS,1");  // transmit the enable total dissolved solids command
  delay(570);
  Wire.write("O,S,1");  // transmit the enable salinity command
  delay(570);
  Wire.write("O,SG,1");  // transmit the enable specific gravity command
  delay(570);
  Wire.endTransmission();  // end the I2C data transmission

  Wire.beginTransmission(EZODO_ADR);  // call the circuit by its ID number
  Wire.write("O,%,1");                // transmit the enable percent saturation command
  delay(300);
  Wire.endTransmission();  // end the I2C data transmission
  debug(F("debug print: done.\n"));
}

void EnviSense::AtlasEZO(uint8_t address, const char *cmd, uint16_t time) {
  Wire.beginTransmission(address);  // call the circuit by its ID number
  Wire.write(cmd);                  // transmit the command that was sent through the serial port
  Wire.endTransmission();           // end the I2C data transmission

  if (strcmp(cmd, "sleep") != 0) {  // if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data

    delay(time);  // wait the correct amount of time for the circuit to complete its instruction

    Wire.requestFrom(address, 32, 1);  // call the circuit and request 32 bytes (this could be too small, but it is the max i2c buffer size for an Arduino)
    byte code = Wire.read();           // the first byte is the response code, we read this separately

    switch (code) {                       // switch case based on what the response code is
      case 1:                             // decimal 1
        debug("debug print: Success\n");  // means the command was successful
        this->ezo_stat = SUCCESS;         // assign status to variable
        break;                            // exits the switch case
      case 2:                             // decimal 2
        debug("debug print: Failed\n");   // means the command has failed
        this->ezo_stat = SYNTAX_ERROR;    // assign status to variable
        break;                            // exits the switch case
      case 254:                           // decimal 254
        debug("debug print: Pending\n");  // means the command has not yet been finished calculating
        this->ezo_stat = NOT_READY;       // assign status to variable
        break;                            // exits the switch case
      case 255:                           // decimal 255
        debug("debug print: No Data\n");  // means there is no further data to send
        this->ezo_stat = NO_DATA;         // assign status to variable
        break;                            // exits the switch case
    }

    while (Wire.available()) {  // are there bytes to receive
      in_char = Wire.read();    // receive a byte
      ezo_data[i] = in_char;    // load this byte into our array
      i += 1;                   // occur the counter for the array element
      if (in_char == 0) {       // if we see that we have been sent a null command
        i = 0;                  // reset the counter i to 0
        break;                  // exit the while loop
      }
    }

    if (strcmp(cmd, "r") == 0 || strcmp(cmd, "R") == 0) {
      debug("debug print: EZO data: ");
      debug(ezo_data);  // print the read data
      debug("\n");      // this just makes the output easier to read by adding an extra blank line
    }
  }
}

enum EnviSense::status EnviSense::getStatus() {
  return this->ezo_stat;  // return status value
}

float EnviSense::getPH() {
  debug(F("debug print: PH:"));
  debug(ezo_data);
  debug(F("\n"));

  ph_val = atof(ezo_data);  // take the ezo value and convert it into floating point number
  return this->ph_val;      // return floating point pH value
}

float EnviSense::getORP() {
  debug(F("debug print: ORP:"));
  debug(ezo_data);
  debug(F("\n"));

  this->orp_val = atof(ezo_data);  // take the ezo value and convert it into floating point number
  return this->orp_val;            // return floating point pH value
}

float EnviSense::getDO() {
  char *DO = strtok(ezo_data, ",");  // break up the CSV string into its 2 individual parts. DO,PERCENT SATURATION
  char *SAT = strtok(NULL, ",");     // let's pars the string at each comma

  debug(F("debug print: DO:"));
  debug(DO);
  debug(F(", "));
  debug(F("SAT:"));
  debug(SAT);
  debug(F("\n"));

  if (strcmp(DO, "38.19") == 0 && strcmp(SAT, "420.0") == 0) {
    this->do_val = 0.00;   // Assigned zero value when sensor probe is not connected
    this->sat_val = 0.00;  // Assigned zero value when sensor probe is not connected
  } else if (strcmp(DO, "100000.00") == 0 && strcmp(SAT, "100000.0") == 0) {
    this->do_val = 0.00;   // Assigned zero value when sensor probe is not connected
    this->sat_val = 0.00;  // Assigned zero value when sensor probe is not connected
  } else {
    this->do_val = atof(DO);    // take the DO value and convert it into floating point number
    this->sat_val = atof(SAT);  // take the percent saturation value and convert it into floating point number
  }
  return this->do_val;  // return floating point DO value
}

float EnviSense::getSAT() {
  return this->sat_val;  // return floating point percent saturation value
}

float EnviSense::getRTD() {
  debug(F("debug print: RTD:"));
  debug(ezo_data);
  debug(F("\n"));

  if (strcmp(ezo_data, "-1023.000") == 0) {  // Check if RTD probe disconnected
    this->temp_val = 0.00;                   // Assigned zero value when sensor probe is not connected
  } else {
    this->temp_val = atof(ezo_data);  // take the temperature value and convert it into floating point number
  }
  return this->temp_val;  // return floating point temperature value
}

float EnviSense::getEC() {
  char *EC = strtok(ezo_data, ",");  // break up the CSV string into its 2 individual parts. EC,TDS, SAL, SG
  char *TDS = strtok(NULL, ",");     // let's pars the string at each comma
  char *SAL = strtok(NULL, ",");     // let's pars the string at each comma
  char *SG = strtok(NULL, ",");      // let's pars the string at each comma

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

  if (strcmp(EC, "0.00") == 0) {  // Check if EC probe disconnected
    this->sg_val = 0.00;          // Assigned zero values when sensor probe is not connected
  } else {
    this->sg_val = atof(SG);    // take the Specific Gravity value and convert it into floating point number
    this->ec_val = atol(EC);    // take the Electrical Conductivity value and convert it into floating point number
    this->tds_val = atol(TDS);  // take the Total Disolve Solids value and convert it into floating point number
    this->sal_val = atof(SAL);  // take the Salinity value and convert it into floating point number
  }
  return this->ec_val;  // return floating point electrical conductivity value
}

float EnviSense::getTDS() {
  return this->tds_val;  // return floating point total dissolved solids value
}

float EnviSense::getSAL() {
  return this->sal_val;  // return floating point salinity value
}

float EnviSense::getSG() {
  return this->sg_val;  // return floating point specific gravity value
}

String timestamp = "";
String at_response = "";
boolean at_complete = false;
void modem_command(String at_command, int at_wait) {
  mySerial.println(at_command);
  delay(at_wait);

  while (mySerial.available() != 0) {     // if data is available on serial port
    char inChar = (char)mySerial.read();  // read character from AT response
    Serial.write(inChar);                 // print character received on to the serial monitor
    at_response += inChar;                // add it to the response string

    if (inChar == '\n') {
      at_complete = true;
    }
  }

  if (at_complete) {
    at_complete = false;
    if (at_response.startsWith("AT+CCLK?")) {  // parse substring that contain timestamp only
      int idx_1, idx_2, idx_3;
      idx_1 = at_response.indexOf(":");
      idx_2 = at_response.indexOf("\"", idx_1 + 1);
      idx_3 = at_response.indexOf("+", idx_2 + 1);
      timestamp = at_response.substring(idx_2, idx_3);  // parse timestamp from AT response
      timestamp.remove(0, 1);                           // remove unwanted character from parsed timestamp

      debug(F("debug print: "));  // print parsed timestamp
      debug(F("GPRS time stamp: "));
      debug(timestamp);
      debug(F("\n"));
    }
    at_response = "";  // clear the string before receiving the at response
  }
  delay(at_wait);
}

void EnviSense::gprsInit(String apn, String username, String password) {
  // power ON the GPRS module
  pinMode(9, OUTPUT);
  delay(1000);
  digitalWrite(9, LOW);
  delay(1000);
  digitalWrite(9, HIGH);
  delay(5000);
  digitalWrite(9, LOW);
  delay(3000);

  // Set GSM module baud rate
  mySerial.begin(9600);
  delay(1000);

  debug(F("debug print: ------- CHECK SIM CHARD ------- \n"));
  modem_command("AT\r", 1000);      // check gsm module
  modem_command("AT+CSQ\r", 1000);  // check signal quality
  modem_command("ATI\r", 1000);     // product Identification
  modem_command("AT+GMI\r", 1000);  // manufacturer Name

  debug(F("debug print: -------   CONFIG GPRS   ------- \n"));
  modem_command("AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r", 5000);     // set connection to GPRS
  modem_command("AT+SAPBR=3,1,\"APN\",\"" + apn + "\"\r", 5000);  // set APN

  if (username != "") {
    modem_command("AT+SAPBR=3,1,USER," + username + "\"\r", 5000);  // set APN username
  }
  if (password != "") {
    modem_command("AT+SAPBR=3,1,PWD," + password + "\"\r", 5000);  // set APN password
  }

  modem_command("AT+SSLOPT=0,1\r", 3000);  // disables SLL certificate checking
  modem_command("AT+CCLK?\r", 1000);       // get initial local time value (invalid time stamp)
}

void EnviSense::gprsThingSpeak(String thingspeak_http_auth, String payload) {
  modem_command("AT+SAPBR=1,1\r", 5000);           // activate a GPRS context
  modem_command("AT+SAPBR=2,1\r", 5000);           // query the GPRS context
  modem_command("AT+HTTPINIT\r", 5000);            // initialize the HTTP service
  modem_command("AT+HTTPPARA=\"CID\",1\r", 5000);  // set HTTP session parameters

  // Create HTTPS URL e.g. https://api.thingspeak.com/update
  String URL = "api.thingspeak.com/update";

  // Create HHTP request payload e.g. api_key=XXXXXXXXXX&field1=null&field2=null...%field8=null
  String thingspeak_payload = "api_key=" + thingspeak_http_auth + "&" + payload;

  modem_command("AT+HTTPPARA=\"URL\",\"" + URL + "\"\r", 5000);                                  // set HTTP server
  modem_command("AT+HTTPPARA=\"CONTENT\",\"application/x-www-form-urlencoded\"\r", 5000);        // set paylaod content type
  modem_command("AT+HTTPDATA=" + String(thingspeak_payload.length()) + ",100000" + "\r", 5000);  // POST data of Bytes size with maximum latency time of 10seconds for inputting the data
  modem_command(thingspeak_payload + "\r", 5000);                                                // print the data packet or payload to http request
  modem_command("AT+HTTPACTION=1\r", 5000);                                                      // HTTP post request
  modem_command("AT+HTTPTERM\r", 3000);                                                          // close HTTP request
  modem_command("AT+SAPBR=0,1\r", 2000);                                                         // close GPRS
}

void EnviSense::gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload) {
  modem_command("AT+SAPBR=1,1\r", 5000);           // activate a GPRS context
  modem_command("AT+SAPBR=2,1\r", 5000);           // query the GPRS context
  modem_command("AT+HTTPINIT\r", 5000);            // initialize the HTTP service
  modem_command("AT+HTTPSSL=1\r", 3000);           // enabling the HTTPS function
  modem_command("AT+HTTPPARA=\"CID\",1\r", 5000);  // set HTTP session parameters

  // Create HTTPS URL e.g. https://envisenseuc-default-rtdb.asia-southeast1.firebasedatabase.app/UC-Bagiuo.json?auth=uwPQgOIusWpyNcSaMZbZZSqpQAU3MC7HoYc7Ewub
  String URL = String("https://") + firebase_host + firebase_path + ".json?auth=" + firebase_auth;

  modem_command("AT+HTTPPARA=\"URL\",\"" + URL + "\"\r", 10000);                      // set HTTP server
  modem_command("AT+HTTPPARA=\"CONTENT\",\"application/json\"\r", 5000);              // set HTTP payload type
  modem_command("AT+HTTPDATA=" + String(payload.length()) + ",100000" + "\r", 2000);  // POST data of Bytes size with maximum latency time of 10seconds for inputting the data
  modem_command(payload + "\r", 5000);                                                // print the data packet or payload to http request
  modem_command("AT+HTTPACTION=1\r", 5000);                                           // HTTP post request
  modem_command("AT+HTTPREAD\r", 5000);                                               // read data from the HTTP server
  modem_command("AT+HTTPTERM\r", 5000);                                               // close HTTP request
  modem_command("AT+SAPBR=0,1\r", 5000);                                              // close GPRS
}

void EnviSense::gprsGoogleSheet(String google_script_id, String payload) {
  modem_command("AT+SAPBR=1,1\r", 5000);           // activate a GPRS context
  modem_command("AT+SAPBR=2,1\r", 5000);           // query the GPRS context
  modem_command("AT+HTTPINIT\r", 5000);            // initialize the HTTP service
  modem_command("AT+HTTPPARA=\"CID\",1\r", 5000);  // set HTTP session parameters

  // Create HTTPS URL e.g. https://script.google.com/macros/s/AKfycbx78mYXnuL5ygu8A3ORZnl9TWeccnddMbbiRKGVZXiP0h4RHEpY/exec?id=ENV002&action=addItem&ph=777&ph2=777&ec=777&tds=777&sal=777&swl=777&temp=777&battlvl=777
  String URL = "https://script.google.com/macros/s/" + google_script_id + "/exec?" + payload;

  modem_command("AT+HTTPPARA=\"URL\",\"" + URL + "\"\r", 10000);  // set HTTP server
  modem_command("AT+HTTPSSL=1\r", 3000);                          // enabling the HTTPS function
  modem_command("AT+HTTPACTION=0\r", 5000);                       // HTTP post request
  modem_command("AT+HTTPTERM\r", 5000);                           // close HTTP request
  modem_command("AT+SAPBR=0,1\r", 5000);                          // close GPRS
}

String EnviSense::gprsGetLocalTime() {
  modem_command("AT+CLTS=1\r", 3000);  // enable the gsm module to get the time from the network
  modem_command("AT+CCLK?\r", 3000);   // get the network time AT response
  return timestamp;                    // return a parsed timestamp value
}

float EnviSense::getBattVolts() {
  // read raw ADC value at D35
  int adc_raw_val = analogRead(0);

  // convert adc raw value to voltage reading
  float input_voltage = adc_raw_val * (5.0 / 1023.0);

  // calculate the voltage of the source batery from voltage divider circuit
  this->battVolts_val = input_voltage / (R2 / (R1 + R2));

  // return the battey voltage reading
  return this->battVolts_val;
}

float EnviSense::getBattPercent() {
  // calculate the battery pertage level
  this->battPercent_val = ((this->battVolts_val - 10.5) / 2.5) * 100.0;

  // check the battery pertage level if in percent range values
  this->battPercent_val = constrain(this->battPercent_val, 0, 100);

  // return the battey percent reading
  return this->battPercent_val;
}

String EnviSense::getBattStatus() {
  // check the battery charge status
  if (battPercent_val <= 100.0 && battPercent_val >= 97.0) {
    this->battStatus_val = "Fully charge";
  } else if (battPercent_val < 97.0 && battPercent_val >= 75.0) {
    this->battStatus_val = "OK";
  } else if (battPercent_val < 75.0 && battPercent_val >= 25.0) {
    this->battStatus_val = "Low charge";
  } else if (battPercent_val < 25.0) {
    this->battStatus_val = "discharge";
  }

  // return the battey charge status
  return this->battStatus_val;
}

float EnviSense::getDepth(uint8_t adc_zero_depth, float depth_sensor_slope) {
  // read raw ADC value at D34
  int adc_raw_val = analogRead(1);

  // calculate the water depth
  this->water_depth = float(adc_raw_val - adc_zero_depth) / depth_sensor_slope;

  // check if calculated value is negative
  if (this->water_depth < 0) {
    this->water_depth = 0;
  }

  // return the water depth reading
  return this->water_depth;
}

float EnviSense::getSWL(float dtg_factor) {
  // calculate static water level (SWL)
  swl_val = dtg_factor - this->water_depth;

  // return the SWL reading
  return swl_val;
}

void EnviSense::initSDCard(const char *path, String header) {
  if (!SD.begin(53)) {
    debug(F("debug print: Card initialization failed! Things to check:\n"));
    debug(F("debug print: * is a card inserted?"));
    debug(F("debug print: * is your wiring correct?"));
    debug(F("debug print: * did you change the chipSelect pin to match your shield or module?"));
    return;
  } else {
    debug(F("debug print: Wiring is correct and a SD card found.\n"));
  }

  debug(F("debug print: Initializing SD card...\n"));
  if (SD.exists(path)) {
    debug(F("debug print: File exit OK.\n"));
  } else {
    debug(F("debug print: File doesn't exist. Creating file...\n"));

    myFile = SD.open(path, FILE_WRITE);

    if (!myFile) {
      debug(F("debug print: Failed to open file for writing!\n"));
      return;
    }

    if (myFile.println(header)) {
      debug(F("debug print: Header written.\n"));
    } else {
      debug(F("debug print: Append failed!\n"));
    }
    myFile.close();
  }
  debug(F("debug print: done.\n"));
}

void EnviSense::logData(const char *path, String datafile) {
  debug("debug print: appending file to " + String(path) + "\n");

  myFile = SD.open(path, FILE_WRITE);

  if (!myFile) {
    debug(F("debug print: Failed to open file for writing!\n"));
    return;
  }

  if (myFile.println(datafile)) {
    debug(F("debug print: File appended.\n"));
  } else {
    debug(F("debug print: Append failed!\n"));
  }
  myFile.close();
}