#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS 13
#define SPI_MOSI 15
#define SPI_MISO 2
#define SPI_SCK 14

String loggedData = "START OF LOGFILE";
String separator = ";";

File logFile;
void initLog(){
    Serial.println("begin of initlog");
SPI.begin(14, 2, 15);
if(!SD.begin(13)){
        Serial.println("Card Mount Failed");
        return;
    }
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

    logFile = SD.open("aetherLog.txt",FILE_WRITE);
    if(logFile){
        logFile.println("init log");
    }

}
void writeLog(String messageToLog){
    //logFile.println(messageToLog);
    loggedData = loggedData + messageToLog + separator;
}
void closeLog(){
    logFile.close();
    SPI.end();
}
void writeLogToSD(){
    Serial.println("begin of writetosd");
    initLog();
    logFile.println(loggedData);
    closeLog();
    Serial.println("end of writetosd");
}