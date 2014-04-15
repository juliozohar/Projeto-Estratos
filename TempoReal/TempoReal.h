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
 
 #ifndef TempoReal_h
 #define TempoReal_h
 
 #include "Arduino.h"
 
 class TempoReal{
   
   public: 
   
     TempoReal(); 
     char ts[20];
     void inicia(); 
     void para(); 
     void acertaDataHora(int ano, int mes, int dia, 
        int hora, int minuto, int segundo, int dia_semana); 
     void leRTC();
     void getTimestamp(); 
     void displayDataHora();
 };
 
 typedef struct ts {
   int ts_ano; 
   int ts_mes; 
   int ts_dia; 
   int ts_hora; 
   int ts_minuto; 
   int ts_segundo; 
 } TIMESTAMP; 

 #endif
