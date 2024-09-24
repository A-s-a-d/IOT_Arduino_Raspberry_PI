#include <Arduino.h>
#include "Ticker.h"
#include "DHT22.h"

// const
#define LED_R 3
#define LED_G 4
#define PERIOD 3000

const uint8_t DHT_PIN = 7;

#define actionPeriod 3000

// prototypes
void action();

// obj
DHT22 dht22;
Ticker timer4(action, actionPeriod);

//************
void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_R, 1);
  Serial.begin(115200);

  dht22.begin(DHT_PIN);

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
  dht22.readValues();

  Serial.print("temperature : ");
  Serial.println(dht22.getTemperature(), 2);
  Serial.print("humidity : ");
  Serial.println(dht22.getHumidity(), 2);
  Serial.print("dewpoint : ");
  Serial.println(dht22.getDewPoint());

  // uint8_t res;
  // res = DHT_startConversion();
  // Serial.println(res, DEC);
}

