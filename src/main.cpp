#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

RTC_DS3231 rtc;
const int relay01 = 6;
const int relay02 = 9;
const int LED01 = 3;
const int LED02 = 5;

int lastSecond = -1;
int lastMinute = -1;
int lastHour = -1;
float lastTemp = -1.0;

unsigned int lastDay = 0;
unsigned int lastMonth = 0;
unsigned int lastYear = 0;

LiquidCrystal_I2C lcd(0x27, 20, 4); // 0x20 para Proteus, 0x27 para Arduino

#define ONE_WIRE_BUS 2
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

float lastTempC = NAN; // Variable para almacenar la última temperatura
int lastLoggedHour = -1; // Variable para controlar la última hora registrada

void setup() {
    Serial.begin(9600);  // Configuración para Virtual Terminal
    Wire.begin();
    lcd.begin(20, 4);
    lcd.backlight();

    // Mensajes de inicio
    lcd.setCursor(2, 1);
    lcd.print("IOT--->LOADING...");
    delay(2000);
    lcd.clear();
    lcd.setCursor(1, 2);
    lcd.print("ARDUINO ENCENDIDO");
    delay(2000);
    lcd.clear();
    // Inicialización del RTC
    if (!rtc.begin()) {
        Serial.println("RTC no encontrado");
        while (1); // Detener si no se encuentra el RTC
    }
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__))); // Ajustar fecha y hora manualmente
    Serial.println("RTC configurado con éxito");
    
    // Configuración de pines
    pinMode(relay01, OUTPUT);
    pinMode(relay02, OUTPUT);
    pinMode(LED01, OUTPUT);
    pinMode(LED02, OUTPUT);
    digitalWrite(relay01, HIGH);
    digitalWrite(relay02, HIGH);

    // Inicializar sensor de temperatura
    Serial.println("Sistema Iniciado: Monitorizando...");
}

void loop() {
    DateTime HoraActual = rtc.now();
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);

    // Mostrar siempre la hora
    lcd.setCursor(0, 0);
    lcd.print("Hora: ");
    lcd.print(HoraActual.hour() < 10 ? "0" : "");  
    lcd.print(HoraActual.hour());
    lcd.print(":");
    lcd.print(HoraActual.minute() < 10 ? "0" : "");
    lcd.print(HoraActual.minute());
    lcd.print(":");
    lcd.print(HoraActual.second() < 10 ? "0" : "");
    lcd.print(HoraActual.second());

    // Mostrar siempre la fecha
    lcd.setCursor(0, 3);  // Línea 4 (ajústalo si tu LCD tiene menos líneas)
    lcd.print("Fecha: ");
    lcd.print(HoraActual.day() < 10 ? "0" : "");
    lcd.print(HoraActual.day());
    lcd.print("/");
    lcd.print(HoraActual.month() < 10 ? "0" : "");
    lcd.print(HoraActual.month());
    lcd.print("/");
    lcd.print(HoraActual.year());

    // Actualizar temperatura si cambia
    if (tempC != lastTemp) {
        lcd.setCursor(3, 2);  // Línea 3 (ajústalo si tu LCD tiene menos líneas)
        lcd.print("TEMP: ");
        lcd.print(tempC, 1);
        lcd.print(" ");
        lcd.write(223);  // Símbolo de grados
        lcd.print("C");
        lastTemp = tempC;
    }

    // Estado de válvulas y relés

 if (HoraActual.hour() == 8 && HoraActual.minute() == 30) {
        digitalWrite(relay01, LOW);
        delay(100); 
        digitalWrite(LED01, HIGH); 
        lcd.setCursor(0, 1);
        lcd.print("Valve[01] --> ON");
        Serial.println("Electroválvula 01 ENCENDIDA - Inicio");
        
    } else if (HoraActual.hour() == 8 && HoraActual.minute() == 34) {
        digitalWrite(relay01, HIGH); 
        delay(100); 
        digitalWrite(LED01, LOW);    
        lcd.setCursor(0, 1);
        lcd.print("Valve[01] <--> OFF");
        Serial.println("Electroválvula 01 APAGADA - Fin");
    }else {
    lcd.setCursor(0, 1);
    lcd.print("                    "); // Sobrescribe con espacios
    }
       
    if (HoraActual.hour() == 8 && HoraActual.minute() == 35) {
        digitalWrite(relay02, LOW);
        delay(100);  
        digitalWrite(LED02, HIGH);   
        lcd.setCursor(0, 1);
        lcd.print("Valve[02] --> ON");
        Serial.println("Electroválvula 02 ENCENDIDA - Inicio");

    } else if (HoraActual.hour() == 8 && HoraActual.minute() == 38) {
        digitalWrite(relay02, HIGH); 
        delay(100);
        digitalWrite(LED02, LOW);    
        lcd.setCursor(0, 1);
        lcd.print("Valve[02] <--> OFF");
        Serial.println("Electroválvula 02 APAGADA - Fin");   
    }
    // Registrar e imprimir datos cada 6 horas
    if ((HoraActual.hour() == 6 || HoraActual.hour() == 12 || HoraActual.hour() == 18) && HoraActual.hour() != lastLoggedHour) {
      
        // Imprimir en Serial
        Serial.println("---- Registro ----");
        Serial.print("Fecha: ");
        Serial.print(HoraActual.day() < 10 ? "0" : "");
        Serial.print(HoraActual.day());
        Serial.print("/");
        Serial.print(HoraActual.month() < 10 ? "0" : "");
        Serial.print(HoraActual.month());
        Serial.print("/");
        Serial.println(HoraActual.year());

        Serial.print("Hora: ");
        Serial.print(HoraActual.hour() < 10 ? "0" : "");
        Serial.print(HoraActual.hour());
        Serial.print(":");
        Serial.print(HoraActual.minute() < 10 ? "0" : "");
        Serial.print(HoraActual.minute());
        Serial.print(":");
        Serial.print(HoraActual.second() < 10 ? "0" : "");
        Serial.println(HoraActual.second());

        Serial.print("Temperatura: ");
        Serial.print(tempC, 1);
        Serial.println(" °C");
        Serial.println("------------------");

        // Actualizar la última hora registrada
        lastLoggedHour = HoraActual.hour();
    }

   delay(1000);
}