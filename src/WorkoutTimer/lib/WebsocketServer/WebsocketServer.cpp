#include <WebsocketServer.h>
#include <WorkoutTimer.h>

WebsocketServer::WebsocketServer() {
    _server = new AsyncWebServer(80);
    _ws = new AsyncWebSocket("/ws");
}

String message = "";
String state = "testing";

void WebsocketServer::notifyClients() {  
  if (_ws->count() > 0) {
    _ws->textAll(state);
  }
}

void WebsocketServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    message = (char*)data;
    Serial.print("message");
    Serial.println(message);

    // TODO: Translate message into timer command
  }
}

void WebsocketServer::init(WorkoutTimer& timer, TimerFrame& frame) {
  _last = millis();
  _frame = &frame;
  _timer = &timer;
  _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s\n", client->id(), client->remoteIP().toString().c_str());
        notifyClients();
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected\n", client->id());
        break;
      case WS_EVT_DATA:
          handleWebSocketMessage(arg, data, len);
          break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
      break;
    }
  });
  _server->addHandler(_ws);
  _server->begin();
}

void WebsocketServer::update() {
    _ws->cleanupClients();

    // push a status update out to the client every 5 seconds
    if (_last + 5000 < millis()) {
      _last = millis();
      notifyClients();
    }
}