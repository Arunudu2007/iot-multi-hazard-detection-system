#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_ADXL345_U.h>

// ---------------- MESH CONFIG ----------------
#define MESH_PREFIX     "HAZARD_MESH"
#define MESH_PASSWORD   "mesh1234"
#define MESH_PORT       5555

// ---------------- VIBRATION THRESHOLD ----------------
#define VIB_THRESHOLD 15.0   // Adjust based on testing

// ---------------- FIXED LOCATION ----------------
#define LATITUDE  16.791902
#define LONGITUDE 80.82113

// ---------------- OLED CONFIG ----------------
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define OLED_ADDR 0x3C

painlessMesh mesh;
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);

bool vibrationActive = false;
unsigned long lastSend = 0;

// ---------- OLED + SERIAL ----------
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

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not detected");
    while (1);
  }

  if (!accel.begin()) {
    Serial.println("ADXL345 not detected");
    while (1);
  }

  printBoth("VIBRATION NODE", "Initializing...", "", "");

  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);

  delay(1500);
}

void loop() {
  mesh.update();

  sensors_event_t event;
  accel.getEvent(&event);

  float x = event.acceleration.x;
  float y = event.acceleration.y;
  float z = event.acceleration.z;

  float magnitude = sqrt(x*x + y*y + z*z);

  bool vibrationDetected = (magnitude > VIB_THRESHOLD);

  // ---------- NORMAL ----------
  if (!vibrationDetected) {
    vibrationActive = false;

    printBoth(
      "VIBRATION NODE",
      "X:" + String(x,1),
      "Y:" + String(y,1),
      "Z:" + String(z,1)
    );
  }

  // ---------- ALERT ----------
  else {
    printBoth(
      "📳 VIB ALERT",
      "Shock Detected",
      "Magnitude:" + String(magnitude,1),
      ""
    );

    if (!vibrationActive || millis() - lastSend > 5000) {
      vibrationActive = true;
      lastSend = millis();

      StaticJsonDocument<256> doc;
      doc["node"]  = "VIBRATION";
      doc["alert"] = 1;
      doc["x"]     = x;
      doc["y"]     = y;
      doc["z"]     = z;
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
