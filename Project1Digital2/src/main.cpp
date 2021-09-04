//*****************************************************************************************
//Universidad del Valle de Guatemala
//BE3015: Electrónica Digital 2
//Jimena Sánchez
//Proyecto 1 Digital

//*****************************************************************************************

//*****************************************************************************************
// Librerías
//*****************************************************************************************
#include <Arduino.h>
#include <esp_adc_cal.h>

//*****************************************************************************************
//Definición de pines
//*****************************************************************************************
//parte 1
#define SensorTemp 35
#define b1 25

// parte 2
#define ledR 27
#define ledV 14
#define ledA 12

#define pwmChannel1 1 // canal para led roja
#define pwmChannel2 2 // canal para led verde
#define pwmChannel3 3 // canal para led azul

#define freqPWMled 5000 //Frecuencia de led en Hz
#define resolution 8    //1-16 bits de resolucion

//timer
#define prescaler 80

//*****************************************************************************************
//Prototipos de funciones
//*****************************************************************************************
void configurarPWM();

//*****************************************************************************************
//Variables Globales
//*****************************************************************************************
int LM35_Sensor = 0; 
float LM35_Temp_Sensor = 0.0;
float Voltaje = 0.0;
//float 

int b1State = 0;
int modo = 0; //que no empiece la lectura aun

//*****************************************************************************************
//ISR
//*****************************************************************************************
void IRAM_ATTR b1Temp()
{
    //para el sensor de temperatura
  LM35_Sensor = analogRead(SensorTemp); //conecta la variable adc float con el pin de salida
  //Voltaje = readADC_Cal(LM35_Sensor); // lee adc en voltaje
  LM35_Temp_Sensor = Voltaje / 10; //como el voltaje esta en mV,lo divido entre 10

  //Imprimir en pantalla de Viasual para comprobar lectura
  Serial.print("Temperatura = ");
  Serial.print("LM35_Temp_Sensor");
  Serial.print(" °C ");
}
//*****************************************************************************************
//Código de configuración
//*****************************************************************************************

void setup() {
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);
  attachInterrupt(b1, b1Temp, HIGH);
  pinMode(ledR, OUTPUT);
  pinMode(ledV, OUTPUT);
  pinMode(ledA, OUTPUT);
  configurarPWM();

}

//*****************************************************************************************
//Loop Principal
//*****************************************************************************************
void loop() {
  if(b1State == 1)
  {
    b1State = 0;
  }
  switch(modo)
  {
    case 1:
    //si la temperatura es menor a 37 °C, el color del semaforo es verde
    if(LM35_Sensor <= 20.0)
    {
      ledcWrite(pwmChannel1, 0);
      ledcWrite(pwmChannel2, 255);
      ledcWrite(pwmChannel3, 0);
    }
    //si la temperatura esta arriba de 37°C y abajo de 37.5°C , se enciende la led azul
    if(20 <= LM35_Sensor && LM35_Sensor <= 40)
    {
      ledcWrite(pwmChannel1, 0);
      ledcWrite(pwmChannel2, 0);
      ledcWrite(pwmChannel3, 255);     
    }
    //si la temperatura es mayor a 37.5°C, el color del semaforo es rojo
    if (LM35_Sensor > 40)
    {
      ledcWrite(pwmChannel1, 255);
      ledcWrite(pwmChannel2, 0);
      ledcWrite(pwmChannel3, 0);      
    }
  }
  

}
//calibracion y lectura de ADC
uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

//*****************************************************************************************
//Configuración de función de PWM
//*****************************************************************************************
void configurarPWM()
{
  //Paso 1: Configurar el modulo PWM
  ledcSetup(pwmChannel1, freqPWMled, resolution);
  ledcSetup(pwmChannel2, freqPWMled, resolution);
  ledcSetup(pwmChannel3, freqPWMled, resolution);

    //Paso 2: Relacionar el GPIO y el canal
  ledcAttachPin(ledR, pwmChannel1);
  ledcAttachPin(ledV, pwmChannel2);
  ledcAttachPin(ledA, pwmChannel3);
}