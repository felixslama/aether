#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"
#include "aetherComs.h"
#include "aetherLog.h"
void setup() {
  Serial.begin(115200);
  initLora();
  initLog();
  writeLog("---------START OF LOG---------");
  writeLog("initLora done");
  initServo();
  writeLog("iniServo done");
  initMPU();
  writeLog("initMPU done");
    writeLog("initMPU done");
      writeLog("initMPU done");
        writeLog("initMPU done");
          writeLog("initMPU done");
            writeLog("initMPU done");
              writeLog("initMPU done");
                writeLog("initMPU done");
                  writeLog("initMPU done");
                    writeLog("initMPU done");
                      writeLog("initMPU done");
                        writeLog("initMPU done");
                          writeLog("initMPU done");
                            writeLog("initMPU done");
                              writeLog("initMPU done");
                                writeLog("initMPU done");
                                  writeLog("initMPU done");

  initESC();
  writeLog("initESC done");
  writeLog("setup done");
  closeLog();
  //initLog();
}

void loop() {
  handleReceivedLora();
  loopControl();
  //loopLog();
}