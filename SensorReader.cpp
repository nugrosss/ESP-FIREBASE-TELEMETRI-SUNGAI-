#include "SensorReader.h"

volatile int SensorReader::tipCount = 0;
unsigned long SensorReader::lastInterruptTime = 0;

 // definisi instance global

SensorReader::SensorReader() {
  pinMode(TIP_PIN, INPUT_PULLUP);
  pinMode(TRIG_PIN, OUTPUT);    // FIXED
  pinMode(ECHO_PIN, INPUT);  }

void SensorReader::begin() {
     // FIXED
  attachInterrupt(digitalPinToInterrupt(TIP_PIN), tipISR, FALLING);
}


float SensorReader::readAverageCurrent() {
  const int numSamples = 10;
  float totalCurrent = 0;
  for (int i = 0; i < numSamples; i++) {
    int adc = analogRead(CURRENT_SENSOR_PIN);
    float voltage = adc * 3.299 / 4095.0;
    float current = (voltage - 1.444) / 0.185;
    totalCurrent += current;
    delay(10);
  }
  return totalCurrent / numSamples;
}

float SensorReader::readAverageVoltage() {
  int analogValue = analogRead(VOLTAGE_SENSOR_PIN);
  float voltage = (analogValue * 3.3) / 4095;
  return (voltage * 5.0) + 0.7;
}

float SensorReader::readAverageTip() {
  noInterrupts();
  int tips = tipCount;
  // tipCount = 0;
  interrupts();

  const float mmPerTip = 0.70;
  return tips * mmPerTip;
}

void SensorReader::handleTip() {
  unsigned long currentTime = millis();
  if (currentTime - lastInterruptTime > 200) {
    tipCount++;
    lastInterruptTime = currentTime;
  }
}
void SensorReader::resetTipCount() {
  noInterrupts();
  tipCount = 0;
  interrupts();
}
int SensorReader::getTipCount() {
  noInterrupts();
  int tips = tipCount;
  interrupts();
  return tips;
}
float SensorReader::readWaterLevel() {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);

  long duration = pulseIn(ECHO_PIN, HIGH);
  return duration * 0.0343 / 2;
}

void IRAM_ATTR SensorReader::tipISR() {
  sensorReader.handleTip();  // panggil handler dari instance global
}
