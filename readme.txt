Cant get the d1 Mini to successfully communicate with the server at the OIT building, ask kelly for opinions. However I can run the same code with updating the ip address at school and it works.

<b>STEPS:</b>

Install Arduino IDE

Install esp8266 Board Manager
- Use “NodeMCU 1.0 (ESP-12E Module)” to program the board

Install Libraries:
- AutoConnect
- PageBuilder
- ArduinoJson
- QList
- Adafruit ADS1x15

Add to AutoConnect.h
    uint16 getInterval(); //.............gets value for Arduino Code
    uint16 getADCFrq(); //.............gets value for Arduino Code
    void  saveInput(uint16 postInterval, uint16 adcFrq); //.............saving values recieved from client/trailerServer

Add functions to AutoConnect.cpp
    /**
     *  gets interval for Arduino Code //.............................
     */
    uint16 AutoConnect::getInterval() {
        AutoConnectCredential credential(_apConfig.boundaryOffset);
        for (uint8_t i = 0; i < 1; i++) {
          credential.load(i, &_credential);
        }

        uint16 interval = atoi(reinterpret_cast<const char*>(_credential.interval));

        return interval;
    }

    /**
     *  gets adcFrq for Arduino Code //...............................
     */
    uint16 AutoConnect::getADCFrq() {
      AutoConnectCredential credential(_apConfig.boundaryOffset);
      for (uint8_t i = 0; i < 1; i++) {
        credential.load(i, &_credential);
      }

      uint16 adcFrq = atoi(reinterpret_cast<const char*>(_credential.adcFrq));

      return adcFrq;
    }

    /**
     *  saves recieved values from trailerServer/client //......................................
     */
    void AutoConnect::saveInput(uint16 postInterval, uint16 adcFrq){
      AutoConnectCredential credential(_apConfig.boundaryOffset); //creates credential object
      //AutoConnectCredential credit(_apConfig.boundaryOffset); //creates credit object
      for (uint8_t i = 0; i < 1; i++) {
        credential.load(i, &_credential); // loads credential
      }

      char intervalChar[10];
      char frqChar[10];

      // convert 123 to string [buf]
      itoa(postInterval, intervalChar, 10);
      itoa(adcFrq, frqChar, 10);

      strncpy(reinterpret_cast<char*>(_credential.interval), (char*)intervalChar, sizeof(_credential.interval)); //........... store             postInterval in object
      strncpy(reinterpret_cast<char*>(_credential.adcFrq), (char*)frqChar, sizeof(_credential.adcFrq)); //........... store adcFrq in           object

      credential.save(&_credential); // saves the updated credential
    }


Then search “//………” in old AutoConnect.cpp to find all personal code that must be ported over to new AutoConnect.cpp

Add to User_interface.h
    Go to Preferences in the Arduino IDE and at the very bottom a path is given to directly edit the preferences.txt. Go to that path then     do \packages\esp8266\hardware\esp8266\2.5.0-beta1\tools\sdk\include\user_interface.h to add:

    ------------------------------------
    struct station_config {
        ....
        uint8 serverName[64];
        uint8 sensorName[64];
        uint8 interval[64];
        uint8 adcFrq[64];
        ....
    };
    --------------------------------------
