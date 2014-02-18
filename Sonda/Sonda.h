/*===============================================================
 *  Modulo   : Sonda.h
 *  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
 *  Tipo     : Arquivo de header 
 *  Data     : 03/04/2014
 *  Objetivo : Modulo de controle da Sonda Estratos. Controlar os
 *             sensores e registrar os resultados obtidos em um 
 *             arquivo de dados. Arquivo de cabecalho, com defi-
 *             nicoes utilizadas no programa.  
 *===============================================================
 *  (C) 2014 Licensed under CERN OHL v.1.2
 *===============================================================
 */
#ifndef __CONTROLE_H__ 
#define __CONTROLE_H__ 
#include <stdio.h> 
 
#define ONE_WIRE_BUS 3 
#define BMP085_ADDRESS 0x77  // I2C address do BMP085

#define DHT_TYPE DHT11 
#define FATOR_MULT 0.09765625
 
 // Mensagens de Erro 
#define ERR            "Erro: " 
#define ERR_SD         "Erro no SDCard: "
#define ERR_DHT_READ   "Falha na leitura do DHT"
#define ERR_CRC_INV    "CRC nao e valido"
#define ERR_DISP_N_REC "Dispositivo nao reconhecido"
#define ERR_SD_INIT    "card.init() falhou"
#define ERR_SD_VOL     "volume.init() falhou()"
#define ERR_SD_ROOT    "openRoot falhou"

//Mensagens ao usuario 
#define TMP            "Temperatura "
#define UMD            "Umidade "
#define MSG_DADO_WRT   "Dados escritos"
#define MSG_ARQ_ABRE   "Arquivo aberto"
#define MSG_ARQ_FECHA  "Arquivo fechado"
#define MSG_SD_INIT    "Inicializando SDCard..."
#define MSG_PGM_INIT   "Inicializando em 5 segundos....."

// Variaveis utilizadas pelos marinheiros. 
float PRESSAO_MAR = 1013.25; // Pressao ao nivel do mar, equivalente a 1013.25 hPa
float K_COEF_PRESSAO = 44330;

const unsigned char OSS = 0;  // Oversampling Setting

//Valores de calibracao do sensor BMP085. 
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

// Assinatura de metodos a implementar. 
float get_temp_ds(); 

float get_temp_dht(); 

float get_umidade(); 

float get_pressao(); 

float get_altitude(); 

float calcula_altitude(); 

void bmp085Calibration(); 

#endif

