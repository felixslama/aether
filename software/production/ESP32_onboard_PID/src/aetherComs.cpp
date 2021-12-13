#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherServoPitchRoll.h"
#include "aetherLog.h"

bool killedComs = false;
String dataReceived;

void handleReceivedLora(){
    dataReceived = readLora();
    if(dataReceived != ""){
        Serial.println(dataReceived);
    }
    if (dataReceived == "ENGINE OFF"){
        escOFF();
    }else if (dataReceived == "ENGINE HOLD"){
        escHold();
    }else if (dataReceived == "ENGINE KILL"){
        Serial.println("kill triggered");
        escKill();
        writeLog(String(millis()) + "kill called");
        if(!killedComs){
            //endLora();
            //writeLogToSD();
        }
    }else if (dataReceived == "ENGINE ON"){
        escON();
    }else if (dataReceived == "NO ENGINE HOLD"){
        escNoHold();
    }
}