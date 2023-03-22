#ifndef RemoteWebServer_h
#define RemoteWebServer_h

#include <LittleFS.h>

#if defined(ESP8266)
#include <ESPAsyncWebServer.h>
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#define _RP2040W_AWS_LOGLEVEL_     4
#define _ASYNCTCP_RP2040W_LOGLEVEL_       4
#include <AsyncWebServer_RP2040W.h>
#endif

class RemoteWebServer {
    public:
        RemoteWebServer(AsyncWebServer* server, FS* fs);

        void init();

    private:
        FS* _fs;
        AsyncWebServer* _server;
};

#endif