#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

#define SD_CS 13
#define SPI_MOSI 15
#define SPI_MISO 2
#define SPI_SCK 14

File logFile;
void initLog(){
   SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI, SD_CS);
    if(!SD.begin(SD_CS, SPI))
  {
      Serial.println("Card Mount Failed");
      return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE)
  {
      Serial.println("No SD card attached");
      return;
  }
    logFile = SD.open("log.txt",FILE_WRITE);
    if (logFile) 
  {
    logFile.println("Initialized Logs ---------------------------");
  } 
    
}
void writeLog(String messageToLog){
    logFile.println(messageToLog);
}
void closeLog(){
    logFile.close();
}