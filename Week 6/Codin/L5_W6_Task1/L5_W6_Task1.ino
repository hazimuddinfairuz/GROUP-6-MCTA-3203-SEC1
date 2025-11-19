#include <ESP32Servo.h>
#include "esp_camera.h"
#include <WiFi.h>
#include "board_config.h"   // your camera pin definitions

// -----------------------
// WIFI SETTINGS
// -----------------------
const char* ssid = "Jimmy";
const char* password = "jemmeh123";

// -----------------------
// SERVO SETTINGS
// -----------------------
Servo panServo;
int panAngle = 90;              // starting angle
const int MIN_ANGLE = 30;
const int MAX_ANGLE = 160;
int stepDirection = 30;           // degrees per step
const int SERVO_PIN = 12;

// -----------------------
// BUTTON SETTINGS
// -----------------------
const int BUTTON_PIN = 13;

// -----------------------
// MOTION DETECTION SETTINGS
// -----------------------
const long SHIFT_THRESHOLD = 24000;   // sensitivity
const size_t LASTJPEG_MAX = 35000;
static uint8_t lastJPEG[LASTJPEG_MAX];
static size_t lastLen = 0;
bool haveLast = false;

// -----------------------
// CAMERA SERVER
// -----------------------
void startCameraServer();

// -----------------------------------------------------------
// Helper: detect movement by sampling differences between two
// JPEG buffers. Returns +1 (right), -1 (left), or 0 (no movement).
// -----------------------------------------------------------
int detectCameraMovementSampled(const uint8_t* prev, size_t prevLen,
                               const uint8_t* curr, size_t currLen)
{
  if (!prev || !curr || prevLen == 0 || currLen == 0) return 0;

  long leftSum = 0;
  long rightSum = 0;

  size_t minLen = (prevLen < currLen) ? prevLen : currLen;
  const size_t STEP = 300;

  for (size_t i = 0; i + 1 < minLen; i += STEP) {
    int d = (int)curr[i] - (int)prev[i];
    if ((i / STEP) % 2 == 0) leftSum += d;
    else                     rightSum += d;
  }

  long diff = rightSum - leftSum;
  if (abs(diff) < SHIFT_THRESHOLD) return 0;
  return (diff > 0) ? +1 : -1;
}

// -----------------------
// SETUP
// -----------------------
void setup() {
  Serial.begin(115200);
  delay(100);

  // attach servo
  panServo.attach(SERVO_PIN);
  panServo.write(panAngle);

  // setup button
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  // camera config
  camera_config_t config;
  config.pin_pwdn = PWDN_GPIO_NUM;
  config.pin_reset = RESET_GPIO_NUM;
  config.pin_xclk = XCLK_GPIO_NUM;
  config.pin_sscb_sda = SIOD_GPIO_NUM;
  config.pin_sscb_scl = SIOC_GPIO_NUM;
  config.pin_d7 = Y9_GPIO_NUM;
  config.pin_d6 = Y8_GPIO_NUM;
  config.pin_d5 = Y7_GPIO_NUM;
  config.pin_d4 = Y6_GPIO_NUM;
  config.pin_d3 = Y5_GPIO_NUM;
  config.pin_d2 = Y4_GPIO_NUM;
  config.pin_d1 = Y3_GPIO_NUM;
  config.pin_d0 = Y2_GPIO_NUM;
  config.pin_vsync = VSYNC_GPIO_NUM;
  config.pin_href = HREF_GPIO_NUM;
  config.pin_pclk = PCLK_GPIO_NUM;
  config.xclk_freq_hz = 20000000;
  config.ledc_timer = LEDC_TIMER_0;
  config.ledc_channel = LEDC_CHANNEL_0;
  config.pixel_format = PIXFORMAT_JPEG;
  config.frame_size = FRAMESIZE_QVGA;
  config.jpeg_quality = 15;
  config.fb_count = 2;

  if (esp_camera_init(&config) != ESP_OK) {
    Serial.println("Camera init failed!");
    while (true) { delay(1000); }
  }

  // connect WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  unsigned long t0 = millis();
  while (WiFi.status() != WL_CONNECTED) {
    if (millis() - t0 > 20000) break;
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  if (WiFi.status() == WL_CONNECTED) {
    Serial.print("WiFi connected. Stream: http://");
    Serial.println(WiFi.localIP());
  }

  startCameraServer();
}

// -----------------------
// MAIN LOOP
// -----------------------
void loop() {
  bool buttonPressed = (digitalRead(BUTTON_PIN) == LOW);

  camera_fb_t* fb = esp_camera_fb_get();
  if (!fb) {
    Serial.println("FRAME ERROR");
    delay(150);
    return;
  }

  int direction = 0;
  if (haveLast) {
    direction = detectCameraMovementSampled(lastJPEG, lastLen, fb->buf, fb->len);
  }

  // store JPEG for next comparison
  size_t copyLen = fb->len;
  if (copyLen > LASTJPEG_MAX) copyLen = LASTJPEG_MAX;
  memcpy(lastJPEG, fb->buf, copyLen);
  lastLen = copyLen;
  haveLast = true;

  esp_camera_fb_return(fb);

  // move servo if button pressed OR motion detected
  if (buttonPressed || direction != 0) {
    if (direction == +1) stepDirection = 5;
    else if (direction == -1) stepDirection = -5;

    panAngle += stepDirection;

    // reverse at limits
    if (panAngle >= MAX_ANGLE) { panAngle = MAX_ANGLE; stepDirection = -stepDirection; }
    if (panAngle <= MIN_ANGLE) { panAngle = MIN_ANGLE; stepDirection = -stepDirection; }

    panServo.write(panAngle);
    Serial.print("Servo angle: "); Serial.println(panAngle);
  }

  delay(120); // smooth motion
}
