#include <Arduino.h>
#include "aetherLora.h"
const buttonPin = 19;
void initButton(){
    pinMode(buttonPin,INPUT);
}

void loopButton(){
    int buttonState;
    buttonState = digitalRead(buttonPin);
    if(buttonState == LOW){
        sendLora("ENGINE KILL");
        Serial.println("button pressed");
    }
}