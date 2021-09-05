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
#include "AdafruitIO_WiFi.h"
#include "Display7Seg.h"

/************************ Adafruit IO Config *******************************/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.
#define IO_USERNAME "your_username"
#define IO_KEY "your_key"
#define WIFI_SSID "your_ssid"
#define WIFI_PASS "your_pass"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);

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

// parte 3
#define servo 25
#define freqPWMservo 50
#define pwmChannel4 4 //canal para el servo
#define tiempoS 5

//display
#define dA 22
#define dB 1
#define dC 3
#define dD 21
#define dE 19
#define dF 18
#define dG 5
#define dP 17

#define d1 16
#define d2 4
#define d3 2

//timer
#define prescaler 80

//*****************************************************************************************
//Prototipos de funciones
//*****************************************************************************************
void configurarPWM();

void IRAM_ATTR b1Temp();

void LEDStemp();

void serfuncionServo();

//*****************************************************************************************
//Variables Globales
//*****************************************************************************************
int LM35_Sensor = 0; 
float LM35_Temp_Sensor = 0.0;
float Voltaje = 0.0;
//float 

int b1State = 0;
int modo = 0; //que no empiece la lectura aun

//LEDS
int ledDutyCycle = 0;

//SERVO
int sDutyCycle = 7;

//*****************************************************************************************
//ISR
//*****************************************************************************************

void IRAM_ATTR b1Temp()
{
    //para el sensor de temperatura
  LM35_Sensor = analogRead(SensorTemp); //conecta la variable adc float con el pin de salida
  //Voltaje = readADC_Cal(LM35_Sensor); // lee adc en voltaje
  LM35_Temp_Sensor = Voltaje / 10; //como el voltaje esta en mV,lo divido entre 10

  delay (100);

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

  pinMode(servo, OUTPUT);
  configurarPWM();

  configurarDisplay(dA, dB, dC, dD , dE, dF, dG, dP);

  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);

  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);

}

//*****************************************************************************************
//Loop Principal
//*****************************************************************************************
void loop() {

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

  ledcSetup(pwmChannel4, freqPWMservo, resolution);

    //Paso 2: Relacionar el GPIO y el canal
  ledcAttachPin(ledR, pwmChannel1);
  ledcAttachPin(ledV, pwmChannel2);
  ledcAttachPin(ledA, pwmChannel3);

  ledcAttachPin(servo, pwmChannel4);
}

//*****************************************************************************************
//Función de LEDS
//*****************************************************************************************
void LEDStemp()
{
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

//*****************************************************************************************
//Función de Servo
//*****************************************************************************************
void serfuncionServo()
{
  
  if (LM35_Sensor <= 20.0) //pase el duty cycle a 8 bits, por lo que el rango irá de 7 a 32
  {
    sDutyCycle = 7.8;

    delay(tiempoS);
    ledcWrite(pwmChannel4, sDutyCycle);
  }
  else if(20 <= LM35_Sensor && LM35_Sensor <= 40)
  {
    sDutyCycle = 18;

    delay(tiempoS);
    ledcWrite(pwmChannel4, sDutyCycle);
  }
  else if(LM35_Sensor > 40)
  {
    sDutyCycle = 26;

    delay(tiempoS);
    ledcWrite(pwmChannel4, sDutyCycle);
  }

}
//*****************************************************************************************
//Función Display
//*****************************************************************************************
