
# IoT-Based Air Quality Index (AQI) Monitoring System 🌫️📶

A real-time air quality monitoring system built using **ESP32**, **Nova SDS011**, and **MQ-7** sensors. The system calculates AQI based on PM2.5, PM10, and CO concentrations and displays it on a dynamic web interface hosted by the ESP32 itself.

---

## 🚀 Features

- 📡 Wireless Data Transmission via ESP32  
- 🧪 Real-time monitoring of:
  - **PM2.5**
  - **PM10**
  - **Carbon Monoxide (CO)**
- 📈 AQI calculated from sensor values and updated every 5 seconds  
- 🎨 Intuitive and responsive **HTML-based dashboard**  
- 🛡️ Built-in **WiFi credential changer interface**  
- 🌐 Hosted entirely on ESP32, no external server required  
- 📊 Color-coded AQI ranges and values  
- 🔐 Admin password-protected credential updates  

---

## 🔧 Hardware Used

| Component          | Description                          |
|--------------------|--------------------------------------|
| ESP32              | Microcontroller with WiFi capability |
| Nova SDS011        | PM2.5 and PM10 particulate sensor    |
| MQ-7               | Carbon Monoxide (CO) sensor          |
| Jumper wires       | For connections                      |
| Breadboard (optional) | For prototyping                   |

---

## 🛠️ Setup and Installation

### 📦 Arduino Libraries Required

- `MQ7.h`  
- `ArduinoJson`  
- `WiFi.h`  
- `WebServer.h`  
- `ESPmDNS.h`  
- `SDS011.h`  

### 🔌 Wiring

| Sensor      | ESP32 Pin |
|-------------|------------|
| SDS011 RX   | GPIO 16    |
| SDS011 TX   | GPIO 17    |
| MQ-7 Analog | GPIO 35    |

> Make sure to power the MQ-7 with 5V and scale the analog output if necessary.

---

## 🖥️ Web Interface

### `/`  
Main dashboard displaying:
- Real-time AQI progress ring  
- PM2.5, PM10, and CO readings  
- Color-coded cards  
- AQI health level table  

### `/read`  
Returns sensor values in JSON:

```json
{
  "PM2.5": 30.23,
  "PM10": 55.60,
  "CO": 2.51
}
````

### `/change`

Allows WiFi SSID & password update with admin access.

---

## 🔐 Admin Access

To change WiFi credentials, go to `/change`, and provide:

* New SSID
* New Password
* **Admin Password**: `YDuthi'25`

---

## 🧠 AQI Calculation Logic

Each pollutant (PM2.5, PM10, CO) is mapped to a sub-index using its own AQI breakpoint table. The final AQI is the **maximum** of the three.

---

## 🌀 Averaging Logic

To ensure stability and avoid spikes:

* PM2.5: average of last 24 values
* PM10: average of last 24 values
* CO: average of last 8 values

---

## 🧪 Calibration

**MQ-7** undergoes calibration for R₀ during setup using:

```cpp
mq7.calibrate();  // Called in setup()
```

> Ensure MQ-7 has been preheated (24 hours recommended) before use for accurate calibration.

---

## 📸 Screenshots

*(Add screenshots or GIFs of the dashboard, `/read` JSON, and `/change` form here.)*

---

## ⚙️ Project Structure

```plaintext
.
├── main.ino               # ESP32 source code
├── html                   # HTML & JS embedded in code
├── MQ7.h                  # MQ-7 sensor driver
└── README.md              # Project documentation
```

---

## 📅 Update Frequency

Sensor switching and reading every **2.5 seconds**
AQI display and average update every **5 seconds**

---

## 📌 Future Improvements

* Add temperature/humidity for CO compensation
* EEPROM storage of WiFi credentials
* Add OTA update and data logging
* Buzzer/LED alert integration

---

