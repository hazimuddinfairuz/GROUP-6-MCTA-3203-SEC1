
const int segmentA = 3;
const int segmentB = 2;
const int segmentC = 8;
const int segmentD = 7;
const int segmentE = 6;
const int segmentF = 4;
const int segmentG = 5;

// Push Button Pins 
const int buttonUp = 10;
const int buttonDown = 11;

int currentNumber = 0;       // start from 0
int lastButtonUpState = HIGH;
int lastButtonDownState = HIGH;

void setup() {
  // Set segment pins as outputs
  pinMode(segmentA, OUTPUT);
  pinMode(segmentB, OUTPUT);
  pinMode(segmentC, OUTPUT);
  pinMode(segmentD, OUTPUT);
  pinMode(segmentE, OUTPUT);
  pinMode(segmentF, OUTPUT);
  pinMode(segmentG, OUTPUT);

  // Set button pins as inputs with pull-ups
  pinMode(buttonUp, INPUT);
  pinMode(buttonDown, INPUT);

  // Display the initial number
  displayNumber(currentNumber);
}

void loop() {
  int upState = digitalRead(buttonUp);
  int resetstate = digitalRead(buttonDown);

  // Increase button pressed 
  if (upState == LOW && lastButtonUpState == HIGH) {
    currentNumber++;
    if (currentNumber > 9) currentNumber = 0;
    displayNumber(currentNumber);
    delay(200); // debounce
  }

  // Reset button pressed
  if (resetstate == LOW && lastButtonDownState == HIGH) {
    currentNumber=0;
    if (currentNumber >= 0) currentNumber = 0;
    displayNumber(currentNumber);
    delay(200); // debounce
  }

  lastButtonUpState = upState;
  lastButtonDownState = resetstate;
}

// === Function to Display Numbers 0-9 ===
void displayNumber(int num) {
  // LOW = ON, HIGH = OFF
  switch (num) {
    case 0:
      setSegments(LOW, LOW, LOW, LOW, LOW, LOW, HIGH); break;
    case 1:
      setSegments(HIGH, LOW, LOW, HIGH, HIGH, HIGH, HIGH); break;
    case 2:
      setSegments(LOW, LOW, HIGH, LOW, LOW, HIGH, LOW); break;
    case 3:
      setSegments(LOW, LOW, LOW, LOW, HIGH, HIGH, LOW); break;
    case 4:
      setSegments(HIGH, LOW, LOW, HIGH, HIGH, LOW, LOW); break;
    case 5:
      setSegments(LOW, HIGH, LOW, LOW, HIGH, LOW, LOW); break;
    case 6:
      setSegments(LOW, HIGH, LOW, LOW, LOW, LOW, LOW); break;
    case 7:
      setSegments(LOW, LOW, LOW, HIGH, HIGH, HIGH, HIGH); break;
    case 8:
      setSegments(LOW, LOW, LOW, LOW, LOW, LOW, LOW); break;
    case 9:
      setSegments(LOW, LOW, LOW, LOW, HIGH, LOW, LOW); break;
  }
}

// === Helper Function to Light Segments ===
void setSegments(int a, int b, int c, int d, int e, int f, int g) {
  digitalWrite(segmentA, a);
  digitalWrite(segmentB, b);
  digitalWrite(segmentC, c);
  digitalWrite(segmentD, d);
  digitalWrite(segmentE, e);
  digitalWrite(segmentF, f);
  digitalWrite(segmentG, g);
}
