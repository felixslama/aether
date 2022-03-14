#include <Arduino.h>
#include "PID.h"

PID::PID(double* Input, double* Output, double* Setpoint) {
    pidOutput = Output;
    pidInput = Input;
    pidSetpoint = Setpoint;
    outputSum = *pidOutput;
    lastInput = *pidInput;
    sampleTime = 100;
    lastTime = millis() - sampleTime;
}

void PID::set(double SkP, double SkI, double SkD, bool dirChange)
{
   if (SkP<0 || SkI<0 || SkD<0) return;
   double sampleTimeInSec = (double)sampleTime/1000;
   isReversed = dirChange;
   kP = SkP;
   kI = SkI * sampleTimeInSec;
   kD = SkD / sampleTimeInSec;
}

bool PID::computePID() {
    unsigned long now = millis();
    unsigned long timeChange = (now - lastTime);
    if (timeChange >= sampleTime) {
        double output;
        double input = *pidInput;
        double error = *pidSetpoint - input;
        double diffInput = (input - lastInput);
        outputSum += (kI * error);
        output = (kP * error);
        output += outputSum - (kD * diffInput);
        if (isReversed == true) {
            *pidOutput = output*(-1) + *pidSetpoint;
        } else {
            *pidOutput = output + *pidSetpoint;
        }
        lastInput = input;
        lastTime = now;
        return true;
    }
    else return false;
}
