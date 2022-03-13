#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "Comms.h"
#include <PID_v1.h>
#include "Logs.h"

// general Vars
int i;
float yawA, rollA, pitchA;
bool collectedSamples = false;
double InputPitch, OutputPitchP, OutputPitchN;
double InputRoll, OutputRollP, OutputRollN;
bool controlReady = false;

// Pins
const int servoPinRoll1 = 25;
const int servoPinRoll2 = 15;
const int servoPinPitch1 = 4;
const int servoPinPitch2 = 2;

// Servo
Servo servoRoll1;
Servo servoRoll2;
Servo servoPitch1;
Servo servoPitch2;

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

// Servo Init
void initServo() {
  servoPitch1.setPeriodHertz(300);
  servoPitch1.attach(servoPinPitch1, 0, 2500);
  servoPitch2.setPeriodHertz(300);
  servoPitch2.attach(servoPinPitch2, 0, 2500);
  servoRoll1.setPeriodHertz(300);
  servoRoll1.attach(servoPinRoll1, 0, 2500);
  servoRoll2.setPeriodHertz(300);
  servoRoll2.attach(servoPinRoll2, 0, 2500);
}

void initMPU() {
  Wire.begin();
  Wire.setClock(400000);
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

bool checkReadyStatus() {
  if (controlReady == true) {
    // put more checks here if necessary
    // this arms the ESC power loop, so things get dangerous!
    return true;
  } else {
    return false;
  }
}

void loopControl(){
  if (!dmpReady) {
    Serial.print(".");
    controlReady = false;
    return;
  }
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    controlReady = true;
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
      InputRoll = rollA;
      InputPitch = pitchA;
      rollPID.Compute();
      NrollPID.Compute();
      pitchPID.Compute();
      NpitchPID.Compute();
      /* 
        To Invert Servo Dir switch plus and minus.
        For example:
        NORMAL: (90+OutputRollP)-OutputRollN;
        REVERSE: (90-OutputRollP)+OutputRollN;
      */
      float pidRoll1 = (90+OutputRollP)-OutputRollN;
      float pidRoll2 = (90-OutputRollP)+OutputRollN;
      float pidPitch1 = (90-OutputPitchP)+OutputPitchN;
      float pidPitch2 = (90+OutputPitchP)-OutputPitchN;
      if (pidRoll1 <= limitRollMax && pidRoll1 >= limitRollMin) {
        servoRoll1.write(pidRoll1);
        servoRoll2.write(pidRoll2);
      }
      if (pidPitch1 <= limitPitchMax && pidPitch1 >= limitPitchMin) {
        servoPitch1.write(pidPitch1);
        servoPitch2.write(pidPitch2);
      }
    }
  }
}