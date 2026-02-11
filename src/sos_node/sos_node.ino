
#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// ---------------- MESH CONFIG ----------------
#define MESH_PREFIX     "HAZARD_MESH"
#define MESH_PASSWORD   "mesh1234"
#define MESH_PORT       5555

// ---------------- SOS BUTTON ----------------
#define SOS_BUTTON 27

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

bool lastState = HIGH;

// ---------- OLED DISPLAY FUNCTION ----------
void showOLED(String l1, String l2, String l3, String l4) {
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
  pinMode(SOS_BUTTON, INPUT_PULLUP);

  Wire.begin(21, 22);

  if (!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR)) {
    Serial.println("OLED not detected");
    while (1);
  }

  showOLED("SOS NODE", "Initializing...", "", "");
  Serial.println("SOS NODE STARTING");

  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);

  delay(1500);
  showOLED("SOS READY", "Press Button", "", "");
  Serial.println("SOS NODE READY");
}

void loop() {
  mesh.update();

  bool state = digitalRead(SOS_BUTTON);

  // Detect button press (falling edge)
  if (lastState == HIGH && state == LOW) {

    StaticJsonDocument<200> doc;
    doc["node"]  = "SOS";
    doc["alert"] = 1;
    doc["lat"]   = LATITUDE;
    doc["long"]  = LONGITUDE;

    String json;
    serializeJson(doc, json);

    Serial.println("=== SOS ALERT ===");
    Serial.println(json);

    // Send to gateway
    mesh.sendBroadcast(json);

    // Display alert on OLED
    showOLED(
      "🆘 SOS ALERT",
      "Location Sent",
      "LAT: 16.7919",
      "LON: 80.8211"
    );

    delay(2000);

    showOLED("SOS READY", "Press Button", "", "");
  }

  lastState = state;
}
