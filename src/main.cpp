#include <Arduino.h>
#include "Ticker.h"

// const
#define LED_R 3
#define LED_G 4
#define PERIOD 3000
const uint8_t DHT_PIN = 7;

#define actionPeriod 3000

// var global
float temperature = 0;
float humidity = 0;
float dewPoint = 0;

enum DHT_STATUS
{
  DHT_OK = 0,
  DHT_ERROR_RESPONSE,
  DHT_ERROR_CHECK_SUM,
  DHT_ERROR_TIMEOUT
};

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
  pinMode(DHT_PIN, INPUT);
  delay(2000);
}

DHT_STATUS DHT_startConversion()
{
  // var loc
  DHT_STATUS status;
  // uc output signal
  pinMode(DHT_PIN, OUTPUT);
  digitalWrite(DHT_PIN, 0);
  delayMicroseconds(1200);
  digitalWrite(DHT_PIN, 1);
  delayMicroseconds(30);

  // en mode entree
  pinMode(DHT_PIN, INPUT);
  bool test;
  // verification 0
  delayMicroseconds(40);
  test = digitalRead(DHT_PIN);
  uint32_t now = millis();
  if (test == 1)
  {
    status = DHT_ERROR_RESPONSE;
    if (millis() - now > 20)
    {
      return DHT_ERROR_TIMEOUT;
    }
    else
    {
      return DHT_ERROR_RESPONSE;
    }
  }
  else
  {
    // verification 1
    delayMicroseconds(80);
    test = digitalRead(DHT_PIN);
    if (test == 0)
    {
      return DHT_ERROR_RESPONSE;
    }
    while (digitalRead(DHT_PIN) == 1)
    {
    }
  }
  return DHT_OK;
}

uint8_t DHT_readbyte()
{
  uint8_t res = 0x00;
  for (int i = 0; i < 8; i++)
  {
    while (digitalRead(DHT_PIN) == 0)
    {
    }
    delayMicroseconds(35);
    if (digitalRead(DHT_PIN) == 1)
    {
      res = res | 1 << (7 - i);
      while (digitalRead(DHT_PIN) == 1)
      {
      }
    }
  }
  return res;
}

DHT_STATUS DHT_readValues()
{
  uint8_t hum_upper = 0x00;
  uint8_t hum_lower = 0x00;
  uint8_t temp_upper = 0x00;
  uint8_t temp_lower = 0x00;
  uint8_t checksum = 0x00;
  uint8_t operation = 0x00;
  uint16_t temp = 0x0000;
  uint16_t hum = 0x0000;
  DHT_STATUS status = DHT_OK;

  status = DHT_startConversion();

  if (status == 0)
  {
    hum_upper = DHT_readbyte();
    hum_lower = DHT_readbyte();
    temp_upper = DHT_readbyte();
    temp_lower = DHT_readbyte();
    checksum = DHT_readbyte();
  }
  operation = hum_upper + hum_lower + temp_upper + temp_lower;

  if (operation == checksum)
  {
    status = DHT_OK;
    hum = (hum_upper << 8) | hum_lower;
    temp = (temp_upper << 8) | temp_lower;
    temperature = temp / 10.0;
    humidity = hum / 10.0;
  }
  else
  {
    return DHT_ERROR_CHECK_SUM;
  }
  return status;
}