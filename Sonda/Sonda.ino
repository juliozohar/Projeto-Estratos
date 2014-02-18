/*============================================================================
 *  Modulo   : Sonda.ino
 *  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
 *  Tipo     : Fonte C
 *  Data     : 03/04/2014
 *  Objetivo : Modulo de controle da Sonda Estratos. Controlar os
 *             sensores e registrar os resultados obtidos em um 
 *             arquivo de dados.  
 *============================================================================
 *  (C) 2014 Licensed under CERN OHL v.1.2
 *============================================================================ 
 *  BMP085 Extended Example Code, by Jim Lindblom, SparkFun Electronics
 *  date: 1/18/11 updated: 2/26/13
 *  license CC BY-SA v3.0 - http://creativecommons.org/licenses/by-sa/3.0/
 *============================================================================
 *  Pin mapping: 
 *    D0 - RX 
 *    D1 - TX
 *    D2 - Push button
 *    D3 - Led de atividade (verde)
 *    D4 - Led de inatividade (vermelho) 
 *    D5 - DHT11 - linha de dados 
 *    D13 - Led arduino

 */
#include "Sonda.h" 
#include <TempoReal.h>
#include <OneWire.h> 
#include <DallasTemperature.h> 
#include <SdFat.h> 
#include <SdFatUtil.h> 
#include <DHT.h> 
#include <DS1307.h>
#include <Wire.h> 

 // Declaracao das variaveis de interesse  
 float temp_externa, temp_externa_min, temp_externa_max; 
 float temp_interna, temp_interna_min, temp_interna_max; 
 float umidade, umidade_min, umidade_max; 
 float pressao, pressao_min, pressao_max, temp_pressao; 
 float altitude, altitude_max;  
 short bmp_teperature; 
 long  bmp_pressao; 
 float bmp_altitude;
 
 // Pins 
 const byte button_pin     = 2; 
 const byte led_ativ_pin   = 3; 
 const byte led_n_ativ_pin = 4;  
 const byte dht_pin        = 5; 

 // Declaracao das variaveis de temperatura / umidade (DHT11) 
 DHT dht(dht_pin, DHT_TYPE); 

 // Declaracao das variaveis do dispositivo (DS18B20)
 OneWire oneWire(ONE_WIRE_BUS); 
 DallasTemperature sensors(&oneWire); 
 DeviceAddress insideThermometer; 

 // Declaracao das variaveis de tempo (DS1307) 
 TempoReal tr = TempoReal(0); 
 int hora, minuto, segundo, dia, mes, ano; 

 // Declaracao das variaveis do SDCard (SdFat) 
 char nome_arq[] = ""; 
 Sd2Card card; 
 SdVolume volume; 
 SdFile root;
 SdFile file; 
  
 // Flags 
 int buttonState = 0; 
 int estado = 0;

void setup() {
  Serial.begin(9600); 
  Serial.flush(); 

  // Inicializa pins  
  pinMode(button_pin,     OUTPUT); 
  pinMode(led_ativ_pin,   OUTPUT); 
  pinMode(led_n_ativ_pin, OUTPUT); 
  pinMode(dht_pin,        OUTPUT); 
  
  digitalWrite(led_ativ_pin, LOW); 
  digitalWrite(led_n_ativ_pin, HIGH);   
   
  //Inicializacao das variaveis; 
  temp_externa, temp_externa_max, temp_interna, temp_interna_max = 0; 
  temp_externa_min, temp_interna_min = 100; 
  umidade, umidade_max = 0; 
  umidade_min = 100; 
  pressao, pressao_max, temp_pressao = 0; 
  pressao_min = PRESSAO_MAR + 1000; 
   
  /* Definindo o pino analogico como referencia INTERNA e nao
  * padrao, para alcancar maiores valores no conversor ADC. 
  */
  analogReference(INTERNAL); 

  dht.begin();      // Inicializa o dispositivo DHT11
  checa_ds18b20();  // Faz a checagem do dispositivo DS18B20S   
  tr.inicia();      // Inicializa o dispositivo DS1307 
  checa_ds1307();   // Faz a checagem do dispositivo DS1307 
  Wire.begin(); 
  bmp085Calibration();
  // Tempo final para as inicializacoes terem efeito. 
  delay(5000); 
}

