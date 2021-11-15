#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherWebinterface.h"
#include "aetherDisplay.h"

void setup() {
  Serial.begin(115200);
  initDisplay();
  initLora();
  initWeb();
  delay(2000);
  sendLora("end of setup");
}

void loop() {
  loopDisplay();
  loopWeb();
}