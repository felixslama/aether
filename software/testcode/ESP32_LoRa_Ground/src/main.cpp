#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"

void setup() {
  Serial.begin(115200);
  initLora();
  initServoPitchRoll();
  delay(2000);
}

void loop() {
  sendLora("pls work");
  // if(readLora() == "pls work"){
  //   Serial.print("worked i guess");
  // }
  loopServoPitchRoll();
}