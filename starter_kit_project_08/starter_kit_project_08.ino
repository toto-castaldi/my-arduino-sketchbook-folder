const int switchPin = 8;
unsigned long previousTime = 0;
int led = 2;
long interval = 3000; //3 secondi

void setup() {
  for (int x = 2; x < 8; x++) {
    pinMode(x, OUTPUT);
  }
  pinMode(switchPin, INPUT);
}

void loop() {
  unsigned long currentTime = millis();
  
  //solo se tengo premuto pulsante
  if (digitalRead(switchPin) == HIGH) {
    if (currentTime - previousTime > interval) {
      previousTime = currentTime;
      digitalWrite(led, HIGH);
      led ++;
      if (led == 8) {
        //clessidra FINITA. Lampeggio
        for (int y = 1; y < 4; y++) {
          for (int x = 2; x < 8; x++) {
            digitalWrite(x, HIGH);
          }
          delay(1000);
          for (int x = 2; x < 8; x++) {
            digitalWrite(x, LOW);
          }
          delay(1000);
        }
        led = 2;
        previousTime = currentTime;
      }
    }
  } else {
    //spengo tutto
    for (int x = 2; x < 8; x++) {
      digitalWrite(x, LOW);
    }
    led = 2;
    previousTime = currentTime;
  }
}
