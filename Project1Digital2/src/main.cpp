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
#define SensorTemp

//*****************************************************************************************
//Prototipos de funciones
//*****************************************************************************************

//*****************************************************************************************
//Variables Globales
//*****************************************************************************************
int LM35_Sensor = 0;
float LM35_Temp_Sensor = 0.0;
float Voltaje = 0.0;

//*****************************************************************************************
//ISR
//*****************************************************************************************

//*****************************************************************************************
//Código de configuración
//*****************************************************************************************

void setup() {
  Serial.begin(115200);
}

//*****************************************************************************************
//Loop Principal
//*****************************************************************************************
void loop() {
  LM35_Sensor = analogRead(SensorTemp); //conecta la variable adc float con el pin de salida
  Voltaje = readADC_Cal(LM35_Sensor); // lee adc en voltaje
  LM35_Temp_Sensor = Voltaje / 10; //como el voltaje esta en mV,lo divido entre 10

  //Imprimir en pantalla de Viasual para comprobar lectura
  Serial.print("Temperatura = ");
  Serial.print("LM35_Temp_Sensor");
  Serial.print(" °C ")
  delay(100);
}

//calibracion y lectura de ADC
uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}