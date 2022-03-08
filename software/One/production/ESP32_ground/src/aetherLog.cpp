#include <Arduino.h>
//#include <FS.h>
#include "SdFat.h"
#include <SPI.h>
#include "aetherLora.h"

//SdFat SD;
#if SPI_DRIVER_SELECT == 2
#define SD_FAT_TYPE 0

String loggedData = "START OF LOGFILE";
String separator = ";";
String loraLog = "";
String currentLora = "";
String receivedLora = "";

const uint8_t SD_CS = 13;
const uint8_t SD_MOSI = 15;
const uint8_t SD_MISO = 2;
const uint8_t SD_SCK = 14;

long lastWriteMillis = 0;
long currentMillis = 0;
const long interval = 500;

//SPIClass SPI2(VSPI);
SoftSpiDriver<SD_MISO, SD_MOSI, SD_SCK> softSpi;

#if ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS, DEDICATED_SPI, SD_SCK_MHZ(0), &softSpi)
#else  // ENABLE_DEDICATED_SPI
#define SD_CONFIG SdSpiConfig(SD_CS_PIN, SHARED_SPI, SD_SCK_MHZ(0), &softSpi)
#endif  // ENABLE_DEDICATED_SPI

SdFat32 sd;
File32 logFile;
void initLog(){
    Serial.println("begin init");
    if(!sd.begin(SD_CONFIG)){
        sd.initErrorHalt();
        sd.initErrorPrint();
    }
    Serial.println(logFile.open("log.txt", O_WRITE | O_CREAT | O_AT_END ));
    logFile.println("init log print");
    Serial.println("end init");
}
void writeLog(String messageToLog){
    Serial.println(String(millis()) + "-" + messageToLog + separator);
    logFile.println(String(millis()) + "-" + messageToLog + separator);
    loggedData = loggedData + String(millis()) + "-" + messageToLog + separator;
    loraLog = String(millis()) + "-" + messageToLog + separator;
    sendLora(loraLog);
}
void closeLog(){
    logFile.close();
}
void loopLog(){
    currentMillis = millis();
    currentLora = readLora();
    if(receivedLora != currentLora){
        writeLog(currentLora);
    }
    if(lastWriteMillis + interval <= currentMillis){
        writeLog("begin looplog " + String(currentMillis));
        closeLog();
        initLog();
        lastWriteMillis = currentMillis;
    }
}
#endif