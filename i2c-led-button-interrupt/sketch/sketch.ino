#include <Wire.h>

volatile int st = LOW;
bool buttonPressed = false;

void setup()
{
  Wire.begin();
  Serial.begin(9600);
  delay(1000);
  Serial.println("ok");

  pinMode(2, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), inputsReady, FALLING);

  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();  
}

void inputsReady() {
  st = HIGH;
}

void loop()
{
  if (st) {
    Wire.requestFrom(0x20, 1);
    if (Wire.available()) {
      byte b = Wire.read();
      Serial.println(b);
      if (b == 255) {
        buttonPressed = true;
      } else if (b == 253) {
        buttonPressed = false;
      } else {
        Serial.print(b);
        Serial.println(" unknow state");
      }
      st = LOW;
    }
  }


  if (buttonPressed) {
    Serial.println("on");
    Wire.beginTransmission(0x20);
    Wire.write((byte)0b00000000);
    Wire.endTransmission();

    delay(500);

    Serial.println("off");
    Wire.beginTransmission(0x20);
    Wire.write((byte)0b11111111);
    Wire.endTransmission();  
  }
  
}