#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>
#include "FastLED.h"

bool firstSketch = true;

WiFiServer server(80);
WiFiClient client;

#define NUM_LEDS 256
#define DATA_PIN 21

CRGB leds[NUM_LEDS];

int ledIndex = 0;

void clearLedArray() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = 0x000000;
  }
}

void setConfigScreen() {
  clearLedArray();

  leds[0] = 0x0000FF;
  leds[6] = 0x0000FF;
  leds[9] = 0x0000FF;
  leds[15] = 0x0000FF;
  leds[21] = 0x0000FF;
  leds[26] = 0x0000FF;
  leds[36] = 0x0000FF;
  leds[43] = 0x0000FF;
  leds[102] = 0x0000FF;
  leds[105] = 0x0000FF;
  leds[150] = 0x0000FF;
  leds[153] = 0x0000FF;
  leds[240] = 0x0000FF;
  leds[255] = 0x0000FF;

  FastLED.show();
}

void setReadyScreen() {
  
  clearLedArray();
  leds[0] = 0x00FF00;
  leds[15] = 0x00FF00;
  leds[23] = 0x00FF00;
  leds[24] = 0x00FF00;
  leds[38] = 0x00FF00;
  leds[39] = 0x00FF00;
  leds[40] = 0x00FF00;
  leds[41] = 0x00FF00;
  leds[135] = 0x00FF00;
  leds[119] = 0x00FF00;
  leds[120] = 0x00FF00;
  leds[136] = 0x00FF00;
  leds[255] = 0x00FF00;
  leds[240] = 0x00FF00;
  FastLED.show();
}

void setup() {
  //Starting up serial
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10); //Number 0-255
  FastLED.clear();

  setConfigScreen();

  Serial.println("Starting Sketch");

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // wm.resetSettings();

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("Pixel-Sketch-Config");

  if(!res) {
      Serial.println("Failed to connect");
      // ESP.restart();
  } else {
    //if you get here you have connected to the WiFi    
    Serial.println("connected...yeey :)");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());

    // Initialize mDNS
    if (!MDNS.begin("pixelsketch")) {   // Set the hostname to "esp32.local"
      Serial.println("Error setting up MDNS responder!");
      while(1) {
        delay(1000);
      }
    }
    Serial.println("mDNS responder started");

    server.begin();
    Serial.println("HTTP server started");

    setReadyScreen();
  }
}

void parseIncomingData() {
  Serial.println("Parsing Data");
  static String buffer = "["; // Buffer to store incoming data
  // Show the LEDs if it's the first update
  if (firstSketch) {
    clearLedArray();
    firstSketch = false;
  }
  while (client.available() > 0) {
    char received = client.read();
    buffer += received;

    if (received == ']') { // End of data packet
      while (buffer.indexOf('[') != -1) {
        int startIdx = buffer.indexOf('[');
        int endIdx = buffer.indexOf(']', startIdx);
        Serial.println(buffer);
        if (endIdx == -1) {
          break; // Malformed data; exit loop
        }

        // Extract content within [ ]
        String pair = buffer.substring(startIdx + 1, endIdx);
        buffer = buffer.substring(endIdx + 1); // Remove processed part

        // Split the pair into index and color
        int commaIdx = pair.indexOf(',');
        if (commaIdx == -1) {
          continue; // Malformed pair; skip it
        }

        // Parse LED index
        int ledIndex = pair.substring(0, commaIdx).toInt();

        // Parse hex color value
        String colorStr = pair.substring(commaIdx + 1);
        long colorValue = strtol(colorStr.c_str(), nullptr, 16);

        // Assign color to LED if the index is valid
        if (ledIndex >= 0 && ledIndex < NUM_LEDS) {
          Serial.print("Led Index: ");
          Serial.print(ledIndex);
          Serial.print(" Color: ");
          Serial.println(colorValue);
          leds[ledIndex] = CRGB((colorValue >> 16) & 0xFF, (colorValue >> 8) & 0xFF, colorValue & 0xFF);
        }
      }

      buffer = "";    // Clear buffer for the next packet
    }
  }
}

void loop() {
  // listen for incoming clients
  client = server.available();
  if (client) {
    // Serial.println("Client Connected");
    while (client.connected()) {

      while (client.available() > 0) {
          parseIncomingData();
      }

      FastLED.show();
    }
    client.stop();
    // Serial.println("Client disconnected");
    }
}
