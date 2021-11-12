#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>

//Pins, an denen das LoRa-Modul am ESP32 angeschlossen ist
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//Frequenzbereich für Europa
#define BAND 866E6

void aetherComsSetup(){
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);
}

void aetherComsSend(String message){
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
}

void setup() {
  Serial.begin(115200);
  aetherComsSetup();
}

void loop() {
  aetherComsSend("Test Test");
  delay(10000);
}