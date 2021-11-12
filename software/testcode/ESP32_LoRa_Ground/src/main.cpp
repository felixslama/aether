#include <SPI.h>
#include <LoRa.h>
#include "aetherLora.h"
#include <Wire.h>

void setup() {
  Serial.begin(115200);
  initLora();
  delay(2000);
}

void loop() {
  sendLora("pls work");
  // if(readLora() == "pls work"){
  //   Serial.print("worked i guess");
  // }
}