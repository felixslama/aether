#include <Arduino.h>
#include <Wire.h>
#include <Adafruit_BMP280.h>
#include "aetherLog.h"

Adafruit_BMP280 bmp;

double atmosphericpressure = 1013.25;
float currentHeight;
float maxHeight;
int interval2 = 200;
long currentMillis2;
long previousMillis2 = 0;

void initBaro(){
    if(!bmp.begin(0x76)){
        Serial.println(F("Could not find a valid BMP280 sensor, check wiring or "
                      "try a different address!"));
        Serial.print("SensorID was: 0x"); Serial.println(bmp.sensorID(),16);
        Serial.print("        ID of 0xFF probably means a bad address, a BMP 180 or BMP 085\n");
        Serial.print("   ID of 0x56-0x58 represents a BMP 280,\n");
        Serial.print("        ID of 0x60 represents a BME 280.\n");
        Serial.print("        ID of 0x61 represents a BME 680.\n");
        while (1) delay(10);
    }
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,     /* Operating Mode. */
                  Adafruit_BMP280::SAMPLING_X2,     /* Temp. oversampling */
                  Adafruit_BMP280::SAMPLING_X16,    /* Pressure oversampling */
                  Adafruit_BMP280::FILTER_X16,      /* Filtering. */
                  Adafruit_BMP280::STANDBY_MS_500); /* Standby time. */
}

void loopBaro(){
    Serial.print(F("Temperature = "));
    Serial.print(bmp.readTemperature());
    Serial.println(" *C");

    Serial.print(F("Pressure = "));
    Serial.print(bmp.readPressure());
    Serial.println(" Pa");

    Serial.print(F("Approx altitude = "));
    Serial.print(bmp.readAltitude(1013.25)); /* Adjusted to local forecast! */
    Serial.println(" m");

    Serial.println();
}
float getHeight(){
    return bmp.readAltitude(atmosphericpressure);
}
void loopApogee(){
    currentMillis2 = millis();
    if(currentMillis2 - previousMillis2 > interval2){
        currentHeight = getHeight();
        writeLog("Current Height: " + String(currentHeight));
        writeLog("Current Apogee: " + String(maxHeight));
        if(currentHeight > maxHeight){
            maxHeight = currentHeight;
            writeLog("New Apogee: " + String(maxHeight));
        }
    }
}