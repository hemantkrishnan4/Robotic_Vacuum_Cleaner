#include <AccelStepper.h>

// Define pins
#define pul1 2
#define dir1 3 
#define pul2 4
#define dir2 5 

// Initialize the AccelStepper objects
AccelStepper stepper1(1, pul1, dir1);
AccelStepper stepper2(1, pul2, dir2);

int distance = 0;
long duration = 0;




void setup() {
  stepper1.setMaxSpeed(200); // Adjust as needed
  stepper1.setAcceleration(100); // Adjust as needed
  stepper2.setMaxSpeed(200); // Adjust as needed
  stepper2.setAcceleration(800); // Adjust as needed

}

void loop() {
  stepper1.setSpeed(750);
  stepper1.moveTo(400);
  stepper1.runSpeedToPosition();

  stepper2.setSpeed(100);
  stepper2.moveTo(200);
  stepper2.runSpeedToPosition();
}
