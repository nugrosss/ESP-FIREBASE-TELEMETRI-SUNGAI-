#ifndef FIREBASE_HANDLER_H
#define FIREBASE_HANDLER_H
#include <Firebase_ESP_Client.h>
#include "addons/TokenHelper.h"
#include "addons/RTDBHelper.h"
#include <Arduino.h>
// #include <FirebaseJson.h>
#include "SensorReader.h"


class FirebaseHandler {
private:
  FirebaseData fbdo;
  FirebaseAuth auth;
  FirebaseConfig config;

  unsigned long lastPatchTime = 0;
  unsigned long lastCreateTime = 0;
  unsigned long lastOffsetTime = 0;
  unsigned long lastPatchTimeFull = 0;
  unsigned long lastPatchTimeTip = 0;
  unsigned long lastTipDay = 0;

  // // int lastTipDay = -1;
  // SensorReader* sensorReader = nullptr;



public:
  float offsetz = 0;
  float adjustedWaterLevel = 0 ;
  float avgCurrent =0;
  

  void setupFirebase() {
    config.api_key = "AIzaSyBaczNXGR500dEnhOM-vF_X6Nz9x7uYobk";
    auth.user.email = "aprilifiros04@gmail.com";
    auth.user.password = "1234567890";
    Firebase.reconnectWiFi(true);
    config.token_status_callback = tokenStatusCallback;
    Firebase.begin(&config, &auth);
    Serial.println("Waiting for Firebase...");
    unsigned long startMillis = millis();
    while (!Firebase.ready()) {
      Serial.print(".");
      delay(500);
      if (millis() - startMillis > 10000) break;
    }
    Serial.println("\nFirebase Ready!");
  }

  void getOffsetFromFirestore() {
  // Mengecek apakah Firebase siap dan jeda waktu sudah terpenuhi
  if (Firebase.ready() && millis() - lastOffsetTime >= 5000) {
    lastOffsetTime = millis();  // Update waktu terakhir pengambilan
    String path = "COBALAGI/z_offset";

    // Ambil dokumen dari Firestore
    if (Firebase.Firestore.getDocument(&fbdo, "coba-login-30bec", "", path.c_str(), "")) {
      FirebaseJson json;
      FirebaseJsonData result;

      // Parse payload JSON dari Firebase
      json.setJsonData(fbdo.payload());

      // Ambil nilai offset dari field "offset" bertipe integer
      if (json.get(result, "fields/offset/integerValue")) {
        offsetz = result.intValue;
        Serial.print("Offset diambil dari Firebase: ");
        Serial.println(offsetz);
      } else {
        Serial.println("Field offset tidak ditemukan atau format salah.");
      }
    } else {
      // Jika gagal mengambil dokumen
      Serial.print("Gagal mengambil offset: ");
      Serial.println(fbdo.errorReason());
    }
  }
}

void sendPatchToFirestore(float avgCurrent, int avgVoltage , int avgTip, int avgWtrLvl,float totalRainfall ) {
  int adjustedWaterLevel = avgWtrLvl - offsetz;

  // PATCH lengkap setiap 1 detik
  if (Firebase.ready() && millis() - lastPatchTimeFull >= 1000) {
    lastPatchTimeFull = millis();

    FirebaseJson json;
    json.set("fields/arus/doubleValue", String(avgCurrent,2));
    // json.set("fields/tip/integerValue", avgTip);
    json.set("fields/water/integerValue", adjustedWaterLevel);
    json.set("fields/tegangan/integerValue", avgVoltage);
    json.set("fields/tip/doubleValue", String(totalRainfall,2));

    if (Firebase.Firestore.patchDocument(&fbdo, "coba-login-30bec", "", "Cobaesp/data", json.raw(), "")) {
      Serial.println("PATCH (FULL) success");
    } else {
      Serial.println(fbdo.errorReason());
    }
  }
}





void sendCreateToFirestore(int avgCurrent, int avgVolatge , int avgTip, int avgWtrLvl,float totalRainfall) {
    if (Firebase.ready() && millis() - lastCreateTime >= 3600000) {
      lastCreateTime = millis();
      int adjustedWaterLevel = offsetz -avgWtrLvl ;
      time_t now;
      struct tm timeinfo;
      time(&now);
      localtime_r(&now, &timeinfo);

      char day[11], timeStr[9], docID[16];
      sprintf(day, "%04d-%02d-%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday);
      sprintf(timeStr, "%02d:%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
      sprintf(docID, "%04d%02d%02d%02d%02d%02d", timeinfo.tm_year + 1900, timeinfo.tm_mon + 1,
              timeinfo.tm_mday, timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);

      FirebaseJson json;
      json.set("fields/arus/doubleValue", avgCurrent);
      json.set("fields/tip/integerValue", totalRainfall);
      json.set("fields/water/integerValue", adjustedWaterLevel);
      json.set("fields/tegangan/doubleValue",avgVolatge);
      json.set("fields/day/stringValue", day);
      json.set("fields/time/stringValue", timeStr);

      if (Firebase.Firestore.createDocument(&fbdo, "coba-login-30bec", "", "COBALAGI/pushData/list", docID, json.raw(), "")) {
        Serial.printf("CREATE success: %s %s\n", day, timeStr);
      } else {
        Serial.println(fbdo.errorReason());
      }
    }
  }
};

#endif