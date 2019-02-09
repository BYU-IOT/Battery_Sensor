#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <AutoConnect.h>
#include <ArduinoJson.h>
#include <ESP8266HTTPClient.h>
#include <QList.h>
#include <Wire.h>
#include <Adafruit_ADS1015.h>  // probably need to download
#include <stdio.h>
#include <stdlib.h>

QList<float> dataQueue;

float current = 0; // current and dedicated queue
int postInterval = 500; // 500ms or 1/2 sec is a good default
int adcFrq = 10; // 10 ms is a good default
//int sleepTime; // in seconds, this is computed based on interval and adcFrq

// these values are used for threading the different processes
unsigned long currentMillis = 0;
unsigned long previousClientMillis = 0;
unsigned long previousDataMillis = 0;

Adafruit_ADS1115 ads(0x48); // inits the adc

ESP8266WebServer Server; // inits the server
AutoConnect Portal(Server);

HTTPClient http; //Declare object of class HTTPClient

// put on the web page, not needed in deployment
void rootPage() {
  char content[] = "RV trailer battery moniter running";
  Server.send(200, "text/plain", content);
}

void setup() {
  delay(1000);
  Serial.begin(115200);
  Serial.println();
  ads.begin(); // starts adc

  Server.on("/", rootPage);
  if (Portal.begin()) {
    Serial.println("HTTP server:" + WiFi.localIP().toString());

    // retrieves values inserted in memory during login process
    postInterval = Portal.getInterval();
    //Serial.println("stored postInterval: " + String(postInterval));
    adcFrq = Portal.getADCFrq();
    //Serial.println("stored adcFrq: " + String(adcFrq));
    
    // optimizes sleep time for usless posts
    if(postInterval < adcFrq){
      postInterval = adcFrq;
    }

    // updateSleepTime(); // calculate sleepTime besed on recieved values
  }
}

void loop() {
    currentMillis = millis();         // capture the latest value of millis() this is
                                      // equivalent to noting the time from a clock use the
                                      // same time for all LED flashes to keep them synchronized
    theClientTick();
    collectDataTick();
}

void theClientTick(){
      if(currentMillis - previousClientMillis >= postInterval){// wait for interval to expire
        postData();                                        // before collecting data
        getCommands();
        
        previousClientMillis += postInterval;
    }
}

void collectDataTick()
{
    if(currentMillis - previousDataMillis >= adcFrq){// wait for interval to expire
        collectData();
        
        previousDataMillis += adcFrq;
    }
}

void collectData()
{
//  int16_t adc0;  // we read from the ADC, we have a sixteen bit integer as a result
//
//  adc0 = ads.readADC_SingleEnded(0);
//  current = (adc0 * 0.1875)/1000; // ????????????
//  
//  Serial.print("AIN0: "); 
//  Serial.print(adc0);
//  Serial.print("\tCurrent: ");
//  Serial.println(current, 7);  
//  Serial.println();
//  delay(1000);
  
  current = random(0, 100);  // we read from the ADC, we have a sixteen bit integer as a result
  dataQueue.push_front(current);
  Serial.println("read sensor");
}

void postData(){
    StaticJsonBuffer<300> JSONbuffer; //You can use ArduinoJson Assistant to compute the exact buffer size
    JsonObject& JSONencoder = JSONbuffer.createObject();
    JSONencoder["sensorType"] = "Current";
    JsonArray& values = JSONencoder.createNestedArray("readings"); //JSON array

    //remove values from queue to Json array to be sent
    while(dataQueue.length() > 0){
        values.add(dataQueue.front());
        dataQueue.pop_front();
    }

    char JSONmessageBuffer[300];
    JSONencoder.prettyPrintTo(JSONmessageBuffer, sizeof(JSONmessageBuffer));

    http.begin("http://192.168.1.205:8000/sendCurrent"); //HTTP
    http.addHeader("Content-Type", "application/json");  //Specify content-type header

    // start connection and send HTTP header
    int httpCode = http.POST(JSONmessageBuffer);     //Send the request
    String payload = http.getString();                  //Get the response payload

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] POST... code: %d\n", httpCode);
      Serial.println(payload);   //Print request response payload
      
    } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void getCommands(){
    http.begin("http://192.168.1.205:8000/getCurrentCommands"); //HTTP
    int httpCode = http.GET(); // start connection and send HTTP header

    // httpCode will be negative on error
    if (httpCode > 0) {
      // HTTP header has been sent and Server response header has been handled
      Serial.printf("[HTTP] GET... code: %d\n", httpCode);

      if (httpCode == HTTP_CODE_OK) { // file found at server
        // Parsing
        const size_t bufferSize = JSON_ARRAY_SIZE(3) + 4*JSON_OBJECT_SIZE(2) + 125;   
        DynamicJsonBuffer jsonBuffer(bufferSize);
        JsonObject& root = jsonBuffer.parseObject(http.getString());
        JsonArray& commands = root["commands"];

         for(int i=0; i<commands.size(); i++){
            const char* command_action = commands[i]["action"];
            int command_value = commands[i]["value"]; // string to convert to int later if needed
            
            interpretCommands(command_action, command_value); //send command to function to be implemented
        }
      }
    } else {
        Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
}

void interpretCommands(const char* command_action, int command_value){
    if(strcmp(command_action, "Update postInterval") == 0){
      postInterval = command_value;
      Serial.println("saving  interval to EEPROM: " + String(postInterval));
    }else if(strcmp(command_action, "Update frequency") == 0){
      adcFrq = command_value;
      Serial.println("saving adcFrq to EEPROM: " + String(adcFrq));
    }

    Portal.saveInput((uint16) postInterval, (uint16) adcFrq); //store value in memory
    
    //update sleep time accordingly
}

/**
 *  Not currently implemented right now, due to requeired immediate response
 */
//void updateSleepTime(){
//  // adcFrq will always be more frequent than postInterval
//  if (adcFrq >= 60000){ // lowest limit for effective sleep is 1 minute
//    sleepTime = adc/1000;
//  }else{
//    sleepTime = 0; // else it is turned off, ignore sleep() call when below 1 minute time
//  }
//}

/**
 *  Not currently implemented right now, due to requeired immediate response
 */
//void sleep(){
//  ESP.deepSleep(sleepTime*1000000);
//}


//commands being recieved by MCU in json
//{
//  "commands": [
//    { "action":"Update sensorName", "value":"The man" },
//    { "action":"Update postInterval", "value":10 },
//    { "action":"Update ssid", "value":"8.8.8.8" },
//    { "action":"Update frequency", "value":1 }
//  ]
//}
//posting data queue to server and Cloud in json
//{
//  "sensor":"current",
//  "readings": [5, 10, 7, 30, 55, 45.66, 35, 33.4, ...]
//}
//user posting to server in json
//{
//  "sensor":"current",
//  "action":"Update sensorName", 
//  "value": "The man"
//}
