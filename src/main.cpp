#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include "esp_wpa2.h"
#include "credentials.h" // Include your credentials header file

const char* SSID = "eduroam";

WiFiServer server(80);
String header;


void setup() {
  Serial.begin(115200);

  Serial.print("Connecting to:  ");
  Serial.println(SSID);
  Serial.print("Using username: ");
  Serial.println(USERNAME);

  WiFi.mode(WIFI_STA);
  WiFi.disconnect(true);

  Serial.println("Scanning for available networks... ");
  int n = WiFi.scanNetworks();
  for (int i = 0; i < n; ++i) {
    if (WiFi.SSID(i) == SSID) {
      Serial.print("Found ");
      Serial.print(SSID);
      Serial.println(" in available networks");      
      break;
    }
  }

  esp_wifi_sta_wpa2_ent_set_ttls_phase2_method(ESP_EAP_TTLS_PHASE2_PAP);
  WiFi.begin(SSID, WPA2_AUTH_PEAP, IDENTITY, USERNAME, PASSWORD);

  Serial.print("Connecting to ");
  Serial.print(SSID);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nConnected to WiFi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  server.begin();
}

void loop() {
  WiFiClient client = server.available(); // Listen for incoming clients

  if (client) { // If a new client connects
    Serial.println("New Client.");
    String currentLine = ""; // Make a String to hold incoming data from the client

    while (client.connected()) { // Loop while the client's connected
      if (client.available()) { // If there's bytes to read from the client,
        char c = client.read(); // Read a byte, then
        Serial.write(c); // Print it out the serial monitor
        header += c; // Add it to the string
        if (c == '\n') { // If the byte is a newline character
          if (currentLine.length() == 0) {
            // HTTP headers always end with a blank line
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println();
            client.println("<!DOCTYPE HTML>");
            client.println("<html>");
            client.println("<h1>Hello from ESP32!</h1>");
            client.println("<p>Client IP: " + client.remoteIP().toString() + "</p>");
            client.println("</html>");
            break;
          } else { // If you get a newline, then clear currentLine:
            currentLine = "";
          }
        } else if (c != '\r') { // If you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    header = ""; // Clear the header variable for next time
    client.stop(); // Close the connection
    Serial.println("Client disconnected.");
  }
}