/*
  Plantcase
*/
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_AHTX0.h>

// Dimensions de l'écran OLED
#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 32 

#define FAN_PIN 2

// Configuration température
#define TEMPERATURE_C_TARGET 25
#define TEMPERATURE_C_ALLOWED_DELTA 0.5

// Configuration humidité
#define HUMIDITY_PERCENT_TARGET 70
#define HUMIDITY_PERCENT_ALLOWED_DELTA 3

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
// The pins for I2C are defined by the Wire-library. 
// On an arduino UNO:       A4(SDA), A5(SCL)
// On an arduino MEGA 2560: 20(SDA), 21(SCL)
// On an arduino LEONARDO:   2(SDA),  3(SCL), ...
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Capteur d'humidité/température
Adafruit_AHTX0 aht;

void setup()
{
  Serial.begin(115200);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  if(!aht.begin())
  {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  
  pinMode(FAN_PIN, OUTPUT);
  digitalWrite(FAN_PIN, LOW);
  pinMode(LED_BUILTIN, OUTPUT);
}

void loop() 
{
  sensors_event_t humidity, temp;

  // Récupération des infos du capteur
  aht.getEvent(&humidity, &temp);

  char tmp_temperature[6];
  char tmp_humidity[6];

  dtostrf(temp.temperature, 4, 2, tmp_temperature);
  dtostrf(humidity.relative_humidity, 4, 2, tmp_humidity);

  // Si température trop élevée
  if(temp.temperature > TEMPERATURE_C_TARGET+TEMPERATURE_C_ALLOWED_DELTA)
  {
    // Mise en route du ventilateur
    digitalWrite(FAN_PIN, HIGH);
    // On allume la LED de l'Arduino
    digitalWrite(LED_BUILTIN, HIGH);
  }
  // Quand température à nouveau bonne
  if(temp.temperature < TEMPERATURE_C_TARGET-TEMPERATURE_C_ALLOWED_DELTA)
  {
    // Arrêt du ventilateur
    digitalWrite(FAN_PIN, LOW);
    // On éteint la LED de l'Arduino
    digitalWrite(LED_BUILTIN, LOW);
  }

  char txt[50];
  sprintf(txt, "T:%s\nH:%s", tmp_temperature, tmp_humidity);
  Serial.println(txt);
  displayTextOnOLED(txt);

  delay(5000);
}


// Affiche le texte passé sur l'écran
void displayTextOnOLED(String text) 
{
  display.clearDisplay();
  display.setTextSize(2);               
  display.setTextColor(SSD1306_WHITE); 
  display.setCursor(0, 0);            
  for (int i = 0; i < text.length(); i++) 
  {
    display.write(char(text[i]));
  }
  display.display();  
}