void loop() {
  buttonState = digitalRead(button_pin); 
  if (buttonState ==HIGH){
    (estado==0)? estado=1 : estado=0;
  }
  if(estado){
    digitalWrite(led_n_ativ_pin, LOW); 
    digitalWrite(led_ativ_pin, HIGH); 
    get_temp_ds(insideThermometer); 
    get_temp_dht(); 
    get_umidade(); 
    get_pressao(); 
    get_altitude();
    grava_registro(); 
    delay(500);  
  }else{
    //Não funcionando
    digitalWrite(led_ativ_pin, LOW);
    digitalWrite(led_n_ativ_pin, HIGH); 
  }
}

/*
* Inicializa o sensor DS18B20. 
*/
void checa_ds18b20(){
  if(!sensors.getAddress(insideThermometer, 0)); 
  Serial.println("Nao foi possivel localizar o sensor");  
  Serial.print("Sensor localizado: "); 
  print_address(insideThermometer);
}

void checa_ds1307(){
  
}

/*
* Imprime o endereco do dispositivo OneWire
*/
void print_address(DeviceAddress devAdd){
  for(int i=0; i < 8; i++){
    if(devAdd[i] < 16) Serial.print("0"); 
    Serial.print(devAdd[i], HEX); 
  }
}

/*
 * Recupera a temperatura externa com o sensor DS18B20. 
 */
float get_temp_ds(DeviceAddress devAdd){
  temp_externa = sensors.getTempC(devAdd);
    if(temp_externa < temp_externa_min)    // Verifica temp minima 
      temp_externa_min = temp_externa; 
     
    if(temp_externa > temp_externa_max)    // Verifica temp maxima
      temp_externa_max = temp_externa; 
}
 
/*
 * Recupera a temperatura interna com o sensor DHT11 
 */
float get_temp_dht(){
  float temp_interna = dht.readTemperature(); 
  if (isnan(temp_interna)){
    Serial.println(ERR_DHT_READ); 
    return -9999;
  }
  if(temp_interna < temp_interna_min)
    temp_interna_min = temp_interna; 
    
  if(temp_interna > temp_interna_max)
    temp_interna_max = temp_interna;
}

/*
 * Recupera a umidade com o sensor DHT11 
 */
float get_umidade(){
  float umidade = dht.readHumidity(); 
  if(isnan(umidade)){
    Serial.print(ERR_DHT_READ); 
    return -9999; 
  }
  if(umidade < umidade_min) 
    umidade_min = umidade; 
    
  if(umidade > umidade_max)
    umidade_max = umidade; 
}


/*********************
 * Rotinas SD FatLib
 *********************/
void error_P(const char* str){
  PgmPrint(ERR); 
  SerialPrintln_P(str); 
  if(card.errorCode()){
    PgmPrint(ERR_SD); 
      Serial.print(card.errorCode(), HEX); 
      Serial.print(','); 
      Serial.print(card.errorData(), HEX); 
  } while (1); 
}

void writeNumber(SdFile& f, uint32_t n){
  uint8_t idx = n;
  uint8_t buf[10]; 
  uint8_t i = 0;
  
  do{
    i++; 
    buf[sizeof(buf -1)] = n%10 + '0';  
    n /= 10; 
  }while(n); 
  
  if(idx < 10)   // Testa a necessidade de incluir leading zero 
    f.write("0");   
  f.write(&buf[sizeof(buf) -i] , i); 
} 

/*
 * Escreve sequencia CR/LF no arquivo. 
 */
void writeCRLF(SdFile& f){
  f.write((uint8_t*) "\r\n", 2); 
}


//************************************

/******************
 * Rotinas BMP085
 ******************/
/*
 * Armazena todos dos valores de calibracao do BMP085 em variaveis
 * globais. Os valores de calibracao sao necessarios para calcular 
 * temperatura e pressao. Esta funcao deve ser chamada pela inicia-
 * lizacao do programa. 
 */
