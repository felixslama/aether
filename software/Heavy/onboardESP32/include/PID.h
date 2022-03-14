class PID {
    public:
        PID(double*, double*, double*);
        bool computePID();
        void set(double SkP, double SkI, double SkD, bool dirChange);
    private:
        double kP, kI, kD;
        double *pidOutput, *pidInput, *pidSetpoint;
        double outputSum, lastInput;
        bool isReversed;
        unsigned long lastTime, sampleTime;
};
