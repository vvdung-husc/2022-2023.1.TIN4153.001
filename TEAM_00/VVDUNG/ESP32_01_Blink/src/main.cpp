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

//1000 ms = 1 seconds
uint rTIME = 7000;   //5 seconds
uint yTIME = 5000;
uint gTIME = 10000;

ulong currentMiliseconds = 0;
bool switchDisplay = true;

ulong ledTimeStart = 0;
ulong nextTimeTotal = 0;
int currentLED = rLED;
int tmCounter = 0;
ulong counterTimeStart = 0;

TM1637Display display(CLK, DIO);

bool IsReady(ulong &ulTimer, uint32_t milisecond);
void cbSwicthDisplay(bool state);
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
  cbSwicthDisplay(true);

  digitalWrite(yLED, LOW);
  digitalWrite(gLED, LOW);
  digitalWrite(rLED, HIGH);
  currentLED = rLED;
  nextTimeTotal += rTIME;
  tmCounter = (rTIME / 1000) -1;
  
  Serial.println("== START ==>");
  Serial.print("1. RED    => GREEN  "); Serial.println(nextTimeTotal);  
}

void loop() {

  currentMiliseconds = millis();
  SwitchDisplay();  
  NonBlocking_Traffic_Light_TM1637();

}

bool IsReady(ulong &ulTimer, uint32_t milisecond)
{
  if (currentMiliseconds - ulTimer < milisecond)
    return false;
  ulTimer = currentMiliseconds;
  return true;
}

void cbSwicthDisplay(bool state) {
  if (state){
    digitalWrite(SWITCH_LED, HIGH);
  }
  else{
    digitalWrite(SWITCH_LED, LOW);
    display.clear();
  }
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
        cbSwicthDisplay(false);        
      }
      else{        
        cbSwicthDisplay(true);
        Serial.println("Switching display ON");
      }
      switchDisplay = !switchDisplay;
    }
    lastState = t;
  }      
}
void NonBlocking_Traffic_Light_TM1637(){
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