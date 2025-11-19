#include <Arduino.h>
#include <WiFi.h>
#include <TM1637Display.h>
#include <HTTPClient.h>

// --- Configuration ---
const char* ssid = "donut9164_2.4G";
const char* password = "24502450jkY";
const char* discordWebhookUrl = "https://discord.com/api/webhooks/1440261151387553864/WkfgIzPky9wd6UyWkynsI5qxtb8k_rrZOJRx2V3rFUYmgW7T4IcASr-OdhLUT8LbOrNm";

// TM1637 Display Pins
#define CLK_PIN 19
#define DIO_PIN 18

// Ultrasonic Sensor Pins
const int trigPin = 14;
const int echoPin = 12;

float duration, distance;
TM1637Display display(CLK_PIN, DIO_PIN);

void connectToWiFi() {
  Serial.print("Connecting to Wi-Fi...");
  WiFi.begin(ssid, password);
  // Increase delay and count attempts for better startup behavior
  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }
  
  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nConnected to WiFi!");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nFailed to connect to WiFi. Continuing offline.");
  }
}

void sendDiscordNotification(String message) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    http.begin(discordWebhookUrl);
    http.addHeader("Content-Type", "application/json");

    String jsonPayload = "{\"content\": \"" + message + "\"}";

    Serial.println("Sending Discord message...");
    int httpResponseCode = http.POST(jsonPayload);

    if (httpResponseCode > 0) {
      Serial.print("HTTP Response code: ");
      Serial.println(httpResponseCode);
    } else {
      Serial.print("HTTP Error code: ");
      Serial.print(httpResponseCode);
      Serial.print(" - ");
      Serial.println(http.errorToString(httpResponseCode));
    }

    http.end();
  } else {
    Serial.println("WiFi not connected. Cannot send message.");
  }
}

// --- Setup ---
void setup() {
  Serial.begin(115200);

  // Initialize Pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  // Display Setup
  display.setBrightness(0x07); // Set brightness (0x00 to 0x0f, 0x07 is default)
  display.clear();

  // Network Setup
  connectToWiFi();
}

// --- Loop ---
void loop() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  duration = pulseIn(echoPin, HIGH, 30000); 
  
  if (duration > 0) {
    distance = (duration * 0.0343) / 2.0; 
  } else {
    distance = 9999; // Set a large value if no echo received
  }
  
  int displayDistance = min((int)distance, 9999); // Cap display at 9999
  display.showNumberDec(displayDistance, false);
  Serial.print("Distance: ");
  Serial.println(distance);
  
  if (distance < 120.0) {
    String notification = "ALERT! Detected Object at " + String(distance, 1) + " cm!";
    sendDiscordNotification(notification);
  } else {

  }

  delay(1000); // Check and alert every 5 seconds to avoid spamming the webhook
}