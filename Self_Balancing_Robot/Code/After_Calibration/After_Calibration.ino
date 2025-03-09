#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
#include <PID_v1.h>

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

// MPU6050 and motor control
MPU6050 mpu;
#define INTERRUPT_PIN 2  // MPU interrupt pin

// MPU control/status vars
bool dmpReady = false; 
uint8_t mpuIntStatus;
uint8_t devStatus;      
uint16_t packetSize;    
uint16_t fifoCount;     
uint8_t fifoBuffer[64]; 

// Orientation vars
Quaternion q; 
VectorFloat gravity;
float ypr[3]; 

// PID variables
#define PID_MIN_LIMIT -255
#define PID_MAX_LIMIT 255
#define PID_SAMPLE_TIME_IN_MILLI 10
#define SETPOINT_PITCH_ANGLE_OFFSET -2 
#define MIN_ABSOLUTE_SPEED 0

double setpointPitchAngle = SETPOINT_PITCH_ANGLE_OFFSET;
double pitchGyroAngle = 0;
double pitchPIDOutput = 0;

#define PID_PITCH_KP_NORMAL 18
#define PID_PITCH_KI_NORMAL 0
#define PID_PITCH_KD_NORMAL 0.5

#define PID_PITCH_KP_OVERSHOOT 15 // Higher gain for overshoot correction
#define PID_PITCH_KI_OVERSHOOT 0
#define PID_PITCH_KD_OVERSHOOT 0.7  // Higher derivative gain

PID pitchPIDNormal(&pitchGyroAngle, &pitchPIDOutput, &setpointPitchAngle, PID_PITCH_KP_NORMAL, PID_PITCH_KI_NORMAL, PID_PITCH_KD_NORMAL, DIRECT);
PID pitchPIDOvershoot(&pitchGyroAngle, &pitchPIDOutput, &setpointPitchAngle, PID_PITCH_KP_OVERSHOOT, PID_PITCH_KI_OVERSHOOT, PID_PITCH_KD_OVERSHOOT, DIRECT);

// Motor pins
int enableMotor1 = 9;
int motor1Pin1 = 6;
int motor1Pin2 = 5;
int motor2Pin1 = 7;
int motor2Pin2 = 8;
int enableMotor2 = 10;

volatile bool mpuInterrupt = false; // MPU interrupt flag

void dmpDataReady() {
    mpuInterrupt = true;
}

// Setup PID control
void setupPID() {
    pitchPIDNormal.SetOutputLimits(PID_MIN_LIMIT, PID_MAX_LIMIT);
    pitchPIDNormal.SetMode(AUTOMATIC);
    pitchPIDNormal.SetSampleTime(PID_SAMPLE_TIME_IN_MILLI);

    pitchPIDOvershoot.SetOutputLimits(PID_MIN_LIMIT, PID_MAX_LIMIT);
    pitchPIDOvershoot.SetMode(AUTOMATIC);
    pitchPIDOvershoot.SetSampleTime(PID_SAMPLE_TIME_IN_MILLI);
}

// Setup motor control
void setupMotors() {
    pinMode(enableMotor1, OUTPUT);
    pinMode(motor1Pin1, OUTPUT);
    pinMode(motor1Pin2, OUTPUT);
    pinMode(enableMotor2, OUTPUT);
    pinMode(motor2Pin1, OUTPUT);
    pinMode(motor2Pin2, OUTPUT);
    rotateMotor(0); // Stop motors at the beginning
}

// Setup MPU6050
void setupMPU() {
    #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
        Wire.begin();
        Wire.setClock(400000); 
    #endif
    
    mpu.initialize();
    pinMode(INTERRUPT_PIN, INPUT);
    devStatus = mpu.dmpInitialize();

    // Set offsets for gyro and accelerometer
    mpu.setXAccelOffset(-5530);
    mpu.setYAccelOffset(63);
    mpu.setZAccelOffset(544);
    mpu.setXGyroOffset(103);
    mpu.setYGyroOffset(18);
    mpu.setZGyroOffset(-15);

    if (devStatus == 0) {
        mpu.setDMPEnabled(true);
        mpuIntStatus = mpu.getIntStatus();
        dmpReady = true;
        packetSize = mpu.dmpGetFIFOPacketSize();
    } else {
        // Handle initialization error
    }
}

// Main setup function
void setup() {
    Serial.begin(115200);
    setupMotors();   
    setupMPU();
    setupPID();
}

// Main loop
void loop() {
    if (!dmpReady) return;

    if (mpu.dmpGetCurrentFIFOPacket(fifoBuffer)) {  
        mpu.dmpGetQuaternion(&q, fifoBuffer);
        mpu.dmpGetGravity(&gravity, &q);
        mpu.dmpGetYawPitchRoll(ypr, &q, &gravity);

        pitchGyroAngle = ypr[1] * 180 / M_PI;  // Convert pitch to degrees

        // PID control based on pitch angle thresholds
        if (pitchGyroAngle > 6 || pitchGyroAngle < -6) {
            // Overshoot mode
            pitchPIDOvershoot.Compute();
        } else {
            // Normal PID mode
            pitchPIDNormal.Compute();
        }

        rotateMotor(pitchPIDOutput);  // Use the PID output to control motors

        // Debug information (optional)
        #ifdef PRINT_DEBUG_BUILD
            Serial.print("Pitch Angle: "); Serial.println(pitchGyroAngle);
            Serial.print("PID Output: "); Serial.println(pitchPIDOutput);
        #endif
    }
}

// Motor control function
void rotateMotor(int speed) {
    if (speed < 0) {
        digitalWrite(motor1Pin1, LOW);
        digitalWrite(motor1Pin2, HIGH);    
        digitalWrite(motor2Pin1, LOW);
        digitalWrite(motor2Pin2, HIGH);    
    } else {
        digitalWrite(motor1Pin1, HIGH);
        digitalWrite(motor1Pin2, LOW);      
        digitalWrite(motor2Pin1, HIGH);
        digitalWrite(motor2Pin2, LOW);      
    }

    speed = abs(speed) + MIN_ABSOLUTE_SPEED;
    speed = constrain(speed, MIN_ABSOLUTE_SPEED, 255);
    
    analogWrite(enableMotor1, speed);
    analogWrite(enableMotor2, speed);    
}
