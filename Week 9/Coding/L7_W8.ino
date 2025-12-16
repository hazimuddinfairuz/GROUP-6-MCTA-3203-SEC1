#include "HUSKYLENS.h"
#include "SoftwareSerial.h"

HUSKYLENS huskylens;
SoftwareSerial mySerial(4, 5); // RX, TX

// RGB LED pins (HW-479)
#define RED_PIN   9
#define GREEN_PIN 10
#define BLUE_PIN  11

void printResult(HUSKYLENSResult result);
void setRGB(int r, int g, int b);

void setup() {
    Serial.begin(115200);
    mySerial.begin(9600);

    // RGB pins
    pinMode(RED_PIN, OUTPUT);
    pinMode(GREEN_PIN, OUTPUT);
    pinMode(BLUE_PIN, OUTPUT);

    // Turn off LED at startup
    setRGB(0, 0, 0);

    while (!huskylens.begin(mySerial)) {
        Serial.println(F("Begin failed!"));
        Serial.println(F("1. Check Protocol Type = Serial 9600"));
        Serial.println(F("2. Check wiring"));
        delay(200);
    }

    Serial.println("HuskyLens Ready — Multi Color + RGB LED Enabled!");
}

void loop() {
    if (!huskylens.request()) {
        Serial.println("Fail to request data from HuskyLens!");
    }
    else if (!huskylens.isLearned()) {
        Serial.println("Nothing learned! Please train colors.");
    }
    else if (!huskylens.available()) {
        Serial.println("No object detected.");
        setRGB(0, 0, 0);  // LED OFF when nothing detected
    }
    else {
        while (huskylens.available()) {
            HUSKYLENSResult result = huskylens.read();
            printResult(result);
        }
    }
}

void printResult(HUSKYLENSResult result){
    if (result.command == COMMAND_RETURN_BLOCK){

        Serial.print("Detected ID = ");
        Serial.println(result.ID);

        // RGB LED reacts based on color ID
        switch(result.ID) {

            case 1:  // RED
                Serial.println("Color: RED");
                setRGB(255, 0, 0);
                break;

            case 2:  // BLUE
                Serial.println("Color: BLUE");
                setRGB(0, 0, 255);
                break;

            case 3:  // GREEN
                Serial.println("Color: GREEN");
                setRGB(0, 255, 0);
                break;

            case 4:  // YELLOW 
                Serial.println("Color: YELLOW");
                setRGB(255, 255, 0);
                break;

            default: 
                Serial.println("Unknown ID — LED OFF");
                setRGB(0, 0, 0);
                break;
        }
    }
}

// ----------------------------------------------------------------
// RGB LED HELPER (common cathode)
// Send values 0–255 for brightness
// ----------------------------------------------------------------
void setRGB(int r, int g, int b) {
    analogWrite(RED_PIN, r);
    analogWrite(GREEN_PIN, g);
    analogWrite(BLUE_PIN, b);
}