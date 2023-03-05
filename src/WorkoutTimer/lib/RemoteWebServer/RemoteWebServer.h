#ifndef RemoteWebServer_h
#define RemoteWebServer_h

#include <ESPAsyncWebServer.h>

class RemoteWebServer {
    public:
        RemoteWebServer(AsyncWebServer* server);

        void init();

        //void update();

    private:
        AsyncWebServer* _server;
};

#endif