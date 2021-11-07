/*
        Arduino Brushless Motor Control
     by Dejan, https://howtomechatronics.com
*/

#include <ESP32Servo.h>

Servo ESC1;
Servo ESC2;

float value = 0;
float modifier = 0.5;
bool once = true;

void setup() {
  Serial.begin(115200);
  ESC1.attach(12,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
  ESC2.attach(14,1000,2000); // (pin, min pulse width, max pulse width in microseconds)
  Serial.println("Wating 10 seconds for ESC");
  delay(10000);
  ESC1.write(0);    // Send the signal to the ESC
  ESC2.write(0);    // Send the signal to the ESC
  Serial.println("Wait 5 seconds befor motor start");
  delay(5000);
}

void loop() {
  if (value > 180 && once == true) {
    once = false;
    modifier = -0.5;
  }
  value = value + modifier;
  Serial.println(value);
  ESC1.write(value);    // Send the signal to the ESC
  ESC2.write(value);    // Send the signal to the ESC
  delay(50);
}
