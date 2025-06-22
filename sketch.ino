#include <WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <WebServer.h>

// Sensor setup
#define DHTPIN 5
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
const int mq135Pin = 34;

// OLED setup
#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define OLED_RESET -1
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// WiFi credentials
const char* ssid = "Wokwi-GUEST";
const char* password = "";

// Web server setup
WebServer server(80);

// Helper function for jogging advice
String getAdvice(int aqi) {
  if (aqi < 300) return " Good - Jog freely";
  else if (aqi < 600) return " Moderate - Jog slowly";
  else return " Poor - Avoid jogging";
}

void setup() {
  Serial.begin(115200);

  // Start sensors
  dht.begin();
  Wire.begin(21, 22); // SDA, SCL for Wokwi I2C

  // Initialize OLED
  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    while (true);
  }

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Jiji Safi Starting...");
  display.display();
  delay(2000);

  // Connect to WiFi
  WiFi.begin(ssid, password);
  Serial.print("Connecting to WiFi");
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }

  Serial.println("\nWiFi connected!");
  Serial.print("Access your server at: http://");
  Serial.println(WiFi.localIP());

  display.clearDisplay();
  display.setCursor(0, 0);
  display.println("WiFi Connected!");
  display.println(WiFi.localIP());
  display.println("Server ready");
  display.display();
  delay(2000);

  // Setup web server route
  server.on("/", HTTP_GET, []() {
    float temp = dht.readTemperature();
    float humid = dht.readHumidity();
    int air_quality = analogRead(mq135Pin);
    String joggingAdvice = getAdvice(air_quality);

    // Web HTML output
    String htmlPage = "<!DOCTYPE html><html><head><meta http-equiv='refresh' content='5'/><title>Jiji Safi</title></head><body>";
    htmlPage += "<h2>Jiji Safi - Air Quality Monitor</h2>";
    htmlPage += "<p>Temperature: " + String(temp) + " °C</p>";
    htmlPage += "<p>Humidity: " + String(humid) + " %</p>";
    htmlPage += "<p>Air Quality: " + String(air_quality) + "</p>";
    htmlPage += "<h3>Advice: " + joggingAdvice + "</h3>";
    htmlPage += "</body></html>";

    server.send(200, "text/html", htmlPage);
  });

  server.begin();
}

void loop() {
  server.handleClient();

  // Periodically update OLED
  float temp = dht.readTemperature();
  float humid = dht.readHumidity();
  int air_quality = analogRead(mq135Pin);
  String advice = getAdvice(air_quality);

  display.clearDisplay();
  display.setCursor(0, 0);
  display.setTextSize(1);
  display.println("Jiji Safi Monitor");
  display.print("Temp: ");
  display.print(temp);
  display.println(" C");
  display.print("Humidity: ");
  display.print(humid);
  display.println(" %");
  display.print("Air: ");
  display.println(air_quality);
  display.println(advice);
  display.display();

  delay(3000);
}
