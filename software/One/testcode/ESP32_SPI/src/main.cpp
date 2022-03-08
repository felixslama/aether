#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherLog.h"

void setup() {
  Serial.begin(115200);
  initLora();
  Serial.println("init lora complete");
  initLog();
  Serial.println("initLog done");
}

void loop() {
  // put your main code here, to run repeatedly:
}