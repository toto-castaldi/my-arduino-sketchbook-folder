#include <Wire.h>

//prototypes
void yON(bool sound = true);
void gON(bool sound = true);
void bON(bool sound = true);
void rON(bool sound = true);

//pins
int PIN_SPEAKER = 12;
int PIN_Y_BUTTON = 2;
int PIN_G_BUTTON = 3;
int PIN_B_BUTTON = 4;
int PIN_R_BUTTON = 5;

enum gameStates {
                  LOBBY, 
                  SEQUENCE_CREATE_UPDATE,
                  SEQUENCE_PRESENTING,
                  PLAYER_WAITING,
                  GAME_OVER
                };

enum buttons {
              BUTTON_YELLOW,
              BUTTON_GREEN,
              BUTTON_BLUE,
              BUTTON_RED,
              BUTTON_NO //null ????
            };

int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
int level;
buttons gameSequence[100];  //100 is the maximum level *TODO: remove*
gameStates gameState;

buttons animationButton;

int presentingIndex;
int playerPlayingIndex;

bool yB, gB, bB, rB;
bool yBR, gBR, bBR, rBR;

bool needWait;

unsigned long timerPlaying, timerPause, timerPlayerWaiting;

void setup() {
  Wire.begin();

  pinMode(PIN_Y_BUTTON, INPUT);
  pinMode(PIN_G_BUTTON, INPUT);
  pinMode(PIN_B_BUTTON, INPUT);
  pinMode(PIN_R_BUTTON, INPUT);

  Serial.begin(9600);

  randomSeed(analogRead(0));

  reset();

  delay(1000);

  Serial.println("setup OK");
}

void loop() {
  switch (gameState) {
    case LOBBY:
      if (playingPassed()) {
        rotateAnimationButton();
      }
      readPlayingButtons();
      if (yB || gB || bB || rB) {
        delay(1000);
        changeGameState(SEQUENCE_CREATE_UPDATE);
      }
    break;
    case SEQUENCE_CREATE_UPDATE:
      Serial.print("level ");
      Serial.print(level);
      Serial.print(" - ");
      Serial.println(500 - penalty(500) + 100);
      for (int n = level - 1; n < 100; n++) { //100 !!!
        if (n < level) {
          gameSequence[n] = randomButton();
        } else {
          gameSequence[n] = BUTTON_NO; //clear prev sequence
        }
      }
      changeGameState(SEQUENCE_PRESENTING);
      presentingIndex = -1;
      break;
    case SEQUENCE_PRESENTING:
      if (playingPassed() && !needWait) {
        if (pausePassed()) {
          presentingIndex ++;
          if (gameSequence[presentingIndex] != BUTTON_NO) {
            switch (gameSequence[presentingIndex]) {
              case BUTTON_YELLOW:
                  Serial.println("Y");
                  yON();
                  needWait = true;
                break;
              case BUTTON_GREEN:
                  Serial.println("G");
                  needWait = true;
                  gON();
                break;
              case BUTTON_BLUE:
                  Serial.println("B");
                  needWait = true;
                  bON();
                break;
              case BUTTON_RED:
                  Serial.println("R");
                  needWait = true;
                  rON();
                break;
            }
          } else {
            changeGameState(PLAYER_WAITING);
            playerWaitingStart();
            playerPlayingIndex = 0;    
          }
        }
      } else if (needWait && playingPassed()) {
        pauseStart();
        stopLeds();
        needWait = false;
      }
      break;
      case PLAYER_WAITING:
        if (playerWaitingTimeOut()) {
          Serial.println("Player TIMEOUT");
          playerTimeOutEffect();
          delay(1000);
          changeGameState(GAME_OVER);
        } else {
          readPlayingButtons();
          if (playingPassed() || yB || gB || bB || rB) {
            stopLeds();
            if (gameSequence[playerPlayingIndex] == BUTTON_NO) {
              Serial.println("New Level");
              delay(500);
              level ++;
              changeGameState(SEQUENCE_CREATE_UPDATE);
            } else {    
              if (yB) {
                if (gameSequence[playerPlayingIndex] == BUTTON_YELLOW) {
                  Serial.print(playerPlayingIndex);
                  Serial.println(" - Y OK");
                  playerWaitingStart();
                  yON();
                  playerPlayingIndex ++;
                } else {
                  Serial.println("Y KO");
                  changeGameState(GAME_OVER);
                }
              } else if (gB) {
                if (gameSequence[playerPlayingIndex] == BUTTON_GREEN) {
                  Serial.print(playerPlayingIndex);
                  Serial.println(" - G OK");
                  playerWaitingStart();
                  gON();
                  playerPlayingIndex ++;
                } else {
                  Serial.println("G KO");
                  changeGameState(GAME_OVER);
                }
              } else if (bB) {
                if (gameSequence[playerPlayingIndex] == BUTTON_BLUE) {
                  Serial.print(playerPlayingIndex);
                  Serial.println(" - B OK");
                  playerWaitingStart();
                  bON();
                  playerPlayingIndex ++;
                } else {
                  Serial.println("B KO");
                  changeGameState(GAME_OVER);
                }
              } else if (rB) {
                if (gameSequence[playerPlayingIndex] == BUTTON_RED) {
                  Serial.print(playerPlayingIndex);
                  Serial.println(" - R OK");
                  playerWaitingStart();
                  rON();
                  playerPlayingIndex ++;
                } else {
                  Serial.println("R KO");
                  changeGameState(GAME_OVER);
                }
              }
            }
          }
        }
      break;
      case GAME_OVER:
        gameOver();
        changeGameState(LOBBY);
        reset();
      break;
  }
}

