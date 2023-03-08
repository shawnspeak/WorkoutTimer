#if defined(ESP8266)
#include <ESP8266WiFi.h>
#include <ESPAsyncWiFiManager.h>
#endif

#include <WorkoutTimer.h>
#include <TimerDisplay.h>
#include <RemoteWebServer.h>
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


#if defined(ESP8266)
  // WiFiManager
  // Local intialization. Once its business is done, there is no need to keep it around
  DNSServer dns;

  AsyncWiFiManager wifiManager(&server,&dns);

  // exit after config instead of connecting
  wifiManager.setBreakAfterConfig(true);

  //reset settings - for testing
  //wifiManager.resetSettings();

  // tries to connect to last known settings
  // if it does not connect it starts an access point
  // and goes into a blocking loop awaiting configuration
  if (!wifiManager.autoConnect("WorkoutTimerConfig", "asdf1234")) {
    Serial.println("failed to connect, we should reset as see if it connects");
    delay(3000);
    ESP.reset();
    delay(5000);
  }
#endif

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