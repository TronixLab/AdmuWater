#include <AdmuWater.h>

#define EZODO_ADR  97                               // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98                               // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99                               // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100                              // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102                              // default I2C address number for EZO RTD circuit board

#define SWL_ADC_PIN 34
#define ADC_ZERO_DEPTH 173
#define DEPTH_SENSOR_SLOPE 38.666667F
#define DTG 18.00F

AdmuWater PhiGO;

void setup() {
  Serial.begin(115200);

  PhiGO.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);

  PhiGO.writeData("/data.txt", "timestamp, Battery level, Charge status, pH, DO, SAT, TempC, EC, TDS, SAL, SG, ORP, Depth, SWL \r\n");
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

  float batt_volt_val, batt_volt_percent_val, depth_val, swl_val;
  String batt_status;

  // read supply battery voltage
  batt_volt_val = PhiGO.getBattVolts();

  // read supply battery voltage percentage
  batt_volt_percent_val = PhiGO.getBattPercent();

  // read supply battery charge status
  batt_status = PhiGO.getBattStatus();

  // read water depth value
  depth_val = PhiGO.getDepth(ADC_ZERO_DEPTH, DEPTH_SENSOR_SLOPE);

  // read static water level
  swl_val = PhiGO.getSWL(DTG);

  // print sensor readings
  Serial.print(F("\n"));
  Serial.print(F("Battery level:"));
  Serial.print(batt_volt_val);
  Serial.print(F("mV, "));
  Serial.print(F("Battery percentage:"));
  Serial.print(batt_volt_percent_val);
  Serial.print(F("%, "));
  Serial.print(F("Battery charge status:"));
  Serial.print(batt_status);
  Serial.print(F(", "));
  Serial.print(F("Depth:"));
  Serial.print(depth_val);
  Serial.print(F("m, "));
  Serial.print(F("SWL:"));
  Serial.print(swl_val);
  Serial.print(F("m"));
  Serial.print(F("\n"));

  Serial.println("Saving data...");
  String timestamp = "11/16/22,10:19:57";

  const int BUFF_SIZE = 256;
  char data[BUFF_SIZE];

  snprintf(data, BUFF_SIZE, "%s, %.2f, %s, %.2f, %.2f, %.2f, %.2f, %u, %u, %.2f, %.2f, %.2f, %.2f, %.2f \r\n", timestamp, batt_volt_val, batt_status, ph_val, do_val, sat_val, temp_val, ec_val, tds_val, sal_val, sg_val, orp_val, depth_val, swl_val);
  Serial.println(data);

  PhiGO.appendData("/data.txt", data);
  delay(1000);
}
