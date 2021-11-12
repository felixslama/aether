#include <ESP32Servo.h> //import ESP32Servo library (get from manager)

Servo myservo1;  // create servo object to control a servo
int servoPin1 = 4;

Servo myservo2;  // create servo object to control a servo
int servoPin2 = 2;

float valueSOne = 90;
float valueSTwo = 90;
float modSOne = 0;
float modSTwo = 0;
bool start = true;

void setup() {
    Serial.begin(115200);
    myservo1.attach(servoPin1, 0, 2500); // attaches the servo on pin 18 to the servo object
    myservo1.write(90);
    myservo2.attach(servoPin2, 0, 2500); // attaches the servo on pin 18 to the servo object
    myservo2.write(90);
    delay(2000);
}

void loop() {
  if (valueSOne > 55 && valueSTwo < 105 && valueSOne > 105 && valueSTwo > 55) {
    valueSOne = valueSOne - 0.5;
    valueSTwo = valueSTwo + 0.5;
    myservo1.write(valueSOne);
    myservo2.write(valueSTwo);
    delay (250);
    Serial.println(valueSOne);
    Serial.println(valueSTwo);
  }
}
