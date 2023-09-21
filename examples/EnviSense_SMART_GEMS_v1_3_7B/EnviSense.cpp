#if ARDUINO >= 100
#include "Arduino.h"
#else
#include "WProgram.h"
#endif

#include "EnviSense.h"
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SoftwareSerial.h>
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

#define PHVALUEADDR 0  //the start address of the pH calibration parameters stored in the EEPROM

#define ENVS_DEBUG  // Comment this out to disable prints and save program memory space

#if defined(ENVS_DEBUG)
#define debug(...) Serial.print(__VA_ARGS__)
#else
#define debug(...)
#endif

#define SS_RX 10
#define SS_TX 11
SoftwareSerial mySerial(SS_RX, SS_TX);
// Connect the GSM/GPRS shield TX (D7) to Arduino pin 10 Software Serial RX.
// Connect the GSM/GPRS shield RX (D8) to Arduino pin 11 Software Serial TX.

File myFile;  // create a file object to store the data

#define BUFF_LENGTH 40
int ADC_ARRAY[BUFF_LENGTH];
int ARRAY_INDEX = 0;

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
  debug(F("debug print: Scanning Scientific Atlas EZO circuit boards..."));
  debug(F("\n"));
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
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
        delay(300);
      } else if (address == this->EZOORP_ADR) {
        debug(F("debug print: Scientific Atlas EZO ORP circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
        delay(300);
      } else if (address == this->EZOPH_ADR) {
        debug(F("debug print: Scientific Atlas EZO PH circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
        delay(300);
      } else if (address == this->EZOEC_ADR) {
        debug(F("debug print: Scientific Atlas EZO EC circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
        delay(300);
      } else if (address == this->EZORTD_ADR) {
        debug(F("debug print: Scientific Atlas EZO RTD circuit found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
        delay(300);
      } else {
        debug(F("debug print: Unknown devices found at address 0x"));
        if (address < 16) debug(F("debug print: 0"));
        debug(address, HEX);
        debug(F("("));
        debug(address);
        debug(F(")"));
        debug(F("!"));
        debug(F("\n"));
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
      debug(F("("));
      debug(address);
      debug(F(")"));
      debug(F("\n"));
    }
  }

  // check the number of Atlas I2C circuit board found
  if (nDevices == 0) {
    debug(F("debug print: No I2C devices found!"));
    debug(F("\n"));
  }

  if (nDevices > 0 && nDevices < 5) {
    debug(F("debug print: Some Atlas circuit board not found! Please check wiring / connect module."));
    debug(F("\n"));
  }

  if (nDevices == 5) {
    debug(F("debug print: Scanning completed."));
    debug(F("\n"));
  }

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

  debug(F("debug print: Initializing Atlas probe..."));
  debug(F("\n"));
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
  debug(F("debug print: done."));
  debug(F("\n"));
}

void EnviSense::AtlasEZO(uint8_t address, const char *cmd, uint16_t time) {
  Wire.beginTransmission(address);  // call the circuit by its ID number
  Wire.write(cmd);                  // transmit the command that was sent through the serial port
  Wire.endTransmission();           // end the I2C data transmission

  if (strcmp(cmd, "sleep") != 0) {  // if the command that has been sent is NOT the sleep command, wait the correct amount of time and request data

    delay(time);  // wait the correct amount of time for the circuit to complete its instruction

    Wire.requestFrom(address, 32, 1);  // call the circuit and request 32 bytes (this could be too small, but it is the max i2c buffer size for an Arduino)
    byte code = Wire.read();           // the first byte is the response code, we read this separately

    switch (code) {                        // switch case based on what the response code is
      case 1:                              // decimal 1
        debug(F("debug print: Success"));  // means the command was successful
        debug(F("\n"));
        this->ezo_stat = SUCCESS;           // assign status to variable
        break;                              // exits the switch case
      case 2:                               // decimal 2
        debug(F("debug print: Failed\n"));  // means the command has failed
        debug(F("\n"));
        this->ezo_stat = SYNTAX_ERROR;       // assign status to variable
        break;                               // exits the switch case
      case 254:                              // decimal 254
        debug(F("debug print: Pending\n"));  // means the command has not yet been finished calculating
        debug(F("\n"));
        this->ezo_stat = NOT_READY;          // assign status to variable
        break;                               // exits the switch case
      case 255:                              // decimal 255
        debug(F("debug print: No Data\n"));  // means there is no further data to send
        debug(F("\n"));
        this->ezo_stat = NO_DATA;  // assign status to variable
        break;                     // exits the switch case
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
      debug(F("debug print: EZO data: "));
      debug(ezo_data);  // print the read data
      debug(F("\n"));   // this just makes the output easier to read by adding an extra blank line
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
    at_response += inChar;                // add it to the response string

    if (inChar == '\n') {         // when AT response terminated new line ending
      at_complete = true;         // it means AT command completed, set variable into 'true'
      debug(F("AT response: "));  // print AT response
      debug(at_response);
      debug(F("\n"));
    }
  }

  if (at_complete) {
    at_complete = false;                                                          // set variable into false, for reading next AT commands
    if (at_response.startsWith("AT+CCLK?") || at_response.startsWith("+CCLK")) {  // parse substring that contain timestamp only
      int idx_1, idx_2, idx_3;                                                    // define AT response delimiters for parsing time
      idx_1 = at_response.indexOf(":");
      idx_2 = at_response.indexOf("\"", idx_1 + 1);
      idx_3 = at_response.indexOf("+", idx_2 + 1);
      timestamp = at_response.substring(idx_2, idx_3);  // parse timestamp from AT response
      timestamp.remove(0, 1);                           // remove unwanted character from parsed timestamp

      debug(F("GPRS time stamp: "));  // print parsed timestamp
      debug(timestamp);
      debug(F("\n"));
    }
    at_response = "";  // clear the string before receiving the at response
  }
  delay(at_wait);  // delay as preparation for the next AT command
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
  modem_command("AT\r", 1000);                     // dummy AT character to wake-up the gprs module
  modem_command("AT+CSCLK=0\r", 1000);             // disable slow clock, module will not enter sleep mode (~20mA)
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
  modem_command("AT+CSCLK=2\r", 1000);                                                           // Slow clock rate to power down (~1mA)
}

void EnviSense::gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload) {
  modem_command("AT\r", 1000);                     // dummy AT character to wake-up the gprs module
  modem_command("AT+CSCLK=0\r", 1000);             // disable slow clock, module will not enter sleep mode (~20mA)
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
  modem_command("AT+CSCLK=2\r", 1000);                                                // Slow clock rate to power down (~1mA)
}

void EnviSense::gprsGoogleSheet(String google_script_id, String payload) {
  modem_command("AT\r", 1000);                     // dummy AT character to wake-up the gprs module
  modem_command("AT+CSCLK=0\r", 1000);             // disable slow clock, module will not enter sleep mode (~20mA)
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
  modem_command("AT+CSCLK=2\r", 1000);                            // Slow clock rate to power down (~1mA)
}

String EnviSense::gprsGetLocalTime() {
  modem_command("AT+CLTS=1\r", 3000);    // enable the gsm module to get the time from the network
  modem_command("AT&W\r", 3000);         // store AT command configuration of the device
  modem_command("AT+CFUN=1,1\r", 3000);  // software restart the modem
  modem_command("AT+CCLK?\r", 3000);     // get the network time AT response
  return timestamp;                      // return a parsed timestamp value
}

float EnviSense::getBattVolts(const int adc_pin) {
  // read raw ADC value at D35
  int adc_raw_val = analogRead(adc_pin);

  // convert adc raw value to voltage reading
  float input_voltage = adc_raw_val * (5.0 / 1023.0);

  // calculate the voltage of the source batery from voltage divider circuit
  this->battVolts_val = input_voltage / (R2 / (R1 + R2));

  // return the battey voltage reading
  return this->battVolts_val;
}

float EnviSense::getBattPercent(float battVolts) {
  // check the measured battery voltage to minimum threshold
  if (battVolts < 10.5) {
    battVolts = 10.5;
  }

  // calculate the battery pertage level
  this->battPercent_val = ((battVolts - 10.5) / 2.5) * 100.0;

  // check the battery pertage level if in percent range values
  this->battPercent_val = constrain(this->battPercent_val, 0, 100);

  // return the battey percent reading
  return this->battPercent_val;
}

String EnviSense::getBattStatus(float battPercent) {
  // check the battery charge status
  if (battPercent <= 100.0 && battPercent >= 97.0) {
    this->battStatus_val = "Fully charge";
  } else if (battPercent < 97.0 && battPercent >= 75.0) {
    this->battStatus_val = "OK";
  } else if (battPercent < 75.0 && battPercent >= 25.0) {
    this->battStatus_val = "Low charge";
  } else if (battPercent < 25.0) {
    this->battStatus_val = "discharge";
  }

  // return the battey charge status
  return this->battStatus_val;
}

void EnviSense::initSDCard(const char *path, String header) {
  if (!SD.begin(53)) {
    debug(F("debug print: Card initialization failed! Things to check:"));
    debug(F("\n"));
    debug(F("debug print: * is a card inserted?"));
    debug(F("\n"));
    debug(F("debug print: * is your wiring correct?"));
    debug(F("\n"));
    debug(F("debug print: * did you change the chipSelect pin to match your shield or module?"));
    debug(F("\n"));
    return;
  } else {
    debug(F("debug print: Wiring is correct and a SD card found."));
    debug(F("\n"));
  }

  debug(F("debug print: Initializing SD card..."));
  debug(F("\n"));

  if (SD.exists(path)) {
    debug(F("debug print: File exit OK."));
    debug(F("\n"));
  } else {
    debug(F("debug print: File doesn't exist. Creating file..."));
    debug(F("\n"));

    myFile = SD.open(path, FILE_WRITE);

    if (!myFile) {
      debug(F("debug print: Failed to open file for writing!"));
      debug(F("\n"));
      return;
    }

    if (myFile.println(header)) {
      debug(F("debug print: Header written."));
      debug(F("\n"));
    } else {
      debug(F("debug print: Append failed!"));
      debug(F("\n"));
    }
    myFile.close();
  }
  debug(F("debug print: done."));
  debug(F("\n"));
}

void EnviSense::logData(const char *path, String datafile) {
  debug("debug print: appending file to " + String(path));
  debug(F("\n"));

  myFile = SD.open(path, FILE_WRITE);

  if (!myFile) {
    debug(F("debug print: Failed to open file for writing!"));
    debug(F("\n"));
    return;
  }

  if (myFile.println(datafile)) {
    debug(F("debug print: File appended."));
    debug(F("\n"));
  } else {
    debug(F("debug print: Append failed!"));
    debug(F("\n"));
  }
  myFile.close();
}

double aveADC(int *arr, int number) {
  int i;
  int max, min;
  double avg;
  long amount = 0;

  if (number < 5) {
    for (i = 0; i < number; i++) {
      amount += arr[i];
    }
    avg = amount / number;
    return avg;
  } else {
    if (arr[0] < arr[1]) {
      min = arr[0];
      max = arr[1];
    } else {
      min = arr[1];
      max = arr[0];
    }
    for (i = 2; i < number; i++) {
      if (arr[i] < min) {
        amount += min;
        min = arr[i];
      } else {
        if (arr[i] > max) {
          amount += max;
          max = arr[i];
        } else {
          amount += arr[i];
        }
      }
    }
    avg = (double)amount / (number - 2);
  }
  return avg;
}

void EnviSense::initDepth(const int adc_pin) {
  for (int i = 0; i <= BUFF_LENGTH; i++) {
    ADC_ARRAY[ARRAY_INDEX++] = analogRead(adc_pin);
    delay(10);
  }
}

float EnviSense::getDepth(const int adc_pin, const float max_depth_m, const float offset_depth_m) {
  // buff array readings
  ADC_ARRAY[ARRAY_INDEX++] = analogRead(adc_pin);

  // check array index, then reset array index to zero
  if (ARRAY_INDEX == BUFF_LENGTH) ARRAY_INDEX = 0;

  // calculate average ADC value and convert it to milli-volts
  this->milliVolts = aveADC(ADC_ARRAY, BUFF_LENGTH) * 5000.00 / 1023.00;

  // calculate average current from ohms law
  this->milliAmps = this->milliVolts / 220.0;

  // check if current difference if exceed to min and max current value (threshold)
  this->delta_milliAmps = this->milliAmps - 4.0;
  if (delta_milliAmps < 0) {
    delta_milliAmps = 0.0;
  } else if (delta_milliAmps > 20.0) {
    delta_milliAmps = 20.0;
  }

  // calculate water depth in meter from current reading (4mA - 20mA)
  this->depth_m = (max_depth_m / 16.0) * delta_milliAmps + offset_depth_m;

  // return the calculated depth value in meter
  return this->depth_m;
}

float EnviSense::getSWL(float depth_m, const float dtg_m) {
  // calculate static water level (SWL)
  this->swl_m = float(dtg_m - this->depth_m);

  // check if water depth exceed dtg value, then swl equals to zero
  if (this->depth_m >= dtg_m) {
    this->swl_m = 0.0;
  }

  // return the SWL value in meter
  return this->swl_m;
}

void EnviSense::initGravitypH() {
  EEPROM_read(PHVALUEADDR, this->_neutralVoltage);  // load the neutral (pH = 7.0) voltage of the pH board from the EEPROM

  if (EEPROM.read(PHVALUEADDR) == 0xFF && EEPROM.read(PHVALUEADDR + 1) == 0xFF && EEPROM.read(PHVALUEADDR + 2) == 0xFF && EEPROM.read(PHVALUEADDR + 3) == 0xFF) {
    this->_neutralVoltage = 1500.0;
    EEPROM_write(PHVALUEADDR, this->_neutralVoltage);  // new EEPROM, write typical voltage
  }

  EEPROM_read(PHVALUEADDR + 4, this->_acidVoltage);  // load the acid (pH = 4.0) voltage of the pH board from the EEPROM

  if (EEPROM.read(PHVALUEADDR + 4) == 0xFF && EEPROM.read(PHVALUEADDR + 5) == 0xFF && EEPROM.read(PHVALUEADDR + 6) == 0xFF && EEPROM.read(PHVALUEADDR + 7) == 0xFF) {
    this->_acidVoltage = 2032.44;
    EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);  // new EEPROM, write typical voltage
  }
}

float EnviSense::getGravitypH(float voltage, float temperature) {
  // based on two point calibration (neutral voltage, 7.0), (acid voltage, 4.0) --> pH = m*x + b
  float slope = (7.0 - 4.0) / ((this->_neutralVoltage - 1500.0) / 3.0 - (this->_acidVoltage - 1500.0) / 3.0);
  float intercept = 7.0 - slope * (this->_neutralVoltage - 1500.0) / 3.0;
  this->_phValue = slope * (voltage - 1500.0) / 3.0 + intercept;
  return _phValue;
}

void EnviSense::calibrateGravitypH(float voltage, float temperature, const char *cmd) {
  this->_voltage = voltage;
  this->_temperature = temperature;
  String sCmd = String(cmd);
  sCmd.toUpperCase();
  phCalibration(cmdParse(sCmd.c_str()));
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

void EnviSense::phCalibration(byte mode) {
  char *receivedBufferPtr;
  static boolean phCalibrationFinish = 0;
  static boolean enterCalibrationFlag = 0;

  switch (mode) {
    case 0:  // wrong calibration code
      if (enterCalibrationFlag) {
        Serial.println(F("debug print: Command Error"));
      }
      break;
    case 1:  // code 1 -> enter calibration
      enterCalibrationFlag = 1;
      phCalibrationFinish = 0;
      Serial.println();
      Serial.println(F("debug print: Enter PH Calibration Mode"));
      Serial.println(F("debug print: Please put the probe into the 4.0 or 7.0 standard buffer solution "));
      Serial.println();
      break;
    case 2:  // code 2 -> calibration pH to reference 4.0 and 7.0
      if (enterCalibrationFlag) {
        if ((this->_voltage > 1322) && (this->_voltage < 1678)) {  // buffer solution: 7.0
          Serial.print(F("\n"));
          Serial.print(F("verbose: pH-mid cal OK."));
          Serial.print(F("\n"));
          this->_neutralVoltage = this->_voltage;
          phCalibrationFinish = 1;
        } else if ((this->_voltage > 1854) && (this->_voltage < 2210)) {  //buffer solution: 4.0
          Serial.print(F("\n"));
          Serial.print(F("verbose: pH-low cal OK."));
          Serial.print(F("\n"));
          this->_acidVoltage = this->_voltage;
          phCalibrationFinish = 1;
        } else {
          Serial.println();
          Serial.print(F("verbose: buffer solution error! Please try again. "));
          Serial.println();  // not buffer solution or faulty operation
          phCalibrationFinish = 0;
        }
      }
      break;
    case 3:  // code 3 -> save calibration settings
      if (enterCalibrationFlag) {
        Serial.println();
        if (phCalibrationFinish) {
          if ((this->_voltage > 1322) && (this->_voltage < 1678)) {
            EEPROM_write(PHVALUEADDR, this->_neutralVoltage);
          } else if ((this->_voltage > 1854) && (this->_voltage < 2210)) {
            EEPROM_write(PHVALUEADDR + 4, this->_acidVoltage);
          }
          Serial.print(F("debug print: Calibration Successful"));
        } else {
          Serial.print(F("debug print: Calibration Failed"));
        }
        Serial.println(F(", exit pH calibration Mode "));
        Serial.println();
        phCalibrationFinish = 0;
        enterCalibrationFlag = 0;
      }
      break;
  }
}


