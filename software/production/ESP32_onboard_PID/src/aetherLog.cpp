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

const uint8_t SD_CS = 13;
const uint8_t SD_MOSI = 15;
const uint8_t SD_MISO = 2;
const uint8_t SD_SCK = 14;

long lastWriteMillis = 0;
long currentMillis = 0;
const long interval = 1000;

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
/*
if(!logFile.open("log.txt", O_RDWR | O_CREAT)){
    Serial.println("begin logfile open");
    sd.errorHalt(F("open failed"));
}*/
Serial.println(logFile.open("log.txt", O_WRITE | O_CREAT | O_AT_END ));
logFile.println("init log print");
Serial.println("end init");
/*
logFile.rewind();
while(logFile.available()){
    Serial.write(logFile.read());
}
Serial.println(logFile.close());
Serial.println("done");
*/
}
void writeLog(String messageToLog){
    if(logFile){
        Serial.println(String(millis()) + "-" + messageToLog + separator);
        logFile.println(String(millis()) + "-" + messageToLog + separator);
        loggedData = loggedData + String(millis()) + "-" + messageToLog + separator;
        loraLog = String(millis()) + "-" + messageToLog + separator;
        //sendLora(loraLog);
    }else{
        Serial.println("catched");
    }
}
void closeLog(){
    Serial.println("closelog start");
    Serial.println(logFile.close());
    Serial.println("closelog end");
}
void loopLog(){
    writeLog("looplog");
    currentMillis = millis();
//    Serial.println("outer looplog" + String(currentMillis));
    if(lastWriteMillis + interval <= currentMillis){
        Serial.println("begin looplog " + String(currentMillis));
        closeLog();
        initLog();
        lastWriteMillis = currentMillis;
//        Serial.println("end looplog");
    }
}
#endif