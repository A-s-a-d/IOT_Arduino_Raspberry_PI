#include <Arduino.h>
#include "Ticker.h"
#include "DHT22.h"
#include "PCT2075.h"
#include "MATRIX.h"
#include "ArduinoGraphics.h"

// const
#define LED_R 3
#define LED_G 4
#define PERIOD 3000
#define A_PCT2075 0x37

const uint8_t DHT_PIN = 7;

byte frame[8][12] = {
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}};

#define actionPeriod 3000

// prototypes
void action();

// obj
DHT22 dht22;
PCT2075 pct2075;
Ticker timer4(action, actionPeriod);
ArduinoLEDMatrix matrix;

//************
void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_R, 1);
  Serial.begin(115200);
  dht22.begin(DHT_PIN);
  pct2075.begin(A_PCT2075);

  timer4.start();

  matrix.begin();

  display();
}

//**************

void loop()
{
  timer4.update();
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  char text[50];

  sprintf(text, "     T : %.2f °C  ", pct2075.get_temp());
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}

void action()
{

  digitalWrite(LED_G, !digitalRead(LED_G));
  digitalWrite(LED_R, !digitalRead(LED_R));
  dht22.readValues();

  Serial.print("temperature : ");
  Serial.println(dht22.getTemperature(), 2);
  Serial.print("humidity : ");
  Serial.println(dht22.getHumidity(), 2);
  Serial.print("dewpoint : ");
  Serial.println(dht22.getDewPoint());
  Serial.print("temp PCT : ");
  Serial.println(pct2075.get_temp());
  matrix.renderBitmap(frame, 8, 12);

  // uint8_t res;
  // res = DHT_startConversion();
  // Serial.println(res, DEC);
}
