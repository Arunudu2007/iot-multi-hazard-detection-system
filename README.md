# iot-multi-hazard-detection-system
IoT-based Multi-Hazard Detection and Alert System using ESP32 Mesh Network

# 🔥 IoT Multi-Hazard Detection and Alert System

A working prototype of a distributed IoT-based safety monitoring system using ESP32 mesh networking.

This system detects fire, gas leaks, abnormal vibration, and emergency SOS events, and forwards alerts to a central gateway and web dashboard for monitoring.

---

## 📌 Project Overview

This project demonstrates a scalable multi-node hazard detection architecture using ESP32 microcontrollers connected through a Wi-Fi mesh network.

Each node monitors a specific hazard and sends structured alert data to a central gateway node. The gateway processes the alert and forwards it to a backend server, where alerts can be monitored through a web dashboard.

This prototype has been tested under controlled conditions and verified for reliable operation.

---

## 🚨 Hazards Monitored

### 🔥 Fire Detection Node
- Smoke detection using MQ2
- Temperature monitoring using DHT11
- Matchstick testing supported
- OLED display for live readings

### 🌫 Gas Leak Detection Node
- MQ135 gas sensor
- Gas percentage calculation
- Perfume threshold testing
- Coffee smell filtered (no false trigger)

### 🆘 SOS Emergency Node
- Manual push-button trigger
- Immediate alert broadcast
- Location tagging

### 📳 Vibration Detection Node
- ADXL345 accelerometer
- Detects abnormal shock or motion
- Suitable for structural monitoring

### 🌐 Gateway Node
- Receives alerts from mesh network
- backend recieves the data in the json format
- Displays alert data via Serial Monitor
- Sends alert to backend server using HTTPS
- Supports web dashboard monitoring
---

## 🧠 System Workflow


The system is modular and scalable for real-world deployment.

---

## 🌐 Web Dashboard Monitoring

Alerts forwarded by the gateway can be monitored in real-time using a web dashboard.

### Dashboard Features:
- Display hazard type
- Show sensor values
- Show location (Latitude & Longitude)
- Monitor multiple nodes
- Log historical alerts

This enables centralized safety monitoring for smart buildings and industrial environments.

---

## 📍 Location Handling Strategy

### Prototype Version (Current)
Fixed GPS coordinates are used for demonstration and integration testing.

Example:
- Latitude: 16.791902  
- Longitude: 80.82113  

This ensures stable backend validation without GPS hardware.

### Real-Time Deployment Version (Future Upgrade)
The architecture supports GPS module integration (e.g., NEO-6M) for accurate live tracking.

Benefits:
- Real-time location mapping
- Disaster response systems
- Industrial-scale deployment
- Map-based dashboard visualization

---

## 🧪 Prototype Testing Results

| Test Scenario | Result |
|---------------|--------|
Matchstick smoke | Fire alert triggered |
Perfume exposure | Gas alert triggered |
Coffee smell | No false alert |
Button press | SOS alert triggered |
Physical shake | Vibration alert triggered |
Mesh transmission | Successful |
Gateway to backend | Successful HTTP delivery |
Dashboard display | Alert visible |

---

## 🛠 Hardware Used

- ESP32 Development Boards
- MQ2 Smoke Sensor
- MQ135 Gas Sensor
- DHT11 Temperature Sensor
- ADXL345 Accelerometer
- SSD1306 OLED Display
- Push Button (SOS)
- Breadboard & Jumper Wires
- 5V Power Supply

---

## 💻 Software & Libraries

- Arduino IDE (1.8.19+)
- painlessMesh
- ArduinoJson
- Adafruit SSD1306
- Adafruit ADXL345
- WiFi & HTTPClient libraries

---

## 🎯 Detection Philosophy

This system performs threshold-based early-stage hazard detection.

It detects:
- Smoke density rise
- Gas concentration increase
- Abnormal vibration magnitude
- Manual emergency trigger

It does not perform predictive hazard forecasting, but the system is architected to support predictive algorithms in future upgrades.

---

## 🔮 Future Enhancements

- GPS-based real-time location tracking
- Machine learning for predictive detection
- Dynamic adaptive thresholds
- Cloud database integration (Firebase / MQTT)
- Real-time map visualization
- Mobile application support

---



