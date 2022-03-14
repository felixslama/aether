#include <Arduino.h>
#include "PID.h"

PID::PID(double Input, double Output, double Setpoint) {
    pidOutput = Output;
    pidInput = Input;
    pidSetpoint = Setpoint;
    outputSum = pidOutput;
    lastInput = pidInput;
    sampleTime = 100;
    lastTime = millis() - sampleTime;
}

void PID::set(double SkP, double SkI, double SkD)
{
   if (SkP<0 || SkI<0 || SkD<0) return;
   double sampleTimeInSec = (sampleTime/1000);
   kP = SkP;
   kI = SkI * sampleTimeInSec;
   kD = SkD / sampleTimeInSec;
}

bool PID::computePID() {
    unsigned long now = millis();
    unsigned long timeChange = (now - lastTime);
    if (timeChange >= sampleTime) {
        double output;
        double input = pidInput;
        double error = pidSetpoint - input;
        double dInput = (input - lastInput);
        outputSum += (kI * error);
        output = kP * error;
        output += outputSum - kD * dInput;
        pidOutput = output;
        lastInput = input;
        lastTime = now;
        return true;
    } 
    else return false;
}
