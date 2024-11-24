#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <ESPmDNS.h>
#include "FastLED.h"

WiFiServer server(80);
WiFiClient client;

#define NUM_LEDS 256
#define DATA_PIN 33

CRGB leds[NUM_LEDS];
CRGB COLORS[16];
int ledIndex = 0;

void setup() {
  //Starting up serial
  Serial.begin(115200);

  Serial.println("Starting Sketch");

  COLORS[0] = CRGB::Black;
  COLORS[1] = CRGB::White;
  COLORS[2] = CRGB::Red;
  COLORS[3] = CRGB::Lime;
  COLORS[4] = CRGB::Blue;
  COLORS[5] = CRGB::Yellow;
  COLORS[6] = CRGB::Cyan;
  COLORS[7] = CRGB::Magenta;
  COLORS[8] = CRGB::Silver;
  COLORS[9] = CRGB::Gray;
  COLORS[10] = CRGB::Maroon;
  COLORS[11] = CRGB::Olive;
  COLORS[12] = CRGB::Green;
  COLORS[13] = CRGB::Purple;
  COLORS[14] = CRGB::Teal;
  COLORS[15] = CRGB::Navy;

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("Pixel-Sketch-Config");

  if(!res) {
      Serial.println("Failed to connect");
      ESP.restart();
      return;
  }

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

  leds[0] = COLORS[3];
  leds[15] = COLORS[3];
  
  leds[135] = COLORS[3];
  leds[119] = COLORS[3];
  leds[120] = COLORS[3];
  leds[136] = COLORS[3];
  leds[255] = COLORS[3];
  leds[240] = COLORS[3];
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

  Serial.print("LED Index: ");
  Serial.println(ledIndex);

  if (ledIndex < 0 || ledIndex > 255) {
    return;
  }

  int colorIndex = getNextInteger(2, ']');

  Serial.print("Color Index: ");
  Serial.println(colorIndex);
  
  if (colorIndex < 0 || colorIndex > 16) {
    return;
  }

  leds[ledIndex] = COLORS[colorIndex];

  FastLED.show();
}

void loop() {
  // listen for incoming clients
  client = server.available();
  if (client) {
    Serial.println("Client Connected");
    while (client.connected()) {

      while (client.available() > 0) {
        char received = client.read();
        if (received == '[') {
          parseIncomingData();
        } 
      }

      delay(10);
    }
    client.stop();
    Serial.println("Client disconnected");
    }
}
