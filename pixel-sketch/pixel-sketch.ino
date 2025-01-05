#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>
#include "FastLED.h"

bool firstSketch = true;

WiFiServer server(80);
WiFiClient client;

#define NUM_LEDS 256
#define DATA_PIN 21

CRGB leds[NUM_LEDS];
CRGB COLORS[8] = {
  CRGB::White,
  CRGB::Black,
  CRGB::Red,
  CRGB::Green,
  CRGB::Blue,
  CRGB::Yellow,
  CRGB::Cyan,
  CRGB::Magenta,
};

int ledIndex = 0;

void clearLedArray() {
  for (int i = 0; i < NUM_LEDS; i++) {
    leds[i] = COLORS[1];
  }
}

void setConfigScreen() {
  clearLedArray();

  leds[0] = COLORS[4];
  leds[6] = COLORS[4];
  leds[9] = COLORS[4];
  leds[15] = COLORS[4];
  leds[21] = COLORS[4];
  leds[26] = COLORS[4];
  leds[36] = COLORS[4];
  leds[43] = COLORS[4];
  leds[102] = COLORS[4];
  leds[105] = COLORS[4];
  leds[150] = COLORS[4];
  leds[153] = COLORS[4];
  leds[240] = COLORS[4];
  leds[255] = COLORS[4];

  FastLED.show();
}

void setReadyScreen() {
  
  clearLedArray();
  leds[0] = COLORS[3];
  leds[15] = COLORS[3];
  leds[23] = COLORS[3];
  leds[24] = COLORS[3];
  leds[38] = COLORS[3];
  leds[39] = COLORS[3];
  leds[40] = COLORS[3];
  leds[41] = COLORS[3];
  leds[135] = COLORS[3];
  leds[119] = COLORS[3];
  leds[120] = COLORS[3];
  leds[136] = COLORS[3];
  leds[255] = COLORS[3];
  leds[240] = COLORS[3];
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

bool isCharInt(int character) {
  if (character < 48 || character > 57) {
    return false;
  }
  return true;
}

int getNextInteger(int maxNumDigits, char terminator) {
  int number = 0;
  char received;
  do {
    received = client.read();

    if (isCharInt(received)) {
      // Serial.print("Interger Found: ");
      // Serial.println(received);
      number = number * 10 + (received - '0');
    } else if (received == terminator) {
      return number;
    }
      else {
      // Serial.print("Non Integer Character: ");
      // Serial.println(received);
      return -1;
    }
  } while (int(received) != -1 && received != ']');
}

void parseIncomingData() {
  int ledIndex = getNextInteger(3, ':');

  // Serial.print("LED Index: ");
  // Serial.println(ledIndex);

  if (ledIndex < 0 || ledIndex > 255) {
    return;
  }

  int colorIndex = getNextInteger(2, ']');

  // Serial.print("Color Index: ");
  // Serial.println(colorIndex);
  
  if (colorIndex < 0 || colorIndex > 16) {
    return;
  }

  if (firstSketch) {
    clearLedArray();
    firstSketch = false;
  }

  leds[ledIndex] = COLORS[colorIndex];
}

void loop() {
  // listen for incoming clients
  client = server.available();
  if (client) {
    // Serial.println("Client Connected");
    while (client.connected()) {

      while (client.available() > 0) {
        char received = client.read();
        if (received == '[') {
          parseIncomingData();
        } else if (received == ';') {
          FastLED.show();
          delay(500);
        }
      }

      FastLED.show();
    }
    client.stop();
    // Serial.println("Client disconnected");
    }
}
