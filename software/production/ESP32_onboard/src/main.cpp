#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"
#include "aetherComs.h"
#include "aetherLog.h"
//Logs WIP
void setup() {
  Serial.begin(115200);
  //initLog();
  initLora();
  sendLora("initLora done");
  //writeLog("initLora done");
  initServo();
  sendLora("initServo done");
  //writeLog("iniServo done");
  initMPU();
  sendLora("initMPU done");
  //writeLog("initMPU done");
  initESC();
  sendLora("initESC done");
  //writeLog("initESC done");
  sendLora("setup done");
  //writeLog("setup done");
  closeLog();
}

void loop() {
  handleReceivedLora();
  loopControl();
}