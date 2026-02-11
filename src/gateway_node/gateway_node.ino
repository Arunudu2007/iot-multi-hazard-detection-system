#include <painlessMesh.h>
#include <ArduinoJson.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>

// ---------------- MESH ----------------
#define MESH_PREFIX    "HAZARD_MESH"
#define MESH_PASSWORD  "mesh1234"
#define MESH_PORT      5555

// ---------------- WIFI ----------------
#define WIFI_SSID      "YOUR_WIFI_NAME"
#define WIFI_PASSWORD  "YOUR_WIFI_PASSWORD"

// ---------------- BACKEND ----------------
#define BACKEND_URL "https://haznet.vercel.app/ingest"

painlessMesh mesh;

// ---------- SEND TO BACKEND ----------
void sendToBackend(String payload) {

  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi not connected.");
    return;
  }

  WiFiClientSecure client;
  client.setInsecure();   // For testing

  HTTPClient http;
  http.begin(client, BACKEND_URL);
  http.addHeader("Content-Type", "application/json");

  int httpCode = http.POST(payload);

  Serial.print("HTTP Response Code: ");
  Serial.println(httpCode);

  http.end();
}

// ---------- RECEIVE CALLBACK ----------
void receivedCallback(uint32_t from, String &msg) {

  Serial.println("\n=================================");
  Serial.print("From Node ID: ");
  Serial.println(from);
  Serial.print("Raw JSON: ");
  Serial.println(msg);

  StaticJsonDocument<256> doc;
  if (deserializeJson(doc, msg)) {
    Serial.println("JSON Parse Error");
    return;
  }

  const char* node = doc["node"];

  if (doc["alert"] == 1) {

    Serial.print("ALERT FROM: ");
    Serial.println(node);

    if (strcmp(node, "FIRE") == 0) {
      Serial.print("Temp: ");
      Serial.println((float)doc["temp"]);
      Serial.print("Smoke: ");
      Serial.println((int)doc["smoke"]);
    }

    else if (strcmp(node, "GAS") == 0) {
      Serial.print("Gas Raw: ");
      Serial.println((int)doc["gas"]);
      Serial.print("Gas %: ");
      Serial.println((float)doc["gas_percent"]);
    }

    else if (strcmp(node, "SOS") == 0) {
      Serial.println("SOS Emergency Triggered");
    }

    else if (strcmp(node, "VIBRATION") == 0) {
      Serial.print("X: ");
      Serial.println((float)doc["x"]);
      Serial.print("Y: ");
      Serial.println((float)doc["y"]);
      Serial.print("Z: ");
      Serial.println((float)doc["z"]);
    }

    Serial.print("Location: ");
    Serial.print((float)doc["lat"]);
    Serial.print(", ");
    Serial.println((float)doc["long"]);

    sendToBackend(msg);
  }

  Serial.println("=================================");
}

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_AP_STA);

  mesh.init(MESH_PREFIX, MESH_PASSWORD, MESH_PORT);
  mesh.onReceive(&receivedCallback);

  Serial.println("Connecting to WiFi...");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\nWiFi Connected");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  mesh.update();
}
