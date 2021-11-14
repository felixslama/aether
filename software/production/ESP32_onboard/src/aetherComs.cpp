#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"

String dataReceived;

void handleReceivedLora(){
    dataReceived = readLora();
    if (dataReceived == "ENGINE OFF"){
        escOFF();
    }else if (dataReceived == "ENGINE HOLD"){
        escHold();
    }else if (dataReceived == "ENGINE KILL"){
        escKill();
    }else if (dataReceived == "ENGINE ON"){
        escON();
    }
}