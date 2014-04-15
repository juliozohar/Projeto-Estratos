#include <TempoReal.h> 
#include <DS1307.h>
#include <Wire.h> 

TempoReal TempoReal; 

void setup() {
  Serial.begin(9600); 
  Serial.flush();
  Wire.begin();

  TempoReal.para(); 
  TempoReal.acertaDataHora(2014,3,18,12,18,0,5); 
  TempoReal.inicia();   
}

void loop() {
  //Serial.print("shongas : ");
  TempoReal.getTimestamp(); 
  Serial.println(TempoReal.ts);
  delay(1000); 
}
