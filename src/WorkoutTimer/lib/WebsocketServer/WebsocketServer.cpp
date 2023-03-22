#include <WebsocketServer.h>
#include <WorkoutTimer.h>

#include <SafeString.h>

WebsocketServer::WebsocketServer(AsyncWebServer* server) {
    _server = server;
    _ws = new AsyncWebSocket("/ws");
}

AsyncWebSocketClient * globalClient = NULL;

// buffers for command messages and arguments
createSafeString(stringBuffer, 50);
createSafeString(command, 10);
createSafeString(arg1, 10);
createSafeString(arg2, 10);
createSafeString(statusBuffer, 50);

void WebsocketServer::notifyClients() {
  if (_ws->count() > 0) {
      // fill buffer with a json status string
      statusBuffer.clear();
      statusBuffer.printf("{\"md\":%u,\"st\":%u,\"in\":%u,\"ti\":%u,\"h\":%u,\"m\":%u,\"s\":%u}",
        _frame->mode,
        _frame->state,
        _frame->interval,
        _frame->intervals,
        _frame->displayTime[TIME_HOURS],
        _frame->displayTime[TIME_MINUTES],
        _frame->displayTime[TIME_SECONDS]);
        
      for (const auto& c : _ws->getClients())
      {
        if (c->status() == WS_CONNECTED)
        {
          c->text(statusBuffer.c_str());
        }
      }
  }
}

void WebsocketServer::handleWebSocketMessage(void *arg, uint8_t *data, size_t len) {
  AwsFrameInfo *info = (AwsFrameInfo*)arg;
  if (info->final && info->index == 0 && info->len == len && info->opcode == WS_TEXT) {
    data[len] = 0;
    stringBuffer = (char*)data;

    // Take string data from message and parse out commands
    int arg1Int = 0;
    int arg2Int = 0;
    size_t nextIdx = 0;
    nextIdx = stringBuffer.stoken(command, nextIdx, ";");

    Serial.println("Command ");
    Serial.println(command);

    if (command == "start") {
      _timer->start(millis());
    } 
    else if (command == "pause") {
      _timer->pause(millis());
    } 
    else if (command == "up") {
      // parse total seconds
      nextIdx++;
      nextIdx = stringBuffer.stoken(arg1, nextIdx, ";");

      Serial.println("Up Seconds ");
      Serial.println(arg1);

      if (arg1.toInt(arg1Int)) {
        _timer->setUp(arg1Int);
      }
    }
    else if (command == "down") {
      // parse total seconds
      nextIdx++;
      nextIdx = stringBuffer.stoken(arg1, nextIdx, ";");

      Serial.println("Down Seconds ");
      Serial.println(arg1);

      if (arg1.toInt(arg1Int)) {
        _timer->setDown(arg1Int);
      }
    }
    else if (command == "emom") {
      // parse intervals and interval seconds
      nextIdx++;
      nextIdx = stringBuffer.stoken(arg1, nextIdx, ";");
      nextIdx++;
      nextIdx = stringBuffer.stoken(arg2, nextIdx, ";");

      Serial.println("Intervals ");
      Serial.println(arg1);
      Serial.println("Interval Seconds ");
      Serial.println(arg2);

      if (arg1.toInt(arg1Int) && arg2.toInt(arg2Int)) {
        _timer->setEmom(arg1Int, arg2Int);
      }
    }
  }
}

void handleRoot(AsyncWebServerRequest *request)
{
	request->send(200, "text/text", "hello");
}

void WebsocketServer::init(WorkoutTimer& timer, TimerFrame& frame) {
  _last = millis();
  _frame = &frame;
  _timer = &timer;
  _ws->onEvent([this](AsyncWebSocket *server, AsyncWebSocketClient *client, AwsEventType type, void *arg, uint8_t *data, size_t len) {
    switch (type) {
      case WS_EVT_CONNECT:
        Serial.printf("WebSocket client #%u connected from %s. Core: ", client->id(), client->remoteIP().toString().c_str());
        Serial.println(get_core_num());
        globalClient = client;
        // notifyClients();
        break;
      case WS_EVT_DISCONNECT:
        Serial.printf("WebSocket client #%u disconnected. Core: ", client->id());
        Serial.println(get_core_num());
        globalClient = NULL;
        break;
      case WS_EVT_DATA:
          Serial.printf("WebSocket client #%u Data. Core: ", client->id());
          Serial.println(get_core_num());
          handleWebSocketMessage(arg, data, len);
          break;
      case WS_EVT_PONG:
      case WS_EVT_ERROR:
      default:
       Serial.printf("WebSocket event");
      break;
    }
  });
  _server->addHandler(_ws);
  // _server->on("/", handleRoot);
}

void WebsocketServer::update() {
    // push a status update out to the clients every 5 seconds
    if (_last + 5000 < millis()) {
      _last = millis();
      if (_ws->count() > 0) {
        notifyClients();
        // Serial.printf("Websocket Server Update. Core: ");
        // Serial.println(get_core_num());
      }
    }
}