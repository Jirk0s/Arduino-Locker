#include <EEPROM.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>

//PINY
#define LED 13
#define BUZZER 12
//
//PROMENE
//
String userValue;
byte Mode = 1;
byte pocetPokusu;
String pass;
char PRKEY;

//KEYPAD
const byte ROWS = 4; 
const byte COLS = 4; 
char keys[ROWS][COLS] = {
  {'1','2','3','A'},
  {'4','5','6','B'},
  {'7','8','9','C'},
  {'*','0','#','D'}
};
byte rowPins[ROWS] = {9,8,7,6}; 
byte colPins[COLS] = {5,4,3,2};
char key;

//MAPOVANI
LiquidCrystal_I2C lcd(0x27,20,4);
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

//LCD MONITOR
void default_LCD(){
  lcd.setCursor(2,0);
  lcd.print("ZADEJ HESLO:");
  lcd.setCursor(2,1);
}

//EEPROM
  void EEPROM_READ(){
    for(int i=0; i<EEPROM.length(); i++){
      if(EEPROM.read(i) == 255)break;
      char eeprom_;
      eeprom_ = (char)EEPROM.read(i);
      pass += eeprom_;
      Serial.print((char)EEPROM.read(i));
      Serial.print(" / ");
    }
  }
  void EEPROM_WRITE(){
    for(int i=0; i<pass.length(); i++){
      EEPROM.update(i,pass[i]);
    }
  }


//MENU
void menu(){
  lcd.setCursor(1,0);
  lcd.print("ZMENA HESLA (A)");
  lcd.setCursor(1,1);
  lcd.print("ODHLASIT SE (B)");
}

//LED BLIKANí + Tón
void BlinkTone(byte time, int pitch){
    tone(BUZZER,pitch);
    digitalWrite(LED, HIGH);
    delay(time);
    digitalWrite(LED, LOW);
    noTone(BUZZER);
}


void setup() {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  default_LCD();
  if(EEPROM.read(0) == 255){
    EEPROM.write(0,'4');
    EEPROM.write(1,'4');
    EEPROM.write(2,'5');
    EEPROM.write(3,'5');
    EEPROM_READ();
    Serial.print(pass);
  }else{
    EEPROM_READ();
    EEPROM_WRITE();
  }
}

void loop() {

 //Zapisování z Keypad 

  key = keypad.getKey(); 
    if(key && pocetPokusu!=3){
      if(key!='#'){
      userValue += key;
      }
      if(key!='#' && Mode!=2){
        lcd.print("*");
      }
    }

    //1. EVENT

    if(Mode == 1){
      //SWITCH SMAZANI, ENTER
      switch(key){
      case '#':{
        if(userValue == pass){
          BlinkTone(500,500);
          lcd.clear();
          Mode = 2;
          menu();
          pocetPokusu = 0;
        }else {
          if(pocetPokusu==3){
          lcd.noBacklight();
          delay(5000);
          pocetPokusu=0;
          }
          lcd.backlight();
          lcd.noBacklight();
          BlinkTone(500,100);
          lcd.backlight();
          userValue=""; 
          lcd.clear();
          default_LCD();
          pocetPokusu++;
          }
      }break;
      case '*':{
        lcd.setCursor(2,0);
        userValue="";
        lcd.clear();
        default_LCD();
      }break;
    }
  }

    //2. EVENT
  
    if(Mode == 2){
      menu();
      switch(key){
        case 'A':{
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("STARE HESLO:");
          lcd.setCursor(2,1);
          userValue="";
          Mode=3;
        }break;
        case 'B':{
          userValue="";
          Mode=1;
          lcd.clear();
          default_LCD();
        }break;
      }
    }

    //3. EVENT

  if(Mode==3){
      switch(key){
      case '#':{
        if(userValue == pass){
          //BLINK
          tone(BUZZER,500);
          digitalWrite(LED, HIGH);
          delay(500);
          digitalWrite(LED, LOW);
          noTone(BUZZER);
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("NOVE HESLO:");
          lcd.setCursor(2,1);
          Mode = 4;
          key="u0000";
          userValue="";
        }else {
          userValue=""; 
          lcd.noBacklight();
          tone(BUZZER,200);
          delay(200);
          lcd.backlight();
          noTone(BUZZER);
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("STARE HESLO:");
          lcd.setCursor(2,1);
          pocetPokusu++;
        }
      }break;
      case '*':{
        lcd.setCursor(2,0);
        userValue="";
        lcd.clear();
        default_LCD();
      }break;
    }
  }

  //4. EVENT

  if(Mode==4){
    switch(key){
      case '#':{
        if(userValue.length() >3){
          //BLINK
          tone(BUZZER,500);
          digitalWrite(LED, HIGH);
          delay(500);
          digitalWrite(LED, LOW);
          noTone(BUZZER);
          lcd.clear();
          Mode = 2;
          pass = userValue;
          EEPROM_WRITE();
        }else {
          userValue=""; 
          lcd.noBacklight();
          tone(BUZZER,200);
          delay(200);
          lcd.backlight();
          noTone(BUZZER);
          lcd.clear();
          lcd.setCursor(2,0);
          lcd.print("NOVE HESLO:");
          lcd.setCursor(2,1);
          pocetPokusu++;
        }
      }break;
      case '*':{
        lcd.setCursor(2,0);
        userValue="";
        lcd.clear();
        default_LCD();
      }break;
    }
  }
}
