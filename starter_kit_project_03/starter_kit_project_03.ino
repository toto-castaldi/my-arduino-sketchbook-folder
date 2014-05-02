const int sensorPin = A0;
const int calibrationLoopCycles = 200;
const float tolleranza = 1.5;
float baseTemperature = 0;
int calibrationLoopCount = 0;

void setup() {
  Serial.begin(9600);
  
  for (int pinNumber = 2; pinNumber < 5; pinNumber ++) {
    pinMode(pinNumber, OUTPUT);
    digitalWrite(pinNumber, LOW);
  }
  
  pinMode(13, OUTPUT);     //led di calibrazione
  digitalWrite(13, LOW);
  
}

void loop() {
  int sensorVal = analogRead(sensorPin);
  
  Serial.print("Sensore : ");
  Serial.print(sensorVal);
  
  float voltage = (sensorVal/1024.0) * 5.0;
  
  Serial.print(", Volt : ");
  Serial.print(voltage);
  
  Serial.print(", gradi C: ");
  float temperature = (voltage - .5) * 100;
  Serial.print(temperature);
  
  if (calibrationLoopCount < calibrationLoopCycles) {
    baseTemperature = baseTemperature + temperature;
    calibrationLoopCount ++;
  } else if (calibrationLoopCount == calibrationLoopCycles) {
    digitalWrite(13, HIGH);
    baseTemperature = baseTemperature / calibrationLoopCycles;
    calibrationLoopCount ++;
  } else {
    Serial.print(" (base : ");
    Serial.print(baseTemperature);
    Serial.print(")");
    
    if (temperature > baseTemperature + tolleranza * 2) {
      digitalWrite(2, LOW);
      digitalWrite(3, LOW);
      digitalWrite(4, HIGH);
    } else if (temperature > baseTemperature + tolleranza) {
      digitalWrite(2, LOW);
      digitalWrite(3, HIGH);
      digitalWrite(4, LOW);
    } else {
      digitalWrite(2, HIGH);
      digitalWrite(3, LOW);
      digitalWrite(4, LOW);
    }
    
  }
  
  Serial.println("");
  
}
