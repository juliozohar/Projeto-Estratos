/*===============================================================
*  Modulo   : Barometro.cpp
*  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
*  Tipo     : Biblioteca Arduino - C++
*  Data     : 09/04/2014
*  Objetivo : Biblioteca de utilização do sensor BMP085, 
*             baseada em implementação de Jim Lindblom, da 
*             Sparkfun Electronics, licenciada sob a Creative 
*             Commons CC BY-SA v3.0
*===============================================================
*  (C) 2014 Licensed under CERN OHL v.1.2
*===============================================================
*/
#include <Wire.h> 
#include "Barometro.h"

/* Endereco I2C do BMP085. Para lista de enderecos de dispositivos
*  I2C, consultar site http:// 
*/
#define BMP085_ADDRESS 0x77

const unsigned char OSS = 0; // Oversampling setting

const float p0 = 101325;     // Pressao ao nivel do mar (Pa)

// Calibration values
int ac1;
int ac2; 
int ac3; 
unsigned int ac4;
unsigned int ac5;
unsigned int ac6;
int b1; 
int b2;
int mb;
int mc;
int md;
long b5; 

short temperature;
long pressure;

// Usado para conversao de altitude 
float altitude;

/*
* Construtor da classe. 
*/
 Barometro::Barometro(){}
 
/*
* Armazena todos os valores de calibracao do BMP085 em variaveis 
* globais. Os valores sao necessarios para calcular temperatura e 
* pressao. Esta funcao deve ser chamada no comeco do programa. 
*/
void Barometro::bmp085Calibration(){
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1 = bmp085ReadInt(0xB6);
  b2 = bmp085ReadInt(0xB8);
  mb = bmp085ReadInt(0xBA);
  mc = bmp085ReadInt(0xBC);
  md = bmp085ReadInt(0xBE);
}

/*
* Calcula a temperatura, dado um ut. Os valores retornados estao 
* em unidades de 0.1 graus centigrados. 
*/
short Barometro::bmp085GetTemperature(unsigned int ut){
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

/*
* Calcula a pressao dada uma up. Os valores de calibracao devem 
* ja estar definidos. b5 e tambem necessario para que o metodo 
* bmp085GetTemperature possa ser chamado. Os valores retornados 
* estarao em unidades de pressao Pascal (Pa). 
*/
long Barometro::bmp085GetPressure(unsigned long up){
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calculate B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calculate B4
  x1 = (ac3 * b6)>>13;
  x2 = (b1 * ((b6 * b6)>>12))>>16;
  x3 = ((x1 + x2) + 2)>>2;
  b4 = (ac4 * (unsigned long)(x3 + 32768))>>15;
  
  b7 = ((unsigned long)(up - b3) * (50000>>OSS));
  if (b7 < 0x80000000)
    p = (b7<<1)/b4;
  else
    p = (b7/b4)<<1;
    
  x1 = (p>>8) * (p>>8);
  x1 = (x1 * 3038)>>16;
  x2 = (-7357 * p)>>16;
  p += (x1 + x2 + 3791)>>4;
  
  return p;
}

/*
* Le 1 byte do BMP085 no 'endereco'. 
*/
char Barometro::bmp085Read(unsigned char address){
  unsigned char data;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;
    
  return Wire.read();
}

/*
* Le 2 bytes do BMP085. O primeiro byte e o endereco, o segundo 
* byte vira de 'endereco+1'. 
*/
int Barometro::bmp085ReadInt(unsigned char address){
  unsigned char msb, lsb;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 2);
  while(Wire.available()<2)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  
  return (int) msb<<8 | lsb;
}

/*
* Le o valor nao compensado de temperatura. 
*/
unsigned int Barometro::bmp085ReadUT(){
  unsigned int ut;
  
  // Escreve 0x2Eno registrador 0xF4
  // Solicita a leitura da temperatura 
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Espere no minimo 4.5ms
  delay(5);
  
  // Le dois bytes dos registradores 0xF6 e 0xF7.
  ut = bmp085ReadInt(0xF6);
  return ut;
}

/*
* Le o valor nao compensado de pressao.
*/
unsigned long Barometro::bmp085ReadUP(){
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Escreve 0x34+(OSS<<6) no registrador 0xF4
  // Solicita a leitura da pressao com o oversampling setting. 
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Espera pela conversao, o delay e dependente do OSS. 
  delay(2 + (3<<OSS));
  
  // Le o registrador 0xF6 (MSB), 0xF7 (LSB) e 0xF8 (XLSB)
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Aguarda o dado ficar disponivel
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}

