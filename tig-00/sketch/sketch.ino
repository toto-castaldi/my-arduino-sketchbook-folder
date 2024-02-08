void yON(bool sound = true);
void gON(bool sound = true);
void bON(bool sound = true);
void rON(bool sound = true);
#include <Wire.h>

int PIN_SPEAKER = 12;
int yellowButtonPin = 2;
int greenButtonPin = 3;
int blueButtonPin = 4;
int redButtonPin = 5;


int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
int level;
int gameSequence[100];  //100 is the maximum level *TODO: remove*
                        //0 - NO COLOR
                        //1 YELLOW
                        //2 GREEN
                        //3 BLUE
                        //4 RED
int gameState;      // 0 SEQ CREATING
                    // 1 SEQ PRESENTING
                    // 3 PLAYER WAIT

int presentingIndex;
int playerPlayingIndex;

bool yB, gB, bB, rB;
bool yBR, gBR, bBR, rBR;

unsigned long timerPlaying;
unsigned long timerPause;

void setup() {
  Wire.begin();
  pinMode(yellowButtonPin, INPUT);
  pinMode(greenButtonPin, INPUT);
  pinMode(blueButtonPin, INPUT);
  pinMode(redButtonPin, INPUT);
  Serial.begin(9600);

  randomSeed(analogRead(0));

  reset();

  delay(1000);

  Serial.println("setup OK");

}

void loop() {
  if (digitalRead(yellowButtonPin)) {
    if (yBR) {
      yBR = false;
      yB = true;
      Serial.println("Y pressed");  
    } else {
      yB = false;
    }
  } else { 
    yBR = true;
    yB = false;
  }
  if (digitalRead(greenButtonPin)) {
    if (gBR) {
      gBR = false;
      gB = true;
      Serial.println("G pressed");
    } else {
      gB = false;
    }
  } else { 
    gBR = true;
    gB = false;
  }
  if (digitalRead(blueButtonPin)) {
    if (bBR) {
      bBR = false;
      bB = true;
      Serial.println("B pressed");
    } else {
      bB = false;
    }
  } else { 
    bBR = true;
    bB = false;
  }
  if (digitalRead(redButtonPin)) {
    if (rBR) {
      rBR = false;
      rB = true;
      Serial.println("R pressed");
    } else {
      rB = false;
    }
  } else { 
    rBR = true;
    rB = false;
  }

  switch (gameState) {
    case 0:
      Serial.print("level ");
      Serial.println(level);
      for (int n = level - 1; n < sizeof(gameSequence)/sizeof(int); n++) {
        if (n < level) {
          gameSequence[n] = random(0, 4) + 1;
        } else {
          gameSequence[n] = 0; //clear prev sequence
        }
      }
      changeGameState(1);
      presentingIndex = 0;
      break;
    case 1:
      if (gameSequence[presentingIndex] > 0) {
        if (playingPassed() && pausePassed()) {
          switch (gameSequence[presentingIndex]) {
            case 1:
                Serial.println("Y");
                yON();
                presentingIndex ++;
              break;
            case 2:
                Serial.println("G");
                presentingIndex ++;
                gON();
              break;
            case 3:
                Serial.println("B");
                presentingIndex ++;
                bON();
              break;
            case 4:
                Serial.println("R");
                presentingIndex ++;
                rON();
              break;
          }
          if (playingPassed()) {
            pauseStart();
          }
        }  
      } else {
        changeGameState(3);
        playerPlayingIndex = 0;
      }
      break;
      case 3:
        if (playingPassed() || yB || gB || bB || rB) {
          stopLeds();
          if (gameSequence[playerPlayingIndex] == 0) {
            Serial.println("New Level");
            level ++;
            changeGameState(0);
          } else {
            
            if (yB) {
              if (gameSequence[playerPlayingIndex] == 1) {
                Serial.print(playerPlayingIndex);
                Serial.println(" - Y OK");
                yON();
                playerPlayingIndex ++;
              } else {
                Serial.println("Y KO");
                changeGameState(4);
              }
            } else if (gB) {
              if (gameSequence[playerPlayingIndex] == 2) {
                Serial.print(playerPlayingIndex);
                Serial.println(" - G OK");
                gON();
                playerPlayingIndex ++;
              } else {
                Serial.println("G KO");
                changeGameState(4);
              }
            } else if (bB) {
              if (gameSequence[playerPlayingIndex] == 3) {
                Serial.print(playerPlayingIndex);
                Serial.println(" - B OK");
                bON();
                playerPlayingIndex ++;
              } else {
                Serial.println("B KO");
                changeGameState(4);
              }
            } else if (rB) {
              if (gameSequence[playerPlayingIndex] == 4) {
                Serial.print(playerPlayingIndex);
                Serial.println(" - R OK");
                rON();
                playerPlayingIndex ++;
              } else {
                Serial.println("R KO");
                changeGameState(4);
              }
            }
          }
        }
      break;
      case 4:
        gameOver();
        changeGameState(0);
        reset();
      break;
  }
}

bool playingPassed() {
  bool res = millis() - timerPlaying >= 500;
  //if (res) Serial.println("playing PASSED");
  return res;
}

bool pausePassed() {
  bool res = millis() - timerPause >= 400;
  //if (res) Serial.println("pause PASSED");
  return res;
}

void pauseStart() {
  //Serial.println("pause START");
  timerPause = millis();
}

void playingStart() {
  //Serial.println("playing START");
  timerPlaying = millis();
}

void reset() {
  presentingIndex = 0;
  playerPlayingIndex = 0;
  level = 1;
  /*
  for (int i = 0; i < level; i++) {
    gameSequence[i] = 1;
  }
  */

  yB, gB, bB, rB = false;
  yBR, gBR, bBR, rBR = true;
  pauseStart();
  playingStart();
}


void changeGameState(int newState) {
  Serial.print("from ");
  Serial.print(gameState);
  Serial.print(" to ");
  Serial.println(newState);
  gameState = newState;
}

void ledOn(byte led, int soundIndex){
  Wire.beginTransmission(0x20);
  Wire.write(led);
  Wire.endTransmission();
  if (soundIndex > -1) {
    tone(PIN_SPEAKER, tones[soundIndex]);
  }
  playingStart();
}

void yON(bool sound) {
  if (sound) ledOn((byte)0b11111110,1); else ledOn((byte)0b11111110,-1);
}

void gON(bool sound) {
  if (sound) ledOn((byte)0b11111101,2); else ledOn((byte)0b11111101,-1);
}

void bON(bool sound) {
  if (sound) ledOn((byte)0b11111011,3); else ledOn((byte)0b11111011,-1);
}

void rON(bool sound) {
  if (sound) ledOn((byte)0b11110111,4); else ledOn((byte)0b11110111,-1);
}

void stopLeds() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();
  noTone(PIN_SPEAKER);
}

void gameOver() {
  int melody[] = {
  262, 196,196, 220, 196,0, 247, 262};

  // note durations: 4 = quarter note, 8 = eighth note, etc.:
  int noteDurations[] = {4, 8, 8, 4,4,4,4,4 };

 for (int thisNote = 0; thisNote < 8; thisNote++) {
    int noteDuration = 1000/noteDurations[thisNote];
    tone(PIN_SPEAKER, melody[thisNote],noteDuration);
    if (thisNote % 4 == 0) yON(false); 
    if (thisNote % 4 == 1) gON(false); 
    if (thisNote % 4 == 2) bON(false); 
    if (thisNote % 4 == 3) rON(false);
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN_SPEAKER);
    stopLeds();
  }
}