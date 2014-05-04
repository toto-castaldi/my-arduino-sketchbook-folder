int sensorValue;
int sensorLow = 1023;
int sensorHigh = 0;
const int ledPin = 13;
void setup() {
  Serial.begin(9600);
  pinMode(ledPin, OUTPUT); //calibration STARTS
  digitalWrite(ledPin, HIGH);
  while (millis() < 5000) {
    sensorValue = analogRead(A0);
    if (sensorValue > sensorHigh) {
      sensorHigh = sensorValue;
    }
    if (sensorValue < sensorLow) {
      sensorLow = sensorValue;
    }
  }
  digitalWrite(ledPin, LOW); //calibration ENDS
}
void loop() {
  sensorValue = analogRead(A0);
  int pitch = map(sensorValue, sensorLow, sensorHigh, 50, 4000);
  Serial.print("sensorLow : "); 
  Serial.print(sensorLow);
  Serial.print(", sensorHigh : "); 
  Serial.print(sensorHigh);
  Serial.print(", sensorValue : "); 
  Serial.print(sensorValue);
  Serial.print(" -> pitch : "); 
  Serial.println(pitch);
  tone(8, pitch, 20);
  delay(5);
}
