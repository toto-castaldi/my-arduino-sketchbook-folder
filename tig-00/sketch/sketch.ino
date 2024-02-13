#include <Wire.h>
#include "LiquidCrystal_I2C.h" // Library for LCD


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

int buttonPressStates;
int buttonReadyStates;

bool needWait;

unsigned long timerPlaying, timerPause, timerPlayerWaiting;

LiquidCrystal_I2C lcd = LiquidCrystal_I2C(0x27, 16, 2);

char displayText [16] = " Level          ";


void setup() {

  Wire.begin();

  for(int i = 0; i < sizeof(buttons)/sizeof(button); ++i) {
    pinMode(buttons[i].pin, INPUT);
  }

  Serial.begin(9600);

  randomSeed(analogRead(0));

  delay(1000);

  Serial.println("setup OK");

  lcd.init();
  lcd.backlight();

  changeGameState(LOBBY);
}

void loop() {
  switch (gameState) {
    case LOBBY:
      if (random(0,50000) == 0) {
        allOn();
        tone(PIN_SPEAKER, tones[random(0,sizeof(tones)/sizeof(int))]);
        delay(500);
        noTone(PIN_SPEAKER);
        stopLeds();
      } else {  
        if (playingPassed()) {
          rotateAnimation();
        }
      }
      readButtons();
      if (buttonPressStates) {
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
          readButtons();
          if (playingPassed() || buttonPressStates) {
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
                    playerWaitingStart();
                    ledOn(i,true);
                    playerPlayingIndex ++;
                    buttonPressedFound = true;
                  } else {
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
      break;
  }
}

int randomButton() {
  return (int) random(0, 4);
}

bool isButtonPressed(int button) {
  return bitRead(buttonPressStates, button) == 1;
}

void readButtons() {
  for (int i = 0; i < sizeof(buttons)/sizeof(button); i++) {
    if (digitalRead(buttons[i].pin)) {
      if (bitRead(buttonReadyStates, i)) {
        bitClear(buttonReadyStates, i);
        buttonPressStates = bitSet(buttonPressStates, i);
      } else {
        buttonPressStates = bitClear(buttonPressStates, i);
      }
    } else {
      bitSet(buttonReadyStates, i);
      buttonPressStates = bitClear(buttonPressStates, i);
    }
  }
}

void rotateAnimation() {
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

void changeGameState(gameStates newState) {
  Serial.print("from ");
  Serial.print(gameState);
  Serial.print(" to ");
  Serial.println(newState);
  gameState = newState;
  switch (gameState) {
    case LOBBY:
      level = 1;
      lcd.clear(); 
      lcd.setCursor(0, 0);
      lcd.print("     TIG 00     ");
      lcd.setCursor(0, 1);
      lcd.print(" Press a button ");
    break;
    case SEQUENCE_PRESENTING:
      presentingIndex = -1;
      needWait = false;
    break;
    case SEQUENCE_CREATE_UPDATE:
      sprintf(displayText + 10, "%02d", level);
      lcd.clear();
      lcd.setCursor(2, 0);
      lcd.print(displayText); 
    break;
    case PLAYER_WAITING:
      playerPlayingIndex = 0;
    break;
    case GAME_OVER:
      lcd.clear();
      lcd.setCursor(0, 0); 
      lcd.print("  Game OVER !!  ");
    break;
  }
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
      rotateAnimation();
    }
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    noTone(PIN_SPEAKER);
    if (melody[thisNote] > 0) {
      stopLeds();
    }
  }
}