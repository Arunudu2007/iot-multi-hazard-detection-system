
#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- MESH ----------------
#define MESH_PREFIX    "HAZARD_MESH"
#define MESH_PASSWORD "mesh1234"
#define MESH_PORT     5555

// ---------------- MQ135 ----------------
#define MQ135_PIN 35
#define GAS_THRESHOLD 900   // mouth-blow demo

// ---------------- FIXED LOCATION ----------------
#define LATITUDE  16.791902
#define LONGITUDE 80.82113

// ---------------- OLED ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

painlessMesh mesh;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

bool gasActive = false;
unsigned long lastSend = 0;

// ---------- COMMON PRINT ----------
void printBoth(String l1, String l2, String l3, String l4) {
  Serial.println(l1);
  Serial.println(l2);
  Serial.println(l3);
  Serial.println(l4);
  Serial.println("--------------------");

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

void setup() {
  Serial.begin(115200);

  analogReadResolution(12);
  analogSetPinAttenuation(MQ135_PIN, ADC_11db);

  Wire.begin(21, 22);
  display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);

  printBoth("GAS NODE", "Starting...", "Warming up", "");

  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  delay(15000);   // warm-up
}

void loop() {
  mesh.update();

  int gas_raw = analogRead(MQ135_PIN);

  // ✅ Gas percentage calculation
  float gas_percent = (gas_raw / 4095.0) * 100.0;

  bool gasNow = (gas_raw > GAS_THRESHOLD);

  // ---------- NORMAL ----------
  if (!gasNow) {
    gasActive = false;

    printBoth(
      "GAS NODE",
      "Raw: " + String(gas_raw),
      "Gas: " + String(gas_percent, 1) + " %",
      "Status: NORMAL"
    );
  }

  // ---------- ALERT ----------
  else {
    printBoth(
      "🌫 GAS ALERT 🌫",
      "LEVEL HIGH!",
      "Gas: " + String(gas_percent, 1) + "%",
      ""
    );

    if (!gasActive || millis() - lastSend > 5000) {
      gasActive = true;
      lastSend = millis();

      StaticJsonDocument<256> doc;
      doc["node"]        = "GAS";
      doc["gas"]         = gas_raw;
      doc["gas_percent"] = gas_percent;   // 
      doc["alert"]       = 1;
      doc["lat"]         = LATITUDE;
      doc["lang"]        = LONGITUDE;

      String json;
      serializeJson(doc, json);

      Serial.println("JSON SENT TO GATEWAY:");
      Serial.println(json);

      mesh.sendBroadcast(json);
    }
  }

  delay(1000);
}
