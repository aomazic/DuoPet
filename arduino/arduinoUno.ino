#include "Servo.h"

#define TRIG_PIN_BOWL 12
#define ECHO_PIN_BOWL 11
#define BOWL_FULL_THRESHOLD 10

#define TRIG_PIN_STORAGE 8
#define ECHO_PIN_STORAGE 9
#define STORAGE_EMPTY_THRESHOLD 10

#define CAT_SERVO_PIN 5
#define DOG_SERVO_PIN_2 6

Servo catServo;
Servo dogServo;

bool servoEnabled = true;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN_BOWL, OUTPUT);
  pinMode(ECHO_PIN_BOWL, INPUT);
  pinMode(TRIG_PIN_STORAGE, OUTPUT);
  pinMode(ECHO_PIN_STORAGE, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  catServo.attach(CAT_SERVO_PIN);
  dogServo.attach(DOG_SERVO_PIN_2);
}

void loop() {
  long distanceBowl = getSonarDistance(TRIG_PIN_BOWL, ECHO_PIN_BOWL);
  long distanceStorage = getSonarDistance(TRIG_PIN_STORAGE, ECHO_PIN_STORAGE);

  Serial.print("Bowl Distance: ");
  Serial.print(distanceBowl);
  Serial.print(" cm, Storage Distance: ");
  Serial.println(distanceStorage);

  if (isStorageEmpty(distanceStorage)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  if (servoEnabled && Serial.available() > 0 && !isBowlFull(distanceBowl)) {
    int servoCommand = Serial.parseInt();
    if (servoCommand == 1) {
      openServo(catServo);
    } else if (servoCommand == 0) {
      openServo(dogServo);
    }
  }

  delay(1000);
}


void openServo(Servo servo) {
  servoEnabled = false;
  servo.write(90); // Adjust angle as needed
  delay(1500); // Adjust duration as needed
  servo.write(0); // Return to original position
  servoEnabled = true;
}

bool isBowlFull(long distance){
  return (distance < BOWL_FULL_THRESHOLD);
}

bool isStorageEmpty(long distance) {
  return (distance < STORAGE_EMPTY_THRESHOLD);
}

long getSonarDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return (duration * 0.0343) / 2;
}
