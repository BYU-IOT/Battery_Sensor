# Installation Steps

1. Install Arduino IDE

2. Install esp8266 Board Manager
    - Use “NodeMCU 1.0 (ESP-12E Module)” to program the board

3. Install Libraries:
    - AutoConnect (version 0.9.8)
    - PageBuilder
    - ArduinoJson
    - QList
    - Adafruit ADS1x15

4. Replace User_interface.h file
    Go to Preferences in the Arduino IDE and at the very bottom a path is given to directly edit the preferences.txt. Go to that path then 
    go to \packages\esp8266\hardware\esp8266\?.?.?\tools\sdk\include\user_interface.h and replace it with the user_interface.h file provided.

5. Replace AutoConnect files
    Go to your arduino folder and your local folder cantaining the AutoConnect files and replace them with these 6 files provided:
    - AutoConnect.cpp
    - AutoConnect.h
    - AutoConnectCredential.cpp
    - AutoConnectCredential.h
    - AutoConnectPage.cpp
    - AutoConnectPage.h
    
    
    
    
# Notes
Cant get the d1 Mini to successfully communicate with the server at the OIT building, ask kelly for opinions. However I can run the same code with updating the ip address at school and it works.
