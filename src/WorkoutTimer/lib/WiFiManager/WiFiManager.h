#ifndef WiFiManager_h
#define WiFiManager_h

#include <ESPAsyncWebServer.h>

class WiFiManager {
    public:
        WiFiManager(AsyncWebServer* server);

        bool init();

        bool connected;

        void update();

    private:
        AsyncWebServer* _server;
        String _ssid;
        String _pass;
        bool _restart;

        bool initWiFi();
};

#endif