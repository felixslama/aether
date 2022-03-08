#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "aetherIndex.h"
#include "aetherLora.h"
#include <ESPAsyncWebServer.h>
#include <Update.h>

float calcValue;

const char* ssid     = "Rocket Telemetry";
const char* password = "rocketgobrr";

AsyncWebServer server(80);

void inputGyro(float newValue){
    calcValue = newValue;
}

void handleUpdate(AsyncWebServerRequest *request) {
  request->send(200, "text/html", updateIndex);
};

void handleDoUpdate(AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
  if (!index){
    size_t content_len;  
    content_len = request->contentLength();
    int cmd = (filename.indexOf(F(".spiffs.bin")) > -1 ) ? U_SPIFFS : U_FLASH;
    if (cmd == U_FLASH && !(filename.indexOf(F(".bin")) > -1) ) return; // wrong image for ESP32
    if (!Update.begin(UPDATE_SIZE_UNKNOWN, cmd)) {
      Update.printError(Serial);
    }
  }
  if (Update.write(data, len) == len) {
    int progress = Update.progress() / 10000;
    Serial.println(progress);
  } else {
    Update.printError(Serial);
  }
  if (final) {    
    if (!Update.end(true)){
      Update.printError(Serial);
    } else {
      AsyncWebServerResponse *response = request->beginResponse(200, "text/html", updateDoneIndex); 
      request->send(response); 
      delay(100);
      ESP.restart();
    }
  }
}

void initWeb(){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", aetherIndex);
    });
    server.on("/update", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send(200, "text/html", updateIndex);
    });
    server.on("/doUpdate", HTTP_POST,
        [](AsyncWebServerRequest *request) {},
        [](AsyncWebServerRequest *request, const String& filename, size_t index, uint8_t *data, size_t len, bool final) {
        handleDoUpdate(request, filename, index, data, len, final);}
    );
    server.on("/readLog", HTTP_GET, [] (AsyncWebServerRequest *request) {
        String logvalue = readLora();
        String logData = logData + logvalue;
        request->send(200, "text/plane", logData);
    });
    server.on("/On", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("ON");
        sendLora("ENGINE ON");
    });
    server.on("/Off", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("OFF");
        sendLora("ENGINE OFF");
    });
    server.on("/holdThrust", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("hold thrust");
        sendLora("ENGINE HOLD");
    });
    server.on("/noHoldThrust", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("no hold thrust");
        sendLora("NO ENGINE HOLD");
    });
    server.on("/kill", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("killed");
        sendLora("ENGINE KILL");
    });
    server.on("/increase", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("increased");
        sendLora("SPEED UP");
    });
    server.on("/decrease", HTTP_GET, [] (AsyncWebServerRequest *request) {
        Serial.println("decrease");
        sendLora("SPEED DOWN");
    });
    server.begin();
    Serial.println("HTTP server started");
}