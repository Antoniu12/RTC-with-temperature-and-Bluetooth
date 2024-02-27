#include <DS3231.h>
#include <LiquidCrystal.h>
#include <SoftwareSerial.h>

//Valori pentru montare Hardware
//////////////////////////////////////////////////////////////////////////////////////////////
DS3231 rtc(SDA, SCL);
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;   // Pins of LCD to connect to Arduino
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
const int buzzer = 13; // Buzzer to Arduino pin 13
const int buttonPin = 7; // Button to stop the buzzer
SoftwareSerial mySerial(8, 9); //Creare Serial pe pinii 8 si 9 (rx, tx)
//////////////////////////////////////////////////////////////////////////////////////////////
 
//Valori pentru setarile ceasului
//////////////////////////////////////////////////////////////////////////////////////////////
char* alarmTime = "";
char* function = "";
String auxFunc = "";
String auxInput = "";
String inputString = "";
String funcString = "";
boolean stringComplete = false;
String a="";
String b="";
String c="";
String day = "";
int ok = 0;
//////////////////////////////////////////////////////////////////////////////////////////////


void setup() {
  pinMode(buzzer, OUTPUT); // Set buzzer - pin 13 as an output
  pinMode(buttonPin, INPUT_PULLUP); // Set button - pin 10 as an input with internal pull-up resistor
  Serial.begin(9600);
  mySerial.begin(9600);  
  rtc.begin();         // Initializing RTC Module
  lcd.begin(16, 2);    // Initializing LCD

  // Setting up the date and time
   rtc.setDOW(MONDAY);        // Set Day-of-Week to SUNDAY
   rtc.setTime(00, 00, 00);    // ora:minut:secunda
   rtc.setDate(01, 01, 2000);  // zi/luna/an
}

void loop() {

//Printare data, ziua, ora si temperatura
//////////////////////////////////////////////////////////////////////////////////////////////
  lcd.clear();

  lcd.setCursor(0, 0);
  lcd.print(rtc.getDateStr());

  lcd.setCursor(13, 0);
  lcd.print(rtc.getDOWStr());

  lcd.setCursor(0, 1);
  lcd.print(rtc.getTimeStr());

  lcd.setCursor(9, 1);
  lcd.print(rtc.getTemp());
  lcd.print(" C");
//////////////////////////////////////////////////////////////////////////////////////////////

  mySerial.write(rtc.getTimeStr());
  mySerial.write("\n");

//Setarile ceasului
//////////////////////////////////////////////////////////////////////////////////////////////
  if(stringComplete){
    auxFunc = funcString;
    auxInput = inputString;
    function = auxFunc.c_str();

    if(strcmp(function, "set alarm")==0){ //Setarea Alarmei
      alarmTime = auxInput.c_str();
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("ALARM SET:");
      lcd.setCursor(0,1);
      lcd.print(alarmTime);
      delay(2500);
    }
    else if(strcmp(function, "set date")==0){ //Setarea datei
      a = inputString.substring(0, 2); //ziua
      b = inputString.substring(3, 5); //luna
      c = inputString.substring(6);    //anul
      int zi = a.toInt();
      int luna = b.toInt();
      int an = c.toInt();
      if (zi>0 && zi<=31 && luna > 0 && luna <= 12 && an > 1900 && an < 3000){
        rtc.setDate(zi, luna, an);  // zi/luna/an
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DATE SET!");
        delay(2500);
      }
      else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("DATA INVALIDA!");
      delay(2500);
      }
    }
    else if(strcmp(function, "set time")==0){ //Setarea orei
      a = inputString.substring(0, 2); //ora
      b = inputString.substring(3, 5); //minutul
      c = inputString.substring(6);    //secunda
      int ora = a.toInt();
      int minut = b.toInt();
      int secunda = c.toInt();
      if (ora>=0 && ora<24 && minut >= 0 && minut < 60 && secunda >=0 && secunda < 60){
        rtc.setTime(ora, minut, secunda);  // ora:minut:secunda
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("TIME SET!");
        delay(2500);
      }
      else {
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("TIMP INVALID!");
      delay(2500);
      }
    }
    else if(strcmp(function, "set day")==0){   //Setarea zilei
      day = auxInput;
      int ok = 0;
      if (day == "monday") {
        rtc.setDOW(MONDAY);
        ok=1;
      } else if (day == "tuesday") {
        rtc.setDOW(TUESDAY);
        ok=1;
      } else if (day == "wednesday") {
        rtc.setDOW(WEDNESDAY);
        ok=1;
      } else if (day == "thursday") {
        rtc.setDOW(THURSDAY);
        ok=1;
      } else if (day == "friday") {
        rtc.setDOW(FRIDAY);
      } else if (day == "saturday") {
        rtc.setDOW(SATURDAY);
        ok=1;
      } else if (day == "sunday") {
        rtc.setDOW(SUNDAY);
        ok=1;
      }
      if(ok==1){
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("DAY SET!");
        delay(2500);
      } else {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("ZI INVALIDA!");
        delay(2500);
      }
    }
  else {
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("FUNCTIE");
    lcd.setCursor(0,1);
    lcd.print("INVALIDA!");
    delay(2500);
  }
  funcString = "";
  inputString = "";
  stringComplete = false;
} 
//////////////////////////////////////////////////////////////////////////////////////////////

//ALARMA
////////////////////////////////////////////////////////////////////////////////////////////// 
  if (strcmp(rtc.getTimeStr(), alarmTime) == 0) {
    while (digitalRead(buttonPin) == HIGH) { // Check button state
      tone(buzzer, 1000);
      lcd.setCursor(0, 0);
      lcd.print("ALARMA!");
      lcd.setCursor(0, 1);
      lcd.print(alarmTime);

      delay(200);
      lcd.clear();
      noTone(buzzer);
      delay(200);
    }
    noTone(buzzer); // Stop the buzzer when the button is pressed 
    inputString = "";
  }
  delay(500);
}
//////////////////////////////////////////////////////////////////////////////////////////////

//Functie pentru citirea din Serial
//////////////////////////////////////////////////////////////////////////////////////////////
void serialEvent() {
  while (Serial.available()) {
    char inChar = (char)Serial.read();
    if(inChar!= '=' && ok==0)
      funcString += inChar;
    else
      ok=1;
    if (inChar != '\n' && ok==1){
      if(inChar != '=')
        inputString += inChar;
    }
    else 
      ok=0;
    if (inChar == '\n') {
      stringComplete = true;
 }
 }
}
//////////////////////////////////////////////////////////////////////////////////////////////