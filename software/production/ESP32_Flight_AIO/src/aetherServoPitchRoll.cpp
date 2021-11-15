#include <Arduino.h>
#include <ESP32Servo.h>
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL

#define INTERRUPT_PIN 2
#define LED_PIN 13
bool blinkState = false;
bool collectedSamples = false;
float yawA, rollA, pitchA;
float correct;
int i;
float value = 0;
float modifier = 0.5;
bool once = true;
float correctOverdrive = 1.005;
long previousMillis = 0;
long interval = 50;   
float offsetRoll = 10;
float offsetPitch = 0;
float offsetYaw = 0;

// Servos

Servo ESC1;
Servo ESC2;

Servo servo1;
int servoPin1 = 4;

Servo servo2;
int servoPin2 = 2;

// MPU

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

//Servo Init

void initServo(){
    servo1.setPeriodHertz(300);
    servo1.attach(servoPin1, 0, 2500);
    servo2.setPeriodHertz(300);
    servo2.attach(servoPin2, 0, 2500);
}

//MPU Init

void initMPU() {
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000);
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    delay(1000);

    devStatus = mpu.dmpInitialize();
    
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788);

    if (devStatus == 0) {
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        mpu.setDMPEnabled(true);
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
}

//ESC Init

void initESC() {
    ESC1.attach(12,1000,2000);
    ESC2.attach(14,1000,2000);
    Serial.println("Wating 2 seconds for ESC");
    delay(2000);
    ESC1.write(0);
    ESC2.write(0);
    Serial.println("Wait 2 seconds befor motor start");
    delay(2000);
}

//Main Loop

void loopControl(){
    if (!dmpReady) {
      return;
    }
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) { // Get the Latest packet 
        #ifdef OUTPUT_READABLE_YAWPITCHROLL
            // display Euler angles in degrees
            mpu.dmpGetQuaternion(&q, fifoBuffer);
            mpu.dmpGetGravity(&gravity, &q);
            mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
            if (i <= 300) {
              Serial.print("Collecting Samples...");
              correct = ypr[0];
              i++;
            } else {
              collectedSamples = true;
              yawA = (ypr[0] * 180/M_PI);
              pitchA = (ypr[1] * 180/M_PI);
              rollA = (ypr[2] * 180/M_PI);
              int servoMapYaw = map(yawA, 90.00, -90.00, 180.00, 0.00);
              int servoMapPitch = map(pitchA, 90.00, -90.00, 180.00, 0.00);
              int servoMapRoll = map(rollA, 90.00, -90.00, 180.00, 0.00);
              float calcValueR = pow(correctOverdrive, (servoMapRoll - 90));
              float calcValueP = pow(correctOverdrive, (servoMapPitch - 90));
              float endValueR = (servoMapRoll*calcValueR)+offsetRoll;
              float endValueP = (servoMapPitch*calcValueP)+offsetPitch;
              if (endValueR > 75 && endValueR < 125) {
                servo1.write(endValueR);
              }
              if (endValueP > 75 && endValueP < 105) {
                servo2.write(endValueP);
              }
            }
        #endif
    }

    unsigned long currentMillis = millis();

    if(currentMillis - previousMillis > interval) {
      previousMillis = currentMillis;
      if (collectedSamples == true) {
        if (value > 180 && once == true) {
          once = false;
          modifier = -0.5;
        }
        if (value < 0 && once == false) {
          modifier = 0; 
        }
        value = value + modifier;
        ESC1.write(value);
        ESC2.write(value);
        Serial.println(value);      
      }
    }
}