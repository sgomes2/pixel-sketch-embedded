#include "WiFi.h"
 
const char* ssid = "Verizon_B3FQWG";
const char* password =  "few6-burrow-pet";
IPAddress ip(192, 168, 1, 161);
WiFiServer server(80);
WiFiClient client;
 
void setup() {
 
  Serial.begin(115200);
 
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

    client.stop();
    Serial.println("Client disconnected");

  }
}