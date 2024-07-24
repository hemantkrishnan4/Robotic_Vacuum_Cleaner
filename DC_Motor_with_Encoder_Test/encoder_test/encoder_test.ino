#define DIR1 4
#define DIR2 5
#define PWM1 6

#define encoderPinA 2
#define encoderPinB 3


volatile long encoderCount = 0;

long previousTime = 0;
float ePrevious = 0;
float eIntegral = 0;

void setup() {
  Serial.begin(9600);

  pinMode(DIR1, OUTPUT);
  pinMode(DIR2, OUTPUT);
  pinMode(PWM1, OUTPUT);
  pinMode(encoderPinA, INPUT);
  pinMode(encoderPinB, INPUT);
  
  attachInterrupt(digitalPinToInterrupt(encoderPinA), handleEncoder, RISING);
  
}

void loop() {
  int target = 1000;

  float kp = 10.0;
  float kd = 0.0;
  float ki = 0.0;
  float u = pidController(target, kp, kd, ki);

  moveMotor(DIR1, DIR2, PWM1, u);

  Serial.print(target);
  Serial.print(", ");
  Serial.println(encoderCount);
}


void handleEncoder(){
  if (digitalRead(encoderPinA) > digitalRead(encoderPinB)){
    encoderCount++;
  }
  else{
    encoderCount--;
  }
}


void moveMotor(int dirPin1,int dirPin2, int pwmPin, float u){

  float speed = fabs(u);
  if (speed > 255){
    speed = 255;
  }

  int direction1 = 1;
  int direction2 = 0;
  if (u < 0){
    direction1 = 0;
    direction2 = 1;
  }

  digitalWrite(dirPin1, direction1);
  digitalWrite(dirPin2, direction2);
  analogWrite(pwmPin, speed);
}


float pidController(int target, float kp, float kd, float ki) {

  long currentTime = micros();
  float deltaT = ((float)(currentTime - previousTime))/ 1000000;

  int e = encoderCount - target;
  float eDerivative = (e - ePrevious ) / deltaT;
  eIntegral = eIntegral + e * deltaT;

  float u = (kp * e) + (kd * eDerivative) + (ki * eIntegral);

  previousTime = currentTime;
  ePrevious = e;

  return u;
}
