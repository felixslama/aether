#include <Arduino.h>
#include "aetherLora.h"

const int buttonPin = 19;

void initButton(){
    pinMode(buttonPin,INPUT);
}

void loopButton(){
    int buttonState;
    buttonState = digitalRead(buttonPin);
    if(buttonState == HIGH){
        sendLora("ENGINE KILL");
        Serial.println("button pressed");
    }
}