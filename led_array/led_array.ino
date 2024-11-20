#include "FastLED.h"
#include "WiFi.h"
 
const char* ssid = "Verizon_B3FQWG";
const char* password =  "few6-burrow-pet";
IPAddress ip(192, 168, 1, 161);
WiFiServer server(80);
WiFiClient client;

#define NUM_LEDS 256
#define DATA_PIN 33

CRGB leds[NUM_LEDS];
CRGB COLORS[16];
int ledIndex = 0;

void setup() {
  // put your setup code here, to run once:
  delay(2000);
  Serial.begin(115200);
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.setBrightness(10); //Number 0-255
  FastLED.clear();

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

  WiFi.config(ip);
  WiFi.begin(ssid, password);
 
  while (WiFi.status() != WL_CONNECTED) {
    delay(3000);
    Serial.println("Connecting to WiFi..");
  }
 
  Serial.println("Connected to the WiFi network");
  ip = WiFi.localIP();
  server.begin();
  Serial.print("IP Address: ");
  Serial.println(ip);
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
  
}

void loop() {
  // listen for incoming clients
  client = server.available();
  if (client) {
    while (client.connected()) {

      while (client.available() > 0) {
        char received = client.read();
        if (received == '[') {
          parseIncomingData();
        } 
      }

      delay(10);
    }

    FastLED.show();

    client.stop();
    Serial.println("Client disconnected");

    }
}
  

