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

const int redButton = 3;
const int greenButton = 2;
const int yellowButton = 1;
const int blueButton = 0;
const int maxLength = 100; //max level
const int statePrepare = 0;
const int statePlay = 1;
const int stateEnd = 2;

int lastRedButtonValue;
int sequenceLength;
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
  
  pinMode(redButton, INPUT);
  digitalWrite(redButton, LOW);
  pinMode(greenButton, INPUT);
  digitalWrite(greenButton, LOW);
  pinMode(yellowButton, INPUT);
  digitalWrite(yellowButton, LOW);
  pinMode(blueButton, INPUT);
  digitalWrite(blueButton, LOW);
  
  sequenceLength = 1;
  state = statePrepare;
  
  randomSeed(analogRead(A5));
}

void loop() {
  
  if (state == statePrepare) {
    //set
    for (int i = 0; i < sequenceLength; i++) {
      int button = random(4);
      if (button == 0) sequence[i] = redButton;
      if (button == 1) sequence[i] = greenButton;
      if (button == 2) sequence[i] = yellowButton;
      if (button == 3) sequence[i] = blueButton;
    }
    
    //play
    diplayWrite("memorizza");
    for (int i = 0; i < sequenceLength; i++) {
    int button = sequence[i];
      delay(300);
      digitalWrite(button, HIGH);
      if (button == redButton) { tone(tonePin, NOTE_C4); digitalWrite(redLed, HIGH); } 
      if (button == greenButton) { tone(tonePin, NOTE_D4); digitalWrite(greenLed, HIGH); } 
      if (button == yellowButton) { tone(tonePin, NOTE_E4); digitalWrite(yellowLed, HIGH); } 
      if (button == blueButton) { tone(tonePin, NOTE_F4); digitalWrite(blueLed, HIGH); } 
      
      delay(1000);
      noTone(tonePin);
      allLedOff();
      
    }
    state = statePlay;
    diplayWrite("ripeti");
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
         diplayWrite("OK !");
         currentPlayingSeqIndex ++;     
       } else {
         diplayWrite("sbagliato");
         state = stateEnd;
       }
      delay(1000);
      noTone(tonePin);
      allLedOff();
    
}

void allLedOff() {
  digitalWrite(redLed, LOW);
  digitalWrite(greenLed, LOW);
  digitalWrite(yellowLed, LOW);
  digitalWrite(blueLed, LOW);
}

boolean pressed(int button) {
  delay(10);
  return digitalRead(button) == HIGH;
}


void diplayWrite(char* text) {
  display.clearDisplay();
  display.setTextSize(4);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.print(text);
  display.display();
}


