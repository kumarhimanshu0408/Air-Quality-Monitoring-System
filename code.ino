
#include "MQ7.h"
#include <SPI.h>
#include <WiFi.h>
#include "esp32-hal-adc.h"  // needed for adc pin reset
#include "soc/sens_reg.h"   // needed for adc pin reset
#include <WiFi.h>
#include <WebServer.h>
#include <ArduinoJson.h>
#include <ESPmDNS.h>
#define ESP32

#include "SDS011.h"

#define max(a, b) (a) > (b) ? (a) : (b)
#define rxPin 16
#define txPin 17
SDS011 nova_sds;

uint64_t reg_b;
#define CO_PIN 35
#define VOLTAGE 5

String ssid = "realme C35";
String password = "Ram@3213";

#define numReadingsPM10 24
#define numReadingsPM25 24
#define numReadingsCO 8

// init MQ7 device
MQ7 mq7(CO_PIN, VOLTAGE);

WebServer server(80);

float ConcentrationINmgm3;
float readingsPM10[numReadingsPM10];  // the readings from the analog input
int readIndexPM10 = 0;                // the index of the current reading
float totalPM10 = 0;                  // the running total
float averagePM10 = 0;                // the average
float readingsPM25[numReadingsPM25];  // the readings from the analog input
int readIndexPM25 = 0;                // the index of the current reading
float totalPM25 = 0;                  // the running total
float averagePM25 = 0;                // the average
float readingsCO[numReadingsCO];      // the readings from the analog input
int readIndexCO = 0;                  // the index of the current reading
float totalCO = 0;                    // the running total
float averageCO = 0;                  // the average

extern const String html;
extern const String change_mode_html;

unsigned long previousMillis = 0;
unsigned int get_read = 1;
unsigned long avg_interval = 2500;  // get average every 5 seconds

IPAddress serverIP;  // to store the Ip address
IPAddress station_serverIP;  // to store the Ip address
static bool isStationMode = false;
const String correctPassword = "YDuthi'25";

const char *Station_SSID = "yourAP";
const char *station_password = "yourPassword";

int error, AQI;
float p10, p25, MQ7ppm;  // to temporarily store values from sds

void handleNotFound() {
  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET) ? "GET" : "POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i = 0; i < server.args(); i++) {
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);
}

bool setUpWifiAccessPointMode() {
  // starting wifi
  WiFi.disconnect(true);
  WiFi.mode(WIFI_AP_STA);

  WiFi.begin(ssid, password);
  Serial.println("\nConnecting to WiFi Network ..");
  Serial.println(ssid);
  Serial.println(password);

  unsigned long lpreviousMillis = millis();
  unsigned long lcurrentMillis;
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    lcurrentMillis = millis();
    if (lcurrentMillis - lpreviousMillis >= 10000) {
      return false;
    }
    delay(100);
  }

  Serial.println("\nConnected to the WiFi network");
  Serial.print("Local ESP32 IP: ");

  serverIP = WiFi.localIP();
  Serial.println(serverIP);

  startServer();
  return true;
  // if (MDNS.begin("esp32")) {
  //   Serial.println("MDNS responder started");
  // }
}

bool setUpWifiStationMode() {
  Serial.println();
  Serial.println("Configuring access point...");

  // You can remove the password parameter if you want the AP to be open.
  // a valid password must have more than 7 characters
  

  if (!WiFi.softAP(Station_SSID, station_password)) {
    log_e("Soft AP creation failed.");
    Serial.println("Soft AP creation Failed...");
    setUpWifiAccessPointMode();
    Serial.println("Returning from here...");
    return false;
  }

  station_serverIP = WiFi.softAPIP();
  Serial.print("AP IP address: ");
  Serial.println(station_serverIP);

  return true;
}

void startServer() {
  server.on("/change", []() {
    String to_send = change_mode_html;
    to_send.replace("##IP##", serverIP.toString());
    to_send.replace("##SSID##", ssid);
    to_send.replace("##PASSWORD##", password);
    server.send(200, "text/html", to_send);
  });

  server.on("/admin/change_creds", HTTP_POST, []() {
    if (server.hasArg("plain") == false) {
      server.send(400, "application/json", "{\"success\":false, \"message\":\"Invalid Request\"}");
      return;
    }

    String body = server.arg("plain");
    DynamicJsonDocument doc(256);
    DeserializationError error = deserializeJson(doc, body);

    if (error) {
      server.send(400, "application/json", "{\"success\":false, \"message\":\"Invalid JSON\"}");
      return;
    }

    String receivedPassword = doc["password"];
    String receivedSSID = doc["ssid"];
    String receivedAdmin = doc["admin"];

    if (receivedAdmin != correctPassword) {
      server.send(403, "application/json", "{\"success\":false, \"message\":\"Incorrect Admin Password\"}");
      return;
    }


    DynamicJsonDocument response(256);
    response["success"] = true;

    String prev_ssid = ssid;
    String prev_password = password;
    ssid = receivedSSID;
    password = receivedPassword;
    
    if (!setUpWifiAccessPointMode()) {
      response["success"] = false;
    }
    if (response["success"]) {
      response["ip"] = serverIP.toString();
    } else {
      ssid = prev_ssid;
      password = prev_password;
      if (!setUpWifiAccessPointMode()) {
        response["message"] = "Connection Timeout. Unable to connect to previous ssid. Status: Not connected.";
      } else {
        response["message"] = "Connection Timeout. Fallback to previous ssid. Status: Connected.";
      }
    }
    response["ssid"] = ssid;
    response["password"] = password;

    String responseString;
    serializeJson(response, responseString);
    server.send(200, "application/json", responseString);
  });


  server.on("/read", []() {
    String json = "{";
    // json += "\"AQI\": " + String(AQI) + ",";
    json += "\"PM2.5\": " + String(averagePM25) + ",";
    json += "\"PM10\": " + String(averagePM10) + ",";
    json += "\"CO\": " + String(averageCO);
    json += "}";

    server.sendHeader("Access-Control-Allow-Origin", "*");
    server.send(200, "application/json", json);
  });

  server.on("/", []() {
    server.send(200, "text/html", html);
  });


  server.onNotFound(handleNotFound);

  server.begin();
  Serial.println("HTTP server started");
}

