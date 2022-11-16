/*
  AdmuWater.h - Library for monitoring groundwater quality and quantity across the Philippines (https://admuwater.com/)
  Created by Paul Ryan A. Santiago, October 25, 2022.
  Released into the public domain.
*/

#ifndef AdmuWater_h
#define AdmuWater_h

#include "Arduino.h"
#include <stdio.h>

class AdmuWater
{
  public:
    AdmuWater(void);

    void initAtlasEZO(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr);

    void scan(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr);

    void AtlasEZO(uint8_t address, const char* cmd, uint16_t time);

    enum status {SUCCESS, SYNTAX_ERROR, NOT_READY, NO_DATA};
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
    float getDEPTH();

    float getBattVolts();
    float getBattPercent();
    String getBattStatus();

    float getDepth(uint8_t adc_zero_depth, float depth_sensor_slope);
    float getSWL(float dtg_factor);

    void initSDCard();
    void initFile();

    void writeData(const char* path, const char* message);
    void appendData(const char* path, const char* message);

    void gprsInit(String apn, String username, String password);
    void gprsGoogleSheet(String google_script_id, String payload);
    void gprsThingSpeak(String thingspeak_http_auth, String payload);
    void gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload);
    String gprsGetLocalTime();

    float getAveVal(float* array, uint8_t len);

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
    uint8_t battPercent_val = 0.00;
    float swl_val = 0.00;

    String battStatus_val = "";

    float R1 = 30000.0;
    float R2 = 10220.0;

    unsigned long int ec_val = 0.00;
    unsigned long int tds_val = 0.00;

    float water_depth = 0.00;
};

#endif
