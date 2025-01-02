#include <Wire.h>
#include <WiFi.h>
#include <WebServer.h>
#include <DHT.h>    // Include the DHT library
#include "BMP180_ESP32.h"
#include "index.h" // Our HTML webpage contents with javascripts

// Define sensor pins
DHT dht(37, DHT11);   // Define DHT sensor
SFE_BMP180 bmp180;    // Create an instance of the BMP180 class

// Define custom pins for I2C
#define SDA_PIN 36  // Change this to your desired SDA pin
#define SCL_PIN 35  // Change this to your desired SCL pin
#define P0 1013.25  // Standard atmospheric pressure at sea level (in hPa)

// Wi-Fi credentials
const char* ssid = "Slippers2sat";
const char* password = "S2S#2024";

// Create a web server on port 80
WebServer server(80);

// Serve the HTML page
void handleRoot() {
  String s = MAIN_page;  // Assuming MAIN_page is defined in "index.h"
  server.send(200, "text/html", s);
}

// Sensor data variables
float humidity, temperature;
double Temperature, pressure, Altitude;

void handleADC() {
  // Start temperature reading
  if (bmp180.startTemperature()) {
    delay(5);  // Wait for the temperature reading to complete

    // Get the temperature value and store it in the 'temperature' variable
    if (bmp180.getTemperature(Temperature)) {
      Temperature = (int)Temperature;  // Convert to integer
    }

    // Start pressure reading (using oversampling = 3 for higher resolution)
    if (bmp180.startPressure(3)) {
      delay(26);  // Wait for the pressure reading to complete

      // Get the pressure value and store it in the 'pressure' variable
      if (bmp180.getPressure(pressure, Temperature)) {
        pressure = (int)pressure;  // Convert to integer
      }
    }
  }

  // Calculate altitude using the pressure value
  int altitude = bmp180.altitude(pressure, P0);
  
  // Read humidity and temperature from DHT sensor
  humidity = dht.readHumidity();  // Read humidity
  temperature = dht.readTemperature();  // Read temperature

  // Prepare the sensor data as JSON
  String data = "{\"Pressure\":\"" + String(pressure) + "\", \"Altitude\":\"" + String(altitude) + "\", \"Temperature\":\"" + String(temperature) + "\", \"Humidity\":\"" + String(humidity) + "\"}";

  // Send the data
  server.send(200, "text/plain", data);
}

void setup() {
  Serial.begin(115200);
  Serial.println();

  // Initialize DHT sensor
  dht.begin();

  // Start I2C communication with custom SDA and SCL pins
  Wire.begin(SDA_PIN, SCL_PIN);

  // Initialize BMP180 sensor
  if (!bmp180.begin()) {
    Serial.println("Failed to initialize BMP180 sensor");
    while (1);  // Stop execution if sensor is not found
  }

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  Serial.println();
  
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Define server routes
  server.on("/", handleRoot);
  server.on("/readADC", handleADC);

  // Start the server
  server.begin();
  Serial.println("HTTP server started");

  Serial.print("WiFi Status: ");
  Serial.println(WiFi.status());
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
}

void loop() {
  server.handleClient();  // Handle incoming client requests
}
