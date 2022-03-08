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

//Variable für die empfangenen Daten
String LoRaData;

void setup() {

  //Seriellen Monitor starten
  Serial.begin(115200);

  //Pins für LoRa definieren und Kommunikation starten
  SPI.begin(SCK, MISO, MOSI, SS);
  LoRa.setPins(SS, RST, DIO0);

  Serial.println("LoRa  OK!");
}

void loop() {

  //Datenpaket parsen
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Paket empfangen ");

    //Paket auslesen
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //RSSI ausgeben
    int rssi = LoRa.packetRssi();
    Serial.print(" mit RSSI ");
    Serial.println(rssi);

  }
}*/
/*********
  LoRa Receiver
  pollux labs
  https://polluxlabs.net
*********/

//Bibliotheken für LoRa
#include <SPI.h>
#include <LoRa.h>

//Bibliotheken für das OLED-Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Pins, an denen das LoRa-Modul am ESP32 angeschlossen ist
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 14
#define DIO0 26

//Frequenzbereich für Europa
#define BAND 866E6

//Pins, an denen das OLED-Display angeschlossen ist
#define OLED_SDA 4
#define OLED_SCL 15
#define OLED_RST 16

//Displaygröße
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Counter für die gesendeten Datenpakete
int counter = 0;

//OLED-Display instanziieren
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

//Variable für die empfangenen Daten
String LoRaData;

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

}

void loop() {

  //Datenpaket parsen
  int packetSize = LoRa.parsePacket();
  if (packetSize) {
    Serial.print("Paket empfangen ");

    //Paket auslesen
    while (LoRa.available()) {
      LoRaData = LoRa.readString();
      Serial.print(LoRaData);
    }

    //RSSI ausgeben
    int rssi = LoRa.packetRssi();
    Serial.print(" mit RSSI ");
    Serial.println(rssi);

  }
}