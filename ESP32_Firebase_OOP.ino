#include "FirebaseHandler.h"
#include "WiFiManager.h"
#include "SensorReader.h"
#include "TimeManager.h"


FirebaseHandler firebaseHandler;
TimeManager timeManager;
SensorReader sensorReader;
void setup() {
  Serial.begin(115200);
  sensorReader.begin();
  WiFiManager::connectWiFi();
  firebaseHandler.setupFirebase();
  timeManager.setupTime();
}

void loop() {
  float avgCurrent = sensorReader.readAverageCurrent();
  int avgVoltage = sensorReader.readAverageVoltage();
  int avgTip = sensorReader.readAverageTip();
  int avgWtrLvl = sensorReader.readWaterLevel();
  // int offset = firebaseHandler.offsetz
  // float fixwater = offset - avgWtrLvl;
  

  
  firebaseHandler.getOffsetFromFirestore();  // opsional: panggil hanya saat perlu
  firebaseHandler.sendPatchToFirestore(avgCurrent, avgVoltage, avgTip,avgWtrLvl );
  firebaseHandler.sendCreateToFirestore(avgCurrent, avgVoltage, avgTip, avgWtrLvl);
  // float distance = sensorReader.readWaterLevel();
  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");
  Serial.println("=================================================");
  // Serial.print("wtrfixx: "); Serial.println(fixwater);
  Serial.print("Current: "); Serial.println(avgCurrent);
  Serial.print("Voltage: "); Serial.println(avgVoltage);
  Serial.print("Tip: "); Serial.println(avgTip);
  Serial.print("Water Level: "); Serial.println(avgWtrLvl);
  Serial.println("=================================================");
  
  delay(1000); // kirim data setiap 5 detik (atur sesuai kebutuhan)
}
