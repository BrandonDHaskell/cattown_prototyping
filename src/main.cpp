#include <Arduino.h>
#include <TMCStepper.h>
#include <SoftwareSerial.h>

// Initialize constants
#define EN_PIN              4       // Enable
#define STEP_PIN            3       // Step
#define DIR_PIN             2       // Direction
#define TX_PIN              1       // UART TX pin
#define RX_PIN              0       // UART RX pin
#define DRIVER_ADDRESS      0b00    // Default address of driver
#define R_SENSE             0.11f   // Match to your driver:
                                        // SilentStepStick series use 0.11
                                        // UltiMachine Einsy and Archim2 boards use 0.2
                                        // Panucatt BSD2660 uses 0.1
                                        // Watterott TMC5160 uses 0.075
const long BAUD_RATE = 115200;        // Baud rate for comms
const uint16_t RMS_CURRENT = 1100;   // Driver current in mA

// Initialize components
// SoftwareSerial TMCSerial(RX_PIN, TX_PIN);
TMC2209Stepper driver = TMC2209Stepper(RX_PIN, TX_PIN, R_SENSE, DRIVER_ADDRESS);

void runMotor(bool direction, unsigned long duration) {
    digitalWrite(DIR_PIN, direction ? HIGH : LOW); // Set direction
    unsigned long startTime = millis();
    while (millis() - startTime < duration) {
        digitalWrite(STEP_PIN, HIGH);   // Generate a step
        delayMicroseconds(90);         // This determines the speed, adjust as necessary
        digitalWrite(STEP_PIN, LOW);
        delayMicroseconds(90);         // This determines the speed, adjust as necessary
    }
}

void enableStealthChop(bool enable) {
  driver.en_spreadCycle(!enable);  // Disable SpreadCycle if enabling StealthChop
  driver.pwm_autoscale(enable);    // Enable automatic current scaling
  driver.pwm_freq(1);              // Set PWM frequency
  driver.pwm_grad(8);              // User defined amplitude gradient
  driver.pwm_ofs(64);             // User defined amplitude offset
  // More configurations can be added here based on your needs
  Serial.println(enable ? "StealthChop Enabled" : "StealthChop Disabled");
}

void enableSpreadCycle(bool enable) {
  driver.en_spreadCycle(enable);   // Enable SpreadCycle
  // Configure SpreadCycle parameters as needed
  driver.TCOOLTHRS(0xFFFFF);        // Set threshold speed for SpreadCycle
//   driver.THIGH(0);                  // Set to zero to turn off coolStep
//   driver.chm(1);                    // Use constant off time mode
//   driver.tbl(2);                    // Set comparator blank time to 24 clocks
  // More configurations can be added here based on your needs
  Serial.println(enable ? "SpreadCycle Enabled" : "SpreadCycle Disabled");
}

void setup() {
    Serial.begin(BAUD_RATE);        // Initialize Serial comms for debugging
    pinMode(STEP_PIN, OUTPUT);      // Set step pin
    pinMode(DIR_PIN, OUTPUT);       // Set direction pin
    pinMode(EN_PIN, OUTPUT);        // Set enable pin
    
    driver.begin();                 // Initialize TMC2209 driver
    driver.rms_current(RMS_CURRENT);
    driver.microsteps(1);          // Set microstepping to 1/4

    // StealthChop Configuration
    // enableStealthChop(true);

    // SpreadCycle Configuration (if you want to start with SpreadCycle enabled, set to true)
    // enableSpreadCycle(false); // Starting with StealthChop enabled
    
    Serial.println("Current set");
    
    Serial.println("Setup complete");
}

void loop() {
    enableSpreadCycle(true);
    runMotor(true, 1000);           // Run motor forward for 1000 ms
    runMotor(false, 1000);          // Run motor in reverse for 1000 ms
    delay(1000);                    // Pause for 1 second

    // if (Serial.available() > 0) {
    //     String command = Serial.readStringUntil('\n');
    //     if (command == "stealthChop") {
    //         enableStealthChop(true);
    //     } else if (command == "spreadCycle") {
    //         enableSpreadCycle(true);
    //     } else if (command == "runForward") {
    //         runMotor(true, 1000); // Run motor forward for 1000 ms
    //     } else if (command == "runReverse") {
    //         runMotor(false, 1000); // Run motor in reverse for 1000 ms
    //     }
    //     delay(100); // Small delay to debounce and process next command
    // }
}
