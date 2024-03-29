#include "LittleFS.h"

#include <WorkoutTimer.h>
#include <TimerDisplay.h>
#include <RemoteWebServer.h>
#include <WiFiManager.h>
#include <WebsocketServer.h>

// Workout timer logic and frame to exchange timer state
WorkoutTimer workoutTimer;
TimerFrame timerFrame;

// Timer display
TimerDisplay timerDisplay;

// Webserver to handle http and websocket requests
AsyncWebServer server(80);

// wifi manager
WiFiManager wifiManager(&server);

// Websocker Server to deal with timer commands
WebsocketServer socketServer(&server);

// Web server to host remote control
RemoteWebServer* remoteServer = new RemoteWebServer(&server, &LittleFS);

void setup() {
  // start serial
  Serial.begin(115200);
  Serial.println("Hello");

  // init filesystem
  if (!LittleFS.begin()) {
      Serial.println("An error has occurred while mounting LittleFS");
  }
  else
  {
      Serial.println("LittleFS mounted successfully");
  }

  if (wifiManager.init()) { // will restart if needed, otherwise will be connected
    // if you get here you have connected to the WiFi
    Serial.println("WiFi connected. Ip address");
    Serial.println(WiFi.localIP());

    // initialize timer
    workoutTimer.init(millis(), timerFrame);

    // initialize display
    timerDisplay.init();

    // init the remote and socket server
    socketServer.init(workoutTimer, timerFrame);
    remoteServer->init();

    // start the webserver
    server.begin();
  }
}

void loop() {
  if (wifiManager.connected) {
    // advance the timer
    workoutTimer.advance(millis(), timerFrame);

    // update the display
    timerDisplay.update(timerFrame);

    // handle socket server updates
    socketServer.update();
  } else {
    wifiManager.update();
  }
}