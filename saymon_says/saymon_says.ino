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
const int stateEnd = 2;
const int stateNewLevel = 3;
const int statePressedOk = 4;

int level;
int state; 
int sequence[maxLength];
int currentPlayingSeqIndex;

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
  
}

void loop() {
  
  if (state == stateNewLevel) {
    level ++;
    String a = "livello ";
     a += level;
    displayWrite(a);
    delay(2000);
    state = statePrepare;
  }
  
  if (state == stateEnd) {
    String a = "punti ";
    a += level;
    displayWrite(a);
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
    displayWrite("memorizza");
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
      
      //Serial.println(delayMemorization);
      
      delay(delayMemorization);
      noTone(tonePin);
      allLedOff();
      delay(50); //to see two or more equal color in sequence
      
    }
    state = statePlay;
    displayWrite("ripeti");
    currentPlayingSeqIndex = 0;
    
  }  
  
  if (pressed(redButton) && (state == statePlay)) {
    checkPressing(redLed, NOTE_C4, redButton); 
  } else if (pressed(greenButton) && (state == statePlay)) {
    checkPressing(greenLed, NOTE_D4, greenButton);
  } else if (pressed(yellowButton) && (state == statePlay)) {
    checkPressing(yellowLed, NOTE_E4, yellowButton);
  } else if (pressed(blueButton) && (state == statePlay)) {
    checkPressing(blueLed, NOTE_F4, blueButton);
  }

}

void checkPressing(int led, unsigned int note, int button) {
        digitalWrite(led, HIGH);
         tone(tonePin, note);
       
       if (sequence[currentPlayingSeqIndex] == button) {
        
         displayWrite("OK !");
         currentPlayingSeqIndex ++;
         if (currentPlayingSeqIndex >= level) {
           state = stateNewLevel;
         } 
       } else {
         displayWrite("sbagliato");
         state = stateEnd;
       }
                delay(1000);
          noTone(tonePin);
          allLedOff();

}

boolean pressed(int button) {
  return analogRead(button) > 900;
}


void allLedOff() {
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
}


void displayWrite(String text) {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(text);
  display.display();
}

