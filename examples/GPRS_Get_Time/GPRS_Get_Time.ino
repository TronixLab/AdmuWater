#include "AdmuWater.h"

AdmuWater PhiGO;

String apn = "internet";               // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                     // APN-Username, not set by default
String apn_p = "";                     // APN-Password, not set by default

void setup() {
  Serial.begin(115200);

  PhiGO.gprsInit(apn, apn_u, apn_p);
}

void loop() {
  String timestamp = PhiGO.gprsGetLocalTime();
  if (!timestamp.equals("")) Serial.println("GPRS time stamp: " + timestamp);
}
