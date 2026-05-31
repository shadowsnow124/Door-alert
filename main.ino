#include <Arduino.h>
#include <WiFi.h>
#include <TM1637Display.h>
#include <HTTPClient.h>

// --- Configuration ---
const char* ssid = "*your-ssid*";
const char* password = "*your-pass*";
const char* discordWebhookUrl = "https://discord.com/api/webhooks/*your-discord-webhook*";

// TM1637 Display
#define CLK_PIN 19
#define DIO_PIN 18

// Ultrasonic Sensor
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

void setup() {
  Serial.begin(115200);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);
  
  display.setBrightness(0x07);
  display.clear();

  connectToWiFi();
}

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
    distance = 9999;
  }
  
  int displayDistance = min((int)distance, 9999);
  display.showNumberDec(displayDistance, false);
  Serial.print("Distance: ");
  Serial.println(distance);
  
  if (distance < 120.0) {
    String notification = "ALERT! Detected Object at " + String(distance, 1) + " cm!";
    sendDiscordNotification(notification);
  } else {

  }

  delay(1000);
}
