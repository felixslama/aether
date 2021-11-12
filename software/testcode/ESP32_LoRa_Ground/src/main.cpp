/*
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
  Serial.println("setup");
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
}

void loop() {
  Serial.println("not send packet");
  LoRa.beginPacket();
  LoRa.print("test test");
  LoRa.endPacket();
  Serial.println("send packet");
  delay(1000);
}*/

/*********
  LoRa Sender
  pollux labs
  https://polluxlabs.net
*********/

//Bibliotheken für LoRa
#include <SPI.h>
#include <LoRa.h>

//Bibliotheken für das OLED-Display
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

//Counter für die gesendeten Datenpakete
int counter = 0;
void aetherComsSend(String message){
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    Serial.println("sent");
}

void setup() {

  
  //Seriellen Monitor starten
  Serial.begin(115200);
  
  //Pins für LoRa definieren und Kommunikation starten
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);
  
  if (!LoRa.begin(BAND)) {
    Serial.println("LoRa konnte nicht gestarten werden!");
    while (1);
  }
  delay(2000);
}

void loop() {
   
  Serial.print("Sende Paket: ");
  Serial.println(counter);

  
  LoRa.beginPacket();
  LoRa.print("Paket Nr. ");
  LoRa.print(counter);
  LoRa.endPacket();
  aetherComsSend("sent");
  
  counter++;
  
  delay(1000);
}