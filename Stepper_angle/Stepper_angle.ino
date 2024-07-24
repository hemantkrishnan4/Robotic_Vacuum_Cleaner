#include <AccelStepper.h>

// Define stepper motor connections and interface type
#define dirPin1 3
#define stepPin1 2
#define dirPin2 5
#define stepPin2 4

// Create instances of the AccelStepper class
AccelStepper stepper1(1, stepPin1, dirPin1);
AccelStepper stepper2(1, stepPin2, dirPin2);

bool startMoving = false;
bool returnToStart = false;
int targetPosition = 346; // Default target position
bool awaitingStepsInput = false;

void setup() {
  // Set the maximum speed and acceleration
  stepper1.setMaxSpeed(200);
  stepper1.setAcceleration(800);

  stepper2.setMaxSpeed(0);
  stepper2.setAcceleration(0);

  // Initialize serial communication
  Serial.begin(9600);
  Serial.println(" ");
  Serial.println("Send 'A' to start the motor.");
  Serial.println("Send 'B' to return the motor to the starting position.");
  Serial.println("Send 'C' to input the number of steps.");
}

void loop() {
  // Check for serial input
  if (Serial.available() > 0) {
    if (awaitingStepsInput) {
      // If we're awaiting step input, read the steps value
      targetPosition = Serial.parseInt();
      Serial.print("Updated target position to: ");
      Serial.println(targetPosition);
      awaitingStepsInput = false;
    } else {
      // Otherwise, read the command
      char input = Serial.read();
      if (input == 'A') {
        startMoving = true;
        stepper1.moveTo(targetPosition);
      } else if (input == 'B') {
        returnToStart = true;
        stepper1.moveTo(0);
      } else if (input == 'C') {
        Serial.println("Enter the number of steps:");
        awaitingStepsInput = true;
      }
    }
  }

  // Run the motor if the startMoving flag is true
  if (startMoving) {
    if (stepper1.distanceToGo() != 0) {
      stepper1.run();
    } else {
      // Stop the motor once the target position is reached
      startMoving = false;
      Serial.println("Motor has reached the target position.");
    }
  }

  // Run the motor if the returnToStart flag is true
  if (returnToStart) {
    if (stepper1.distanceToGo() != 0) {
      stepper1.run();
    } else {
      // Stop the motor once it returns to the starting position
      returnToStart = false;
      Serial.println("Motor has returned to the starting position.");
    }
  }
}
