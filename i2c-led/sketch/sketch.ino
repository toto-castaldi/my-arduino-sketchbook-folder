#include <Wire.h>

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("ok");
}

void loop()
{
  //blue ON
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111110);
  Wire.endTransmission();

  delay(500);

  //giallo ON
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111100);
  Wire.endTransmission();

  delay(500);

  //verde ON
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111000);
  Wire.endTransmission();

  delay(500);

  //rosso ON
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11110000);
  Wire.endTransmission();

  delay(500);

  Serial.println("rosso OFF");
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();

  delay(500);

}