//https://www.adafruit.com/product/3430


#include <Wire.h>

int speakerPin = 12;
int yellowButtonPin = 2;
int greenButtonPin = 3;
int blueButtonPin = 4;
int redButtonPin = 5;


int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
int level = 1;
int gameSequence[100];  //100 is the maximum level *TODO: remove*
                        //0 - NO COLOR
                        //1 YELLOW
                        //2 GREEN
                        //3 BLUE
                        //4 RED
int gameState = 0;  // 0 SEQ CREATING
                    // 1 SEQ PRESENTING
                    // 2 SEQ PRESENTING NEXT
                    // 3 PLAYER WAIT

int presentingIndex = 0;
int playerPlayingIndex = 0;

unsigned long playingTimer;

void setup() {
  Wire.begin();
  pinMode(yellowButtonPin, INPUT);
  pinMode(greenButtonPin, INPUT);
  pinMode(blueButtonPin, INPUT);
  pinMode(redButtonPin, INPUT);
  Serial.begin(9600);

  randomSeed(analogRead(0));

  delay(1000);
  Serial.println("ok");
}

void loop() {
  switch (gameState) {
    case 0:
      for (int n = 0; n < sizeof(gameSequence)/sizeof(int); n++) {
        if (n < level) {
          gameSequence[n] = random(0, 4) + 1;
        } else {
          gameSequence[n] = 0; //clear prev sequence
        }
      }
      gameState = 1;
      presentingIndex = 0;
      break;
    case 1:
      switch (gameSequence[presentingIndex]) {
        case 1:
            yellowLedOn();
          break;
        case 2:
            greenLedOn();
          break;
        case 3:
            blueLedOn();
          break;
        case 4:
            redLedOn();
          break;
      }
      gameState = 2;
      break;
    case 2:
      if (millis() - playingTimer >= 500) {
        stopLeds();
        presentingIndex ++;
        if (gameSequence[presentingIndex] > 0) {
          gameState = 1;
        } else {
          gameState = 3;
          playerPlayingIndex = 0;
        }
      }
      break;
      case 3:
        if (digitalRead(yellowButtonPin)) {
          yellowLedOn();
        } else if (digitalRead(greenButtonPin)) {
          greenLedOn();
        } else if (digitalRead(blueButtonPin)) {
          blueLedOn();
        } else if (digitalRead(redButtonPin)) {
          redLedOn();
        } else {
          stopLeds();
        }
      break;
  }
}

void yellowLedOn() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111110);
  Wire.endTransmission();
  tone(speakerPin, tones[1]);
  playingTimer = millis();
}

void greenLedOn() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111101);
  Wire.endTransmission();
  tone(speakerPin, tones[2]);
  playingTimer = millis();
}

void blueLedOn() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111011);
  Wire.endTransmission();
  tone(speakerPin, tones[3]);
  playingTimer = millis();
}

void redLedOn() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11110111);
  Wire.endTransmission();
  tone(speakerPin, tones[4]);
  playingTimer = millis();
}

void stopLeds() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();
  noTone(speakerPin);
}