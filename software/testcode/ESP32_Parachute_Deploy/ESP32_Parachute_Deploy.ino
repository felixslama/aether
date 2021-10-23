#include <ESP32Servo.h> //import ESP32Servo library (get from manager)

Servo myservo;  // create servo object to control a servo
int servoPin = 32; // servo pin (GPIO 18)
float value = 0;
bool start = true;

void setup() {
    Serial.begin(115200);
    myservo.attach(servoPin, 0, 3500); // attaches the servo on pin 18 to the servo object
    myservo.write(0);
    delay(2000);
}

void loop() {
    if (start == false && value <= 0) {
      start = true;
    }
    if (value < 180 && start == true) {
      myservo.write(value);
      Serial.println(value);
      value = value + 0.5;
      delay (5);
    } else {
      start = false;
      value = value - 0.5;
      myservo.write(value);
      Serial.println(value);
      delay (5);
    }
}
