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

unsigned long previousFeedTime = 0;
unsigned long feedInterval = 3600000; // 1 hour

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

  Serial.println("status : " + String(distanceStorage2) + "," + String(distanceStorage1) + "," + String(distanceBowl) + "\n");

  if (isStorageEmpty1(distanceStorage1) || isStorageEmpty2(distanceStorage2)) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  isBowlFullFlag = isBowlFull(distanceBowl);

  if (Serial.available() > 0) {
    String receivedData = Serial.readStringUntil('\n');
    int index = receivedData.indexOf(':');
    String type = receivedData.substring(0, index - 1);
    String data = receivedData.substring(index + 2);
    data.trim();

    if (type == "rfid") {
      if (data == "1") {
        Serial.println("detection : " + String(isBowlFullFlag) + ", cat");
        if (!isBowlFullFlag && servoEnabled)
          openServo(catServo);
      } else if (data == "0") {
        Serial.println("detection : " + String(isBowlFullFlag) + ", dog");
        if (!isBowlFullFlag && servoEnabled)
          openServo(dogServo);
      }
    } else if (type == "feed") {
      // Example: data = "cat,40"
      int commaIndex = data.indexOf(',');
      String petType = data.substring(0, commaIndex);
      String amountStr = data.substring(commaIndex + 1);
      amountStr.trim();
      int amount = amountStr.toInt();
      
      // Perform feeding based on pet type and amount
      if (petType == "cat") {
        feedPets(catServo, amount);
      } else if (petType == "dog") {
        feedPets(dogServo, amount);
      }
    } else if (type == "timeInterval") {
      // Convert time interval data to milliseconds
      int intervalMinutes = data.toInt();
      feedInterval = intervalMinutes * 60 * 1000; // Convert minutes to milliseconds
    }
  }

  delay(1000);
}

void feedPets(Servo servo, int amount) {
  servoEnabled = false;

  // Adjust the servo angle and duration based on the amount
  int angle = map(amount, 0, 100, 0, 180); // Map amount to servo angle (assuming 0-100% range)
  int duration = map(amount, 0, 100, 500, 2000); // Map amount to servo duration (adjust as needed)

  servo.write(angle);
  delay(duration);
  servo.write(0); // Return to the original position
  servoEnabled = true;

  Serial.println("Feeding: " + String(amount) + " grams");
}

void openServo(Servo servo) {
  servoEnabled = false;
  servo.write(90); // Adjust angle as needed
  delay(1500); // Adjust duration as needed
  servo.write(0); // Return to original position
  servoEnabled = true;
}

bool isBowlFull(long distance) {
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
