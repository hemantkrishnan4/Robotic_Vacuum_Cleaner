#include <AccelStepper.h>

// Define pins
#define pul1 2
#define dir1 3 
#define pul2 4
#define dir2 5 
#define trigg1 9
#define echo1 10

// Initialize the AccelStepper objects
AccelStepper stepper1(1, pul1, dir1);
AccelStepper stepper2(1, pul2, dir2);

int distance = 0;
long duration = 0;

enum RobotState {IDLE, CHECK_DISTANCE, TURN1, FORWARD1, TURN2, TURN3, FORWARD2, TURN4};
RobotState currentState = IDLE;
bool distanceCheckNeeded = true;
bool l_flag = false;
bool executionEnabled = false;  // Flag to control execution based on Bluetooth input

int degreesToSteps(float degrees) {
  return (int)(200 * (degrees / 360.0));
}

void setup() {
  Serial.begin(9600);

  // Set up stepper motor parameters
  stepper1.setMaxSpeed(200); // Adjust as needed
  stepper1.setAcceleration(1000); // Adjust as needed
  stepper2.setMaxSpeed(200); // Adjust as needed
  stepper2.setAcceleration(1000); // Adjust as needed

  // Set up ultrasonic sensor pins
  pinMode(trigg1, OUTPUT);
  pinMode(echo1, INPUT);

  // Timer setup
  noInterrupts();
  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1  = 0;
  OCR1A = 31250;
  TCCR1B |= (1 << WGM12);
  TCCR1B |= (1 << CS12);
  TIMSK1 |= (1 << OCIE1A);
  interrupts();
  delay(2000);

  Serial.println("Setup complete");
}

ISR(TIMER1_COMPA_vect) {
  if (distanceCheckNeeded) {
    digitalWrite(trigg1, LOW);
    delayMicroseconds(2);
    digitalWrite(trigg1, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigg1, LOW);
    duration = pulseIn(echo1, HIGH);
    distance = duration * 0.034 / 2;
    Serial.print("Distance: ");
    Serial.println(distance);
    currentState = CHECK_DISTANCE;
  }
}

void loop() {
  if (Serial.available()) {
    char rec_data = Serial.read();
    Serial.println(rec_data);
    if (rec_data == 'A') {
      executionEnabled = true;
      Serial.println("Execution enabled");
      currentState = CHECK_DISTANCE;
    } else if (rec_data == 'B') {
      executionEnabled = false;
      Serial.println("Execution disabled");
      stopMotors();  // Stop motors immediately when 'B' is received
    }
  }

  if (executionEnabled) {
    switch (currentState) {
      case IDLE:
        break;

      case CHECK_DISTANCE:
        if (distance > 40) {
          Serial.println("Distance > 40: Moving forward");
          forward();
          currentState = IDLE;  // Go back to IDLE after moving forward
        } else if (distance < 40) {
          Serial.println("Distance < 40: Starting turn sequence");
          distanceCheckNeeded = false;  // Disable distance checking until sequence is completed
          if (!l_flag) {
            currentState = TURN1;
            turn1();
          } else {
            currentState = TURN3;
            turn3();
          }
        }
        break;

      case TURN1:
        runToPosition();
        Serial.println("Turn1 complete: Moving forward");
        currentState = FORWARD1;
        forward();
        break;

      case FORWARD1:
        runToPosition();
        Serial.println("Forward complete: Starting Turn2");
        currentState = TURN2;
        turn2();
        break;

      case TURN2:
        runToPosition();
        Serial.println("Turn2 complete: Returning to IDLE");
        currentState = IDLE;
        distanceCheckNeeded = true;  // Re-enable distance checking
        l_flag = true;  // Set the flag
        break;

      case TURN3:
        runToPosition();
        Serial.println("Turn3 complete: Moving forward");
        currentState = FORWARD2;
        forward();
        break;

      case FORWARD2:
        runToPosition();
        Serial.println("Forward complete: Starting Turn4");
        currentState = TURN4;
        turn4();
        break;

      case TURN4:
        runToPosition();
        Serial.println("Turn4 complete: Returning to IDLE");
        currentState = IDLE;
        distanceCheckNeeded = true;  // Re-enable distance checking
        l_flag = false;  // Clear the flag
        break;
    }

    // Run the steppers
    stepper1.run();
    stepper2.run();
  }
}

void forward() {
  Serial.println("Forward: Moving 400 steps");
  stepper1.moveTo(stepper1.currentPosition() + 400); // Move 400 steps forward
  stepper2.moveTo(stepper2.currentPosition() - 400); // Move 400 steps forward
}

void backward() {
  Serial.println("Backward: Moving 400 steps");
  stepper1.moveTo(stepper1.currentPosition() - 400); // Move 400 steps backward
  stepper2.moveTo(stepper2.currentPosition() + 400); // Move 400 steps backward
}

void turn1() {
  Serial.println("Turn1: Moving 600 steps");
  stepper1.moveTo(stepper1.currentPosition() + 600);
  stepper2.moveTo(stepper2.currentPosition());
}

void turn2() {
  Serial.println("Turn2: Moving 600 steps");
  stepper1.moveTo(stepper1.currentPosition() + 600);
  stepper2.moveTo(stepper2.currentPosition());
}

void turn3() {
  Serial.println("Turn3: Moving 600 steps");
  stepper1.moveTo(stepper1.currentPosition());
  stepper2.moveTo(stepper2.currentPosition() - 600);
}

void turn4() {
  Serial.println("Turn4: Moving 600 steps");
  stepper1.moveTo(stepper1.currentPosition());
  stepper2.moveTo(stepper2.currentPosition() - 600);
}

void runToPosition() {
  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
    stepper1.run();
    stepper2.run();
  }
}

void stopMotors() {
  stepper1.stop();
  stepper2.stop();
  while (stepper1.distanceToGo() != 0 || stepper2.distanceToGo() != 0) {
    stepper1.run();
    stepper2.run();
  }
  Serial.println("Motors stopped");
}
