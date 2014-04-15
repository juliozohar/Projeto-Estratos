/*============================================================
* Modulo   : SondaAleph.ino 
* Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
* Tipo     : Fonte C 
* Data     : 03/04/2014 
* Objetivo : Modulo de controle do logger SondaAleph. Visa 
*            controlar os sensores e registrar os resultados 
*            obtidos em um arquivo de dados. 
*=============================================================
* (C) 2014 Licensed under CERN OHL v.1.2
*=============================================================
* Pin Mapping 
*   D0 - RX 
*   D1 - TX
*   D2 - Push button
*   D3 - Led de atividade (verde) 
*   D4 - 
*   D5 - 
*   D6 - DS18B20
*   D7 - 
*   D8 - 
*   D9 - 
*   D10 - 
*   D11 -
*   D12 -
*   D13 - Led integrado
*
*   A0 - DHT11 - dados 
*   A1 - 
*   A2 - 
*   A3 - 
*   A4 - SPI/SDA - BMP085 e DS1307
*   A5 - SPI/SCL - BMP085 e DS1307
*
*=============================================================*/
#include "SondaAleph.h" 
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <SdFat.h> 
#include <SdFatUtil.h> 
#include <Wire.h> 
#include <DS1307.h> 
#include <DHT.h> 

#include <Barometro.h>
#include <TempoReal.h> 

//Declaracao de variaveis SD Card
Sd2Card card; 
SdVolume volume; 
SdFile root; 
SdFile file; 
char nome[] = "LOG-0412.txt";  //O nome do arquivo deve ter o formato 8.3 

// Declaracao das variaveis DS18B20 
OneWire oneWire(ONE_WIRE_BUS); 
DallasTemperature sensors(&oneWire);
DeviceAddress termometro_externo; 

//Declaracao de variavel do DHT 
DHT dht(DHT_PIN, DHT_TYPE); 

// Variavel do barometro
Barometro br; 

// Variaveis de TempoReal 
TempoReal tr; 
char ts[20]; 

//Variaveis medidas 
float temp_externa, temp_externa_min, temp_externa_max;//DS18B20
float temp_interna, temp_interna_min, temp_interna_max;//DHT
float umidade, umidade_min, umidade_max;               //DHT
short bmp_temp; // Variavel utilitaria, necessaria para//BMP085
long  bmp_pressao, bmp_pressao_min, bmp_pressao_max;   //BMP085
long  bmp_altitude, bmp_altitude_min, bmp_altitude_max;//BMP085
float bmp_tempy, bmp_tempy_min, bmp_tempy_max;         //BMP085 

int seql = 0; // Numero sequencial dos registros 
const byte ledAtividadePin = 13; 

void setup(){
  Serial.begin(9600); 
  Serial.flush(); 
  Serial.println("Inicio do setup"); 
  analogReference(INTERNAL); 
  pinMode(ledAtividadePin, OUTPUT); 
  
  //Inicializa variaveis de temperatura
  temp_externa, temp_externa_max, temp_interna, temp_interna_max = 0; 
  temp_externa_min, temp_interna_min = 100; 
  umidade, umidade_max = 0; 
  umidade_min = 200; 
  
  //Inicializa variaveis de pressao e do BMP085 
  bmp_pressao, bmp_pressao_max = 0; 
  bmp_pressao_min = PRESSAO_MAR + 10000; 
  bmp_altitude, bmp_altitude_max = 0; 
  bmp_altitude_min = 80000; 
  
  Wire.begin(); 
  delay(1000); 
  // Inicializa BMP085 
  init_bmp085(); 
  
  //Inicializa DS18B20
  init_ds18b20(); 
  
  //Inicializa DHT
  init_dht(); 

  //Inicializa SDCard
  init_sd(); 

  Serial.println("Fim do setup"); 
}

void loop(){
  Serial.println("Inicializa o loop"); 
  digitalWrite(ledAtividadePin, HIGH); 
  seql++; 
  
    get_pressao_bmp(); 
    get_altitude_bmp(); 
    get_temp_ds(termometro_externo); 
    get_temp_dht(); 
    get_umid_dht(); 
    get_timestamp(); 
    grava_registro(); 
  
  digitalWrite(ledAtividadePin, LOW); 
  Serial.println("Finaliza o loop"); 
  delay(2000); 
}

/*****************************************************
*             R O T I N A S   L O C A I S     
******************************************************/ 

void init_sd(){
  /* 
  * Rotina de inicializacao do cartao SD. 
  * Inicializa o cartao em SPI_HALF_SPEED, para evitar erros de 
  * barramento com as protoboards. Utilize SIP_FULL_SPEED para 
  * um melhor desempenho, caso o cartao suporte esta opcao. 
  */
  if(!card.init(SPI_HALF_SPEED)) Serial.println(ERR_SD_INIT); 
  
  //Inicializa volume fat 
  if(!volume.init(&card)) Serial.println(ERR_SD_VOL); 
  
  //Abre o diretorio raiz 
  if(!root.openRoot(&volume)) Serial.println(ERR_SD_ROOT); 
}

/*
* Rotina de inicializacao do sensor BMP085. 
*/
void init_bmp085(){
  br.bmp085Calibration();
}

