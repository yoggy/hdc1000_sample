//
// HDC1000 sample for Arduino
//
#include <Wire.h>

void setup() {
  Serial.begin(9600);
  Wire.begin();
  hdc1000_setup();
}

void loop() {
  float t, h;
  hdc1000_get(t, h);
  Serial.print("t=");
  Serial.print(t);
  Serial.print(", h=");
  Serial.print(h);
  Serial.println();
  delay(1000);
}

///////////////////////////////////////////////////////
//
// functions for HDC1000
//
#define HDC1000_ADDR 0x40

void hdc1000_setup() {
  delay(100);
  Wire.beginTransmission(HDC1000_ADDR);
  Wire.write(0x20);
  Wire.write(0x10);
  Wire.write(0x00);
  Wire.endTransmission();
  delay(100);
}

void hdc1000_get(float &temperature, float &humidity) {
  unsigned int t = 0, h = 0;

  Wire.beginTransmission(HDC1000_ADDR);
  Wire.write(0x00); // temperature
  Wire.endTransmission();

  delay(100);
  
  Wire.requestFrom(HDC1000_ADDR, 4);
  while (Wire.available() < 4) {}

  t = Wire.read() << 8;
  t |= Wire.read();

  h = Wire.read() << 8;
  h |= Wire.read();

  temperature = t / 65536.0 * 165.0 - 40.0;
  humidity = h / 65536.0 * 100.0;
}

