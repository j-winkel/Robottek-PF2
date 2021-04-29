#include <LiquidCrystal.h>

// dedicated analog ports
int redLED = A0;
int greenLED = A1;
int LDR = A4;

// digital ports
//These values are for the LCD display
int DB7 = 2;
int DB6 = 3;
int DB5 = 4;
int DB4 = 5;
int E = 6;
int RS = 7;

//The encoder
int DT = 12; //The turning value
int CLK = 13; //The clicking value

int buzzer = 8;
int motorInput_1 = 10; //Locks the box
int motorInput_2 = 11; //Unlocks the box

////////////////////////////
//variables
////////////////////////////
LiquidCrystal lcd(7, 6, 5, 4, 3, 2);

char data[5]; // the array that is displayed on the lcd
int lockState; // 1 = open 0 = locked
String stringCode = "12234"; // the correct pin code
int state; //Determines which sensor output should get priority over the LCD display
int lastState; //Stores the state and if changed then change the display
String s; //String version of the data array
int encoderState; // encoderState and encoderLastState is there to see if the rotary encoder has been used.
int encoderLastState; 
int encoderCount; //The current encoder value
int encoderCode = 32; //The correct encoderCode to open the lockbox
int nextNumber = 0; //Used to cycle through the data array


////////////////////////////
// setup and loop
////////////////////////////


void setup(){
  Serial.begin(9600);
  lcd.begin(16,2);

  //Marks the pins active
  
  pinMode(greenLED, OUTPUT);
  pinMode(redLED, OUTPUT);
  pinMode(motorInput_1, OUTPUT);
  pinMode(motorInput_2, OUTPUT);
  pinMode(LDR, INPUT);
  pinMode(CLK, INPUT);
  pinMode(DT, INPUT);

  //Sets the starting state and lockState
  state = 0;
  lockState = 0;

  encoderLastState = digitalRead(CLK);
  
}

void loop(){
  readKeyInput(); 
  incoderRead();
  lcd.setCursor(0,0);
  display();
  lastState = state;
  if (sizeof(keyInputToString()) == 6 && keyInputToString() == stringCode && readLDR() > 100 && incoderRead() == encoderCode){
    digitalWrite(redLED, LOW);
    digitalWrite(greenLED ,HIGH);
    buzzerUnlock();

    if (lockState == 0){
        motorControl("backwards");
        delay(500);
        motorControl("stop");
        lockState = 1;
      }
    
    for(int i=0;i< sizeof(data);i++){ 
      data[i]={};
    }
  }
  else if(sizeof(keyInputToString()) == 6 && data[4] != 0){
    buzzerLock();
    
    for(int i=0;i< sizeof(data);i++){ 
        data[i]={};
        }
    nextNumber = 0;
  } else {
    digitalWrite(redLED, HIGH);
    digitalWrite(greenLED ,LOW);
  }

  //Serial.println(state);
  readLDR();
  if(lockState == 0){
    closeBox();
  }
}

////////////////////////////
// Functions
////////////////////////////

void closeBox(){
    motorControl("forward");
    lockState = 0;
}

int readLDR(){
    int analogLightValue = analogRead(LDR);
    int mappedLightValue = map(analogRead(LDR),0,1023,0,255);
    return mappedLightValue;
}

//String LDRToString(){
//  return String(readLDR());
//}

void readKeyInput(){
  while(Serial.available()){
  char input = Serial.read();
    Serial.println(input);
    Serial.println(data);
  if (input) {
    if (nextNumber < sizeof(data)){
      data[nextNumber]=input;
      nextNumber+=1;
      state = 1;
    }

    if(input == '#'){
      for(int i=0;i< sizeof(data);i++){ 
        data[i]={};
        nextNumber = 0;
      }
      lockState = 0;
    }
  }
  }
  //state = 1;
}

String keyInputToString(){ 
    s = "";
    for (int i = 0; i < sizeof(data); i++){
        s = s + data[i];
    } 
    return s;
}

void motorControl(String input){
  if (input == "forward"){
    digitalWrite(motorInput_1, HIGH);
    digitalWrite(motorInput_2, LOW);
  } else if (input == "backwards"){
    digitalWrite(motorInput_1, LOW);
    digitalWrite(motorInput_2, HIGH);
  } else if (input == "stop"){
    digitalWrite(motorInput_1, LOW);
    digitalWrite(motorInput_2, LOW);
  }
}

void buzzerControl(int frequency, int duration){
  tone(buzzer, frequency, duration);
  // gives not declared error in arduino ide
}

int incoderRead(){

  encoderState = digitalRead(CLK);

  if (encoderState != encoderLastState  && encoderState == 1){
    if (digitalRead(DT) != encoderState) {
      encoderCount --;
      state = 3;
    } else {
      encoderCount ++;
      state = 3;
    }

    if (encoderCount > 50){
      encoderCount = 50;
    } else if (encoderCount < 0){
      encoderCount = 0;
    }
  }
  
  encoderLastState = encoderState;
  
  return encoderCount;
  delay(1); // needed for syncing rotation reading 
}

String encoderReadToString(){
  return String(encoderCount);
}

void display(){
  if(state != lastState){
    lcd.clear();
  }

  if (encoderState != encoderLastState){
    state == 3;
  }
  
  if (state == 1){
    lcd.print("input: ");
    lcd.print(keyInputToString());
  } else if (state == 3){
    lcd.print("encoder: ");
    lcd.print(encoderReadToString());
  }

}


void buzzerUnlock(){
  tone(buzzer, 1000); 
  delay(50);      
  noTone(buzzer);    
  tone(buzzer, 1200);
  delay(50);
  noTone(buzzer);
}

void buzzerLock(){
  tone(buzzer, 1000); 
  delay(50);      
  noTone(buzzer);    
  tone(buzzer, 1000);
  delay(50);
  tone(buzzer, 1000);
  delay(50);
  noTone(buzzer);
}
