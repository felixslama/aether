#include <Arduino.h>
#include <ESP32Servo.h>
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include "aetherLora.h"
#include <PID_v1.h>
#include "aetherLog.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// MPU

//create gyro object
MPU6050 mpu;

// general purpose vars

int i;
float value = 0;
float modifier = 0.5;
bool once = true;
long previousMillis = 0;
long interval = 50;   
bool killed = false;
bool runLoop = true;
double InputPitch, OutputPitchP, OutputPitchN;
double InputRoll, OutputRollP, OutputRollN;
float servoRoll, servoPitch;

// ESC

//two servo objects for ESC's (PWM)
Servo ESC1;
Servo ESC2;

// Servo

//pin assignments
Servo myservo1;
int servoPin1 = 4;
Servo myservo2;
int servoPin2 = 2;
Servo myservo3;
int servoPin3 = 25;
Servo myservo4;
int servoPin4 = 15;

// PID

//tuning params Roll
float rKp=1.1, rKi=0, rKd=0;
//tuning params Pitch
float pKp=1.1, pKi=0, pKd=0;
// Setpoint (value to maintain)
double Setpoint = 0;

//pid loops assignment for positive/negative Roll
PID rollPID(&InputRoll, &OutputRollP, &Setpoint, rKp, rKi, rKd, DIRECT);
PID NrollPID(&InputRoll, &OutputRollN, &Setpoint, rKp, rKi, rKd, REVERSE);
//pid loops assignment for positive/negative Pitch
PID pitchPID(&InputPitch, &OutputPitchP, &Setpoint, pKp, pKi, pKd, DIRECT);
PID NpitchPID(&InputPitch, &OutputPitchN, &Setpoint, pKp, pKi, pKd, REVERSE);

// MPU

#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 3
//#define LED_PIN 13
bool blinkState = false;
bool collectedSamples = false;
float yawA, rollA, pitchA;
float correct;
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
    myservo1.setPeriodHertz(300);
    myservo1.attach(servoPin1, 0, 2500);
    myservo2.setPeriodHertz(300);
    myservo2.attach(servoPin2, 0, 2500);
    myservo3.setPeriodHertz(300);
    myservo3.attach(servoPin3, 0, 2500);
    myservo4.setPeriodHertz(300);
    myservo4.attach(servoPin4, 0, 2500);
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
  
  // further PID configs

  rollPID.SetMode(AUTOMATIC);
  NrollPID.SetMode(AUTOMATIC);
  rollPID.SetOutputLimits(0,30);
  NrollPID.SetOutputLimits(0,30);
  pitchPID.SetMode(AUTOMATIC);
  NpitchPID.SetMode(AUTOMATIC);
  NpitchPID.SetOutputLimits(0,30);
  pitchPID.SetOutputLimits(0,30);
}

//ESC Init

void initESC() {
    ESC1.attach(12,1000,2000);
    ESC2.attach(23,1000,2000);
    writeLog("Wating 2 seconds for ESC");
    delay(2000);
    ESC1.write(0);
    ESC2.write(0);
    writeLog("Wait 2 seconds befor motor start");
    writeLog("Init esc done");
    delay(2000);
}

//Main Loop

bool holdEngine = false;

void loopControl(){
    if (!dmpReady) {
      Serial.println("noready");
      return;
    }
    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
        if (i <= 300) {
          Serial.print("Collecting Samples...");
          correct = ypr[0];
          i++;
        } else {
          collectedSamples = true;
          pitchA = (ypr[1] * 180/M_PI);
          rollA = (ypr[2] * 180/M_PI);
          writeLog("Roll: " + String(rollA) + " Pitch:" + String(pitchA));
          InputRoll = rollA;
          InputPitch = pitchA;
          rollPID.Compute();
          NrollPID.Compute();
          pitchPID.Compute();
          NpitchPID.Compute();
          servoRoll = (90+OutputRollP)-OutputRollN;
          servoPitch = (90-OutputPitchP)+OutputPitchN;
          float servoRoll2 = (90-OutputRollP)+OutputRollN;
          float servoPitch2 = (90+OutputPitchP)-OutputPitchN;
          myservo1.write(servoRoll);
          myservo2.write(servoPitch);
          myservo3.write(servoRoll2);
          myservo4.write(servoPitch2);
          writeLog("Servo 1: " + String(servoRoll) + " Servo 2: " + String(servoPitch) + " Servo 3: " + String(servoRoll2) + " Servo4: " + String(servoPitch2));
        }
    }
    
    unsigned long currentMillis = millis();
    if(runLoop == true){
        if(currentMillis - previousMillis > interval) {
          previousMillis = currentMillis;
          if (collectedSamples == true) {
            if(holdEngine == false){
              if (value > 180 && once == true) {
                once = false;
                modifier = -0.5;
              }
              if (value < 0 && once == false) {
                modifier = 0; \
              }
              value = value + modifier;
            }
            
            ESC1.write(value);
            ESC2.write(value);
            Serial.println(value);
            writeLog("ESC Value: " + String(value));
          }
        }
    } 
}

void escOFF(){
    collectedSamples = false;
    ESC1.write(0);
    ESC2.write(0);
}

void escHold(){
  holdEngine = true;
}

void escNoHold(){
  holdEngine = false;
}

void escKill(){
  if(killed == false){
    Serial.println("Called KILL");
    runLoop = false;
    ESC1.write(0);
    ESC2.write(0);
    Serial.println(ESC1.read());
    Serial.println(ESC2.read());
    sendLora("Killed");
    killed = true;
  }else{
    Serial.println("already dead");
  }
  //implement engine kill
}

void escON(){
  //implement engine on
}