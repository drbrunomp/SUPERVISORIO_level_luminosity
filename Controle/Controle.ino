#include <LiquidCrystal.h>
#include<stdio.h>
#include "SimpleModbusSlave.h"

enum 
{     
  ILUMLDR,
  VDIGA0,
  NIVELTANQUE,
  VALV1,
  VALV2,
  PUMP,
  LED_VERM,
  LED_AMAR,
  LED_VERD,
  HOLDING_REGS_SIZE
 };

unsigned int holdingRegs[HOLDING_REGS_SIZE]; 
unsigned int vdigA0,
             vdigA1;

float tensaoA0,
      ilumldr;

const int led_verm = 12,
          led_amar = 11,
          led_verd = 10,
          valv1 = 9,
          valv2 = 8,
          pump = 7,
          rs = 6,
          en = 5,
          d4 = 4,
          d5 = 3,
          d6 = 2,
          d7 = 13;

int ledverm = 0,
    ledamar = 0,
    ledverd = 0,
    v1 = 0,
    v2 = 0,
    pm = 0,
    nivelTanque;


LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

void setup()
{
  modbus_configure(&Serial, 9600, SERIAL_8N2, 1, 2,HOLDING_REGS_SIZE, holdingRegs);
  modbus_update_comms(9600, SERIAL_8N1, 1); 
  lcd.begin(16, 2);
  
  pinMode(led_verm, OUTPUT);
  pinMode(led_amar, OUTPUT);
  pinMode(led_verd, OUTPUT);
  pinMode(valv1, OUTPUT);
  pinMode(valv2, OUTPUT);
  pinMode(pump, OUTPUT);
}

void controltank()
{
  vdigA0 = analogRead(A0);
  tensaoA0 = vdigA0 * 0.00488759;
  nivelTanque = tensaoA0/0.005;
  

  lcd.setCursor(0, 0);        
  lcd.print("FETEC - 2024");
  lcd.setCursor(0, 1);        
  lcd.print("TANQUE:");
  lcd.setCursor(8, 1);        
  lcd.print(nivelTanque);
  lcd.print("L   ");

  if(nivelTanque <= (0.25*1000))
  {
    digitalWrite(valv1, HIGH);
    digitalWrite(valv2, LOW);
    digitalWrite(pump, HIGH);
    v1 = 1;
    v2 = 0;
    pm = 1;
  }
  else if(nivelTanque > (0.25*100) && nivelTanque <= (0.50*1000))
  {
    digitalWrite(valv1, HIGH);
    digitalWrite(valv2, HIGH);
    digitalWrite(pump, HIGH);
    v1 = 1;
    v2 = 1;
    pm = 1;
  }
  else if(nivelTanque > (0.50*100) && nivelTanque <= (0.75*1000))
  {
    digitalWrite(valv1, HIGH);
    digitalWrite(valv2, HIGH);
    digitalWrite(pump, LOW);
    v1 = 1;
    v2 = 1;
    pm = 0;
  }
   else if(nivelTanque > (0.75*1000))
  {
    digitalWrite(valv1, LOW);
    digitalWrite(valv2, HIGH);
    digitalWrite(pump, LOW);
    v1 = 0;
    v2 = 1;
    pm = 0;
  }
}

void readLDR()
{
  vdigA1 = analogRead(A1);
  ilumldr = map(vdigA1, 1023, 0, 0, 100); 
  if(ilumldr <= 33)
  {    
    digitalWrite(led_verm, HIGH);
    digitalWrite(led_amar, LOW);
    digitalWrite(led_verd, LOW);
    ledverm = 1;
    ledamar = 0;
    ledverd = 0; 
  }
  else if(ilumldr > 33 && ilumldr <= 66)
  {    
    digitalWrite(led_verm, LOW);
    digitalWrite(led_amar, HIGH);
    digitalWrite(led_verd, LOW);
    ledverm = 0;
    ledamar = 1;
    ledverd = 0; 
  }
  else if (ilumldr > 66)
  {    
    digitalWrite(led_verm, LOW);
    digitalWrite(led_amar, LOW);
    digitalWrite(led_verd, HIGH);
    ledverm = 0;
    ledamar = 0;
    ledverd = 1;     
  }
}


void transmiter()
{
  holdingRegs[ILUMLDR] =  ilumldr;
  holdingRegs[VDIGA0] = vdigA0;
  holdingRegs[NIVELTANQUE] = nivelTanque;
  holdingRegs[VALV1] = v1;
  holdingRegs[VALV2] = v2;
  holdingRegs[PUMP] = pm;
  holdingRegs[LED_VERM] = ledverm;
  holdingRegs[LED_AMAR] = ledamar;
  holdingRegs[LED_VERD] = ledverd;
}

void loop()
{
  modbus_update();
  readLDR();
  controltank();
  transmiter(); 
}
