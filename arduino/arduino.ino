#include <Wire.h>
#include "Arduino.h"
#include "SI114X.h"

#define led 24
#define button 23
#define pot A0
#define ilumPin 25
#define pinMotor 27
#define testMotor 28
#define testLed 29
boolean ilum = false;
int ledSum = 0;
//SI114X SI1145 = SI114X();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(ilumPin, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(testLed, OUTPUT);
  pinMode(testMotor, OUTPUT);
  pinMode(button, INPUT);
//  while (!SI1145.Begin()){
//    Serial.println("Not ready");
//    delay(1000);
//  }
}

void loop() {
  //Espera a recibir un dato(horas de luz), luego envia datos
  //faltantes y vuelve a la espera.
   
    checkLight();                                                             
    if(Serial.available() > 0){
      blinks();
      int option = Serial.parseInt();
      switch(option){
        case 0:
        {
          //pedir valor de sensores
          int buttonValue = digitalRead(button);
          int pot = analogRead(pot);
          pot = map(pot, 0, 1023, 0, 100);
          //led error
          Serial.println(buttonValue);
          delay(20);
          //nivel base
          Serial.println(pot);
          delay(20);
          //nivel acido
          Serial.println(pot);
          delay(20);
          //nivel macro
          Serial.println(pot);
          delay(20);
          //nivel micro
          Serial.println(pot);
          delay(20);
          //nivel agua
          Serial.println(pot);
          delay(1000);
          Serial.flush();
          break;
        }
        case 1:
        {
        //Encender iluminación
          //digitalWrite(ilumPin, HIGH);
          ilum = true;
          break;
        }
        case 2:
        {
          //digitalWrite(ilumPin, LOW);
          ilum = false;
          break;
        }
        case 3:
        {
        //Encender Bomba de agua
          digitalWrite(testMotor, HIGH);
          //digitalWrite(pinMotor, HIGH);
          break;
        }
        case 4:
        {
          //apagar bomba de agua
          digitalWrite(testMotor, LOW);
          //digitalWrite(pinMotor, LOW);
          break;
        }
        case 5:
        {
          //encender macro y micro por primera vez

          break;
        }
        case 6:
        {
          //encender macro y micro para renivelar
          break;
        }
        case 7:
        {
          //revisar nivel de micro nutrientes
          boolean nivel = true;
          if(nivel == true){
            Serial.println(1);
          }else{
            Serial.println(0);
          }
          break;
        }
        case 8:
        {
          //revisar nivel de macro nutrientes
           boolean nivel = true;
          if(nivel == true){
            Serial.println(1);
          }else{
            Serial.println(0);
          }
          break;
        }
        case 9:
        {
          //revisar nivel de ph base
          boolean nivel = true;
          if(nivel == true){
            Serial.println(1);
          }else{
            Serial.println(0);
          }
          break;
        }
        case 10:
        {
          //revisar nivel de ph acido
          boolean nivel = true;
          if(nivel == true){
            Serial.println(1);
          }else{
            Serial.println(0);
          }
          break;
        }
        case 11:
        {
          //revisar nivel de agua
          boolean nivel = true;
          if(nivel == true){
            Serial.println(1);
          }else{
            Serial.println(0);
          }
          break;
        }
      }
    }
    //Espera 1 segundo y lo suma a el contador 
    
    if(digitalRead(ilumPin) == HIGH) ledSum += 1;
  
}
void checkLight(){
  //si el sensor tiene la minima cantidad de luz, 
  //SI1145.ReadVisible() < 280 && 
  if(ilum == true){
    digitalWrite(testLed, HIGH);
  }else{
    digitalWrite(testLed, LOW);
  }
}
void reset(){
  ledSum = 0;
}
void blinks(){
    if(digitalRead(led)==LOW){
      digitalWrite(led, HIGH);
    }else{
      digitalWrite(led, LOW);
    }
}

