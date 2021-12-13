#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>
#include "aetherLora.h"

String loggedData = "START OF LOGFILE";
String separator = ";";
String loraLog = "";

#define SD_CS 13
#define SD_MOSI 15
#define SD_MISO 2
#define SD_SCK 14

SPIClass SPI2(VSPI);

File logFile;
void initLog(){
SPI2.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
if(!SD.begin(SD_CS)){
        Serial.println("Card Mount Failed");
        return;
    }
    logFile = SD.open("/log.txt",FILE_APPEND);
    /*
    uint8_t cardType = SD.cardType();

    if(cardType == CARD_NONE){
        Serial.println("No SD card attached");
        return;
    }

    Serial.print("SD Card Type: ");
    if(cardType == CARD_MMC){
        Serial.println("MMC");
    } else if(cardType == CARD_SD){
        Serial.println("SDSC");
    } else if(cardType == CARD_SDHC){
        Serial.println("SDHC");
    } else {
        Serial.println("UNKNOWN");
    }

    uint64_t cardSize = SD.cardSize() / (1024 * 1024);
    Serial.printf("SD Card Size: %lluMB\n", cardSize);
    Serial.printf("Total space: %lluMB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Used space: %lluMB\n", SD.usedBytes() / (1024 * 1024));
    */
}
void writeLog(String messageToLog){
    Serial.println(String(millis()) + "-" + messageToLog + separator);
    logFile.println(String(millis()) + "-" + messageToLog + separator);
    loggedData = loggedData + String(millis()) + "-" + messageToLog + separator;
    loraLog = String(millis()) + "-" + messageToLog + separator;
    sendLora(loraLog);
}
void closeLog(){
    Serial.println("closelog start");
    logFile.close();
    Serial.println("closelog end");
}