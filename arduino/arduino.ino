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
          int baseLevel = 70;
          int acidLevel = 70;
          int macroLevel = 70;
          int microLevel = 70;
          int waterLevel = 70;
          
          //nivel base
          Serial.println(baseLevel);
          delay(20);
          //nivel acido
          Serial.println(acidLevel);
          delay(20);
          //nivel macro
          Serial.println(macroLevel);
          delay(20);
          //nivel micro
          Serial.println(microLevel);
          delay(20);
          //nivel agua
          Serial.println(waterLevel);
          delay(1500);
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
      }
    }
  
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

void blinks(){
    if(digitalRead(led)==LOW){
      digitalWrite(led, HIGH);
    }else{
      digitalWrite(led, LOW);
    }
}

