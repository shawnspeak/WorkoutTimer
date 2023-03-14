#include <RemoteWebServer.h>
#include "LittleFS.h"

RemoteWebServer::RemoteWebServer(AsyncWebServer* server) {
    _server = server;
}


void RemoteWebServer::init() {
    // Route for root / web page
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/remote/index.html", "text/html", false);
    });
    
    _server->serveStatic("/", LittleFS, "/remote/");
}