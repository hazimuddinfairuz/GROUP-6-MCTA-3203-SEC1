// --- Motor A pins ---
int IN1 = 12;
int IN2 = 13;
int ENA = 10;

// --- Simulated RPM settings ---
const float maxRPM_12V = 10000.0;  // Motor rated speed at 12V
const float supplyVoltage = 5.0;   // Laptop USB output voltage
const float ratedVoltage  = 12.0;
float maxRPM = (supplyVoltage / ratedVoltage) * maxRPM_12V; // Scaled for 5V

void setup() {
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  Serial.begin(9600);
  Serial.println("DC Motor (A) RPM Simulation Starting...");
  Serial.print("Estimated Max RPM at 5V: ");
  Serial.println(maxRPM);
}

void loop() {
  // === Forward motion ===
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  analogWrite(ENA, 255); // full power
  printSimulatedRPM(255);
  delay(2000);

  // === Stop ===
  analogWrite(ENA, 0);
  printSimulatedRPM(0);
  delay(1000);

  // === Reverse motion ===
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  analogWrite(ENA, 255);
  printSimulatedRPM(255);
  delay(2000);

  // === Stop again ===
  analogWrite(ENA, 0);
  printSimulatedRPM(0);
  delay(1000);
}

void printSimulatedRPM(int pwmVal) {
  float rpm = (pwmVal / 255.0) * maxRPM;

  Serial.print("Motor A (simulated): ");
  Serial.print(rpm);
  Serial.println(" RPM");
}