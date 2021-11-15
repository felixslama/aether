#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherWebinterface.h"
#include "aetherDisplay.h"

void setup() {
  Serial.begin(115200);
  initLora();
  Serial.println("initLora done");
  sendLora("initLora done");
  initDisplay();
  Serial.println("initDisplay done");
  sendLora("initDisplay done");
  initWeb();
  Serial.println("initWeb done");
  sendLora("initWeb done");
  sendLora("Setup Done");
}

void loop() {
  loopDisplay();
  loopWeb();
}