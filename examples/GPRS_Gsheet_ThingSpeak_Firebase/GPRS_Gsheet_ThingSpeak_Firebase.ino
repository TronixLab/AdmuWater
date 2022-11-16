#include "AdmuWater.h"

String GOOGLE_SCRIPT_ID = "AKfycbx7tTKEAYH7EIymHOa0cQefjXvR4fWczm_SaOm5gwWOc6k0VGk5bX6KQ9toE_xgDeBB";
String THINGSPEAK_AUTH  = "Z4OX2LWYJDKAS83F";
String FIREBASE_HOST    = "test-esp32-87a8f-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH    = "HaNKAUG0lgP1GP4otMP59hYViXIDClxwuazJEbIY";
String FIREBASE_PATH    = "/Test-Data";

AdmuWater PhiGO;

String apn = "internet";               // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                     // APN-Username, not set by default
String apn_p = "";                     // APN-Password, not set by default

void setup() {
  Serial.begin(115200);

  PhiGO.gprsInit(apn, apn_u, apn_p);
}

void loop() {
  int sensor = random(0, 100);

  String Firebase_payload = String("{\"Value\":\"") + String(sensor) + "\"}";
  PhiGO.gprsGoogleFirebase(FIREBASE_HOST, FIREBASE_PATH, FIREBASE_AUTH, Firebase_payload);

  String GSheet_payload = String("sensor=ESP32") + "&value=" + String(sensor);
  PhiGO.gprsGoogleSheet(GOOGLE_SCRIPT_ID, GSheet_payload);

  String TS_payload = String("&field1=") + String(sensor);
  PhiGO.gprsThingSpeak(THINGSPEAK_AUTH, TS_payload);
}
