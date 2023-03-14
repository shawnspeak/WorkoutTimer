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

// Websocker Server to deal with timer commands
WebsocketServer socketServer(&server);

// Web server to host remote control
RemoteWebServer remoteServer(&server);

void setup() {
  // start serial
  Serial.begin(115200);
  Serial.println();

  // init filesystem
  if (!LittleFS.begin()) {
      Serial.println("An error has occurred while mounting LittleFS");
  }
  else
  {
      Serial.println("LittleFS mounted successfully");
  }

  // Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager(&server);
  wifiManager.init(); // will restart if needed, otherwise will be connected

  // if you get here you have connected to the WiFi
  Serial.println("WiFi connected. Ip address");
  Serial.println(WiFi.localIP());

  // initialize timer
  workoutTimer.init(millis(), timerFrame);

  // initialize display
  timerDisplay.init();

  // init the remote and socket server
  socketServer.init(workoutTimer, timerFrame);
  remoteServer.init();

  // start the webserver
  server.begin();
}

void loop() {
  // advance the timer
  workoutTimer.advance(millis(), timerFrame);

  // update the display
  timerDisplay.update(timerFrame);

  // handle socket server updates
  socketServer.update();
}