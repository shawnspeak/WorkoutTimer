#include <WiFiManager.h>
#include "LittleFS.h"

WiFiManager::WiFiManager(AsyncWebServer* server) {
    _server = server;
    _restart = false;
}

const char* PARAM_INPUT_1 = "ssid";
const char* PARAM_INPUT_2 = "pass";

// File paths to save input values permanently
const char* ssidPath = "/cfg/ssid.txt";
const char* passPath = "/cfg/pass.txt";

// Read File from LittleFS
String readFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readStringUntil('\n');
    break;
  }
  file.close();
  return fileContent;
}

// Write file to LittleFS
void writeFile(fs::FS &fs, const char * path, const char * message){
  Serial.printf("Writing file: %s\r\n", path);

  File file = fs.open(path, "w");
  if(!file){
    Serial.println("- failed to open file for writing");
    return;
  }
  if(file.print(message)){
    Serial.println("- file written");
  } else {
    Serial.println("- frite failed");
  }
  file.close();
}

// uint8_t AsyncWiFiManager::waitForConnectResult()
// {
//   if (_connectTimeout == 0)
//   {
//     return WiFi.waitForConnectResult();
//   }
//   else
//   {
//     DEBUG_WM(F("Waiting for connection result with time out"));
//     unsigned long start = millis();
//     boolean keepConnecting = true;
//     uint8_t status;
//     while (keepConnecting)
//     {
//       status = WiFi.status();
//       if (millis() > start + _connectTimeout)
//       {
//         keepConnecting = false;
//         DEBUG_WM(F("Connection timed out"));
//       }
//       if (status == WL_CONNECTED || status == WL_CONNECT_FAILED)
//       {
//         keepConnecting = false;
//       }
//       delay(100);
//     }
//     return status;
//   }
// }

// Initialize WiFi
bool WiFiManager::initWiFi() {
  if(_ssid=="" || _pass==""){
    Serial.println("Undefined SSID and/or Password.");
    return false;
  }

  WiFi.mode(WIFI_STA);
//   localIP.fromString(ip.c_str());
//   localGateway.fromString(gateway.c_str());

//   if (!WiFi.config(localIP, localGateway, subnet)){
//     Serial.println("STA Failed to configure");
//     return false;
//   }
  WiFi.begin(_ssid.c_str(), _pass.c_str());

  Serial.println("Connecting to WiFi...");

  WiFi.waitForConnectResult();

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect.");
    return false;
  }

  return true;
}

void WiFiManager::init() {
    // try to load ssid and pass
    _ssid = readFile(LittleFS, ssidPath);
    _pass = readFile(LittleFS, passPath);

    if (initWiFi()) {
        Serial.println("Connected to WiFi...");
        Serial.println(WiFi.localIP());
        return;
    }

    // host the wifi manager
    _server->reset();

    // Connect to Wi-Fi network with SSID and password
    Serial.println("Setting AP (Access Point)");

    // NULL sets an open Access Point
    WiFi.softAP("WorkoutTimerConfig"); // password option

    delay(500); // without delay I've seen the IP address blank

    IPAddress IP = WiFi.softAPIP();
    Serial.print("AP IP address: ");
    Serial.println(IP); 

    // Web Server Root URL
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request){
      request->send(LittleFS, "/wifimanager.html", "text/html");
    });
    
    _server->serveStatic("/", LittleFS, "/");

    // Route for root / web page
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        request->send(LittleFS, "/wifi-manager/index.html", "text/html", false);
    });
    
    _server->serveStatic("/", LittleFS, "/wifi-manager/");

    _server->on("/", HTTP_POST, [this](AsyncWebServerRequest *request) {
      int params = request->params();
      for(int i=0;i<params;i++){
        AsyncWebParameter* p = request->getParam(i);
        if(p->isPost()){
          // HTTP POST ssid value
          if (p->name() == PARAM_INPUT_1) {
            this->_ssid = p->value().c_str();
            Serial.print("SSID set to: ");
            Serial.println(this->_ssid);

            // Write file to save value
            writeFile(LittleFS, ssidPath, this->_ssid.c_str());
          }
          // HTTP POST pass value
          if (p->name() == PARAM_INPUT_2) {
            this->_pass = p->value().c_str();
            Serial.print("Password set to: ");
            Serial.println(this->_pass);
            // Write file to save value
            writeFile(LittleFS, passPath, this->_ssid.c_str());
          }
        //   // HTTP POST ip value
        //   if (p->name() == PARAM_INPUT_3) {
        //     ip = p->value().c_str();
        //     Serial.print("IP Address set to: ");
        //     Serial.println(ip);
        //     // Write file to save value
        //     writeFile(LittleFS, ipPath, ip.c_str());
        //   }
        //   // HTTP POST gateway value
        //   if (p->name() == PARAM_INPUT_4) {
        //     gateway = p->value().c_str();
        //     Serial.print("Gateway set to: ");
        //     Serial.println(gateway);
        //     // Write file to save value
        //     writeFile(LittleFS, gatewayPath, gateway.c_str());
        //   }
          //Serial.printf("POST[%s]: %s\n", p->name().c_str(), p->value().c_str());
        }
      }
      _restart = true;
      request->send(200, "text/plain", "Done. Timer will restart and connect to your WiFi");
    });

    _server->begin(); // web server start

    while(true)
    {
        delay(5000);
        if (_restart) {
            ESP.restart();
        }
    }
}