/*============================================================
* Modulo   : Sonda.ino 
* Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
* Tipo     : Fonte C 
* Data     : 03/04/2014 
* Objetivo : Modulo de controle do logger Sonda Aleph. Visa 
*            controlar os sensores e registrar os resultados 
*            obtidos em um arquivo de dados. 
*=============================================================
* (C) 2014 Licensed under CERN OHL v.1.2
*=============================================================*/ 

#define ERR_SD         " Erro no SDCard "
#define ERR_SD_INIT    " card.init falhou "
#define ERR_SD_VOL     " volume.init falhou "
#define ERR_SD_ROOT    " openRoot falhou " 
#define MSG_SD_INIT    " Inicializando SDCard " 
#define MSG_ARQ_ABRE   " Abrindo o arquivo "

#define DS_MSG_N_LOC   " Nao foi possivel localizar o sensor "
#define DS_MSG_LOC     " Sensor localizado " 

#define ERR_DHT_READ   "Nao foi possivel ler o sensor DHT"


/*===================================================
*    D E F I N I C O E S   A P I   D S 1 8 B 2 0 
*====================================================*/

#define ONE_WIRE_BUS    6  

/*===================================================
*    D E F I N I C O E S   A P I    B M P 0 8 5 
*====================================================*/
const float PRESSAO_MAR  = 101325; 
float K_COEF_PRESSAO     =  44330; 

/*===================================================
*    D E F I N I C O E S   A P I    D H T 
*====================================================*/
#define DHT_PIN     2 
#define DHT_TYPE    DHT22


void init_sd(); 

void init_bmp085(); 

void init_ds18b20(); 

void get_pressao_bmp(); 

void get_altitude_bmp(); 
