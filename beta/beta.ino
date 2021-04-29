#include <Keypad.h>
#include <SoftwareSerial.h>

const byte ROWS = 4;
const byte COLS = 4;
byte rowPins[ROWS] = {9,8,7,6};
byte colPins[COLS] = {5,4,3,2};
char keyInput;

SoftwareSerial s(1,0); // RX TX pins

char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};

Keypad keypad4x4 = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS);
                            
void setup()
{
  Serial.begin(9600);
  //s.begin(9600);

}
                            
void loop()
{
  keyInput = keypad4x4.getKey();
  if (keyInput){
    Serial.write(keyInput);
  }
}
