#include <WiFi.h>
#include <HTTPClient.h>
#include <Wire.h>
#include "HUSKYLENS.h"

// --- CONFIGURATION ---
const char* ssid = "";
const char* password = "";
const char* walletIP = "IP address"; // <--- CHANGE THIS to your Wallet ESP32 IP
const int buttonPin = 25;              // Updated to Pin 25

HUSKYLENS huskylens;
bool huskyConnected = false;

// Logic Variables
bool buttonActive = false;       // Is the button toggle ON?
bool lastButtonState = HIGH;     // For edge detection
unsigned long lastDebounce = 0;

bool huskyActive = false;        // Is the camera currently seeing the object?
bool lastHuskySent = false;      // To prevent spamming WiFi

void setup() {
  Serial.begin(115200);
  pinMode(buttonPin, INPUT_PULLUP);
  Wire.begin(21, 22); // SDA, SCL

  // 1. Connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // 2. Init HuskyLens (NON-BLOCKING)
  // We use an 'if' instead of 'while' so the button works even if camera fails
  if (huskylens.begin(Wire)) {
    Serial.println("HuskyLens Online!");
    huskyConnected = true;
  } else {
    Serial.println("WARNING: HuskyLens not found (Check wiring/I2C Protocol).");
    Serial.println("Button will still work.");
    huskyConnected = false;
  }
}

void loop() {
  // --- PART A: BUTTON TOGGLE LOGIC ---
  int reading = digitalRead(buttonPin);

  // Check if button changed (Pressed LOW)
  if (reading == LOW && lastButtonState == HIGH && (millis() - lastDebounce > 200)) {
    buttonActive = !buttonActive; // Toggle the state (On becomes Off, Off becomes On)
    lastDebounce = millis();
    
    Serial.print("Button Toggled: ");
    Serial.println(buttonActive ? "ON" : "OFF");

    // Send command immediately
    if (buttonActive) sendRequest("/button/on");
    else sendRequest("/button/off");
  }
  lastButtonState = reading;


  // --- PART B: HUSKYLENS LOGIC ---
  bool currentDetecting = false;
  
  if (huskyConnected && huskylens.request()) {
    if (huskylens.isLearned() && huskylens.available()) {
      currentDetecting = true;
    }
  }

  // Only send WiFi request if the camera status CHANGES (to save speed)
  if (currentDetecting != huskyActive) {
    huskyActive = currentDetecting;
    if (huskyActive) {
      Serial.println("Object Found -> Sending Husky ON");
      sendRequest("/husky/on");
    } else {
      Serial.println("Object Lost -> Sending Husky OFF");
      sendRequest("/husky/off");
    }
  }
  
  delay(50); // Small delay for stability
}

void sendRequest(const char* path) {
  if (WiFi.status() == WL_CONNECTED) {
    HTTPClient http;
    String url = "http://" + String(walletIP) + path;
    http.begin(url);
    int httpCode = http.GET();
    http.end();
  } else {
    Serial.println("WiFi Disconnected!");
  }
}