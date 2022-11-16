#include <AdmuWater.h>

/*
  ADC pin mapping:
  ADC1_CH0 >>> GPIO36
  ADC1_CH1 >>> Not available on this board Devkit DoIT ( 30 pins version )
  ADC1_CH2 >>> NA
  ADC1_CH3 >>> GPIO39
  ADC1_CH6 >>> GPIO34
  ADC1_CH7 >>> GPIO35
  ADC1_CH4 >>> GPIO32
  ADC1_CH5 >>> GPIO33

  #define ADC1_CH0 36   // VP
  #define ADC1_CH3 39   // VN
  #define ADC1_CH4 32   // D32
  #define ADC1_CH5 33   // D33
  #define ADC1_CH6 34   // D34
  #define ADC1_CH7 35   // D35

  ADC reading when above water (zero depth constant):
  173, 175, 169, 182, etc.

  Computed slope constant:
  #define DEPTH_SENSOR_SLOPE 133.0          // zdp: 175
  #define DEPTH_SENSOR_SLOPE 32.83          // computed slope for Malabon, zdp: 169
  #define DEPTH_SENSOR_SLOPE 136.17         // computed slope of line (5m)
  #define DEPTH_SENSOR_SLOPE 37.21          // computed slope of line (20m) - Version 1
  #define DEPTH_SENSOR_SLOPE 137.76         // computed slope of line (20m) - Version 2
  #define DEPTH_SENSOR_SLOPE 136.59         // computed slope for beer tower test setup 1
  #define DEPTH_SENSOR_SLOPE 34.27419355    // computed slope of line (10 core) - Version 3 Muntinlupa
  #define DEPTH_SENSOR_SLOPE 35.8974359     // computed slope for Iloilo
  #define DEPTH_SENSOR_SLOPE 37.21          // computed slope for Alabang, zdp: 160
  #define DEPTH_SENSOR_SLOPE 38.666667      // computed slope for Bukidnon

  DTG constant
  #define DTG 8.00                          // distance of depth sensor tip to ground in meters - Malabon
  #define DTG 18.00                         // distance of depth sensor tip to ground in meters - Marikina
  #define DTG 24                            // distance of depth sensor tip to ground in meters - Alabang
  #define DTG 39.67                         // distance of depth sensor tip to ground in meters - Muntinlupa
  #define DTG 0.3                           // Beer Tower
  #define DTG 1.984                         // not-Beer Tower
  #define DTG 6.75                          // Iloilo1 (Brgy. Consolacion)
  #define DTG 6.85                          // Iloilo3 (Pavia Elementary School)
  #define DTG 10.25                         // Iloilo4 (Pavia HS)
*/

#define SWL_ADC_PIN 33
#define ADC_ZERO_DEPTH 173
#define DEPTH_SENSOR_SLOPE 38.666667F
#define DTG 18.00F

AdmuWater PhiGO;

void setup() {
  Serial.begin(115200);
}

void loop() {
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

  delay(3000);
}
