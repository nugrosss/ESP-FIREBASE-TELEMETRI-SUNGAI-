#include "FirebaseHandler.h"
#include "WiFiManager.h"
#include "SensorReader.h"
#include "TimeManager.h"

int lastTipDay = -1;
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
 // Simpan tanggal terakhir reset dalam format "YYYY-MM-DD"

void loop() {
  
  float avgCurrent = sensorReader.readAverageCurrent();
  int avgVoltage = sensorReader.readAverageVoltage();
  int avgTip = sensorReader.readAverageTip();
  int avgWtrLvl = sensorReader.readWaterLevel();
  int tips = sensorReader.getTipCount();  
  // jumlah tipping total
  float mmPerTip = 0.70;
  float totalRainfall = tips * mmPerTip;  // konversi ke mm
  int roundedRainfall = int (totalRainfall) ;
  // int offset = firebaseHandler.offsetz
  // float fixwater = offset - avgWtrLvl;
  

  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Gagal mendapatkan waktu NTP");
    delay(1000);
    return;
  }

  // Reset tip harian jika hari berubah
  if (lastTipDay != timeinfo.tm_mday) {
    lastTipDay = timeinfo.tm_mday;
    sensorReader.resetTipCount();  // ✅ objek, bukan pointer
    Serial.println("Tip count di-reset karena hari berganti.");
  }
  firebaseHandler.getOffsetFromFirestore();  // opsional: panggil hanya saat perlu
  firebaseHandler.sendPatchToFirestore(avgCurrent, avgVoltage, avgTip,avgWtrLvl,totalRainfall);
  firebaseHandler.sendCreateToFirestore(avgCurrent, avgVoltage, avgTip, avgWtrLvl,totalRainfall);
  
  // float distance = sensorReader.readWaterLevel();
  // Serial.print("Distance: ");
  // Serial.print(distance);
  // Serial.println(" cm");
  Serial.println("=================================================");
  // Serial.print("wtrfixx: "); Serial.println(fixwater);
  Serial.print("Tanggal: ");
  Serial.printf("%04d-%02d-%02d %02d:%02d:%02d\n",
                timeinfo.tm_year + 1900, timeinfo.tm_mon + 1, timeinfo.tm_mday,
                timeinfo.tm_hour, timeinfo.tm_min, timeinfo.tm_sec);
  Serial.print("Current: "); Serial.println(avgCurrent);
  Serial.print("Voltage: "); Serial.println(avgVoltage);
  Serial.print("Tip: "); Serial.println(tips);
  Serial.print("Water Level: "); Serial.println(avgWtrLvl);
  Serial.println("=================================================");
  
  delay(1000); // kirim data setiap 5 detik (atur sesuai kebutuhan)
}
