
#include <SoftwareSerial.h>
#include <Stepper.h>

SoftwareSerial Serial2(2, 3); // RX, TX

unsigned long n = 0, dti = 0 , dte = 0;
int dirPin = 4;
int stepPin = 5;
int enablePin = 6;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(dirPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(enablePin, OUTPUT);
  digitalWrite(enablePin, LOW);
  while (!Serial);
}

void loop() {
  // put your main code here, to run repeatedly:
  
  if (Serial2.available()){
    String data = Serial2.readStringUntil('\n');
    Serial.println(data);
    int first = 0;
    int last = data.indexOf(" ");
    n = data.substring(first, last).toInt() * 32;
    first = last + 1;
    last = data.indexOf(" ", first);
    dti = data.substring(first, last).toInt() / 32;
    first = last + 1;
    last = data.length();
    dte = data.substring(first, last).toInt() / 32;
    data = "";
    digitalWrite(enablePin, HIGH);
    delay(5000);
    digitalWrite(enablePin, LOW);
  }

  digitalWrite(dirPin, LOW);
  for (int i = 0; i < n; i++)
  {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(dti);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(dti);
  }
  delay(1);

  digitalWrite(dirPin, HIGH);
  for (int j = 0; j < n; j++)
  {
      digitalWrite(stepPin, HIGH);
      delayMicroseconds(dte);
      digitalWrite(stepPin, LOW);
      delayMicroseconds(dte);
  }
  delay(1);
}
