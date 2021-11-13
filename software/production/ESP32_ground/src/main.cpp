#include <SPI.h>
#include <LoRa.h>
#include <Wire.h>
#include "aetherLora.h"
#include "aetherWebinterface.h"

void setup() {
  Serial.begin(115200);
  initLora();
  initWeb();
  delay(2000);
  sendLora("end of setup");
}

void loop() {
  
  loopWeb();
  // if(readLora() == "pls work"){
  //   Serial.print("worked i guess");
  // }
}