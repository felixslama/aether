class PID {
    public:
        PID(double, double, double);
        bool computePID();
        void set(double, double, double);
    private:
        double kP, kI, kD;
        double pidOutput, pidInput, pidSetpoint;
        double outputSum, lastInput;
        unsigned long lastTime, sampleTime;
};
