class PID {
    public:
    PID(double, double, double, double, double, double);
    float computePID();
    void set(double, double, double);
    private:
    void init();
    double kP;
    double kI;
    double kD;
    double pidOutput;
    double pidInput;
    double pidSetpoint;
    double outputSum, lastInput;
    unsigned long lastTime;
    unsigned long sampleTime;
};