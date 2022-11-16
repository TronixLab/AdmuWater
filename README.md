# About AdmuWater
[Admuwater](https://admuwater.com/) is a collection of projects for monitoring ground water quality and quantity across the Philippines. It provides real-time remote monitoring and data visualization dashboard for the following projects, this includes [Ground Water Management Plan](https://admuwater.com/gmp), [Philippine Ground Water Outlook](https://admuwater.com/phigo), and [Water for Tourism](https://admuwater.com/wft)

<details>
<summary>Ground Water Management Plan (GMP)</summary>
The GMP project is funded by National Water Resources Board (NWRB) and DOST- Philippine Council for Industry, Energy, and Emerging Technology Research and Development (PCIEERD). This project is spearheaded by Ateneo de Manila University (ADMU), Departments of Environmental Science (ES) and Electronics, Computer, and Communications Engineering (ECCE) in collaboration with NWRB, DOST-PCIEERD, DOST-PAGASA and Ateneo de Zamboanga University (ADZU). Real time monitoring of water level and quality thru installation of telemetry sensors shall be done in various areas in Zamboanga, Iloilo, Cagayan de Oro, Metro Manila and its surrounding areas. This project is headed by Maria Aileen Leah G. Guzman, Ph.D, Assistant Professor from the Department of Environmental Science.
</details>

<details>
<summary>Philippine Groundwater Outlook (PhiGO)</summary>
The PhiGO is a three-year collaborative project led by Andrew Barkwith, Ph.D. from the British Geological Survey (BGS) and Ma. Aileen Leah G. Guzman, Ph.D. from the Ateneo de Manila University (AdMU). This project is funded by the PH-UK Newton Agham Joint S&T Cooperation Program on Understanding the Impacts of Hydrometeorological Hazards in the Philippines

This project is funded by the Natural Environment Research Council (NERC) for its British constituents and by the Department of Science and Technology-Philippine Council for Industry, Energy, and Emerging Technology Research and Development (DOST-PCIEERD) for its Philippine constituents.
</details>

<details>
<summary>Water for Tourism</summary>
The Water for Tourism is headed by Maria AIleen Leah G. Guzman, PhD, Associate Professor and Department Chairperson of Environmental Science, Ateneo de Manila University. The proposed program aims to create a comprehensive water resource assessment and management plan for the Nabaoy River Watershed and Boracay Island through two projects. The project also targets UN SDG 6 which aims to ensure availability and sustainable management of water and sanitation for all.

This program is in response to Executive Order No. 56 (2018). It will be undertaken with member agencies (NWRB, DOST-PCIEERD, DOST-PNRI, DOST-PAGASA) of the Boracay Inter-Agency Task Force (tasked to reverse the degradation of Boracay Island) in collaboration with both local (Ateneo de Manila University and Aklan State University) and international academic institutions (British Geological Survey and Imperial College London).
</details>

# AdmuWater Library
Arduino library for ESP32 provides simplified functions for AdmuWater projects. The following features are:
* Reading, scanning, and calibrating sensor data from Scientific Atlas EZO sensor board.
* Serial port, Bluetooth and Wi-Fi terminal user command line interface for sensor calibration and network configuration.
* Logging sensor data to micro-SD card.
* Wireless connection via GSM GPRS SIM 800l, and Wi-Fi module for cloud computing.
* Getting local timestamp from GSM SIM 800l module.
* Storing data to Google sheet, [ThingSpeak](https://thingspeak.com/), and [Google Firebase](https://firebase.google.com/).
* Visualization of sensor data at Admuwater remote dashboard.

# AdmuWater API
## AdmuWater(void)
* **Description:** Create an object instance.
* **Example Code**:
``` 
AdmuWater PhiGO;
```

## scan(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr)
* **Description:** scan Atlas EZO sensor board I2C board.
* **Parameters:** ezodo_adr, ezoorp_adr, ezoph_adr, ezoec_adr, and ezortd_adr are the decimal or hex I2C address of the EZO sensor board. 
* **Returns:** print the scanned devices.
* **Example Code**:
``` 
#define EZODO_ADR  97       // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98       // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99       // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100      // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102      // default I2C address number for EZO RTD circuit board
...
Serial.begin(115200);
...
PhiGO.scan(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
```

## initAtlasEZO(uint8_t ezodo_adr, uint8_t ezoorp_adr, uint8_t ezoph_adr, uint8_t ezoec_adr, uint8_t ezortd_adr)
* **Description:** initialize Atlas EZO sensor board for I2C communication.
* **Parameters:** ezodo_adr, ezoorp_adr, ezoph_adr, ezoec_adr, and ezortd_adr are the decimal or hex I2C address of the EZO sensor board. 
* **Returns:** none.
* **Example Code**:
``` 
#define EZODO_ADR  97       // default I2C address number for EZO DO circuit board
#define EZOORP_ADR 98       // default I2C address number for EZO ORP circuit board
#define EZOPH_ADR  99       // default I2C address number for EZO pH circuit board
#define EZOEC_ADR  100      // default I2C address number for EZO EC circuit board
#define EZORTD_ADR 102      // default I2C address number for EZO RTD circuit board
...
PhiGO.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
```

## AtlasEZO(uint8_t address, const char* cmd, uint16_t time)
* **Description:** writes commands to Atlas EZO sensor board.
* **Parameters:** *address* - decimal or hex I2C address of the EZO sensor board. *cmd* - Atlas EZO sensor board command for I2C protocol. *time* - the required delay in milli-second to send the command.
* **Returns:** none.
* **Example Code**:
``` 
#define EZOPH_ADR  99       // default I2C address number for EZO pH circuit board
...
PhiGO.initAtlasEZO(EZODO_ADR, EZOORP_ADR, EZOPH_ADR, EZOEC_ADR, EZORTD_ADR);
...
// read Atlas Scientific EZO-pH sensor board
PhiGO.AtlasEZO(EZOPH_ADR, "R", 815);
```

## getStatus()
* **Description:** receives I2C response after sending command.
* **Parameters:** none.
* **Returns:** *SUCCESS* if command was successful. *SYNTAX_ERROR* if command has failed. *NOT_READY* if command has not yet been finished calculating. *NO_DATA* if there is no further data to send. 
* **Example Code**:
``` 
// read Atlas Scientific EZO-pH sensor board
PhiGO.AtlasEZO(EZOPH_ADR, "R", 815);
if (PhiGO.getStatus() == AdmuWater::SUCCESS) {
  // dome something here...
}
```

## getPH()
* **Description:** get the *pH* readings from Scientific Atlas sensor probe.
* **Parameters:** none.
* **Returns:** float data type ranges from 0.001 − 14.000 with a resolution of 0.001 and accuracy of +/– 0.002.
* **Example Code**:
``` 
float ph_val = PhiGO.getPH();
```

## getORP()
* **Description:** get the *oxidation or reduction potential* readings from Scientific Atlas sensor probe.
* **Parameters:** none.
* **Returns:** float data type ranges from -1019.9mV − 1019.9mV, with an accuracy of +/– 1mV.
* **Example Code**:
``` 
float orp_val = PhiGO.getORP();
```

## getDO()
* **Description:** get the *disolve oxygen* readings from Scientific Atlas sensor probe. This also enable readings percent saturation.
* **Parameters:** none.
* **Returns:** float data type ranges from 0.01 − 100+ mg/L, with an accuracy of +/– 0.05 mg/L.
* **Example Code**:
``` 
float do_val = PhiGO.getDO();
```

## getSAT()
* **Description:** get the *percent saturation* from Scientific Atlas sensor probe. The readings derived from getDO() function.
* **Parameters:** none.
* **Returns:** float data type ranges from 0.1 − 400+ % saturation.
* **Example Code**:
``` 
float sat_val = PhiGO.getSAT();
```

## getRTD()
* **Description:** get the *temperature* readings in degrees celcius from Scientific Atlas sensor probe.
* **Parameters:** none.
* **Returns:** float data type ranges from -126.000 °C − 1254 °C, with a resolution of 0.001 and accuracy of +/– (0.1 + 0.0017 x °C).
* **Example Code**:
``` 
float temp_val = PhiGO.getRTD();
```

## getEC()
* **Description:** get the *electrical conductivity* readings from Scientific Atlas sensor probe. This also enable readings for total disolve solids, salinity, and specific gravity.
* **Parameters:** none.
* **Returns:** long data type ranges from 0.07 − 500,000+ μS/cm, with an accuracy of +/– 2%.
* **Example Code**:
``` 
unsigned long int ec_val = PhiGO.getEC();
```

## getTDS()
* **Description:** get the *total disolve solids* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** long data type, the data range, resolution, and accuracy are not stated in the datasheet.
* **Example Code**:
``` 
unsigned long int tds_val = PhiGO.getTDS();
```

## getSAL()
* **Description:** get the *salinity* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** float data type ranges from PSU (ppt) 0.00 – 42.00.
* **Example Code**:
``` 
float sal_val = PhiGO.getSAL();
```

## getSG()
* **Description:** get the *specific gravity* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** float data type ranges from 1.00 – 1.300 (sea water only).
* **Example Code**:
``` 
float sg_val = PhiGO.getSG();
```

## getBattVolts()
* **Description:** get the calculated *battery voltage* (13V max) readings from onboard voltage sensor.
* **Parameters:** none.
* **Returns:** float data type ranges from 0 - 13V.
* **Example Code**:
``` 
float batt_volt_val = PhiGO.getBattVolts();
```

## getBattPercent()
* **Description:** get the calculated *battery percent level* readings from onboard voltage sensor.
* **Parameters:** none.
* **Returns:** int data type ranges from 0 - 100% (10.5V - 13V).
* **Example Code**:
``` 
int batt_volt_percent_val = PhiGO.getBattPercent();
```

## getBattStatus()
* **Description:** get the *battery status* readings from onboard voltage sensor.
* **Parameters:** none.
* **Returns:** string data type, *"Fully charge"* if *battery percent level* <= 100% and >= 97%, *"OK"* if *battery percent level* < 97% and >= 75%, *"Low charge"* if *battery percent level* < 75% and >= 25%, and *"discharge"* if *battery percent level* < 25%.
* **Example Code**:
``` 
String batt_status = PhiGO.getBattStatus();
```

## getDEPTH()
* **Description:** get the calculated *water depth* (0 - 30meters proportional to 4 - 20mA) readings from Sendo pressure sensor. The calculation based on ohms law and slope intercept equations.
* **Parameters:** none.
* **Returns:** float data type ranges from 0 - 30m.
* **Example Code**:
``` 
float DEPTH_val = PhiGO.getDEPTH();
```

## getDepth(uint8_t adc_zero_depth, float depth_sensor_slope)
* **Description:** get the calculated *water depth* readings from Sendo pressure sensor. The calculation based on predefined calibration.
* **Parameters:** *adc_zero_depth* - a constant factor for zero reference, and *depth_sensor_slope* - a computed constant factor for linearity.
* **Returns:** float data type, ranges are not defined.
* **Example Code**:
``` 
#define ADC_ZERO_DEPTH 173
#define DEPTH_SENSOR_SLOPE 38.666667F
...
float depth_val = PhiGO.getDepth(ADC_ZERO_DEPTH, DEPTH_SENSOR_SLOPE);
```

## initSDCard()
* **Description:** initialized communication for micro SD card interface. Check if the SD card is properly mounted.
* **Parameters:** none.
* **Returns:** prints SD card status.
* **Example Code**:
``` 
PhiGO.initSDCard();
```

## writeData(const char* path, const char* message)
* **Description:** writes new data to the predefined file path directory on SD card. This will overide the previous data.
* **Parameters:** *path* - is the file directory where the data to be stored, *message* - the data to be write.
* **Returns:** none.
* **Example Code**:
``` 
// writing header labels for the data to be log
PhiGO.writeData("/data.txt", "timestamp, Battery level, Charge status, pH, DO, SAT, TempC, EC, TDS, SAL, SG, ORP, Depth, SWL \r\n");
```

## appendData(const char* path, const char* message)
* **Description:** add new data to the predefined file path directory on SD card.
* **Parameters:** *path* - is the file directory where the data to be stored, *message* - the data to be write.
* **Returns:** none.
* **Example Code**:
``` 
// sensor data to be log
const int BUFF_SIZE = 256;
char data[BUFF_SIZE];
snprintf(data, BUFF_SIZE, "%s, %.2f, %s, %.2f, %.2f, %.2f, %.2f, %u, %u, %.2f, %.2f, %.2f, %.2f, %.2f \r\n", timestamp, batt_volt_val, batt_status, ph_val, do_val, sat_val, temp_val, ec_val, tds_val, sal_val, sg_val, orp_val, depth_val, swl_val);

PhiGO.appendData("/data.txt", data);
```

## gprsInit(String apn, String username, String password)
* **Description:** initialize GPRS connection of GSM SIM 800l module for cellular network data transmission.
* **Parameters:** *apn* - access point name e.g., "internet" or "smartlte" for SMART sim card, *username* - apn username if set, and *password* - apn password if set.
* **Returns:** none.
* **Example Code**:
``` 
String apn = "internet";               // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                     // APN-Username, not set by default
String apn_p = "";                     // APN-Password, not set by default
...
PhiGO.gprsInit(apn, apn_u, apn_p);
```

## gprsGoogleSheet(String google_script_id, String payload)
* **Description:** logs sensor data to Google spreadsheet via GPRS HTTP request.
* **Parameters:** *google_script_id* - Google ID obtain from script file, *payload* - sensor data format to store file to Google spreadsheet.
* **Returns:** none.
* **Example Code**:
``` 
String GOOGLE_SCRIPT_ID = "AKfycbx7tTKEAYH7EIymHOa0cQefjXvR4fWczm_SaOm5gwWOc6k0VGk5bX6KQ9toE_xgDeBB";
...
String GSheet_payload = String("sensor=ESP32") + "&value=" + String(sensor);
PhiGO.gprsGoogleSheet(GOOGLE_SCRIPT_ID, GSheet_payload);
```

## gprsThingSpeak(String thingspeak_http_auth, String payload)
* **Description:** logs sensor data to ThingSpeak IoT platform via GPRS HTTP request.
* **Parameters:** *thingspeak_http_auth* - authentication code obtain from thingspeak settings, *payload* - sensor data format to store file to thingspeak channel.
* **Returns:** none.
* **Example Code**:
``` 
String THINGSPEAK_AUTH  = "Z4OX2LWYJDKAS83F";
...
String TS_payload = String("&field1=") + String(sensor);
PhiGO.gprsThingSpeak(THINGSPEAK_AUTH, TS_payload);
```

## gprsGoogleFirebase(String firebase_host, String firebase_path, String firebase_auth, String payload)
* **Description:** logs sensor data to Google Firebase real-time database via GPRS HTTP request.
* **Parameters:** *firebase_host* - url code obtain firebase project, *firebase_path* - location where the sensor data to be stored, and *payload* - sensor data format should be in Javascript object notation (JSON).
* **Returns:** none.
* **Example Code**:
``` 
String FIREBASE_HOST    = "test-esp32-87a8f-default-rtdb.asia-southeast1.firebasedatabase.app";
String FIREBASE_AUTH    = "HaNKAUG0lgP1GP4otMP59hYViXIDClxwuazJEbIY";
String FIREBASE_PATH    = "/Test-Data";
...
String Firebase_payload = String("{\"Value\":\"") + String(sensor) + "\"}";
PhiGO.gprsGoogleFirebase(FIREBASE_HOST, FIREBASE_PATH, FIREBASE_AUTH, Firebase_payload);
```

## gprsGetLocalTime()
* **Description:** gets the local timestamp obtain from GSM SIM800l module.
* **Parameters:** none.
* **Returns:** parsed string data type of current date and time.
* **Example Code**:
``` 
String apn = "internet";               // APN e.g., Smart: "internet", "smartlte", Globe: "internet.globe.com.ph", "http.globe.com.ph", "mms.globe.com.ph"
String apn_u = "";                     // APN-Username, not set by default
String apn_p = "";                     // APN-Password, not set by default
...
PhiGO.gprsInit(apn, apn_u, apn_p);
...
String timestamp = PhiGO.gprsGetLocalTime();
if (!timestamp.equals("")) Serial.println("GPRS time stamp: " + timestamp);
```

# AdmuWater Command Lines
| Terminal Command | Description |
| ---------------- | ----------- |
| admuwater -help | print command line for reference. |
| admuwater -info | print device/product information. |
| admuwater conf -net gprs | config network type to GPRS. |
| admuwater conf -net wifi | config network type to Wi-Fi. |
| admuwater cal -s ph -p low | pH two point calibration at lowpoint (pH 4). |
| admuwater cal -s ph -p mid | pH single point calibration at midpoint (pH 7). |
| admuwater cal -s ph -p high | pH three point calibration at highpoint (pH 10). |
| admuwater cal -s ph clear | pH delete calibration data. |
| admuwater cal -s do -p atm | DO (Disolve Oxygen) calibrate to atmospheric oxygen levels. |
| admuwater cal -s do -p zero | DO (Disolve Oxygen) calibrate device to 0 dissolved oxygen. |
| admuwater cal -s do clear | DO (Disolve Oxygen) delete calibration data. |
| admuwater cal -s ec -p low | EC (Electrical Conductivity) low end calibration at 12880uS/cm. |
| admuwater cal -s ec -p high | EC (Electrical Conductivity) high end calibration at 80000uS/cm. |
| admuwater cal -s ec -p dry | EC (Electrical Conductivity) dry calibration. |
| admuwater cal -s ec clear | EC (Electrical Conductivity) delete calibration data. |
| admuwater cal -s rtd -p boil | Temperature calibration at boiling point (100°C). |
| admuwater cal -s rtd clear | Temperature delete calibration data. |
