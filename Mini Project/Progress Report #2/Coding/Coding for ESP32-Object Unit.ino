#include <WiFi.h>
#include <WebServer.h>

const char* ssid = "";
const char* password = "";

// --- PIN DEFINITIONS ---
const int buzzerPin = 5;  // Buzzer Pin
const int ledPin = 18;    // LED Pin (Only for HuskyLens)

WebServer server(80);

// State variables to track who wants the buzzer ON
bool buttonRequest = false; 
bool huskyRequest = false;  

// --- LOGIC: Update Outputs based on Requests ---
void updateOutputs() {
  // 1. BUZZER LOGIC:
  // Ring if Button is pressed OR Husky sees object
  if (buttonRequest || huskyRequest) {
    digitalWrite(buzzerPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
  }

  // 2. LED LOGIC:
  // Light up ONLY if Husky sees object
  if (huskyRequest) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }
}

// --- WEB HANDLERS ---
void handleButtonOn() {
  buttonRequest = true;
  updateOutputs();
  server.send(200, "text/plain", "Button ON");
  Serial.println("Command: Button ON (Buzzer Only)");
}

void handleButtonOff() {
  buttonRequest = false;
  updateOutputs();
  server.send(200, "text/plain", "Button OFF");
  Serial.println("Command: Button OFF");
}

void handleHuskyOn() {
  huskyRequest = true;
  updateOutputs();
  server.send(200, "text/plain", "Husky ON");
  Serial.println("Command: Husky ON (Buzzer + LED)");
}

void handleHuskyOff() {
  huskyRequest = false;
  updateOutputs();
  server.send(200, "text/plain", "Husky OFF");
  Serial.println("Command: Husky OFF");
}

void setup() {
  Serial.begin(115200);
  
  // Setup Pins
  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);
  
  // Start OFF
  digitalWrite(buzzerPin, LOW);
  digitalWrite(ledPin, LOW);

  // Connect WiFi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nReceiver Ready!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Register URLs
  server.on("/button/on", handleButtonOn);
  server.on("/button/off", handleButtonOff);
  server.on("/husky/on", handleHuskyOn);
  server.on("/husky/off", handleHuskyOff);

  server.begin();
}

void loop() {
  server.handleClient();
}