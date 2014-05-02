const int greenLEDPin = 9;
const int redLEDPin = 11;
const int blueLEDPin = 10;

const int redSensorPin = A2;
const int greenSensorPin = A1;
const int blueSensorPin = A0;

int redValue = 0;
int greenValue = 0;
int blueValue = 0;

int redSensorValue = 0;
int greenSensorValue = 0;
int blueSensorValue = 0;

void setup() {
  Serial.begin(9600);
  pinMode(greenLEDPin, OUTPUT);
  pinMode(redLEDPin, OUTPUT);
  pinMode(blueLEDPin, OUTPUT);
}

void loop() {
  redSensorValue = analogRead(redSensorPin);
  delay(5);
  greenSensorValue = analogRead(greenSensorPin);
  delay(5);
  blueSensorValue = analogRead(blueSensorPin);
  delay(5); //il convertitore ADC ha bisogno di tempo !
  Serial.print("Raw sensor value:");
  Serial.print("\t Red:");
  Serial.print(redSensorValue);
  Serial.print("\t Green:");
  Serial.print(greenSensorValue);
  Serial.print("\t Blue:");
  Serial.println(blueSensorValue);
  redValue = redSensorValue /4;
  greenValue = greenSensorValue /4;
  blueValue = blueSensorValue /4;
  Serial.print("Mapped sensor value:");
  Serial.print("\t Red:");
  Serial.print(redValue);
  Serial.print("\t Green:");
  Serial.print(greenValue);
  Serial.print("\t Blue:");
  Serial.println(blueValue);
  
  analogWrite(redLEDPin, redValue);
  analogWrite(greenLEDPin, greenValue);
  analogWrite(blueLEDPin, blueValue);
  
}
