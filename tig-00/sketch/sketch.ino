#include <Wire.h>

int PIN_SPEAKER = 12;

typedef struct { 
  int pin;
  int tone;
  byte ledSignal;
} button;

enum gameStates {
                  LOBBY, 
                  SEQUENCE_CREATE_UPDATE,
                  SEQUENCE_PRESENTING,
                  PLAYER_WAITING,
                  GAME_OVER
                };


const button buttons[] {
    {2, 261, (byte)0b11111110}, //Y
    {3, 277, (byte)0b11111101}, //G
    {4, 294, (byte)0b11111011}, //B
    {5, 311, (byte)0b11110111}  //RED
};

int tones[] = {261, 277, 294, 311, 330, 349, 370, 392, 415, 440};
//            mid C  C#   D    D#   E    F    F#   G    G#   A
int level;
int gameSequence[100];  //100 is the maximum level *TODO: remove*
gameStates gameState;

int animationButton;

int presentingIndex;
int playerPlayingIndex;

int buttonsState;
bool yBR, gBR, bBR, rBR;

bool needWait;

unsigned long timerPlaying, timerPause, timerPlayerWaiting;

void setup() {

  Wire.begin();

  for(int i = 0; i < sizeof(buttons)/sizeof(button); ++i) {
    pinMode(buttons[i].pin, INPUT);
  }

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
      if (buttonsState) {
        allOn();
        delay(1500);
        stopLeds();
        delay(500);
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
          gameSequence[n] = -1; //clear prev sequence
        }
      }
      changeGameState(SEQUENCE_PRESENTING);
      presentingIndex = -1;
      break;
    case SEQUENCE_PRESENTING:
      if (playingPassed() && !needWait) {
        if (pausePassed()) {
          presentingIndex ++;
          int currentButton = gameSequence[presentingIndex];
          if (currentButton != -1) {
            ledOn(currentButton, true);
            needWait = true;
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
          if (playingPassed() || buttonsState) {
            stopLeds();
            if (gameSequence[playerPlayingIndex] == -1) {
              Serial.println("New Level");
              delay(500);
              level ++;
              changeGameState(SEQUENCE_CREATE_UPDATE);
            } else {
              bool buttonPressedFound = false;
              int len = sizeof(buttons)/sizeof(button);
              int i = 0;
              while (!buttonPressedFound && i < len) {
                if (isButtonPressed(i)) {
                  if (gameSequence[playerPlayingIndex] == i) {
                    Serial.print(playerPlayingIndex);
                    Serial.print(" - ");
                    Serial.print(i);
                    Serial.println(" - OK");
                    playerWaitingStart();
                    ledOn(i,true);
                    playerPlayingIndex ++;
                    buttonPressedFound = true;
                  } else {
                    Serial.print(playerPlayingIndex);
                    Serial.print(" - ");
                    Serial.print(i);
                    Serial.println(" - KO");
                    changeGameState(GAME_OVER);
                  }
                }  
                i ++;
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

int randomButton() {
  return (int) random(0, 4);
}

bool isButtonPressed(int button) {
  return bitRead(buttonsState, button) == 1;
}

void readPlayingButtons() {
  if (digitalRead(buttons[0].pin)) {
    if (yBR) {
      yBR = false;
      buttonsState = bitSet(buttonsState, 0);
    } else {
      buttonsState = bitClear(buttonsState, 0);
    }
  } else { 
    yBR = true;
    buttonsState = bitClear(buttonsState, 0);
  }
  if (digitalRead(buttons[1].pin)) {
    if (gBR) {
      gBR = false;
      buttonsState = bitSet(buttonsState, 1);
    } else {
      buttonsState = bitClear(buttonsState, 1);
    }
  } else { 
    gBR = true;
    buttonsState = bitClear(buttonsState, 1);
  }
  if (digitalRead(buttons[2].pin)) {
    if (bBR) {
      bBR = false;
      buttonsState = bitSet(buttonsState, 2);
    } else {
      buttonsState = bitClear(buttonsState, 2);
    }
  } else { 
    bBR = true;
    buttonsState = bitClear(buttonsState, 2);
  }
  if (digitalRead(buttons[3].pin)) {
    if (rBR) {
      rBR = false;
      buttonsState = bitSet(buttonsState, 3);
    } else {
      buttonsState = bitClear(buttonsState, 3);
    }
  } else { 
    rBR = true;
    buttonsState = bitClear(buttonsState, 3);
  }
}

void rotateAnimationButton() {
  int transTable[4] = {0,1,3,2};
  if (++animationButton > 3) animationButton = 0;
  ledOn(transTable[animationButton], false);
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
  yBR, gBR, bBR, rBR = true;
  animationButton = 0;
}


void changeGameState(gameStates newState) {
  Serial.print("from ");
  Serial.print(gameState);
  Serial.print(" to ");
  Serial.println(newState);
  gameState = newState;
}

void ledOn(int ledIndex, bool sound){
  button b = buttons[ledIndex];
  Wire.beginTransmission(0x20);
  Wire.write(b.ledSignal);
  Wire.endTransmission();
  if (sound) {
    tone(PIN_SPEAKER, b.tone);
  }
  playingStart();
}

void allOn() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11110000);
  Wire.endTransmission();
}

void playerTimeOutEffect() {
  allOn();
  tone(PIN_SPEAKER, tones[4]);
}

void startMatchEffect() {
  allOn();
}

void stopLeds() {
  Wire.beginTransmission(0x20);
  Wire.write((byte)0b11111111);
  Wire.endTransmission();
  noTone(PIN_SPEAKER);
}

void gameOver() {
  int melody[] = { 262, 196, 196, 220, 196,0, 247, 262};

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