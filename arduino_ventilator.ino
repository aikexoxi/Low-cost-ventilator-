#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
#include "HX710B.h"
#include <SoftwareSerial.h>


#define IE_PIN          A2
#define TIDAL_VOL_PIN   A1
#define BPM_PIN         A0
#define ALARM_BUZZ_PIN  7
#define ALARM_LED_PIN   A3



LiquidCrystal_I2C lcd(0x27, 16, 2);
HX710B ps1, ps2; 
SoftwareSerial Serial2(8, 9); // RX, TX


float AIR_PRESSURE = 0;
float AIR_FLOW = 0;
float P1 = 0;
float P2 = 0;
float prev_P1 = 0;
float prev_P2 = 0;
int TIDAL_VOLUME = 400;
int BREATH_PER_MINUTE = 5;
int I_TO_E_RATIO = 1;
bool bpm_increased = false;
long double last_time; 


float smooth_pa1; // initialization at generic value
float smooth_pa2; // initialization at generic value
float smooth_flow;



unsigned long n = 0;
unsigned long dti = 1;
unsigned long dte = 1;

int TV_val = 0; // Tidal Volume potentiometer value
int long TV_previous = 0;
int long TV_newval = 0;


int IE_val = 0; // Inspiration-Expiration ratio potentiometer value
int IE_previous = 0;
int long IE_newval = 0;


int BPM_val = 0; // Breathing/Minute potentiometer value
int BPM_previous = 0;
int long BPM_newval = 0;




void setup() {
  Serial.begin(9600);
  Serial2.begin(9600);
  pinMode(IE_PIN, INPUT);
  pinMode(TIDAL_VOL_PIN, INPUT);
  pinMode(BPM_PIN, INPUT);
  pinMode(ALARM_BUZZ_PIN, OUTPUT);
  pinMode(ALARM_LED_PIN, OUTPUT);
  digitalWrite(ALARM_LED_PIN, HIGH);
  pinMode(2, INPUT);

  
  lcd.begin();
  lcd.backlight();
  lcd.home();
  lcd.print("Ventilator...");
  delay(1000);
  lcd.clear();

  ps1.begin(12, 3);
  ps2.begin(4, 5);

  last_time = millis();
}

void loop() {
  // put your main code here, to run repeatedly:
  unsigned long t = millis();
  
  TV_val = analogRead(TIDAL_VOL_PIN);
  if ((TV_val > TV_previous + 3) || (TV_val < TV_previous - 3)) {
      TV_newval = map(TV_val, 0, 1022, 400, 1000); // ml
      TIDAL_VOLUME = TV_newval;
      TV_previous = TV_val;
  }
  BPM_val = analogRead(BPM_PIN);
  if ((BPM_val > BPM_previous + 3) || (BPM_val < BPM_previous - 3)) {
      BPM_newval = map(BPM_val, 0, 1022, 5, 30); 
      BREATH_PER_MINUTE = BPM_newval;
      BPM_previous = BPM_val;
  }
  
  IE_val = analogRead(IE_PIN);
  if ((IE_val > IE_previous + 6) || (IE_val < IE_previous - 6)) {
      IE_newval = map(IE_val, 0, 200, 1, 6); 
      if (IE_newval > 6) IE_newval = 6;
      I_TO_E_RATIO = IE_newval;
      IE_previous = IE_val;
  }

  if (ps1.is_ready()) {
    prev_P1 = P1;
    P1 = ps1.pascal() * (575.0/483.5);
  }
  if (ps2.is_ready()) {
    prev_P2 = P2;
    P2 = ps2.pascal();
  }


  unsigned long l_n, l_dti, l_dte;
  
  l_n = TIDAL_VOLUME + 80;

  //dti = 60 / (n * (1 + I_TO_E_RATIO) * BREATH_PER_MINUTE); // in seconds
  l_dti = (60000000 / BREATH_PER_MINUTE) / (l_n * (1 + I_TO_E_RATIO)); // in microseconds
  if (l_dti < 1)
  {
      l_dti = 1;
  }
  //dte = 60 * I_TO_E_RATIO / (n * (1 + I_TO_E_RATIO) * BREATH_PER_MINUTE); // in seconds
  l_dte = l_dti * I_TO_E_RATIO; // in microseconds
  if (l_dte < 1)
  {
      l_dte = 1;
  }

  if (abs(l_n - n) > 20 || abs(l_dti - dti) > 80 || abs(l_dte - dte) > 80) {
    long double current_time = millis();
    if ((current_time - last_time) > 500) {
      Serial2.print(l_n);
      Serial2.print(" ");
      Serial2.print(l_dti);
      Serial2.print(" ");
      Serial2.println(l_dte);
//      
//      Serial.print(l_n);
//      Serial.print(" ");
//      Serial.print(l_dti);
//      Serial.print(" ");
//      Serial.println(l_dte);
      last_time = current_time;
    }
  }

  n = l_n;
  dti = l_dti;
  dte = l_dte;

  float alpha = 1.0;   // exponential smoothing constant
  float flow = (prev_P1 - P1) * 5;
  float m = 15;

//    Serial.print(P1);
//    Serial.print(" ");
//    Serial.print(P2);
//    Serial.print(" ");
//    Serial.println(flow);
//  if(P1 >= P2){
//  flow = m*sqrt(P1-P2) - m; 
//  }
//  if(P2 > P1){
//  flow = m - m*sqrt(P2-P1);
//  }
  
//  if (abs(flow) < 1) flow = 0;

  AIR_PRESSURE = P1;
  AIR_FLOW = flow;



//  if (smooth_pa1 < 570 || smooth_pa2 < 570)
//  {
//      tone(ALARM_BUZZ_PIN, 3000, 1500);
//      digitalWrite(ALARM_LED_PIN, LOW);
//      if (!bpm_increased) {
//        BREATH_PER_MINUTE += 5;
//        bpm_increased = true;
//      }
//      digitalWrite(ALARM_LED_PIN, HIGH);
//      last_time = millis();
//  }
//  else if (smooth_pa1 > 585 || smooth_pa2 > 585)
//  {
//      if (bpm_increased) {
//        BREATH_PER_MINUTE -= 5;
//        bpm_increased = false;
//      }
//      digitalWrite(ALARM_LED_PIN, LOW);
//      tone(ALARM_BUZZ_PIN, 3000, 10000);        
//      digitalWrite(ALARM_LED_PIN, HIGH);
//  }
//  else
//  {
//      digitalWrite(ALARM_LED_PIN, HIGH);
//  }



    
  lcd.setCursor(0, 1);
  lcd.print(AIR_PRESSURE);
  lcd.setCursor(7, 1);
  lcd.print("           ");
  lcd.setCursor(7, 1);
  lcd.print(AIR_FLOW);

  lcd.setCursor(0, 0);
  lcd.print(I_TO_E_RATIO);
  lcd.setCursor(2, 0);
  if (TIDAL_VOLUME < 1000)
  {
      lcd.print(0);
      lcd.print(TIDAL_VOLUME);
  }
  else
  {
      lcd.print(TIDAL_VOLUME);
  }
  lcd.setCursor(7, 0);

  if (BREATH_PER_MINUTE < 10)
  {
      lcd.print(0);
      lcd.print(BREATH_PER_MINUTE);
  }
  else
  {
      lcd.print(BREATH_PER_MINUTE);
  }


  
//  Serial.println(millis() - t);
}
