STEPS:

Download Battery_Monitor folder and place in your Arduino folder along side other projects and the Libraries folder

Add the supplied Battery_Sensor libraries into your Arduino Libraries folder

Go to Preferences in the Arduino IDE and at the very bottom a path is given to directly edit the preferences.txt. Go to that path then do \packages\esp8266\hardware\esp8266\2.5.0-beta1\tools\sdk\include\user_interface.h to alter "struct station_config" to look like:

------------------------------------
struct station_config {
    uint8 ssid[32];
    uint8 password[64];
    uint8 serverName[64];
    uint8 sensorName[64];
    uint8 interval[64];
    uint8 adcFrq[64];
    uint8 bssid_set;    // Note: If bssid_set is 1, station will just connect to the router
                        // with both ssid[] and bssid[] matched. Please check about this.
    uint8 bssid[6];
    wifi_fast_scan_threshold_t threshold;
    bool open_and_wep_mode_disable; // Can connect to open/wep router by default.
};
--------------------------------------

use the provided user_interface.h for reference.
