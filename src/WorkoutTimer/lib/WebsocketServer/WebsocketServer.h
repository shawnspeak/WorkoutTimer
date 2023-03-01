#ifndef WebsocketServer_h
#define WebsocketServer_h

#include <ESPAsyncWebServer.h>
#include <WorkoutTimer.h>

class WebsocketServer {
    public:
        WebsocketServer();

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