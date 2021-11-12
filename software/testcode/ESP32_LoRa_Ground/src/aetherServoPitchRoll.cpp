#include <Arduino.h>
#include <ESP32Servo.h>
#include "MPU6050_6Axis_MotionApps20.h"

#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif
MPU6050 mpu;
#define OUTPUT_READABLE_YAWPITCHROLL

#define INTERRUPT_PIN 2  // use pin 2 on Arduino Uno & most boards
#define LED_PIN 13 // (Arduino is 13, Teensy is 11, Teensy++ is 6)
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

Servo ESC1;
Servo ESC2;

Servo myservo1;  // create servo object to control a servo
int servoPin1 = 4;

Servo myservo2;  // create servo object to control a servo
int servoPin2 = 2;

// MPU control/status vars
bool dmpReady = false;  // set true if DMP init was successful
uint8_t mpuIntStatus;   // holds actual interrupt status byte from MPU
uint8_t devStatus;      // return status after each device operation (0 = success, !0 = error)
uint16_t packetSize;    // expected DMP packet size (default is 42 bytes)
uint16_t fifoCount;     // count of all bytes currently in FIFO
uint8_t fifoBuffer[64]; // FIFO storage buffer

// orientation/motion vars
Quaternion q;           // [w, x, y, z]         quaternion container
VectorInt16 aa;         // [x, y, z]            accel sensor measurements
VectorInt16 aaReal;     // [x, y, z]            gravity-free accel sensor measurements
VectorInt16 aaWorld;    // [x, y, z]            world-frame accel sensor measurements
VectorFloat gravity;    // [x, y, z]            gravity vector
float euler[3];         // [psi, theta, phi]    Euler angle container
float ypr[3];           // [yaw, pitch, roll]   yaw/pitch/roll container and gravity vector

// packet structure for InvenSense teapot demo
uint8_t teapotPacket[14] = { '$', 0x02, 0,0, 0,0, 0,0, 0,0, 0x00, 0x00, '\r', '\n' };

volatile bool mpuInterrupt = false;     // indicates whether MPU interrupt pin has gone high
void dmpDataReady() {
    mpuInterrupt = true;
}

void initServoPitchRoll(){
    //Serial.begin(115200);
    
    myservo1.setPeriodHertz(300);    // standard 50 hz servo
    myservo1.attach(servoPin1, 0, 2500); // attaches the servo on pin 18 to the servo object
    myservo2.setPeriodHertz(300);    // standard 50 hz servo
    myservo2.attach(servoPin2, 0, 2500); // attaches the servo on pin 18 to the servo object
    
    // join I2C bus (I2Cdev library doesn't do this automatically)
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
    #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
        Fastwire::setup(400, true);
    #endif
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    delay(1000);

    devStatus = mpu.dmpInitialize();
    
    // supply your own gyro offsets here, scaled for min sensitivity
    mpu.setXGyroOffset(220);
    mpu.setYGyroOffset(76);
    mpu.setZGyroOffset(-85);
    mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

    // make sure it worked (returns 0 if so)
    if (devStatus == 0) {
        // Calibration Time: generate offsets and calibrate our MPU6050
        mpu.CalibrateAccel(6);
        mpu.CalibrateGyro(6);
        mpu.PrintActiveOffsets();
        mpu.setDMPEnabled(true);
        
        attachInterrupt(digitalPinToInterrupt(INTERRUPT_PIN), dmpDataReady, RISING);
        mpuIntStatus = mpu.getIntStatus();

        dmpReady = true;

        // get expected DMP packet size for later comparison
        packetSize = mpu.dmpGetFIFOPacketSize();
    }
    ESC1.attach(12,1000,2000); // (pin, min pulse width, max pulse width in microseconds) 
    ESC2.attach(14,1000,2000); // (pin, min pulse width, max pulse width in microseconds)
    Serial.println("Wating 2 seconds for ESC");
    delay(2000);
    ESC1.write(0);    // Send the signal to the ESC
    ESC2.write(0);    // Send the signal to the ESC
    Serial.println("Wait 2 seconds befor motor start");
    delay(2000);

}
void loopServoPitchRoll(){
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
                myservo1.write(endValueR);
              }
              if (endValueP > 75 && endValueP < 105) {
                myservo2.write(endValueP);
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
        ESC1.write(value);    // Send the signal to the ESC
        ESC2.write(value);    // Send the signal to the ESC
        Serial.println(value);      
      }
    }
}