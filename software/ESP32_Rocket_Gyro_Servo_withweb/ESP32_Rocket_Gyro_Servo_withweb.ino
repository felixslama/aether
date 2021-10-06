/*
 CREDITS FOR THE BEAUTIFUL LIBRARIES:
 https://github.com/jrowberg/i2cdevlib
 https://github.com/madhephaestus/ESP32Servo
*/

#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include "I2Cdev.h"
#include <ESP32Servo.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "index.h"

/*
 ############################################
 ##############   SERVO PART   ##############
 ############################################
*/

Servo myservo;
int servoPin;
int servoMapYaw;
int servoMapPitch;
int servoMapRoll;
float calcValue;

/*
 ############################################
 ##############   WEB PART     ##############
 ############################################
*/

const char* ssid     = "Rocket Telemetry";
const char* password = "rocketgobrr";
WebServer server(80);

void handleRoot() {
 String s = MAIN_page;
 server.send(200, "text/html", s);
}

void handleGyroReading() {
 float a = calcValue;
 String adcValue = String(a);
 server.send(200, "text/plane", adcValue);
}

void handleOn() { 
 Serial.println("ON");
 handleRoot();
}

void handleOff() { 
 Serial.println("OFF");
 handleRoot();
}

/*
 ############################################
 ##############   GYRO PART    ##############
 ############################################
*/

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

MPU6050 mpu;

#define OUTPUT_READABLE_YAWPITCHROLL

#define INTERRUPT_PIN 2
#define LED_PIN 13
bool blinkState = false;

float yawA, rollA, pitchA;
float correct;
int i;

float correctOverdrive = 1.005;

bool dmpReady = false;
uint8_t mpuIntStatus;
uint8_t devStatus;
uint16_t packetSize;
uint16_t fifoCount;
uint8_t fifoBuffer[64];
Quaternion q;
VectorInt16 aa;
VectorInt16 aaReal;
VectorInt16 aaWorld;
VectorFloat gravity;
float euler[3];
float ypr[3];
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };
volatile bool mpuInterrupt = false;
void dmpDataReady() {
    mpuInterrupt = true;
}

/*
 ############################################
 ##############   SETUP PART   ##############
 ############################################
*/

void setup() {
  //Start Serial
  Serial.begin(115200);
  
  //attach servo
  myservo.attach(servoPin);
  
  //Start AP
  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  server.on("/", handleRoot);
  server.on("/readGyro", handleGyroReading);
  server.on("/On", handleOn);
  server.on("/Off", handleOff);
  server.begin();
  Serial.println("HTTP server started");
  
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
      Wire.begin();
      Wire.setClock(400000); //i2c clockrate
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
      Fastwire::setup(400, true);
  #endif
  mpu.initialize();
  pinMode(INTERRUPT_PIN, INPUT);
  Serial.println("Initializing in 3 Seconds!");
  Serial.println("3...");
  delay(1000);
  Serial.println("2...");
  delay(1000);
  Serial.println("1...");
  delay(1000);
  Serial.println(F("Initializing DMP..."));
  devStatus = mpu.dmpInitialize();
  
  //offsets
  mpu.setXGyroOffset(220);
  mpu.setYGyroOffset(76);
  mpu.setZGyroOffset(-85);
  mpu.setZAccelOffset(1788);
  if (devStatus == 0) {
      mpu.CalibrateAccel(6);
      mpu.CalibrateGyro(6);
      mpu.PrintActiveOffsets();
      Serial.println(F("Enabling DMP..."));
      mpu.setDMPEnabled(true);
      Serial.print(F("Enabling interrupt detection (Arduino external interrupt "));
      Serial.print(digitalPinToInterrupt(INTERRUPT_PIN));
      Serial.println(F(")..."));
      attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
      mpuIntStatus = mpu.getIntStatus();
      Serial.println(F("DMP ready! Waiting for first interrupt..."));
      dmpReady = true;
      packetSize = mpu.dmpGetFIFOPacketSize();
  } else {
      Serial.print(F("DMP Initialization failed (code "));
      Serial.print(devStatus);
      Serial.println(F(")"));
  }
}

void handleGyro() {
    if (!dmpReady) return;
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            if (i <= 300) {
              correct = ypr[0];
              i++;
            } else {
              yawA = (ypr[0] * 180/M_PI);
              pitchA = (ypr[1] * 180/M_PI);
              rollA = (ypr[2] * 180/M_PI);
              servoMapYaw = map(yawA, 90, -90, 180, 0);
              servoMapPitch = map(pitchA, 90, -90, 180, 0);
              servoMapRoll = map(rollA, 90, -90, 180, 0);
              calcValue = pow(correctOverdrive, (servoMapRoll - 90));
              myservo.write(servoMapRoll*calcValue);
            }
        #endif
        // blink LED to indicate activity
        blinkState = !blinkState;
        digitalWrite(LED_PIN, blinkState);
    }
}

/*
 ############################################
 ##############   MAIN LOOP    ##############
 ############################################
*/

void loop() {
    server.handleClient();
    handleGyro();
}
