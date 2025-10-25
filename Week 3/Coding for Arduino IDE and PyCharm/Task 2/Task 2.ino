#include <Servo.h>
Servo myservo;


int potPin = A0;
int ledPin = 7;
bool stopControl = false;
int potValue, angle;


void setup() {
  Serial.begin(9600);
  myservo.attach(9);
  pinMode(ledPin, OUTPUT);
}


void loop() {
  if (Serial.available() > 0) {
    char command = Serial.read();


    if (command == 'x') {
      stopControl = true;
      myservo.detach();
      digitalWrite(ledPin, LOW);
      Serial.println("STOP");}
  }
  if (!stopControl) {
    potValue = analogRead(potPin);
    angle = map(potValue, 0, 1023, 0, 180);


    myservo.write(angle);
   
    // LED control: ON bila lebih dari 90°
    digitalWrite(ledPin, angle > 90 ? HIGH : LOW);

    // Send to Python
    Serial.println(angle);
    delay(50);}
}