buttons randomButton() {
  int r = random(0, 4) + 1;
  if (r == 1) return BUTTON_YELLOW;
  if (r == 2) return BUTTON_GREEN;
  if (r == 3) return BUTTON_BLUE;
  if (r == 4) return BUTTON_RED;
}

void readPlayingButtons() {
  if (digitalRead(PIN_Y_BUTTON)) {
    if (yBR) {
      yBR = false;
      yB = true;
    } else {
      yB = false;
    }
  } else { 
    yBR = true;
    yB = false;
  }
  if (digitalRead(PIN_G_BUTTON)) {
    if (gBR) {
      gBR = false;
      gB = true;
    } else {
      gB = false;
    }
  } else { 
    gBR = true;
    gB = false;
  }
  if (digitalRead(PIN_B_BUTTON)) {
    if (bBR) {
      bBR = false;
      bB = true;
    } else {
      bB = false;
    }
  } else { 
    bBR = true;
    bB = false;
  }
  if (digitalRead(PIN_R_BUTTON)) {
    if (rBR) {
      rBR = false;
      rB = true;
    } else {
      rB = false;
    }
  } else { 
    rBR = true;
    rB = false;
  }
}

void rotateAnimationButton() {
  if (animationButton == BUTTON_YELLOW) animationButton = BUTTON_GREEN;
  else if (animationButton == BUTTON_GREEN) animationButton = BUTTON_RED;
  else if (animationButton == BUTTON_RED) animationButton = BUTTON_BLUE;
  else if (animationButton == BUTTON_BLUE) animationButton = BUTTON_YELLOW;
  switch (animationButton) {
    case BUTTON_YELLOW:
        yON(false);
      break;
    case BUTTON_GREEN:
        gON(false);
      break;
    case BUTTON_BLUE:
        bON(false);
      break;
    case BUTTON_RED:
        rON(false);
      break;
  }
}

int penalty (int base) {
  double difficulty = (double)-1/((double)level * (double)level) + 0.5;
  int penalty = 0;
  if (difficulty > 0) {
    penalty += (int) floor(difficulty * base);
  }
  return penalty;
}

bool playerWaitingTimeOut() {
  return millis() - timerPlayerWaiting >= 5000;
}

bool playingPassed() {
  return millis() - timerPlaying >= 500 - penalty(400);
}

bool pausePassed() {
  return millis() - timerPause >= 300 - penalty(200);
}

void pauseStart() {
  timerPause = millis();
}

void playingStart() {
  timerPlaying = millis();
}

void playerWaitingStart() {
  timerPlayerWaiting = millis();
}

void reset() {
  needWait = false;
  presentingIndex = -1;
  playerPlayingIndex = 0;
  level = 1;
  gameState = LOBBY;
  yB, gB, bB, rB = false;
  yBR, gBR, bBR, rBR = true;
  animationButton = BUTTON_YELLOW;
}


void changeGameState(gameStates newState) {
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

void playerTimeOutEffect() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11110000);
  Wire.endTransmission();
  tone(PIN_SPEAKER, tones[4]);
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
    if (melody[thisNote] > 0) {
      rotateAnimationButton();
    }
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN_SPEAKER);
    if (melody[thisNote] > 0) {
      stopLeds();
    }
  }
}