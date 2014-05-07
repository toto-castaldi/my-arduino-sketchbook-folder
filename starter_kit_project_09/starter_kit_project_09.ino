const int switchPin = 2;
const int motorPin = 9;

void setup() {
  
  pinMode(motorPin, OUTPUT);
  pinMode(switchPin, INPUT);

}

void loop() {
  
  if (digitalRead(switchPin) == HIGH) {
  
    digitalWrite(motorPin, HIGH);
  
  } else {
  
    digitalWrite(motorPin, LOW);
  
  }
  
}
