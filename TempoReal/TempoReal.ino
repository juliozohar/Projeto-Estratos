/*===============================================================
 *  Modulo   : TempoReal.cpp
 *  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
 *  Tipo     : Fonte C++
 *  Data     : 07/04/2014
 *  Objetivo : Biblioteca de utilização de Real Time Clock (RTC), 
 *             baseada em implementação no chip DS1307. 
 *===============================================================
 *  (C) 2014 Licensed under CERN OHL v.1.2
 *===============================================================
 */
#include "TempoReal.h"
#ifndef __DS1307_h__
   #include <Wire.h>  
   #include <DS1307.h> 
#endif

int dia, mes, ano; 
int hora, minuto, segundo;

//TIMESTAMP ts; 
 /*
  * Construtor da classe. 
  */
 TempoReal::TempoReal(){}

/*
 * Inicializa a execucao do relogio de tempo real. 
 */
void TempoReal::inicia(){
  RTC.start();
}
 
/*
 * Para a execucao do relogio de tempo real. 
 */ 
void TempoReal::para(){
  RTC.stop();  
}
 
/*
 * Atualiza o registro de data e hora do RTC. A atribuicao do RTC se mantem 
 * efeiva enquanto houver alimentacao no circuito, seja pela linha de alimen- 
 * tacao principal, seja pela bateria de backup ligada diretamente ao terminal 
 * do circuito. Recebe as variaveis de tempo isoladamente, conforme assinatura
 * da funcao. 
 */
void TempoReal::acertaDataHora(int ano, int mes, int dia, 
        int hora, int minuto, int segundo, int dia_semana){
  RTC.set(DS1307_SEC  , segundo ); 
  RTC.set(DS1307_MIN  , minuto ); 
  RTC.set(DS1307_HR   , hora );
  RTC.set(DS1307_DOW  , dia_semana );
  RTC.set(DS1307_DATE , dia );
  RTC.set(DS1307_MTH  , mes);
  RTC.set(DS1307_YR   , ano);
}

/*
 * Recupera o timestamp atual e o devolve formatado, no seguinte gabarito: 
 * 'yyyy-mm-dd-hh.mm.ss'. Este formato e compativel com o tipo de dados 
 * TIMESTAMP do DB2, podendo ser movido diretamente para registro em banco de 
 * dados.
 * TODO : O registrador do ano esta sendo devolvido, consistentemente, como 
 * ano 2000. Nao consegui localizar a fonte do problema, e para resolver tem-
 * porariamente a inconsistencia, adiciono +14 a variavel de ano. Procurar 
 * solucao na documentacao do DS1307 para resolver este problema. 
 */ 
void TempoReal::leRTC(){
  hora    = RTC.get(DS1307_HR, true); 
  minuto  = RTC.get(DS1307_MIN, false);
  segundo = RTC.get(DS1307_SEC, false);
  dia     = RTC.get(DS1307_DATE, false);
  mes     = RTC.get(DS1307_MTH, false);
  ano     = RTC.get(DS1307_YR, false);
  ano    += 14;
}

void TempoReal::getTimestamp(){
  leRTC();
  sprintf(ts, "%4d-%02d-%02d-%02d.%02d.%02d", ano, mes, dia, hora, minuto, segundo);   
}

/*
 * Exibe o timestamp atual do RTC. Caso ele ainda nao tenha sido acertado, 
 * retorna uma string default, da data mais antiga suportada pelo chip. 
 */ 
void TempoReal::displayDataHora(){
  Serial.print("Timestamp lido: "); 
  Serial.println(ts); 
} 


