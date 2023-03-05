#include <RemoteWebServer.h>
#include "LittleFS.h"

RemoteWebServer::RemoteWebServer(AsyncWebServer* server) {
    _server = server;
}


void RemoteWebServer::init() {
    if (!LittleFS.begin()) {
        Serial.println("An error has occurred while mounting LittleFS");
    }
    else{
        Serial.println("LittleFS mounted successfully");
    }

    // Route for root / web page
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/index.html", "text/html", false);
    });
    
    _server->serveStatic("/", LittleFS, "/");
}