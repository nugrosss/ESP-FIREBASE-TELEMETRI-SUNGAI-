#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WiFi.h>

class WiFiManager {
public:
  static void connectWiFi() {
    WiFi.begin("MyPublicWiFi", "12345678");
    Serial.print("Connecting to WiFi");
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print(".");
      delay(300);
    }
    Serial.println("\nConnected to WiFi!");
  }
};

#endif