//saymon says http://it.wikipedia.org/wiki/Simon_(gioco)
#include "pitches.h"
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_MOSI   9
#define OLED_CLK   10
#define OLED_DC    11
#define OLED_CS    12
#define OLED_RESET 13
Adafruit_SSD1306 display(OLED_MOSI, OLED_CLK, OLED_DC, OLED_RESET, OLED_CS);


const int redLed = 7;
const int greenLed = 6;
const int yellowLed = 5;
const int blueLed = 4;

const int tonePin = 8;

const int redButton = A0;
const int greenButton = A1;
const int yellowButton = A2;
const int blueButton = A3;
const int maxLength = 100; //max level
const int statePrepare = 0;
const int statePlay = 1;
const int stateGameOver = 2;
const int stateNewLevel = 3;
const int stateEnd = 4;

const int buttonUnknow = 0;
const int buttonDown = 1;
const int buttonPressed = 3;


int level;
int state; 
int sequence[maxLength];
int currentPlayingSeqIndex;
int buttonStates[4]; 
unsigned long buttonFeedBackStartMillis;

void setup() {
  Serial.begin(9600);

  display.begin(SSD1306_SWITCHCAPVCC);
  
  pinMode(redLed, OUTPUT);
  pinMode(greenLed, OUTPUT);
  pinMode(yellowLed, OUTPUT);
  pinMode(blueLed, OUTPUT);
    
  level = 0;
  state = stateNewLevel;
  
  randomSeed(analogRead(A5));
  
  //initial set
  for (int i = 0 ; i < level; i++) {
    sequence[i] = redButton;
  }
  
  for (int i = 0; i < 4; i++ ) {
    buttonStates[i] = buttonUnknow;
  }
  
  buttonFeedBackStartMillis = 0;
}

void loop() {
 
  if (state == stateNewLevel) {
    level ++;
    displayStringInt("livello ", level);
    delay(2000);
    state = statePrepare;
  }
  
  if (state == statePrepare) {
    //set
    for (int i = level - 1 ; i < level; i++) {
      int button = random(4);
      if (button == 0) sequence[i] = redButton;
      if (button == 1) sequence[i] = greenButton;
      if (button == 2) sequence[i] = yellowButton;
      if (button == 3) sequence[i] = blueButton;
    }
    
    //play
    displayString("memorizza");
    for (int i = 0; i < level; i++) {
      int button = sequence[i];
      if (button == redButton) { tone(tonePin, NOTE_C4); digitalWrite(redLed, HIGH); } 
      if (button == greenButton) { tone(tonePin, NOTE_D4); digitalWrite(greenLed, HIGH); } 
      if (button == yellowButton) { tone(tonePin, NOTE_E4); digitalWrite(yellowLed, HIGH); } 
      if (button == blueButton) { tone(tonePin, NOTE_F4); digitalWrite(blueLed, HIGH); } 
      
      int maxMapLevel = 16;
      int mapLevel = level;
      if (mapLevel > maxMapLevel) {
        mapLevel = maxMapLevel;
      }
      int delayMemorization = 500 + map(mapLevel, 1, maxMapLevel, 800, 0);
         
      delay(delayMemorization);
      noTone(tonePin);
      allLedOff();
      delay(50); //to see two or more equal color in sequence
      
    }
    state = statePlay;
    displayString("ripeti");
    currentPlayingSeqIndex = 0;
    
  }
  
   if (state == statePlay) {
   //if (state == 100) {
     
      if (buttonStates[0] == buttonPressed) {
        checkPressing(redButton);
        buttonStates[0] = buttonUnknow;
      } else if (buttonStates[1] == buttonPressed) {
        checkPressing(greenButton);
        buttonStates[1] = buttonUnknow;
      } else if (buttonStates[2] == buttonPressed) {
        checkPressing(yellowButton);
        buttonStates[2] = buttonUnknow;
      } else if (buttonStates[3] == buttonPressed) {
        checkPressing(blueButton);
        buttonStates[3] = buttonUnknow;
      } 
  }
  
  if (state == stateGameOver) {
    displayString("sbagliato");
    delay(1000);
    displayStringInt("punti ", level);
    state = stateEnd;
  }
  
       //manage pressing
      computeButtonState(0, redButton, redLed, NOTE_C4);  
      computeButtonState(1, greenButton, greenLed, NOTE_D4);  
      computeButtonState(2, yellowButton, yellowLed, NOTE_E4);  
      computeButtonState(3, blueButton, blueLed, NOTE_F4);
    
      pressingFeedback(false);
      
}

void pressingFeedback(boolean waitForButtonEnd) {
    if (waitForButtonEnd && buttonFeedBackStartMillis > 0 && millis() - buttonFeedBackStartMillis < 1000) {
      delay(1000 - (millis() - buttonFeedBackStartMillis));
      noTone(tonePin);
        buttonFeedBackStartMillis = 0;
        //allLedOff();
    } else {
      if (buttonFeedBackStartMillis > 0 && millis() > 1000 + buttonFeedBackStartMillis) {
        noTone(tonePin);
        buttonFeedBackStartMillis = 0;
        //allLedOff();
      }
    }
}

void displayStringInt(char * str, int i) {
    display.setTextSize(2);
    display.setTextColor(WHITE);
    display.setCursor(0,0);
    display.clearDisplay();
    display.print(str);
    display.print(i);
    display.display();
}

void displayString(char * str) {
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.clearDisplay();
  display.println(str);
  display.display();
}

void allLedOff() {
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
}


void computeButtonState(int index, int button, int led, int note) {
      int tmpVal = analogRead(button);
      delay(20);
      if (tmpVal > 900) {
        allLedOff();
        buttonStates[index] = buttonDown;
        digitalWrite(led, HIGH);
        tone(tonePin, note);
        buttonFeedBackStartMillis = millis();
      } else {
        if (buttonStates[index] == buttonDown) {
          buttonStates[index] = buttonPressed;
          allLedOff();
        }
      }  
}


void checkPressing(int button) {
  if (sequence[currentPlayingSeqIndex] == button) {
    currentPlayingSeqIndex ++;
    if (currentPlayingSeqIndex >= level) {
      state = stateNewLevel;
      pressingFeedback(true);
    }
  } else {
    state = stateGameOver;
    pressingFeedback(true);
  }
  
}
