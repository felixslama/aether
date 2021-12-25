#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "aetherIndex.h"
#include "aetherLora.h"
#include <ESPAsyncWebServer.h>

float calcValue;

const char* ssid     = "Rocket Telemetry";
const char* password = "rocketgobrr";

AsyncWebServer server(80);

void inputGyro(float newValue){
    calcValue = newValue;
}

void initWeb(){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        request->send_P(200, "text/html", aetherIndex);
    });
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