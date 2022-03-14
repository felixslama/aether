#include <Arduino.h>
#include <ESP32Servo.h>
#include <Wire.h>
#include "MPU6050_6Axis_MotionApps20.h"
#include "Comms.h"
#include "PID.h"
#include "Logs.h"
#include "Control.h"

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
double InputPitch, OutputPitch;
double InputRoll, OutputRoll;
double Setpoint = 90;
PID rollPID(InputRoll, OutputRoll, Setpoint);
PID pitchPID(InputPitch, OutputPitch, Setpoint);

// MPU
MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL
#define INTERRUPT_PIN 3
int sampleCount;
bool controlReady = false;
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

// MPU init
void dmpDataReady() {
  mpuInterrupt = true;
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
}

// PID init
void initPID() {
  // ( kP, kI, kD )
  rollPID.set(1,0,0);
  pitchPID.set(1,0,0);
}

// status check
bool checkReadyStatus() {
  if (controlReady == true) {
    // put more checks here if necessary
    // this arms the ESC power loop, so things get dangerous!
    return true;
  } else {
    return false;
  }
}

//main control loop
void loopControl(){
  if (!dmpReady) {
    controlReady = false;
    return;
  }
  if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {
    mpu.dmpGetQuaternion(&q, fifoBuffer);
    mpu.dmpGetGravity(&gravity, &q);
    mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);
    if (sampleCount <= 300) {
      Serial.print("._.-");
      correct = ypr[0];
      sampleCount++;
    } else {
      // assume control loop is ready to operate
      controlReady = true;
      // get degree input from axis to PID controller
      InputPitch = (ypr[1] * 180/M_PI);
      InputRoll = (ypr[2] * 180/M_PI);
      // compute PID values based on degree vals
      rollPID.computePID();
      pitchPID.computePID();
      if (OutputRoll <= limitRollMax && OutputRoll >= limitRollMin) {
        servoRoll1.write(OutputRoll);
        servoRoll2.write(OutputRoll);
      }
      if (OutputPitch <= limitPitchMax && OutputPitch >= limitPitchMin) {
        servoPitch1.write(OutputPitch);
        servoPitch2.write(OutputPitch);
      }
    }
  }
}
