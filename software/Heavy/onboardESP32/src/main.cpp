#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "Control.h"
#include "Power.h"
#include "Comms.h"
#include "Logs.h"
#include "Baro.h"

void setup() {
  Serial.begin(115200);
  initServo();
  initMPU();
  initPID();
  initESC();
}

void loop() {
  loopControl();
  // we check if control loop is ready to handle things
  if (checkReadyStatus() == true) {
    loopPower();
  } else {
    return;
  }
}