// int lower[8] = {0, 51, 101, 201, 301,401, 501, 1001};
// float concl_p10[8] = {0, 51,101,251,351,431,605, 1005};
// float concl_p25[8] = {0, 31,61,91,121,251,501, 605};
// float concl_co[8] = {0, 1.1,2.1,10.1,17.1,34,51, 61};

/**

NOTE : This function is not used in the sensor because of its high cost. 
Once we know the average umg/m3 values . Then we don't need to calculate AQI here only .. 
It can be calculated on the client's end. 
Hence weights required by this functions are also commented.

**/
// int get_subindex(float* conc, float curr) {
//   int index = 0;
//   while (index < 7) {
//     if (curr < conc[index]) {
//       index --;
//       break;
//     }
//     index ++;
//   }
//   float denominator = (conc[index + 1] - conc[index]);
//   int conc_diff = lower[index + 1] - lower[index];
//   float extra = ( (curr - conc[index]) * conc_diff ) / denominator;
//   return (int)(lower[index] + extra);
// }

void getReadings() {
  MQ7ppm = max((mq7.readPpm() ), 0);
  Serial.print("CO: ");
  Serial.println(MQ7ppm);


  // reading values from sds sensor
  error = nova_sds.read(&p25, &p10);

  if (!error) {
  } else {
    Serial.println("Error in reading Nova Sensor:  " + String(error));
  }
}

void calculateAverage() {

  ConcentrationINmgm3 = MQ7ppm * (1.146);  // converting PPM to mg/m3. Ideal temperature and pressure must also be included in calculation but we are not reading temperature and pressure for now.

  // calculating average PM10 using circular history
  totalPM10 = totalPM10 - readingsPM10[readIndexPM10];  // removing previous reading from total
  readingsPM10[readIndexPM10] = p10;
  totalPM10 = totalPM10 + readingsPM10[readIndexPM10];  // adding new reading from total
  readIndexPM10 = readIndexPM10 + 1;
  if (readIndexPM10 >= numReadingsPM10) {
    readIndexPM10 = 0;
  }

  averagePM10 = totalPM10 / numReadingsPM10;

  // calculating average PM25
  totalPM25 = totalPM25 - readingsPM25[readIndexPM25];  // removing previous reading from total
  readingsPM25[readIndexPM25] = p25;
  totalPM25 = totalPM25 + readingsPM25[readIndexPM25];  // adding new reading from total
  readIndexPM25 = readIndexPM25 + 1;
  if (readIndexPM25 >= numReadingsPM25) {
    readIndexPM25 = 0;
  }
  averagePM25 = totalPM25 / numReadingsPM25;

  // average CO concentration
  totalCO = totalCO - readingsCO[readIndexCO];  // removing previous reading from total
  readingsCO[readIndexCO] = ConcentrationINmgm3;
  totalCO = totalCO + readingsCO[readIndexCO];  // adding new reading from total
  readIndexCO = readIndexCO + 1;
  if (readIndexCO >= numReadingsCO) {
    readIndexCO = 0;
  }
  averageCO = totalCO / numReadingsCO;

  Serial.println(averageCO);
  Serial.println(averagePM10);
  Serial.println(averagePM25);
}

void setup() {
  nova_sds.begin(rxPin, txPin);
  Serial.begin(9600);
  while (!Serial) {
    ;  // wait for serial connection
  }

  Serial.println("");  // blank new line

  WiFi.mode(WIFI_AP_STA);
  setUpWifiAccessPointMode();
  setUpWifiStationMode();

  Serial.println("Calibrating MQ7");

  mq7.calibrate();  // calculates R0

  Serial.println("Calibration done!");
}

void loop() {
  server.handleClient();
  unsigned long currentMillis = millis();

  // read after atleast "interval" millisecs
  if ((unsigned long)(currentMillis - previousMillis) >= avg_interval) {
    previousMillis = currentMillis;
    if (get_read == 1) {
      getReadings();
      get_read = 0;
    } else {
      get_read = 1;
    }
    calculateAverage();
  }

  delay(1000);
}