/*
* Rotina de inicializacao do sensor DS18B20. 
*/
void init_ds18b20(){
  sensors.begin(); 
  if(!sensors.getAddress(termometro_externo, 0)); 
    Serial.println(DS_MSG_N_LOC); 
    
  Serial.println(DS_MSG_LOC); 
  imprime_endereco(termometro_externo); 
}

/*
* Rotina de inicializacao do sensor DHT 
*/
void init_dht(){
  dht.begin(); 
}

/*
* Grava registro de sensores no cartao SD. 
*/
void grava_registro(){
  file.open(&root, nome, O_CREAT | O_APPEND | O_WRITE); 
  file.timestamp(7, 2014, 04, 11, 18, 45, 00);  
  writeNumber(file, seql); 
  writeString(file, "|");
  writeString(file, tr.ts); 
  writeString(file, "|"); 
  writeNumber(file, temp_externa); 
  writeString(file, "|"); 
  writeNumber(file, temp_interna); 
  writeString(file, "|"); 
  writeNumber(file, umidade); 
  writeString(file, "|"); 
  writeNumber(file, bmp_pressao); 
  writeString(file, "|"); 
  writeNumber(file, bmp_altitude); 
  writeString(file, "|"); 
  
}

/*****************************************************
*             R O T I N A S   B M P 0 8 5  
******************************************************/ 
/*
*  Recupera a pressao barometrica com o sensor BMP085
*/
void get_pressao_bmp(){
  bmp_temp    = br.bmp085GetTemperature(br.bmp085ReadUT()); 
  bmp_pressao = br.bmp085GetPressure(br.bmp085ReadUP()); 
  
  if(bmp_pressao > bmp_pressao_max)
    bmp_pressao_max = bmp_pressao; 
    
  if(bmp_pressao < bmp_pressao_min) 
    bmp_pressao_min = bmp_pressao;
    
  if(bmp_temp > bmp_tempy_max) 
    bmp_tempy_max = bmp_temp; 
  
  if(bmp_temp < bmp_tempy_min) 
    bmp_tempy_min = bmp_temp; 
}

/*
* Recupera a altitude do modulo em funcao da pressao barometrica. 
*/
void get_altitude_bmp(){
  bmp_altitude = (float)44330 * (1 -pow(((float) bmp_pressao/PRESSAO_MAR), 0.190295));
  if(bmp_altitude > bmp_altitude_max) 
    bmp_altitude_max = bmp_altitude;   
}


/*****************************************************
*             R O T I N A S   S D - C A R D      
******************************************************/ 

/*
* Escreve a sequencia CRLF no arquivo. 
*/
void writeCRLF(SdFile& f){
  f.write((uint8_t*)"\r\n", 2);
}

/*
* Escreve um numero no arquivo. 
*/
void writeNumber(SdFile& f, uint32_t n){
  uint8_t idx = n; 
  uint8_t buf[10]; 
  uint8_t i = 0; 
  do{
    i++; 
    buf[sizeof(buf) -i] = n%10 + '0'; 
    n /= 10;
  }while (n); 
  
  if(idx < 10)
    f.write("0"); 
  f.write(&buf[sizeof(buf) -1], i); 
}

/*
* Escreve uma string no arquivo. 
*/
void writeString(SdFile& f, char *str){
  f.write(str);    
}

/*****************************************************
*             R O T I N A S   D S 1 8 B 2 0      
******************************************************/ 

/*
* Imprime o endereco de um dispositivo OneWire. 
*/
void imprime_endereco(DeviceAddress devAdd){
  for(int i=0; i < 8; i++){
    if(devAdd[i] < 16) Serial.print("0"); 
    Serial.println(devAdd[i], HEX); 
  }
}

/*
* Recupera a temperatura externa. 
*/
float get_temp_ds(DeviceAddress devAdd){
  temp_externa = sensors.getTempC(devAdd); 
  if(temp_externa < temp_externa_min) 
    temp_externa_min = temp_externa; 
    
    if(temp_externa > temp_externa_max)
      temp_externa_max = temp_externa; 
}

/*****************************************************
*             R O T I N A S   D H T       
******************************************************/ 
/*
* Leitura do parametro de temperatura a partir do sensor DHT. 
*/
float get_temp_dht(){
  float t = dht.readTemperature(); 
  
  if(isnan(t)){
    Serial.println(ERR_DHT_READ);
    t = -99999;
  }
  return t;
} 

/*
* Leitura do parametro de umidade a partir do sensor DHT. 
*/
float get_umid_dht(){
  float h = dht.readHumidity(); 
  
  if(isnan(h)){
    Serial.println(ERR_DHT_READ); 
    h = -99999;
  }
  return h; 
}

/*****************************************************
*             R O T I N A S   D S 1 3 0 7    
******************************************************/ 

/*
* Recupera a data e hora do relogio de tempo real. Retorna no formato 
* timestamp (AAAA-MM-DD-hh-mm-ssssss), com 19 posicoes.
*/ 
void get_timestamp(){
  tr.getTimestamp(); 
}

/*
* Acerta a hora do relogio de tempo real. 
*/
void acerta_relogio(int ano, int mes, int dia, int hora, 
                    int minuto, int segundo, int dia_semana){
  tr.para(); 
  tr.acertaDataHora(ano, mes, dia, hora, minuto, segundo, dia_semana); 
  tr.inicia(); 
}

/*****************************************************************************************
*                 F   I   M       D   E       P   R   O   G   R   A   M   A   
*****************************************************************************************/
