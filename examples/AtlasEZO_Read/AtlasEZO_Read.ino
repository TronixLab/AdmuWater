#include <AdmuWater.h>

#define EZODO_ADR  97                               // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98                               // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99                               // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100                              // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102                              // default I2C address number for EZO RTD circuit board

AdmuWater PhiGO;

void setup() {
  Serial.begin(115200);

  PhiGO.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
}

void loop() {
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
