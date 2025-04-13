#include <WiFi.h>
#include <ESP32Servo.h>
#include "SinricPro.h"
#include "SinricProSwitch.h"

#include "password.h"

#define BAUD_RATE         115200             
#define BUTTON_PIN        34
#define SERVO_PIN         12
#define LIGHT_OPEN_PIN         14
#define ALWAYS_ON_PIN         26

#define BUTTON_PRESS_MIN_DURATION 200  // tempo mínimo em ms para considerar botão pressionado
  
bool myPowerState = false;
unsigned long lastBtnPress = 0;
Servo servo1;

unsigned long buttonPressedSince = 0;
bool buttonPreviouslyPressed = false;

void action(){
  digitalWrite(LIGHT_OPEN_PIN, myPowerState?HIGH:LOW);

  int startAngle = myPowerState ? 120 : 0;
  int endAngle = myPowerState ? 0 : 120;

  int step = (startAngle < endAngle) ? 1 : -1;
  int totalSteps = abs(endAngle - startAngle);
  int delayPerStep = 2000 / totalSteps; 

  for (int angle = startAngle; angle != endAngle; angle += step) {
    servo1.write(angle);
    delay(delayPerStep);
  }

  servo1.write(endAngle); // Garante o ângulo final exato
  
}

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
  action();

  return true; 
}

void handleButtonPress() {
  unsigned long actualMillis = millis();
  int val = analogRead(BUTTON_PIN); 

  bool buttonPressed = (val == 4095); 
  if (buttonPressed) {
    if (!buttonPreviouslyPressed) {
      // começou a pressionar agora
      buttonPressedSince = actualMillis;
      buttonPreviouslyPressed = true;
    }

    // Se estiver pressionado há tempo suficiente
    if ((actualMillis - buttonPressedSince >= BUTTON_PRESS_MIN_DURATION) && 
        (actualMillis - lastBtnPress > 1000)) {

      myPowerState = !myPowerState; // inverte estado

      if (!SinricPro.isConnected()) {
        Serial.println("Not connected to Sinric Pro...!");
        return;
      }

      SinricProSwitch& mySwitch = SinricPro[SWITCH_ID];
      mySwitch.sendPowerStateEvent(myPowerState);
      Serial.printf("Device %s turned %s (manually via flashbutton)\r\n",
                    mySwitch.getDeviceId().c_str(),
                    myPowerState ? "on" : "off");

      lastBtnPress = actualMillis;
      action();      
    }

  } else {
    // botão solto
    buttonPreviouslyPressed = false;
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
  pinMode(LIGHT_OPEN_PIN,OUTPUT);
  pinMode(ALWAYS_ON_PIN,OUTPUT);

  digitalWrite(ALWAYS_ON_PIN,HIGH);
  
  servo1.setPeriodHertz(50);
  servo1.attach(SERVO_PIN);    

  Serial.begin(BAUD_RATE); Serial.printf("\r\n\r\n");
  setupWiFi();
  setupSinricPro();
  action();
}

void loop() {
  handleButtonPress();  
  SinricPro.handle(); 
}

