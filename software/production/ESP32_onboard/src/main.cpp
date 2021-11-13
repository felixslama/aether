#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"
#include "aetherComs.h"

void setup() {
  Serial.begin(115200);
  initLora();
  initServo();
  initMPU();
  initESC();
  delay(2000);
  sendLora("setup done");
}

void loop() {
  handleReceivedLora();
  loopControl();
}