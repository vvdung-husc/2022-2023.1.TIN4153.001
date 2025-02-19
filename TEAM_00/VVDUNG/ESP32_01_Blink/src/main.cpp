#include <Arduino.h>
#include <TM1637Display.h>


#define SWITCH_PIN 23  // the number of the pushbutton pin
#define SWITCH_LED 21

#define CLK 18
#define DIO 19

//Pin
#define rLED  27
#define yLED  26
#define gLED  25

#define LIGHT_SENSOR_PIN 13

//1000 ms = 1 seconds
uint rTIME = 7000;   //5 seconds
uint yTIME = 5000;
uint gTIME = 10000;

ulong currentMiliseconds = 0;
bool switchDisplay = true;


ulong nextTimeTotal = 0;
int currentLED = rLED;


int darkThreshold = 1000;

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void SwitchDisplay();
void NonBlocking_Traffic_Light_TM1637();

void setup() {
  Serial.begin(115200);
  // initialize the LED pin as an output:
  pinMode(SWITCH_LED, OUTPUT);
  // initialize the pushbutton pin as an input:
  pinMode(SWITCH_PIN, INPUT_PULLUP);

  pinMode(rLED, OUTPUT);
  pinMode(yLED, OUTPUT);
  pinMode(gLED, OUTPUT);


  display.setBrightness(7);
  digitalWrite(SWITCH_LED, HIGH);
  
  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  currentLED = rLED;
  nextTimeTotal += rTIME;
  
  
  Serial.println("== START ==>");
  Serial.print("1. RED    => GREEN  "); Serial.println(nextTimeTotal);  
}

bool isDark(){
  static ulong ldrTimeStart = 0;
  static int lastValue = darkThreshold;
  static bool darkNow = false;

  if (!IsReady(ldrTimeStart, 1000)) return darkNow;
  int analogValue = analogRead(LIGHT_SENSOR_PIN);
  if (lastValue == analogValue) return darkNow;

  if (analogValue < darkThreshold) {
    if (!darkNow){      
      darkNow = true;
      digitalWrite(currentLED, LOW);
      display.clear();
      Serial.print("DARK  value = ");
      Serial.println(analogValue);   // the raw analog reading  
    }        
  }
  else{
    if (darkNow){
      darkNow = false;
      digitalWrite(yLED, LOW);
      Serial.print("LIGHT value = ");
      Serial.println(analogValue);   // the raw analog reading
    }
  }

  lastValue = analogValue;
  return darkNow;  
}

void Led_Yellow_Blink(){
  static ulong lastTime = 0;
  static bool isLED_ON = false;
  if (!IsReady(lastTime, 1000)) return;
  if (!isLED_ON)
  {
    digitalWrite(yLED, HIGH); // Bật LED        
  }
  else
  {
    digitalWrite(yLED, LOW); // Tắt LED    
  }
  isLED_ON = !isLED_ON;
}

void loop() {

  currentMiliseconds = millis();
  SwitchDisplay();  
  if (!isDark()) NonBlocking_Traffic_Light_TM1637();
  else Led_Yellow_Blink();
  
}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}

void SwitchDisplay() {
  static int lastState = HIGH;
  static ulong lastTime = 0;
  
  if (!IsReady(lastTime, 50)) return;

  int t = digitalRead(SWITCH_PIN);
  if (t != lastState){
    if (t == LOW){
      if (switchDisplay){
        Serial.println("Switching display OFF");
        digitalWrite(SWITCH_LED, LOW); 
        display.clear();
      }
      else{        
        digitalWrite(SWITCH_LED, HIGH);
        Serial.println("Switching display ON");
      }
      switchDisplay = !switchDisplay;
    }
    lastState = t;
  }      
}
void NonBlocking_Traffic_Light_TM1637(){
  static ulong ledTimeStart = 0;
  static ulong counterTimeStart = 0;
  static uint tmCounter = (rTIME / 1000) -1;

  bool bShowCounter = false;
  switch (currentLED) {
    case rLED: // Đèn đỏ: 5 giây
      if (IsReady(ledTimeStart, rTIME)) {
        digitalWrite(rLED, LOW);
        digitalWrite(gLED, HIGH);
        currentLED = gLED;
        nextTimeTotal += gTIME;
        tmCounter = (gTIME / 1000) - 1 ; 
        bShowCounter = true;  
        counterTimeStart = currentMiliseconds;        
        Serial.print("2. GREEN  => YELLOW "); Serial.println(nextTimeTotal);        
      } 
      break;

    case gLED: // Đèn xanh: 7 giây
      if (IsReady(ledTimeStart,gTIME)) {        
        digitalWrite(gLED, LOW);
        digitalWrite(yLED, HIGH);
        currentLED = yLED;
        nextTimeTotal += yTIME;
        tmCounter = (yTIME / 1000) - 1; 
        bShowCounter = true;   
        counterTimeStart = currentMiliseconds;    
        Serial.print("3. YELLOW => RED    "); Serial.println(nextTimeTotal);        
      }
      break;

    case yLED: // Đèn vàng: 2 giây
      if (IsReady(ledTimeStart,yTIME)) {        
        digitalWrite(yLED, LOW);
        digitalWrite(rLED, HIGH);
        currentLED = rLED;
        nextTimeTotal += rTIME;
        tmCounter = (rTIME / 1000) - 1; 
        bShowCounter = true;       
        counterTimeStart = currentMiliseconds;        
        Serial.print("1. RED    => GREEN  "); Serial.println(nextTimeTotal);
      }
      break;
  }
  
  if (!bShowCounter) bShowCounter = IsReady(counterTimeStart, 1000);
  if (bShowCounter) {
    if (switchDisplay) display.showNumberDec(tmCounter, true, 2, 2);  
    tmCounter--;
  }
}