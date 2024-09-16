#include <WiFi.h>
#include <ESP32Servo.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include "password.h"

#define BAUD_RATE         115200             
#define BUTTON_PIN        34
#define RELAY_PIN         13
  
bool myPowerState = false;
unsigned long lastBtnPress = 0;
Servo servo1;

/* bool onPowerState(String deviceId, bool &state) 
 *
 * Callback for setPowerState request
 * parameters
 *  String deviceId (r)
 *    contains deviceId (useful if this callback used by multiple devices)
 *  bool &state (r/w)
 *    contains the requested state (true:on / false:off)
 *    must return the new state
 * 
 * return
 *  true if request should be marked as handled correctly / false if not
 */
bool onPowerState(const String &deviceId, bool &state) {
  Serial.printf("Device %s turned %s (via SinricPro) \r\n", deviceId.c_str(), state?"on":"off");
  myPowerState = state;

  //digitalWrite(RELAY_PIN, myPowerState?LOW:HIGH);
  servo1.write(myPowerState?0:90);

  return true; 
}

void handleButtonPress() {
  unsigned long actualMillis = millis(); 
  
  if (digitalRead(BUTTON_PIN) == HIGH  && actualMillis - lastBtnPress > 1000)  {   
    if (myPowerState) {     // flip myPowerState: if it was true, set it to false, vice versa
      myPowerState = false;
    } else {
      myPowerState = true;
    }
    //digitalWrite(RELAY_PIN, myPowerState?LOW:HIGH); // if myPowerState indicates device turned on: turn on led (builtin led uses inverted logic: LOW = LED ON / HIGH = LED OFF)
    servo1.write(myPowerState?0:90);

    if (SinricPro.isConnected() == false) {
      Serial.printf("Not connected to Sinric Pro...!\r\n");
      return; 
    }

    // get Switch device back
    SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
    // send powerstate event
    mySwitch.sendPowerStateEvent(myPowerState); // send the new powerState to SinricPro server
    Serial.printf("Device %s turned %s (manually via flashbutton)\r\n", mySwitch.getDeviceId().c_str(), myPowerState?"on":"off");

    lastBtnPress = actualMillis;  // update last button press variable
  } 
}

// setup function for WiFi connection
void setupWiFi() {
  Serial.printf("\r\n[Wifi]: Connecting");

    WiFi.setSleep(false); 
    WiFi.setAutoReconnect(true);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.printf(".");
    delay(250);
  }
  Serial.printf("connected!\r\n[WiFi]: IP-Address is %s\r\n", WiFi.localIP().toString().c_str());
}

// setup function for SinricPro
void setupSinricPro() {
  // add device to SinricPro
  SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];

  // set callback function to device
  mySwitch.onPowerState(onPowerState);

  // setup SinricPro
  SinricPro.onConnected([](){ Serial.printf("Connected to SinricPro\r\n"); }); 
  SinricPro.onDisconnected([](){ Serial.printf("Disconnected from SinricPro\r\n"); });
  
  SinricPro.begin(APP_KEY, APP_SECRET);
}

// main setup function
void setup() {
  
  pinMode(BUTTON_PIN, INPUT);  
  
  servo1.attach(RELAY_PIN);    

  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
}

void loop() {
  handleButtonPress();
  SinricPro.handle();
}

