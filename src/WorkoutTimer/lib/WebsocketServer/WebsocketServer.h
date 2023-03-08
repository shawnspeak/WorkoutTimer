#ifndef WebsocketServer_h
#define WebsocketServer_h

#if defined(ESP8266)
#include <ESPAsyncWebServer.h>
#endif

#if defined(ARDUINO_RASPBERRY_PI_PICO_W)
#include <AsyncWebServer_RP2040W.h>
#endif

#include <WorkoutTimer.h>

class WebsocketServer {
    public:
        WebsocketServer(AsyncWebServer* server);

        void init(WorkoutTimer& timer, TimerFrame& frame);

        void update();

    private:
        AsyncWebServer* _server;
        AsyncWebSocket* _ws;
        TimerFrame* _frame;
        WorkoutTimer* _timer;
        uint64_t _last;

        void handleWebSocketMessage(void *arg, uint8_t *data, size_t len);
        void notifyClients();
};

#endif