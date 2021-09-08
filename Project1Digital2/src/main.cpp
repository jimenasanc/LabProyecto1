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
#include "Display7Seg.h"
#include "AdafruitIO_WiFi.h"

/******** Adafruit IO Config ***********/

// visit io.adafruit.com if you need to create an account,
// or if you need your Adafruit IO key.

#define IO_USERNAME "jimenasanc"
#define IO_KEY "aio_jypx8925mzUPQDT5TuxZibmqpmhg"
#define WIFI_SSID "TIGO-958C-5G"
#define WIFI_PASS "2NB112105126"

AdafruitIO_WiFi io(IO_USERNAME, IO_KEY, WIFI_SSID, WIFI_PASS);
AdafruitIO_Feed *Termometro = io.feed("temperatura");

//*****************************************************************************************
//Definición de pines
//*****************************************************************************************
//parte 1
#define SensorTemp 35
#define b1 32

// parte 2
#define ledR 33
#define ledV 25
#define ledA 26

#define pwmChannel1 1 // canal para led roja
#define pwmChannel2 2 // canal para led verde
#define pwmChannel3 3 // canal para led azul

#define freqPWMled 5000 //Frecuencia de led en Hz
#define resolution 8    //1-16 bits de resolucion

// parte 3
#define servo 34
#define freqPWMservo 50
#define pwmChannel4 4 //canal para el servo
//#define tiempoS 5

//display
#define dA 4
#define dB 21
#define dC 18
#define dD 5
#define dE 22
#define dF 2
#define dG 15
#define dP 19

#define d1 13
#define d2 12
#define d3 14

//*****************************************************************************************
//Prototipos de funciones
//*****************************************************************************************
void configurarPWM(void);

void LEDStemp(void);

void Display7S(int LM35_Sensor);

float ReadVoltage(int ADC_Raw);

//*****************************************************************************************
//Variables Globales
//*****************************************************************************************
int LM35_Sensor = 0;          //resultado, almacena la conversi[on del adc]
float LM35_Temp_Sensor = 0.0; //temp medida

//en vez de usar delay, usar[e esta funci[on]]
long lastTime;
int sampleTime = 3000;

//estado para el boton
int b1State = 0;

//LEDS
int ledDutyCycle = 0;

//SERVO
float sDutyCycle = 0.0;

//Display
int decenas = 0;
int unidades = 0;
int decimales = 0;

//filtro EMA
double adcfilt = 0; //ema. media móvil exponencial
double alpha = 0.09;

//Adafruit
int contador = 0;

//*****************************************************************************************
//ISR
//*****************************************************************************************

//*****************************************************************************************
//Código de configuración
//*****************************************************************************************

void setup()
{
  Serial.begin(115200);
  pinMode(SensorTemp, INPUT);
  pinMode(b1, INPUT_PULLUP);

  pinMode(ledR, OUTPUT);
  pinMode(ledV, OUTPUT);
  pinMode(ledA, OUTPUT);

  lastTime = millis();

  pinMode(servo, OUTPUT);
  configurarPWM();

  configurarDisplay(dA, dB, dC, dD, dE, dF, dG, dP);
  
  pinMode(d1, OUTPUT);
  pinMode(d2, OUTPUT);
  pinMode(d3, OUTPUT);

  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  
  //Adafruit
  while (!Serial)
    ;

  Serial.print("Conectando a Adafruit IO");
  //Connect to io.adafruit.com
  io.connect();

  //Se esperala conexión
  while (io.status() < AIO_CONNECTED)
  {
    Serial.print(".");
    delay(500);
  }
  //Ya se ha conectado
  Serial.println();
  Serial.println(io.statusText());
  
}

