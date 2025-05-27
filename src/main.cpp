#include <WiFi.h>
#include <WiFiUdp.h>
#include <NTPClient.h>
#include <MD_Parola.h>
#include <MD_MAX72xx.h>
#include <SPI.h>

// LED Matrix config
#define HARDWARE_TYPE MD_MAX72XX::FC16_HW
#define MAX_DEVICES 4
#define CS_PIN 5

MD_Parola display = MD_Parola(HARDWARE_TYPE, CS_PIN, MAX_DEVICES);

// Wi-Fi config
const char* ssid = "765KV";
const char* password = "transistor";

// NTP client for IST (UTC+5:30 = 19800 seconds)
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 19800, 60000); // Using direct IP

// Blinking colon variables
bool showColon = true;
unsigned long previousMillis = 0;
const long blinkInterval = 1000;

void setup() {
  Serial.begin(115200);
  display.begin();
  display.setIntensity(1);
  display.displayClear();

  WiFi.begin(ssid, password);
  Serial.print("Connecting to Wi-Fi");

  // Setup initial scroll
  display.displayScroll("Connecting to WiFi", PA_CENTER, PA_SCROLL_LEFT, 50);

  // Loop until connected
  while (WiFi.status() != WL_CONNECTED) {
    if (display.displayAnimate()) {
      // Restart scrolling when finished
      display.displayScroll("Connecting to WiFi", PA_CENTER, PA_SCROLL_LEFT, 50);
    }
    delay(10);
  }

  Serial.println("\nWiFi connected");
  display.displayClear();
  display.print("WiFi OK");
  delay(1000);

  timeClient.begin();
  display.displayClear();
}

void loop() {
  timeClient.update();

  // Blink colon every second
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= blinkInterval) {
    previousMillis = currentMillis;
    showColon = !showColon;
  }

  // Format and display time
  String timeStr = timeClient.getFormattedTime();  // Format: HH:MM:SS
  String hours = timeStr.substring(0, 2);
  String minutes = timeStr.substring(3, 5);
  String colon = showColon ? ":" : " ";
  String displayStr = hours + colon + minutes;
  

  display.setTextAlignment(PA_CENTER);
  display.print(displayStr);

  delay(200); // Refresh rate

}
