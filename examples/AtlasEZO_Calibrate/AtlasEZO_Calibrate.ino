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
  Serial.println("calibrating PH...");
  PhiGO.AtlasEZO(EZOPH_ADR, "Cal,low,4", 900);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("pH low-point calibration OK.\n\n"));
  }
  PhiGO.AtlasEZO(EZOPH_ADR, "Cal,mid,7", 900);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("pH mid-point calibration OK.\n\n"));
  }
  PhiGO.AtlasEZO(EZOPH_ADR, "Cal,high,10", 900);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("pH high-point calibration OK.\n\n"));
  }
  PhiGO.AtlasEZO(EZOPH_ADR, "Cal,clear", 300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("pH clear calibration data OK.\n\n"));
  }

  Serial.println("calibrating DO...");
  PhiGO.AtlasEZO(EZODO_ADR, "Cal", 1300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("DO atmospheric oxygen level calibration OK.\n\n"));
  }

  PhiGO.AtlasEZO(EZODO_ADR, "Cal,0", 1300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("DO zero dissolved oxygen calibration OK.\n\n"));

    // This is necessary, add this line to maintain percent saturation (SAT) reading
    PhiGO.AtlasEZO(EZODO_ADR, "O,%,1", 300);
  }

  PhiGO.AtlasEZO(EZODO_ADR, "Cal,clear", 300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("DO clear calibration data OK.\n\n"));

    // This is necessary, add this line to maintain percent saturation (SAT) reading
    PhiGO.AtlasEZO(EZODO_ADR, "O,%,1", 300);
  }

  Serial.println("calibrating RTD...");
  PhiGO.AtlasEZO(EZORTD_ADR, "Cal,100", 600);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("RTD boiling-point calibration OK.\n\n"));
  }

  PhiGO.AtlasEZO(EZORTD_ADR, "Cal,clear", 300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("RTD clear calibration data OK.\n\n"));
  }

  Serial.println("calibrating EC...");
  PhiGO.AtlasEZO(EZOEC_ADR, "Cal,low,12880", 600);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("EC low-point calibration OK.\n\n"));
  }

  PhiGO.AtlasEZO(EZOEC_ADR, "Cal,high,80000", 900);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("EC high-point calibration OK.\n\n"));
  }

  PhiGO.AtlasEZO(EZOEC_ADR, "Cal,dry", 600);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("EC dry calibration OK.\n\n"));
  }

  PhiGO.AtlasEZO(EZOEC_ADR, "Cal,clear", 300);
  if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
    Serial.print(F("EC clear calibration data OK.\n\n"));
  }
}
