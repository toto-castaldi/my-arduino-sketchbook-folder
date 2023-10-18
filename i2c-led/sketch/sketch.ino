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
  Serial.println("on");
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b00000000);
  Wire.endTransmission();

  delay(500);

  Serial.println("off");
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();

  delay(500);

}