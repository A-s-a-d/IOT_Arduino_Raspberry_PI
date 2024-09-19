#include <Arduino.h>
#include "Ticker.h"

// const
#define LED_R 3
#define LED_G 4
#define PERIOD 3000

const uint8_t DHT_PIN = 7;

#define actionPeriod 3000

// prototypes
void action();
void DHT_begin();
DHT_STATUS DHT_startConversion();
uint8_t DHT_readbyte();
DHT_STATUS DHT_readValues();

// obj
Ticker timer4(action, actionPeriod);

//************
void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_R, 1);
  Serial.begin(115200);
  DHT_begin();
  timer4.start();
}

//**************

void loop()
{
  timer4.update();
}

void action()
{

  digitalWrite(LED_G, !digitalRead(LED_G));
  digitalWrite(LED_R, !digitalRead(LED_R));

  DHT_STATUS status = DHT_readValues();
  if (status == DHT_OK)
  {
    Serial.print("temperature : ");
    Serial.println(temperature, 2);
    Serial.print("humidity : ");
    Serial.println(humidity, 2);
  }

  // uint8_t res;
  // res = DHT_startConversion();
  // Serial.println(res, DEC);
}

void DHT_begin()
{
}

DHT_STATUS DHT_startConversion()
{
}

uint8_t DHT_readbyte()
{
}

DHT_STATUS DHT_readValues()
{
}