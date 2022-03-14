#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "Comms.h"
#include "Control.h"
#include "Power.h"
#include "Logs.h"

// LoRa Pins
#define BAND 866E6
#define SCK 5
#define MISO 19
#define MOSI 27
#define SS 18
#define RST 23
#define DIO0 26

// other vars
String LoRaData;
long prevMillis = 0;
long intvl = 1000;
bool killedComs = false;
String dataReceived; 

void handleReceivedLora(){
    dataReceived = readLora();
    if(dataReceived != ""){
        writeLog(dataReceived + "handlereceivedlora");
    }
    if (dataReceived == "ENGINE OFF"){
        return;
    }else if (dataReceived == "ENGINE HOLD"){
        toggleESCHold();
    }else if (dataReceived == "ENGINE KILL"){
        Serial.println("kill triggered");
        killPower();
    }else if (dataReceived == "ENGINE ON"){
        return;
    }else if (dataReceived == "NO ENGINE HOLD"){
        toggleESCHold();
    }
}

void initLora(){
    SPI.begin(SCK, MISO, MOSI, SS);
    LoRa.setPins(SS, RST, DIO0);
    if (!LoRa.begin(BAND)) {
        Serial.println("wasnt able to start lora");
        while (1);
    }
}

void sendLora(String messageToSend){
    unsigned long currMillis = millis();
    
    if(currMillis - prevMillis > intvl) {
        prevMillis = currMillis;
        LoRa.beginPacket();
        LoRa.print(messageToSend);
        LoRa.endPacket();
        Serial.println(messageToSend);
    }
}

String readLora(){
    int packetSize = LoRa.parsePacket();
    if (packetSize){
        while(LoRa.available()){
            LoRaData = LoRa.readString();
            Serial.print(LoRaData);
        }
    }
    return LoRaData;
}