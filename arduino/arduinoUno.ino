#include "Servo.h"

#define TRIG_PIN_BOWL 12
#define ECHO_PIN_BOWL 11
#define BOWL_FULL_THRESHOLD 10

#define TRIG_PIN_STORAGE1 8
#define ECHO_PIN_STORAGE1 9
#define STORAGE_EMPTY_THRESHOLD1 10

#define TRIG_PIN_STORAGE2 2
#define ECHO_PIN_STORAGE2 3
#define STORAGE_EMPTY_THRESHOLD2 10

#define CAT_SERVO_PIN 5
#define DOG_SERVO_PIN_2 6

Servo catServo;
Servo dogServo;

bool servoEnabled = true;
bool isBowlFullFlag = false;

void setup() {
  Serial.begin(115200);
  pinMode(TRIG_PIN_BOWL, OUTPUT);
  pinMode(ECHO_PIN_BOWL, INPUT);
  pinMode(TRIG_PIN_STORAGE1, OUTPUT);
  pinMode(ECHO_PIN_STORAGE1, INPUT);
  pinMode(TRIG_PIN_STORAGE2, OUTPUT);
  pinMode(ECHO_PIN_STORAGE2, INPUT);
  pinMode(LED_BUILTIN, OUTPUT);

  catServo.attach(CAT_SERVO_PIN);
  dogServo.attach(DOG_SERVO_PIN_2);
}

void loop() {
  long distanceBowl = getSonarDistance(TRIG_PIN_BOWL, ECHO_PIN_BOWL);
  long distanceStorage1 = getSonarDistance(TRIG_PIN_STORAGE1, ECHO_PIN_STORAGE1);
  long distanceStorage2 = getSonarDistance(TRIG_PIN_STORAGE2, ECHO_PIN_STORAGE2);

  Serial.print("Bowl Distance: ");
  Serial.print(distanceBowl);
  Serial.print(" cm, Storage 1 Distance: ");
  Serial.println(distanceStorage1);
  Serial.print(" cm, Storage 2 Distance: ");
  Serial.println(distanceStorage2);

  if (isStorageEmpty1(distanceStorage1) || isStorageEmpty2(distanceStorage2)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

   isBowlFullFlag = isBowlFull(distanceBowl);
  if (servoEnabled && Serial.available() > 0 && !isBowlFullFlag) {
    if (isBowlFullFlag) {
        Serial.println("yes");
      } else {
        Serial.println("no");
      }
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

bool isStorageEmpty1(long distance) {
  return (distance < STORAGE_EMPTY_THRESHOLD1);
}

bool isStorageEmpty2(long distance) {
  return (distance < STORAGE_EMPTY_THRESHOLD2);
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
