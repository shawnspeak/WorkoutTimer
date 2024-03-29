#include <WiFiManager.h>
#include "LittleFS.h"

WiFiManager::WiFiManager(AsyncWebServer* server) {
    _server = server;
    _restart = false;
    connected = false;
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

// Read File from LittleFS
String readEntireFile(fs::FS &fs, const char * path){
  Serial.printf("Reading file: %s\r\n", path);

  File file = fs.open(path, "r");
  if(!file || file.isDirectory()){
    Serial.println("- failed to open file for reading");
    return String();
  }

  String fileContent;
  while(file.available()){
    fileContent = file.readString();
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

// Initialize WiFi
bool WiFiManager::initWiFi() {
  if(_ssid=="" || _pass==""){
    Serial.println("Undefined SSID and/or Password.");
    return false;
  }

  // WiFi.mode(WIFI_STA);
  WiFi.begin(_ssid.c_str(), _pass.c_str());

  Serial.println("Connecting to WiFi...");

  WiFi.waitForConnectResult();

  if(WiFi.status() != WL_CONNECTED) {
    Serial.println("Failed to connect.");
    return false;
  }

  return true;
}

bool WiFiManager::init() {
    // try to load ssid and pass
    _ssid = readFile(LittleFS, ssidPath);
    _pass = readFile(LittleFS, passPath);

    if (initWiFi()) {
        Serial.println("Connected to WiFi...");
        Serial.println(WiFi.localIP());

        connected = true;
        return true;
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

    // Route for root / web page
    _server->on("/", HTTP_GET, [](AsyncWebServerRequest *request) {
        Serial.print("Requesting config root");
        String file = readEntireFile(LittleFS, "/wifi-manager/index.html");
        request->send(200, "text/html", file);
    });
    
    // _server->serveStatic("/", LittleFS, "/wifi-manager/");

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
            writeFile(LittleFS, passPath, this->_pass.c_str());
          }
        }
      }
      _restart = true;
      request->send(200, "text/plain", "Done. Timer will restart and connect to your WiFi");
    });

    Serial.print("Starting AP Config Server");
    _server->begin(); // web server start
    Serial.print("Started AP Config Server");

   return false;
}

void WiFiManager::update() {
  if (_restart){
    delay(5000);

    #if defined(ESP8266)
    ESP.restart();
    #endif
  }
}