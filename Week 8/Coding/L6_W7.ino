#include <BluetoothSerial.h>
#include <DHT.h>

#define DHT11_PIN 21
#define DHTTYPE   DHT11
#define LED_PIN   2   // GPIO2 for external LED

String device_name = "ESP32_FaZim'sLogic";

BluetoothSerial SerialBT;
DHT dht11(DHT11_PIN, DHTTYPE);

#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth not enabled in this ESP32
#endif

void setup() {
  Serial.begin(9600);
  SerialBT.begin(device_name);

  dht11.begin();

  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);

  Serial.println("ESP32 ready. Pair via Bluetooth.");
}

void loop() {
  // ---- Bluetooth LED Control ----
  if (SerialBT.available()) {
    char cmd = SerialBT.read();

    if (cmd == '1') {
      digitalWrite(LED_PIN, HIGH);
      SerialBT.println("LED ON");
    }
    else if (cmd == '0') {
      digitalWrite(LED_PIN, LOW);
      SerialBT.println("LED OFF");
    }
  }

  // ---- DHT11 readings ----
  float humi  = dht11.readHumidity();
  float tempC = dht11.readTemperature();
  float tempF = dht11.readTemperature(true);

  if (!isnan(humi) && !isnan(tempC)) {
    SerialBT.print("Humidity: ");
    SerialBT.print(humi);
    SerialBT.print("%  |  Temp: ");
    SerialBT.print(tempC);
    SerialBT.print("°C / ");
    SerialBT.print(tempF);
    SerialBT.println("°F");
  } else {
    SerialBT.println("Failed to read from DHT11");
  }

  delay(2000);
}
