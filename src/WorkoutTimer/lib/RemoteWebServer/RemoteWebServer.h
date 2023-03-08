#ifndef RemoteWebServer_h
#define RemoteWebServer_h

#if defined(ESP8266)
#include <ESPAsyncWebServer.h>
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <AsyncWebServer_RP2040W.h>
#endif

class RemoteWebServer {
    public:
        RemoteWebServer(AsyncWebServer* server);

        void init();

        //void update();

    private:
        AsyncWebServer* _server;
};

#endif