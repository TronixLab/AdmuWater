#include <AdmuWater.h>

#define EZODO_ADR  97       // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98       // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99       // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100      // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102      // default I2C address number for EZO RTD circuit board

AdmuWater PhiGO;

void setup() {
  Serial.begin(115200);
}

void loop() {
  PhiGO.scan(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
  delay(3000);
}
