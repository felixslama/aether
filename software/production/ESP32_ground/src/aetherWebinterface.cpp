#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "aetherIndex.h"
#include "aetherLora.h"

float calcValue;

const char* ssid     = "Rocket Telemetry";
const char* password = "rocketgobrr";
WebServer server(80);

void handleRoot() {
 String s = aetherIndex;
 server.send(200, "text/html", s);
}

void inputGyro(float newValue){
    calcValue = newValue;
}

void handleGyroReading() {
 float a = calcValue;
 String adcValue = String(a);
 server.send(200, "text/plane", adcValue);
}

void handleOn() { 
 Serial.println("ON");
 sendLora("ENGINE ON");
 handleRoot();
}

void handleOff() { 
 Serial.println("OFF");
 sendLora("ENGINE OFF");
 handleRoot();
}
void handleHoldThrust(){
    Serial.println("hold thrust");
 sendLora("ENGINE HOLD");
 handleRoot();
}
void handleKill(){
    Serial.println("killed");
 sendLora("ENGINE KILL");
 handleRoot();
}

void initWeb(){
    WiFi.mode(WIFI_AP);
    WiFi.softAP(ssid, password);
    server.on("/", handleRoot);
    server.on("/readGyro", handleGyroReading);
    server.on("/On", handleOn);
    server.on("/Off", handleOff);
    server.on("/holdThrust", handleHoldThrust);
    server.on("/kill",handleKill);
    server.begin();
    Serial.println("HTTP server started");
}

void loopWeb(){
    server.handleClient();
}