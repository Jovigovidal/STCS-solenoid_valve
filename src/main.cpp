#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <RTClib.h>
#include <OneWire.h>
#include <DallasTemperature.h>

RTC_DS3231 rtc;
int relay01 = 6;
int relay02 = 9;
const int LED01 = 12;
const int LED02 = 13;

const long interval = 5000;
LiquidCrystal_I2C lcd(0x27, 20, 4);

byte A_acute[8] = {
    B00100,  // Tilde
    B00000,  // Espacio para separar la tilde
    B01110,  // Parte superior de la "A"
    B10001,
    B10001,
    B11111,  // Parte central de la "A"
    B10001,
    B10001
};


byte a_acute[8] = {
    B00100,  // Tilde
    B00000,  // Espacio para separar tilde de la letra
    B01110,  // Parte superior de la letra "a"
    B00001,
    B01111,
    B10001,
    B01111,
    B00000
};

byte customChar[8] = {
  B00000,
  B00100,
  B01110,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000
};


void setup() {
   //HOLA MUNDOOOOOOOOOOOOOOOOOOO GAAAAAAAAAAAAAA
    Wire.begin();

    
    lcd.begin(16, 2);
    lcd.backlight(); 
    lcd.clear();
    lcd.setCursor(2, 4);
    lcd.print("IOT--->LOADING...");
    delay(4000);
    lcd.setCursor(2, 4);
    lcd.print("***PROCESO[2]***");
    delay(2000);

    ///// VISUALIZAR EL ARDUINO QUE ESTE ACTIVO
    pinMode(13, OUTPUT); 
    digitalWrite(13, HIGH);

    lcd.begin(16, 2);
    lcd.backlight();
    lcd.clear();
    lcd.setCursor(1, 2);
    lcd.print("ARDUINO ENCENDIDO");
    delay(2000); 
    digitalWrite(13, LOW);

    lcd.clear();
    // Inicializa el RTC
    if (!rtc.begin()) {
        lcd.setCursor(0, 1);
        lcd.print("Error RTC");
        while (1);  // Si no se detecta el RTC, se detiene el programa
    }

    // Configura el reloj si es necesario
    if (rtc.lostPower()) {
        rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));  // Establece la hora del compilado
    }

    // Configura los pines de los relés
    pinMode(relay01, OUTPUT);
    pinMode(relay02, OUTPUT);
    pinMode(LED01, OUTPUT);  
    pinMode(LED02, OUTPUT); 

    // Inicializa los relés en estado alto (apagado)
    digitalWrite(relay01, HIGH);
    digitalWrite(relay02, HIGH);
    lcd.createChar(2, A_acute);
    lcd.clear();
    lcd.setCursor(4, 1);
    lcd.print("UNIVERSIDAD");
    lcd.setCursor(5, 2);
    lcd.print("DE HU");
    lcd.write(byte(2)); // letra [á]
    lcd.print("NUCO");
    delay(4000);
    lcd.clear();
    lcd.createChar(0, customChar);
    lcd.init();
    lcd.backlight();
  
    // Imprimir en la primera fila
    lcd.setCursor(0, 0);
    lcd.print("STARTING...  ( )/");
    
    // Imprimir en la segunda fila
    lcd.setCursor(0, 1);
    lcd.print("              )(/");

    // Imprimir en la tercera fila
    lcd.setCursor(0, 2);
    lcd.print(" ____________( /)");

    // Imprimir en la cuarta fila
    lcd.setCursor(0, 3);
    lcd.print("()__)________)))");
    lcd.write(byte(0));
    delay(3000);

    lcd.init();
    lcd.backlight();  
}

void loop() {

    DateTime HoraActual = rtc.now();
    lcd.setCursor(0, 0);
    lcd.print("Hora: ");
    if (HoraActual.hour() < 10) lcd.print("0");  
    lcd.print(HoraActual.hour());
    lcd.print(":");
    if (HoraActual.minute() < 10) lcd.print("0");
    lcd.print(HoraActual.minute());
    lcd.print(":");
    if (HoraActual.second() < 10) lcd.print("0");
    lcd.print(HoraActual.second());

    //ESTADO DE LAS VALVULAS
    // Válvula-01

    if (HoraActual.second() == 1) {
        digitalWrite(relay01, LOW); 
        digitalWrite(LED01, HIGH); 
        if(HoraActual.second()==1) 
        lcd.setCursor(0, 1);
        lcd.print("Valvula[01] --> ON");
        
     } else if (HoraActual.second() == 5) {
        lcd.createChar(2, a_acute);
        digitalWrite(relay01, HIGH);  
        digitalWrite(LED01, LOW);    
        lcd.setCursor(0, 1);
        lcd.print("V");
        lcd.write(byte(2));
        lcd.print("lvula[01] <--> OFF");
      
    }

     // Válvula-02
    if (HoraActual.second() == 7) {   // RANG0 DE FECHA
        digitalWrite(relay02, LOW);  
        digitalWrite(LED02, HIGH);   
        lcd.setCursor(0, 1);
        lcd.print("Valvula[02] --> ON");
        
    } else if (HoraActual.second() == 10) { // RANGO DE FECHA
        digitalWrite(relay02, HIGH); 
        digitalWrite(LED02, LOW);    
        lcd.setCursor(0, 1);
        lcd.print("Valvula[02] <--> OFF");
       
    }
       
    //SENSOR DE TEMP
    #define ONE_WIRE_BUS 2
    OneWire oneWire(ONE_WIRE_BUS);
    DallasTemperature sensors(&oneWire);

    //Serial.begin(9600);
    sensors.begin();   //Se inicia el sensor
    sensors.requestTemperatures();
    float tempC = sensors.getTempCByIndex(0);
    char tempString[10];
    dtostrf(tempC, 5, 2, tempString);
    lcd.clear();
    lcd.setCursor(3, 2);
    lcd.print("TEMP: ");
    lcd.print(tempString);
    lcd.print(" ");
    lcd.write(223); 
    lcd.print("C");

}
