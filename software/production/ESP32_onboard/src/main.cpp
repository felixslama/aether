#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"
#include "aetherComs.h"

void setup() {
  Serial.begin(115200);
  initLora();
  sendLora("initLora done");
  initServo();
  sendLora("initServo done");
  initMPU();
  sendLora("initMPU done");
  initESC();
  sendLora("initESC done");
  sendLora("setup done");
}

void loop() {
  handleReceivedLora();
  loopControl();
}