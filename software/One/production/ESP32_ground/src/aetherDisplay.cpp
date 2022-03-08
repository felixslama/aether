#include <SPI.h>
#include "aetherLora.h"
//Bibliotheken für das OLED-Display
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//Pins, an denen das OLED-Display angeschlossen ist
#define OLED_SDA 21
#define OLED_SCL 22
#define OLED_RST 16

//Displaygröße
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//Counter für die gesendeten Datenpakete
int counter = 0;

//OLED-Display instanziieren
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RST);

void initDisplay() {

  //Reset des OLED-Displays
  pinMode(OLED_RST, OUTPUT);
  digitalWrite(OLED_RST, LOW);
  delay(20);
  digitalWrite(OLED_RST, HIGH);

  //Display initialisieren
  Wire.begin(OLED_SDA, OLED_SCL);
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3c, false, false)) { // I2C-Adresse: 0x3C
    Serial.println(F("Display nicht gefunden."));
    for(;;); //Programm nicht weiter ausführen, wenn das Display nicht gefunden wird
  }
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.print("Starting up");
  display.display();

}

void loopDisplay() {
  display.clearDisplay();
  display.setCursor(0,0);
  display.println("AETHER GROUND STATION");
  display.setCursor(0,20);
  display.setTextSize(1);
  display.print("Status: ");
  //display.setCursor(105,20);
  display.print(readLora());      
  display.display();
}
void writeDisplay(String messageToDisplay){
    display.println(messageToDisplay);
}