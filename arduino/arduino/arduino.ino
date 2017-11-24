#include <Wire.h>
#include "Arduino.h"
#include "SI114X.h"
//Digital
#define ilumPin 22
#define pinMotor 23
#define dosifBase 42
#define dosifAcido 43
#define dosifMacro 40
#define dosifMicro 41
#define led 53
//Analog
#define sensorPh 7
#define baseSensor 1
#define acidoSensor 2
#define macroSensor 3
#define microSensor 4
#define aguaSensor 5
#define flowSensor 6
boolean ilum = false;
unsigned long int ledTime = 0;
unsigned long int initMark = 0;
unsigned long int endMark = 0;
boolean flow = false;
int ledSum = 0;
unsigned long int avgValue;  //Store the average value of the sensor feedback
float b;
int buf[10],temp;
SI114X SI1145 = SI114X();
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  pinMode(led, OUTPUT);
  pinMode(ilumPin, OUTPUT);
  pinMode(pinMotor, OUTPUT);
  pinMode(dosifBase, OUTPUT);
  pinMode(dosifAcido, OUTPUT);
  pinMode(dosifMacro, OUTPUT);
  pinMode(dosifMicro, OUTPUT);
  while (!SI1145.Begin()){
    Serial.println("Not ready");
    delay(1000);
  }
}

void loop() {
  //Espera a recibir un dato(horas de luz), luego envia datos
  //faltantes y vuelve a la espera.
    checkFlow();
    checkLight(); 
    //delay(500);
//Serial.println("entered serial");    
    if(Serial.available() > 0){
      blinks();
      
      int option = Serial.parseInt();
      switch(option){
        case 0:
        {
          int baseLevel = 85;
          int acidLevel = 85;
          int macroLevel = 85;
          int microLevel = 85;
          int waterLevel = 85;
          
          //nivel base 309
          baseLevel = map(analogRead(baseSensor),100,320,0,100);
          baseLevel = 88;
          Serial.println(baseLevel);
          delay(20);
          //nivel acido 131
          acidLevel = map(analogRead(acidoSensor),0,170,0,100);
          Serial.println(acidLevel);
          delay(20);
          //nivel macro
          macroLevel = map(analogRead(macroSensor),499,612,0,100);
          macroLevel = 85;
          Serial.println(macroLevel);
          delay(20);
          //nivel micro
          microLevel = map(analogRead(microSensor),500,900,0,100);
          Serial.println(microLevel);
          delay(20);
          //nivel agua
          waterLevel = map(analogRead(aguaSensor),499,800,0,100);
          Serial.println(waterLevel);
          //delay(2500); 736
          //Serial.flush();
          break;
        }
        case 1:
        {
        //Encender iluminación
          ilum = true;
          Serial.flush();
          break;
        }
        case 2:
        {
          ilum = false;
          Serial.flush();
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
          digitalWrite(pinMotor, LOW);
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
          Serial.flush();
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
          Serial.flush();
          break;
        }
        case 8:
        {
          //consultar ph y aplicar correción
          controlPh();
          break;
        }
        case 9:
        {
          //Envia horas de led en un día. (se llama apenas finaliza 
          //tiempo de luz)
          Serial.println(ledTime);
          ledTime = 0;
        }
        case 10:
        {
          //check flow
          int result = 0;
          if (flow == true){
            result = 1;
          }else{
            result = 0;
          }
          Serial.println(result);
          flow = false;
        }
        case 11:
        {
          Serial.flush();
          //leer ph
          for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(sensorPh);
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
  phValue=3.5*phValue;
          Serial.println(phValue);
          break;
        }
        case 12:
        {
         int light = SI1145.ReadIR();
         Serial.println(light);
         break; 
        }
      }
    }
  
}
void checkFlow(){
  if (analogRead(flowSensor) > 50){
    flow = true;
  }
}
void controlPh(){
    for(int i=0;i<10;i++)       //Get 10 sample value from the sensor for smooth the value
  { 
    buf[i]=analogRead(sensorPh);
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
    delay(1000);
    digitalWrite(dosifAcido, LOW);
    delay(500);
  }
  else
  {
    if(phValue<5.5)
  {
    digitalWrite(dosifBase, HIGH);
    delay(1000);
    digitalWrite(dosifBase, LOW);
    delay(500);
  
  }
  }
}
void checkLight(){
  //si el sensor tiene la minima cantidad de luz, 
  //SI1145.ReadVisible() < 280 && 
  int light = SI1145.ReadIR();
  if(ilum == true && light < 340){
    digitalWrite(ilumPin, HIGH);
    initMark = millis();
  }else if(ilum == true && light > 340){
    digitalWrite(ilumPin, LOW);
    endMark = millis();
    if(initMark != 0){
      ledTime += (endMark - initMark);
      endMark = 0;
      initMark = 0;
    }
  }else if(ilum == false){
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

