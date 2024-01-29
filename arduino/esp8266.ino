#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>
#include <PubSubClient.h>

#define SS_PIN 4    // Define the SS_PIN for the RFID module
#define RST_PIN 16  // Define the RST_PIN for the RFID module

// WiFi credentials
const char* ssid = "your-ssid";
const char* password = "your-password";

// MQTT broker information
const char* mqttServer = "rpi10";
const int mqttPort = 1883;
const char* mqttUser = "your-mqtt-username";
const char* mqttPassword = "your-mqtt-password";

MFRC522 mfrc522(SS_PIN, RST_PIN);  // Create MFRC522 instance

String catCardUid = "92204351";
String dogCardUid = "70c5bf55";

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {
  Serial.begin(115200);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(250);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());

  client.setServer(mqttServer, mqttPort);
  // Set the callback function for when a message is received
  client.setCallback(callback);

  SPI.begin();
  mfrc522.PCD_Init(); // Initiate MFRC522
  Serial.println("RFID reader initialized.");
}

void callback(char* topic, byte* payload, unsigned int length) {
  // Handle MQTT messages if needed
}

void reconnect() {
  // Loop until we're reconnected to the MQTT broker
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    // Attempt to connect
    if (client.connect("ESP8266Client", mqttUser, mqttPassword)) {
      Serial.println("connected");
      // Subscribe to the topics you need
      // client.subscribe("topic_name");
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void loop() {
  if (!client.connected()) {
    reconnect();
  }

  client.loop();

  if (mfrc522.PICC_IsNewCardPresent() && mfrc522.PICC_ReadCardSerial()) {
    String cardUID = "";
    for (byte i = 0; i < mfrc522.uid.size; i++) {
      cardUID += String(mfrc522.uid.uidByte[i] < 0x10 ? "0" : "");
      cardUID += String(mfrc522.uid.uidByte[i], HEX);
    }

    if (cardUID == catCardUid) {
      Serial.println("0"); // Cat card detected
      client.publish("rfid/status", "Cat card detected");
    } else if (cardUID == dogCardUid) {
      Serial.println("1"); // Dog card detected
      client.publish("rfid/status", "Dog card detected");
    } else {
      Serial.println("-1"); // Unknown card
      client.publish("rfid/status", "Unknown card detected");
    }

    mfrc522.PICC_HaltA(); 
    mfrc522.PCD_StopCrypto1(); 
  }
}