void bmp085Calibration(){
  ac1 = bmp085ReadInt(0xAA);
  ac2 = bmp085ReadInt(0xAC);
  ac3 = bmp085ReadInt(0xAE);
  ac4 = bmp085ReadInt(0xB0);
  ac5 = bmp085ReadInt(0xB2);
  ac6 = bmp085ReadInt(0xB4);
  b1  = bmp085ReadInt(0xB6);
  b2  = bmp085ReadInt(0xB8);
  mb  = bmp085ReadInt(0xBA);
  mc  = bmp085ReadInt(0xBC);
  md  = bmp085ReadInt(0xBE);
}

// Calcula a temperatura, dado um ut.
// Valor retornado sera em unidades de 0.1 graus C
short bmp085GetTemperature(unsigned int ut)
{
  long x1, x2;
  
  x1 = (((long)ut - (long)ac6)*(long)ac5) >> 15;
  x2 = ((long)mc << 11)/(x1 + md);
  b5 = x1 + x2;

  return ((b5 + 8)>>4);  
}

// Calcula a pressao dado um valor de pressao nao calibrado (up). 
// Os valores de calibracao devem estar disponiveis. 
// b5 tambem e necessario, dai a necessidade de se chamar primeiro 
// bmp085GetTemperature. Valor retornado da pressao em Pascal (Pa).
long bmp085GetPressure(unsigned long up)
{
  long x1, x2, x3, b3, b6, p;
  unsigned long b4, b7;
  
  b6 = b5 - 4000;
  // Calcula B3
  x1 = (b2 * (b6 * b6)>>12)>>11;
  x2 = (ac2 * b6)>>11;
  x3 = x1 + x2;
  b3 = (((((long)ac1)*4 + x3)<<OSS) + 2)>>2;
  
  // Calcula B4
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

//Le 1 byte do BMP085 no 'address'
char bmp085Read(unsigned char address)
{
  unsigned char data;
  
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(address);
  Wire.endTransmission();
  
  Wire.requestFrom(BMP085_ADDRESS, 1);
  while(!Wire.available())
    ;
    
  return Wire.read();
}

// Le os dois primeiros bytes do BMP085
// Primeiro byte sera o endereco
// Segundo byte sera o endereco + 1
int bmp085ReadInt(unsigned char address)
{
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

// Le o valor da temperatura nao compensado
unsigned int bmp085ReadUT()
{
  unsigned int ut;
  
  // Escreve 0x2E no registro 0xF4
  // Pede a leitura da temperatura
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x2E);
  Wire.endTransmission();
  
  // Espera pela latencia do sensor, ao menos 4.5ms
  delay(5);
  
  // Le dois bytes dos registradores 0xF6 e 0xF7
  ut = bmp085ReadInt(0xF6);
  return ut;
}

// Le o valor da pressao nao compnsado
unsigned long bmp085ReadUP()
{
  unsigned char msb, lsb, xlsb;
  unsigned long up = 0;
  
  // Escreve 0x34+(OSS<<6) no registro 0xF4
  // Le a pressao com oversamplng
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF4);
  Wire.write(0x34 + (OSS<<6));
  Wire.endTransmission();
  
  // Espera pela conversao, o delay dependendo do OSS
  delay(2 + (3<<OSS));
  
  // Le o registro 0xF6 (MSB), oxF7 (LSB) e 0xF8 (XLSB).
  Wire.beginTransmission(BMP085_ADDRESS);
  Wire.write(0xF6);
  Wire.endTransmission();
  Wire.requestFrom(BMP085_ADDRESS, 3);
  
  // Espera pela disponibilizacao dos dados
  while(Wire.available() < 3)
    ;
  msb = Wire.read();
  lsb = Wire.read();
  xlsb = Wire.read();
  
  up = (((unsigned long) msb << 16) | ((unsigned long) lsb << 8) | (unsigned long) xlsb) >> (8-OSS);
  
  return up;
}


//************************************

/*
 * Recupera a pressao barometrica com o sensor BMP085 
 */
float get_pressao(){
  Serial.println("getPressao");
}
 
/*
 * Recupera a altitude do modulo em funcao da pressao barometrica
 */
float get_altitude(){
  calcula_altitude(); 
}
 
/*
 * Calcula a altitude em funcao da pressao barometrica
 */
float calcula_altitude(){
  Serial.println("calculaAltitude");
  //altitude = K_COEF_PRESSAO * (1-(pressao - PRESSAO_MAR)^(1/5255)); 
}
 
void grava_registro(){
     
}