//*****************************************************************************************
//Loop Principal
//*****************************************************************************************
void loop()
{
  
  //Segmento de Adafruit IO
  io.run();

  // save count to the 'counter' feed on Adafruit IO
  Serial.print("sending temp ");
  Serial.println(contador);
  Termometro->save(LM35_Temp_Sensor);

  if (digitalRead(b1) == LOW)
  {
    b1State = HIGH;
  }
  else if (b1State == HIGH)
  {
    b1State = LOW;
    //para el sensor de temperatura
    lastTime = millis();
    LM35_Sensor = analogReadMilliVolts(SensorTemp); //conecta la variable adc float con el pin de salida
    LM35_Temp_Sensor = LM35_Temp_Sensor / 10.0;     //como el voltaje esta en mV,lo divido entre 10

    //Imprimir en pantalla de Viasual para comprobar lectura
    Serial.print("Temperatura = ");
    Serial.print(LM35_Temp_Sensor);
    //Serial.println(analogReadMilliVolts(SensorTemp) / 10.0);
    Serial.println(" °C ");
  }
  Display7S(LM35_Sensor); //cálculos
  //DISPLAYS
  digitalWrite(d1, HIGH); //controla el primer display = decenas
  digitalWrite(d2, LOW);
  digitalWrite(d3, LOW);
  Display7S(decenas);
  desplegarPunto(0);

  lastTime = millis();
  while (millis() < lastTime + sampleTime)
    ;
  digitalWrite(d1, LOW);
  digitalWrite(d2, HIGH); //controla el segundo display = unidades
  digitalWrite(d3, LOW);
  Display7S(unidades);
  desplegarPunto(1);

  lastTime = millis();
  while (millis() < lastTime + sampleTime)
    ;

  digitalWrite(d1, LOW);
  digitalWrite(d2, LOW);
  digitalWrite(d3, HIGH); //controla el tercer display = decimales
  Display7S(decimales);
  desplegarPunto(0);
  lastTime = millis();
  while (millis() < lastTime + sampleTime)
    ;

  adcfilt = (alpha * LM35_Temp_Sensor) + ((1 - alpha) * adcfilt); //filtro ema

  //llamo a las demas funciones
  LEDStemp();
}
float ReadVoltage(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_11db, ADC_WIDTH_12Bit, 1100, &adc_chars);
  return (esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
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
//Función de LEDS y servo
//*****************************************************************************************
void LEDStemp()
{
  //si la temperatura es menor a 37 °C, el color del semaforo es verde
  if (LM35_Temp_Sensor <= 37.0)
  {
    ledcWrite(pwmChannel1, 0);
    ledcWrite(pwmChannel2, 255);
    ledcWrite(pwmChannel3, 0);

    sDutyCycle = 7.8;

    ledcWrite(pwmChannel4, sDutyCycle);
  }
  //si la temperatura esta arriba de 37°C y abajo de 37.5°C , se enciende la led azul
  if (37.0 > LM35_Temp_Sensor && LM35_Temp_Sensor < 37.5)
  {
    ledcWrite(pwmChannel1, 0);
    ledcWrite(pwmChannel2, 0);
    ledcWrite(pwmChannel3, 255);

    sDutyCycle = 18;

    ledcWrite(pwmChannel4, sDutyCycle);
  }
  //si la temperatura es mayor a 37.5°C, el color del semaforo es rojo
  if (LM35_Temp_Sensor >= 37.5)
  {
    ledcWrite(pwmChannel1, 255);
    ledcWrite(pwmChannel2, 0);
    ledcWrite(pwmChannel3, 0);

    sDutyCycle = 26;

    ledcWrite(pwmChannel4, sDutyCycle);
  }
}

//*****************************************************************************************
//Función de Displays
//*****************************************************************************************
void Display7S(int LM35_Sensor)
{
  //hago las operaciones para cada que display imprima cada valor
  decenas = LM35_Temp_Sensor / 10;
  unidades = LM35_Temp_Sensor - (decenas * 10);
  decimales = (LM35_Temp_Sensor * 10) - (decenas * 100) - (unidades = 10);

}