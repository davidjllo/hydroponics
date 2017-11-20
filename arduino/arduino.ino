#include <Wire.h>
#include "Arduino.h"
#include "SI114X.h"
#define ilumPin 22
#define pinMotor 25
#define dosifBase 29
#define dosifAcido 30
#define dosifMacro 33
#define dosifMicro 34
#define sensorPh 0
boolean ilum = false;
int ledSum = 0;
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
//SI114X SI1145 = SI114X();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  pinMode(led, OUTPUT);
  pinMode(ilumPin, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(dosifBase, OUTPUT);
  pinMode(dosifAcido, OUTPUT);
  pinMode(dosifMacro, OUTPUT);
  pinMode(dosifMicro, OUTPUT);
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
          //delay(2500);
          //Serial.flush();
          break;
        }
        case 1:
        {
        //Encender iluminación
          ilum = true;
          break;
        }
        case 2:
        {
          ilum = false;
          break;
        }
        case 3:
        {
        //Encender Bomba de agua
          digitalWrite(pinMotor, HIGH);
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
          digitalWrite(dosifMacro, HIGH);
          digitalWrite(dosifMicro, HIGH);
          delay(22000);
          digitalWrite(dosifMicro, LOW);
          delay(33000);
          digitalWrite(dosifMacro, LOW);
          break;
        }
        case 6:
        {
          //encender macro y micro para renivelar 4.5 lt
          digitalWrite(dosifMacro, HIGH);
          digitalWrite(dosifMicro, HIGH);
          delay(4400);
          digitalWrite(dosifMicro, LOW);
          delay(7400);
          digitalWrite(dosifMacro, LOW);
          break;
        }
        case 7:
        {
          //indicar inicio
          digitalWrite(ilumPin, HIGH);
          delay(500);
          digitalWrite(ilumPin, LOW);
          delay(500);
          digitalWrite(ilumPin, HIGH);
          delay(500);
          digitalWrite(ilumPin, LOW);
          delay(500);
          break;
        }
        case 8:
        {
          //consultar ph y aplicar correción
          break;
        }
      }
    }
  
}
void controlPh(){
    for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(SensorPin);
    delay(10);
  }
  for(int i=0;i<9;i++)        //sort the analog from small to large
  {
    for(int j=i+1;j<10;j++)
    {
      if(buf[i]>buf[j])
      {
        temp=buf[i];
        buf[i]=buf[j];
        buf[j]=temp;
      }
    }
  }
  avgValue=0;
  for(int i=2;i<8;i++)                      //take the average value of 6 center sample
    avgValue+=buf[i];
  float phValue=(float)avgValue*5.0/1024/6; //convert the analog into millivolt
  phValue=3.5*phValue;                      //convert the millivolt into pH value
  digitalWrite(13, HIGH);       
  delay(800);
  digitalWrite(13, LOW); 
  if(phValue>6.5)
  {
    digitalWrite(dosifAcido, HIGH);
    delay(500)
    digitalWrite(dosifAcido, LOW);
    delay(500)
  }
  else
  {
    if(phValue<5.5)
  {
    digitalWrite(dosifBase, HIGH);
    delay(500)
    digitalWrite(dosifBase, LOW);
    delay(500)
  
  }
  }
}
void checkLight(){
  //si el sensor tiene la minima cantidad de luz, 
  //SI1145.ReadVisible() < 280 && 
  if(ilum == true){
    digitalWrite(ilumPin, HIGH);
  }else{
    digitalWrite(ilumPin, LOW);
  }
}

void blinks(){
    if(digitalRead(led)==LOW){
      digitalWrite(led, HIGH);
    }else{
      digitalWrite(led, LOW);
    }
}

