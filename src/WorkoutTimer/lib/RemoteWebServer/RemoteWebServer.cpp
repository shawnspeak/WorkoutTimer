#include <RemoteWebServer.h>

RemoteWebServer::RemoteWebServer(AsyncWebServer* server, FS* fs) 
{
    _server = server;
    _fs = fs;
}

void RemoteWebServer::init() 
{
    _server->on("/", HTTP_GET, [this](AsyncWebServerRequest *request) {
        request->send(*_fs, "/remote/index.html", "text/html", false);
    });

     _server->serveStatic("/", *_fs, "/remote/");
}