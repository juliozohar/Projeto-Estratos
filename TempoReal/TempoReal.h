/*===============================================================
 *  Modulo   : TempoReal.h
 *  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
 *  Tipo     : Arquivo de header 
 *  Data     : 07/04/2014
 *  Objetivo : Biblioteca de utilização de Real Time Clock (RTC), 
 *             baseada em implementação no chip DS1307. Arquivo de 
 *             cabecalho, com definicoes utilizadas no programa.  
 *===============================================================
 *  (C) 2014 Licensed under CERN OHL v.1.2
 *===============================================================
 */
 
 #ifndef __TempoReal_H__
 #define __TempoReal_H__
 
 #include "Arduino.h"
 
 class TempoReal{
   public: 
     TempoReal(int pin); 
     void inicia(); 
     void para(); 
     void acertaDataHora(int ano, int mes, int dia, 
        int hora, int minuto, int segundo, int dia_semana); 
     void displayDataHora(); 
     char* getTimestamp(); 

   private: 
     int pino;
 };
 #endif 
