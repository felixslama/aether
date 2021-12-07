/*
 * ESP32-WROOM
 * Connect the SD card to the following pins:
 *
 * SD Card | ESP32
 *    D2       -
 *    D3       SS
 *    CMD      MOSI
 *    VSS      GND
 *    VDD      3.3V
 *    CLK      SCK
 *    VSS      GND
 *    D0       MISO
 *    D1       -
 */

#include "FS.h"
#include "SD.h"
#include "SPI.h"
#include <string>

//pinout SD|ESP32
#define SCK  18
#define MISO  19
#define MOSI  23
#define CS  5
//VCC       5V
//GND       Ground


//void setup(){
void initLog(){
    Serial.begin(115200); 
}


void loop(){
    String a = "1";
    string_to_sdcard(a);
    String b = "2";
    string_to_sdcard(b);
    String c = "3";
    string_to_sdcard(c);
    delay(1000);

}

void writeLog(String logData){
    //the beginning of log.txt file is buggy, even with logData from previos test runs.
    const char path[] = "/log.txt";
    const char *logChar = logData.c_str();
    
    
    if(!SD.begin(CS)) {
      Serial.println("Card Mount Failed");
      return;
    } 
    fs::FS &fs = SD;
    //Serial.printf("Appending to file: %s\n", path);
    File file = fs.open(path, FILE_APPEND);
    
    if(!file){
        Serial.println("Failed to open file");
        return;
    }
    if(!file.println(logChar)){
        Serial.println("Failed writing to file");
        return;
    } 
    //file.close();
}
