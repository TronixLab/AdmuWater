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
* **Parameters:** *address*: decimal or hex I2C address of the EZO sensor board. *cmd*: Atlas EZO sensor board command for I2C protocol. *time*: the required delay in milli-second to send the command.
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
ph_val = PhiGO.getPH();
```

## getORP()
* **Description:** get the *oxidation or reduction potential* readings from Scientific Atlas sensor probe.
* **Parameters:** none.
* **Returns:** float data type ranges from -1019.9mV − 1019.9mV, with an accuracy of +/– 1mV.
* **Example Code**:
``` 
orp_val = PhiGO.getORP();
```

## getDO()
* **Description:** get the *disolve oxygen* readings from Scientific Atlas sensor probe. This also enable readings percent saturation.
* **Parameters:** none.
* **Returns:** float data type ranges from 0.01 − 100+ mg/L, with an accuracy of +/– 0.05 mg/L.
* **Example Code**:
``` 
do_val = PhiGO.getDO();
```

## getSAT()
* **Description:** get the *percent saturation* from Scientific Atlas sensor probe. The readings derived from getDO() function.
* **Parameters:** none.
* **Returns:** float data type ranges from 0.1 − 400+ % saturation.
* **Example Code**:
``` 
sat_val = PhiGO.getSAT();
```

## getRTD()
* **Description:** get the *temperature* readings in degrees celcius from Scientific Atlas sensor probe.
* **Parameters:** none.
* **Returns:** float data type ranges from -126.000 °C − 1254 °C, with a resolution of 0.001 and accuracy of +/– (0.1 + 0.0017 x °C).
* **Example Code**:
``` 
temp_val = PhiGO.getRTD();
```

## getEC()
* **Description:** get the *electrical conductivity* readings from Scientific Atlas sensor probe. This also enable readings for total disolve solids, salinity, and specific gravity.
* **Parameters:** none.
* **Returns:** long data type ranges from 0.07 − 500,000+ μS/cm, with an accuracy of +/– 2%.
* **Example Code**:
``` 
ec_val = PhiGO.getEC();
```

## getTDS()
* **Description:** get the *total disolve solids* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** long data type, the data range, resolution, and accuracy are not stated in the datasheet.
* **Example Code**:
``` 
tds_val = PhiGO.getTDS();
```

## getSAL()
* **Description:** get the *salinity* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** float data type ranges from PSU (ppt) 0.00 – 42.00.
* **Example Code**:
``` 
sal_val = PhiGO.getSAL();
```

## getSG()
* **Description:** get the *specific gravity* readings from Scientific Atlas sensor probe. The readings derived from getEC() function.
* **Parameters:** none.
* **Returns:** float data type ranges from 1.00 – 1.300 (sea water only).
* **Example Code**:
``` 
sg_val = PhiGO.getSG();
```
