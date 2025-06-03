#ifndef SENSOR_READER_H
#define SENSOR_READER_H

#include <Arduino.h>

#define VOLTAGE_SENSOR_PIN 32
#define CURRENT_SENSOR_PIN 33
#define TRIG_PIN 5
#define ECHO_PIN 18
#define TIP_PIN 14

class SensorReader {
public:
  SensorReader();

  void begin();
  float readAverageCurrent();
  float readAverageVoltage();
  float readAverageTip();
  float readWaterLevel();
  void handleTip();  // untuk dipanggil dari ISR

  static void IRAM_ATTR tipISR();  // static ISR

private:
  static volatile int tipCount;
  static unsigned long lastInterruptTime;
};
extern SensorReader sensorReader;

  // deklarasi instance global

#endif
