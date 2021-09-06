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

#define distiempo 5
//timer
#define prescaler 80

//*****************************************************************************************
//Prototipos de funciones
//*****************************************************************************************
void configurarPWM();

void b1Temp();

void LEDStemp();

void serfuncionServo();

void Display7S(int modo);

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

//Display
int decenas = 0;
int unidades = 0;
int decimales = 0;

//*****************************************************************************************
//ISR
//*****************************************************************************************

//*****************************************************************************************
//Código de configuración
//*****************************************************************************************

void setup() {
  Serial.begin(115200);
  pinMode(b1, INPUT_PULLUP);

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
  digitalWrite(servo,LOW);
  digitalWrite(ledR,LOW);
  digitalWrite(ledV,LOW);
  digitalWrite(ledA,LOW);

}

//*****************************************************************************************
//Loop Principal
//*****************************************************************************************
void loop() {

//DISPLAYS
Display7S(modo);

digitalWrite(d1, HIGH); //controla el primer display = decenas
digitalWrite(d2, LOW);
digitalWrite(d3, LOW);
Display7S(decenas);
distiempo;

digitalWrite(d1, LOW); 
digitalWrite(d2, HIGH); //controla el segundo display = unidades
digitalWrite(d3, LOW);
Display7S(unidades);
distiempo;

digitalWrite(d1, LOW); 
digitalWrite(d2, LOW); 
digitalWrite(d3, HIGH);//controla el tercer display = decimales
Display7S(decimales);
distiempo;

//llamo a las demas funciones
b1Temp();
LEDStemp();
serfuncionServo();

}

//calibracion y lectura de ADC
uint32_t readADC_Cal(int ADC_Raw)
{
  esp_adc_cal_characteristics_t adc_chars;
  
  esp_adc_cal_characterize(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100, &adc_chars);
  return(esp_adc_cal_raw_to_voltage(ADC_Raw, &adc_chars));
}

//*****************************************************************************************
//Función temperatura
//*****************************************************************************************
void b1Temp()
{
  if (digitalRead(b1)== LOW)
  {
    b1State = HIGH;
  }
  else if (b1State = HIGH)
  {
    b1State = LOW;

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
void Display7S(int modo)
{
//hago las operaciones para cada que display imprima cada valor
decenas = (LM35_Temp_Sensor) / (10);
unidades = (LM35_Temp_Sensor) - (decenas*10);
decimales = (LM35_Temp_Sensor*10) - (decenas*100) - (unidades*10);

//traigo los valores para cada numero en el display
switch (modo)
    {
    case 0:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, HIGH);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, LOW);
        break;

    case 1:
        digitalWrite(dA, LOW);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, LOW);
        digitalWrite(dE, LOW);
        digitalWrite(dF, LOW);
        digitalWrite(dG, LOW);
        break;

    case 2:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, LOW);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, HIGH);
        digitalWrite(dF, LOW);
        digitalWrite(dG, HIGH);
        break;

    case 3:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, LOW);
        digitalWrite(dF, LOW);
        digitalWrite(dG, HIGH);
        break;

    case 4:
        digitalWrite(dA, LOW);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, LOW);
        digitalWrite(dE, LOW);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, HIGH);
        break;

    case 5:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, LOW);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, LOW);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, HIGH);
        break;

    case 6:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, LOW);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, HIGH);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, HIGH);
        break;

    case 7:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, LOW);
        digitalWrite(dE, LOW);
        digitalWrite(dF, LOW);
        digitalWrite(dG, LOW);
        break;

    case 8:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, HIGH);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, HIGH);
        break;

    case 9:
        digitalWrite(dA, HIGH);
        digitalWrite(dB, HIGH);
        digitalWrite(dC, HIGH);
        digitalWrite(dD, HIGH);
        digitalWrite(dE, LOW);
        digitalWrite(dF, HIGH);
        digitalWrite(dG, HIGH);
        break;

    default:
        break;
    }

}
