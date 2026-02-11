
#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>

// ---------------- MESH ----------------
#define MESH_PREFIX    "HAZARD_MESH"
#define MESH_PASSWORD "mesh1234"
#define MESH_PORT     5555

// ---------------- PINS ----------------
#define MQ2_PIN 34
#define DHT_PIN 4
#define DHT_TYPE DHT11   // Change to DHT22 if needed

// ---------------- THRESHOLDS ----------------
#define SMOKE_THRESHOLD 1200
#define TEMP_THRESHOLD 30

// ---------------- FIXED LOCATION ----------------
#define LATITUDE  16.791902
#define LONGITUDE 80.82113

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

painlessMesh mesh;
DHT dht(DHT_PIN, DHT_TYPE);
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// ---------- SERIAL + OLED PRINT ----------
void printBoth(String l1, String l2, String l3, String l4) {
  // Serial
  Serial.println(l1);
  Serial.println(l2);
  Serial.println(l3);
  Serial.println(l4);
  Serial.println("--------------------");

  // OLED
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println(l1);
  display.println(l2);
  display.println(l3);
  display.println(l4);
  display.display();
}

bool fireActive = false;
unsigned long lastSend = 0;

void setup() {
  Serial.begin(115200);

  // MQ2 ADC setup
  analogReadResolution(12);
  analogSetPinAttenuation(MQ2_PIN, ADC_11db);

  // Init sensors
  dht.begin();
  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  printBoth(
    "FIRE NODE",
    "Starting...",
    "",
    ""
  );

  // Mesh start
  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  delay(1500);
}

void loop() {
  mesh.update();

  int smoke = analogRead(MQ2_PIN);
  float temp = dht.readTemperature();

  bool fireNow = (smoke > SMOKE_THRESHOLD || temp > TEMP_THRESHOLD);

  // ---------- NORMAL MODE ----------
  if (!fireNow) {
    fireActive = false;

    printBoth(
      "FIRE NODE",
      "Temp: " + String(temp, 1) + " C",
      "Smoke: " + String(smoke),
      "Status: NORMAL"
    );
  }

  // ---------- FIRE ALERT MODE ----------
  else {
    printBoth(
      "🔥 FIRE ALERT 🔥",
      "TEMP / SMOKE HIGH",
      "EVACUATE!",
      ""
    );

    // Send JSON every 5 seconds
    if (!fireActive || millis() - lastSend > 5000) {
      fireActive = true;
      lastSend = millis();

      StaticJsonDocument<256> doc;
      doc["node"]  = "FIRE";
      doc["fire"]  = 1;
      doc["alert"] = 1;
      doc["temp"]  = temp;
      doc["smoke"] = smoke;
      doc["lat"]   = LATITUDE;
      doc["long"]  = LONGITUDE;

      String json;
      serializeJson(doc, json);

      Serial.println("JSON SENT TO GATEWAY:");
      Serial.println(json);

      mesh.sendBroadcast(json);
    }
  }

  delay(1000);
}
