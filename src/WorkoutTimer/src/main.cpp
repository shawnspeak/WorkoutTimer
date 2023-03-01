#include <FS.h>                   //this needs to be first, or it all crashes and burns...

#include <ESP8266WiFi.h>          //https://github.com/esp8266/Arduino
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <WiFiManager.h>          //https://github.com/tzapu/WiFiManager

#include <WorkoutTimer.h>
#include <TimerDisplay.h>

// Workout timer logic and frame to exchange timer state
WorkoutTimer workoutTimer;
TimerFrame timerFrame;

// Timer display
TimerDisplay timerDisplay;

void setup() {
  // start serial
  Serial.begin(115200);
  Serial.println();

  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //reset settings - for testing
  //wifiManager.resetSettings();

  // tries to connect to last known settings
  // if it does not connect it starts an access point with the specified name
  // here  "AutoConnectAP" with password "password"
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("AutoConnectAP", "asdf1234")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }

  // if you get here you have connected to the WiFi
  Serial.println("WiFi connected. Ip address");
  Serial.println(WiFi.localIP());

  // initialize timer
  workoutTimer.init(millis(), timerFrame);

  // initialize display
  timerDisplay.init();
}

void loop() {
  // deal with command inputs

  // advance the timer
  workoutTimer.advance(millis(), timerFrame);

  // update the display
  timerDisplay.update(timerFrame);
}