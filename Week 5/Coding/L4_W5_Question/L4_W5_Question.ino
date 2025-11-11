// Run 2 DC Motors (Motor A & B) in sync on L298P shield

// Motor A pins
int IN1 = 12;
int IN2 = 13;
int ENA = 10;

// Motor B pins
int IN3 = 8;
int IN4 = 9;
int ENB = 11;

void setup() {
  // Set all pins as outputs
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(ENA, OUTPUT);

  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENB, OUTPUT);
}
void loop() {
  // === Forward ===
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
  
  analogWrite(ENA, 255);  // Speed (0–255)
  analogWrite(ENB, 255);
  delay(2000);

  // === Stop ===
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(1000);

  // === Reverse ===
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
  
  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  delay(2000);

  // === Stop again ===
  analogWrite(ENA, 0);
  analogWrite(ENB, 0);
  delay(1000);
}
