/*===============================================================
 *  Modulo   : Sonda.ino
 *  Autor    : Julio Cesar Torres (juliozohar@gmail.com) 
 *  Tipo     : Fonte C
 *  Data     : 03/04/2014
 *  Objetivo : Modulo de controle da Sonda Estratos. Controlar os
 *             sensores e registrar os resultados obtidos em um 
 *             arquivo de dados.  
 *===============================================================
 *  (C) 2014 Licensed under CERN OHL v.1.2
 *===============================================================
 *  Pin mapping: 
 *    D2 - Push button
 *    D3 - DHT11 - linha de dados 
 *    D7 - Led de atividade
 *    D8 - Led de inatividade
 */
 #include "Controle.h" 
 #include <OneWire.h> 
 #include <DallasTemperature.h> 
 #include <SdFat.h> 
 #include <SdFatUtil.h> 
 #include <DHT.h> 

 // Declaracao das variaveis de interesse  
 float temp_externa, temp_externa_min, temp_externa_max; 
 float temp_interna, temp_interna_min, temp_interna_max; 
 float umidade, umidade_min, umidade_max; 
 float pressao, pressao_min, pressao_max; 
 float altitude, altitude_max; 
 
 // Pins 
 const byte button_pin     = 2; 
 const byte dht_pin        = 3; 
 const byte led_ativ_pin   = 7; 
 const byte led_n_ativ_pin = 8;  

 // Declaracao das variaveis de temperatura / umidade (DHT11) 
 DHT dht(dht_pin, DHT_TYPE); 

 // Declaracao das variaveis do dispositivo 
 OneWire oneWire(ONE_WIRE_BUS); 
 DallasTemperature sensors(&oneWire); 
 DeviceAddress insideThermometer; 

 // Declaracao das variaveis de tempo (DS1307) 
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
  pinMode(led_ativ_pin,   OUTPUT); 
  pinMode(led_n_ativ_pin, OUTPUT); 
  pinMode(button_pin,     OUTPUT); 
  pinMode(dht_pin,        OUTPUT); 
  
  digitalWrite(led_ativ_pin, LOW); 
  digitalWrite(led_n_ativ_pin, HIGH);   
   
  //Inicializacao das variaveis; 
  temp_externa, temp_externa_max, temp_interna, temp_interna_max = 0; 
  temp_externa_min, temp_interna_min = 100; 
  umidade, umidade_max = 0; 
  umidade_min = 100; 
  pressao, pressao_max = 0; 
  pressao_min = PRESSAO_MAR + 1000; 
   
  /* Definindo o pino analogico como referencia INTERNA e nao
  * padrao, para alcancar maiores valores no conversor ADC. 
  */
  analogReference(INTERNAL); 

  dht.begin(); // Inicializa o dispositivo DHT11
  checa_ds18b20();  // Faz a checagem do dispositivo DS18B20S   
  checa_ds1307();   // Faz a checagem do dispositivo DS1307 
  
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
