#ifndef EnviSense_h
#define EnviSense_h

#include "Arduino.h"
#include <stdio.h>

#define ReceivedBufferLength 10

class EnviSense {
public:
  EnviSense(void);

  void initAtlasEZO(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr);

  void scan(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr);

  void AtlasEZO(uint8_t address, const char* cmd, uint16_t time);

  enum status { SUCCESS,
                SYNTAX_ERROR,
                NOT_READY,
                NO_DATA };
  enum status getStatus();

  float getPH();
  float getORP();
  float getDO();
  float getSAT();
  float getRTD();
  float getEC();
  float getTDS();
  float getSAL();
  float getSG();
  float getSWL();

  float getBattVolts(const int adc_pin);
  float getBattPercent(float battVolts);
  String getBattStatus(float battPercent);

  void initSDCard(const char* path, String header);
  void logData(const char* path, String datafile);

  void gprsInit(String apn, String username, String password);
  void gprsGoogleSheet(String google_script_id, String payload);
  void gprsThingSpeak(String thingspeak_http_auth, String payload);
  void gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload);
  String gprsGetLocalTime();

  void initDepth(const int adc_pin);
  float getDepth(const int adc_pin, const float max_depth_m, const float offset_depth_m);
  float getSWL(float depth_m, const float dtg_m);

  void initGravitypH();
  float getGravitypH(float voltage, float temperature);
  void calibrateGravitypH(float voltage, float temperature, const char* cmd);
  byte cmdParse(const char* cmd);
  void phCalibration(byte mode);

private:
  uint8_t EZODO_ADR;
  uint8_t EZOORP_ADR;
  uint8_t EZOPH_ADR;
  uint8_t EZOEC_ADR;
  uint8_t EZORTD_ADR;

  char ezo_data[32];
  byte in_char = 0;
  byte i = 0;

  enum status ezo_stat;

  float ph_val = 0.00;
  float do_val = 0.00;
  float sat_val = 0.00;
  float orp_val = 0.00;
  float temp_val = 0.00;
  float sal_val = 0.00;
  float sg_val = 0.00;

  float battVolts_val = 0.00;
  float battPercent_val = 0.00;

  String battStatus_val = "";

  float R1 = 30000.0;
  float R2 = 7500.0;

  unsigned long int ec_val = 0.00;
  unsigned long int tds_val = 0.00;

  float delta_milliAmps = 0.0;
  float milliVolts = 0.00;
  float milliAmps = 0.00;
  float depth_cm = 0.00;
  float depth_m = 0.00;
  float swl_m = 0.00;

  float _temperature = 25.0;
  float _phValue = 7.0;
  float _acidVoltage = 2032.44;
  float _neutralVoltage = 1500.0;
  float _voltage = 1500.0;
};

#endif
