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

// general Vars
int i;
float value = 0;
float modifier = 0.5;
float yawA, rollA, pitchA;
bool once = true;
bool holdEngine = false;
bool killed = false;
bool runLoop = true;
bool collectedSamples = false;
long previousMillis = 0;
long interval = 50;   
double InputPitch, OutputPitchP, OutputPitchN;
double InputRoll, OutputRollP, OutputRollN;

// Pins
const int servoPinRoll1 = 4;
const int servoPinRoll2 = 2;
const int servoPinPitch1 = 25;
const int servoPinPitch2 = 15;
const int escPin1 = 12;
const int escPin2 = 23;

// ESCs
Servo ESC1;
Servo ESC2;

// Servo
Servo servoRoll1;
Servo servoRoll2;
Servo servoPitch1;
Servo servoPitch2;

// Inverters per Axis
const int inverterR1 = 1;
const int inverterR2 = -1;
const int inverterP1 = 1;
const int inverterP2 = -1;

// Axis Limiters
const int limitRollMin = 70;
const int limitRollMax = 110;
const int limitPitchMin = 70;
const int limitPitchMax = 110;

// PID
// tuning params Roll
float rKp=0.9, rKi=0, rKd=0;
// tuning params Pitch
float pKp=0.9, pKi=0, pKd=0;
// Setpoint (value to maintain)
double Setpoint = 0;
// PIDloops
PID rollPID(&InputRoll, &OutputRollP, &Setpoint, rKp, rKi, rKd, DIRECT);
PID NrollPID(&InputRoll, &OutputRollN, &Setpoint, rKp, rKi, rKd, REVERSE);
PID pitchPID(&InputPitch, &OutputPitchP, &Setpoint, pKp, pKi, pKd, DIRECT);
PID NpitchPID(&InputPitch, &OutputPitchN, &Setpoint, pKp, pKi, pKd, REVERSE);

// Servo Init
void initServo(){
  servoPitch1.setPeriodHertz(300);
  servoPitch1.attach(servoPinPitch1, 0, 2500);
  servoPitch2.setPeriodHertz(300);
  servoPitch2.attach(servoPinPitch2, 0, 2500);
  servoRoll1.setPeriodHertz(300);
  servoRoll1.attach(servoPinRoll1, 0, 2500);
  servoRoll2.setPeriodHertz(300);
  servoRoll2.attach(servoPinRoll2, 0, 2500);
}

// i2cDev MPU Init
MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 3
bool blinkState = false;
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
  rollPID.SetMode(AUTOMATIC);
  NrollPID.SetMode(AUTOMATIC);
  rollPID.SetOutputLimits(0,30);
  NrollPID.SetOutputLimits(0,30);
  pitchPID.SetMode(AUTOMATIC);
  NpitchPID.SetMode(AUTOMATIC);
  NpitchPID.SetOutputLimits(0,30);
  pitchPID.SetOutputLimits(0,30);
}

// ESC Init
void initESC() {
  ESC1.attach(escPin1,1000,2000);
  ESC2.attach(escPin2,1000,2000);
  writeLog("2s for ESC");
  delay(2000);
  ESC1.write(0);
  ESC2.write(0);
  writeLog("2s to motor start");
  writeLog("ESC init done");
  delay(2000);
}

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
      float pidRoll = (90+OutputRollP)-OutputRollN;
      float pidPitch = (90+OutputPitchP)-OutputPitchN;
      if (pidRoll <= limitRollMax && pidRoll >= limitRollMin) {
        servoRoll1.write(pidRoll*inverterR1);
        servoRoll2.write(pidRoll*inverterR2);
      }
      if (pidPitch <= limitPitchMax && pidPitch >= limitPitchMin) {
        servoPitch1.write(pidPitch*inverterP1);
        servoPitch2.write(pidPitch*inverterP2);
      }
      writeLog("Pitch: " + String(pidPitch) + " Roll: " + String(pidRoll));
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

// turn off ESC
void escOFF(){
  collectedSamples = false;
  ESC1.write(0);
  ESC2.write(0);
}

// hold ESC value
void escHold(){
  holdEngine = true;
}

// stop holding ESC value
void escNoHold(){
  holdEngine = false;
}

// terminate ESCs
void escKill(){
  if(killed == false){
    Serial.println("Called KILL");
    runLoop = false;
    ESC1.write(0);
    ESC2.write(0);
    Serial.println(ESC1.read());
    Serial.println(ESC2.read());
    //sendLora("Killed");
    killed = true;
  }else{
    Serial.println("already dead");
  }
}

void escON(){
  // implement engine on
}