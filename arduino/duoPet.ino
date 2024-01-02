#include <Servo.h>
#include <MFRC522.h>
#include <SPI.h>
#include <Ultrasonic.h>

// Constants for RFID
#define SS_PIN RST_PIN = 10;
#define RST_PIN = 9;
MFRC522 rfid(SS_PIN, RST_PIN);

// Constants for Ultrasonic Sensors
#define TRIG_PIN_BOWL 5
#define ECHO_PIN_BOWL 6
#define TRIG_PIN_STORAGE 7
#define ECHO_PIN_STORAGE 8
#define BOWL_FULL_THRESHOLD 20
#define STORAGE_EMPTY_THRESHOLD 2

// Constants for Servo Motors
#define SERVO_DOG_PIN 3
#define SERVO_CAT_PIN 4

// Constants for LED
#define LED_PIN 2

// Set up the ultrasonic sensors
Ultrasonic ultrasonicBowl(TRIG_PIN_BOWL, ECHO_PIN_BOWL);
Ultrasonic ultrasonicStorage(TRIG_PIN_STORAGE, ECHO_PIN_STORAGE);

// Set up the servo motors
Servo servoDog;
Servo servoCat;

// Variables for default food ammount in grams
int dogDefault = 20;
int catDefault = 10;

void setup() {
  Serial.begin(115200);

  // Initialize RFID reader
  SPI.begin();
  rfid.PCD_Init();

  // Attach servos to pins
  servoDog.attach(SERVO_DOG_PIN);
  servoCat.attach(SERVO_CAT_PIN);

  // Set LED pin as output
  pinMode(LED_PIN, OUTPUT);
}

void loop() {
  if (isPetNear()) {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
      if (isDog(rfid.uid)) {
        // Activate dog food servo
        dispenseFood(servoDog, dogDefault); 
      } else if (isCat(rfid.uid)) {

        // Activate cat food servo
        dispenseFood(servoCat, catDefault);
      }
    }
  }

  // Check food storage level
  if (isStorageEmpty()) {
    digitalWrite(LED_PIN, HIGH);
    sendStorageEmptyNotification();
  } else {
    digitalWrite(LED_PIN, LOW);
  }
  delay(500);
}

bool isPetNear() {
  // Implement pet detection logic
  // Return true if a pet is detected, false otherwise
}

bool isDog(MFRC522::Uid uid) {
  return uid.uidByte[0] == 'P'; // Uid of dog begins with P
}

bool isCat(MFRC522::Uid uid) {
  return uid.uidByte[0] == 'C'; // Uid of dog begins with C
}

void dispenseFood(Servo servo, int amount) {
  if (isBowlFull) return; // If bowl is full dont fill again

  // Calibrated values: 90 degrees rotation dispenses 50g of food
  const int anglePerGram = 90 / 50; // Calculate the angle for 1 gram of food
  int angleToRotate = anglePerGram * amount; // Calculate the total rotation angle for the desired amount

  // Rotate the servo to dispense the food
  servo.write(angleToRotate); // Rotate to the calculated angle
  delay(1000); // Wait for the food to dispense

  // Return to the original position
  servo.write(0);
  delay(1000);
}

bool isBowlFull(){
    distance = getSonarDistance(TRIG_PIN_STORAGE, ECHO_PIN_STORAGE)
 if (distance < STORAGE_EMPTY_THRESHOLD) { 
    return true;
  } else {
    return false;
  } 
}

bool isStorageEmpty(){
 distance = getSonarDistance(TRIG_PIN_BOWL, ECHO_PIN_BOWL)
 if (distance < BOWL_FULL_THRESHOLD) { 
    return true;
  } else {
    return false;
  }
}

long getSonarDistance(int trigPin, int echoPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  long duration = pulseIn(echoPin, HIGH);
  return (duration*.0343)/2;
}

void sendStorageEmptyNotification() {
  // Implement the logic to send a notification to the server
}
