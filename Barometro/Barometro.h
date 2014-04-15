/*===============================================================
*  Modulo   : Barometro.cpp
*  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
*  Tipo     : Biblioteca Arduino - Cabeçalho h
*  Data     : 09/04/2014
*  Objetivo : Biblioteca de utilização do sensor BMP085, 
*             baseada em implementação de Jim Lindblom, da 
*             Sparkfun Electronics, licenciada sob a Creative 
*             Commons CC BY-SA v3.0
*===============================================================
*  (C) 2014 Licensed under CERN OHL v.1.2
*===============================================================
*/
#ifndef Barometro_h
#define Barometro_h 

#include "Arduino.h" 



class Barometro{

  public: 
    
    Barometro(); 
    void  bmp085Calibration(); 
    short bmp085GetTemperature(unsigned int ut); 
    long  bmp085GetPressure(unsigned long up); 
    char  bmp085Read(unsigned char address); 
    int   bmp085ReadInt(unsigned char address); 
    unsigned int  bmp085ReadUT(); 
    unsigned long bmp085ReadUP(); 
};

#endif
