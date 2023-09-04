int buttonPin = A0;     

void setup() 
{
    Serial.begin(9600);           
}

void loop() {
  int temp = analogRead(buttonPin);
  //Serial.println(temp);            
  if (temp < 100)
  { 
    //no buttons
  } else if (temp < 150)
  {
    Serial.println("first");
  }
  else if (temp < 250)
  {
    Serial.println("second");
  }
  else if (temp < 350)
  {
    Serial.println("thirth");
  }
  else if (temp < 850)
  {
    Serial.println("fourth");
  }
  else
  {
    Serial.println("fifth");
  }
  delay(100);                         //Delay for stability
}