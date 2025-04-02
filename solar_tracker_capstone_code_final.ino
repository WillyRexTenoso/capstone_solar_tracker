/*
  Solar Tracker Control Code
  ---------------------------
  Authors:
    Alyssa Lancione
    Ryan Annecca
    Tanner O'Leary
    Willy Rex Tenoso

  Date: February 6, 2025

  Description:
  Control of a dual-axis solar panel tracking system using two stepper motors
  and photodiode sensors. The system monitors light imbalance and adjusts panel
  orientation using discrete step movements. Directional movement is based on the
  light detected by each quadrant of the panel.
*/


// -------------------------------------------------
// Pin Definitions
// -------------------------------------------------

// Pin definitions for TB6600 (Horizontal Axis)
const int dirPlusPinH = 2;    // DIR+ for horizontal motor
const int dirMinusPinH = 5;   // DIR- for horizontal motor
const int stepPlusPinH = 3;   // PUL+ for horizontal motor
const int stepMinusPinH = 6;  // PUL- for horizontal motor

// Pin definitions for TB6600 (Vertical Axis)
const int dirPlusPinV = 7;    // DIR+ for vertical motor
const int dirMinusPinV = 8;   // DIR- for vertical motor
const int stepPlusPinV = 10;  // PUL+ for vertical motor
const int stepMinusPinV = 11; // PUL- for vertical motor

const int enPin = 4;          // Enable pin (shared or separate as needed)

// -------------------------------------------------
// Motor Parameters
// -------------------------------------------------
const int stepsPerRevolution = 200;
const int microstepSetting = 16;
const int delayTime = 1000;   // Microsecond delay between steps

// -------------------------------------------------
// Photodiode Sensor Configuration
// -------------------------------------------------
const int opt101Pins[] = {A0, A1, A2, A3};
const int numSensors = 4;

// -------------------------------------------------
// Threshold for Movement
// -------------------------------------------------
const int threshold = 200;

// -------------------------------------------------
// PWM Output Pin
// -------------------------------------------------
#define PWM_PIN 9

// -------------------------------------------------
// Setup Routine
// -------------------------------------------------
void setup() {
  Serial.begin(9600);

  // Initialize motor control pins
  pinMode(dirPlusPinH, OUTPUT);
  pinMode(dirMinusPinH, OUTPUT);
  pinMode(stepPlusPinH, OUTPUT);
  pinMode(stepMinusPinH, OUTPUT);

  pinMode(dirPlusPinV, OUTPUT);
  pinMode(dirMinusPinV, OUTPUT);
  pinMode(stepPlusPinV, OUTPUT);
  pinMode(stepMinusPinV, OUTPUT);

  pinMode(enPin, OUTPUT);
  digitalWrite(enPin, LOW); // Enable motors

  // Set all control pins LOW initially
  digitalWrite(dirPlusPinH, LOW);
  digitalWrite(dirMinusPinH, LOW);
  digitalWrite(stepPlusPinH, LOW);
  digitalWrite(stepMinusPinH, LOW);

  digitalWrite(dirPlusPinV, LOW);
  digitalWrite(dirMinusPinV, LOW);
  digitalWrite(stepPlusPinV, LOW);
  digitalWrite(stepMinusPinV, LOW);

  pinMode(PWM_PIN, OUTPUT);
  setupTimer1();

  Serial.println("Solar tracker initialized...");
}

// -------------------------------------------------
// Timer Configuration for PWM Output (Timer1)
// -------------------------------------------------
void setupTimer1() {
  TCCR1A = _BV(COM1A1) | _BV(WGM11);
  TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10);

  ICR1 = 354;
  OCR1A = 283;
}

// -------------------------------------------------
// Main Loop: Read Sensors and Move Motors
// -------------------------------------------------
void loop() {
  int topLeft = analogRead(opt101Pins[0]);
  int topRight = analogRead(opt101Pins[1]);
  int bottomLeft = analogRead(opt101Pins[2]);
  int bottomRight = analogRead(opt101Pins[3]);

  Serial.print("TL:"); Serial.print(topLeft);
  Serial.print(" TR:"); Serial.print(topRight);
  Serial.print(" BL:"); Serial.print(bottomLeft);
  Serial.print(" BR:"); Serial.println(bottomRight);

  int horizontalDiff = (topRight + bottomRight) - (topLeft + bottomLeft);
  int verticalDiff = (topLeft + topRight) - (bottomLeft + bottomRight);

  if (horizontalDiff > threshold) {
    rotateMotor(true, 10); // Move right
  } else if (horizontalDiff < -threshold) {
    rotateMotor(false, 10); // Move left
  }

  if (verticalDiff > threshold) {
    rotateMotorVertical(true, 10); // Move up
  } else if (verticalDiff < -threshold) {
    rotateMotorVertical(false, 10); // Move down
  }

  delay(100);
}

// -------------------------------------------------
// Horizontal Motor Control
// -------------------------------------------------
void rotateMotor(bool clockwise, int steps) {
  digitalWrite(dirPlusPinH, clockwise ? HIGH : LOW);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPlusPinH, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPlusPinH, LOW);
    delayMicroseconds(delayTime);
  }
}

// -------------------------------------------------
// Vertical Motor Control
// -------------------------------------------------
void rotateMotorVertical(bool upward, int steps) {
  digitalWrite(dirPlusPinV, upward ? LOW : HIGH);
  for (int i = 0; i < steps; i++) {
    digitalWrite(stepPlusPinV, HIGH);
    delayMicroseconds(delayTime);
    digitalWrite(stepPlusPinV, LOW);
    delayMicroseconds(delayTime);
  }
}
