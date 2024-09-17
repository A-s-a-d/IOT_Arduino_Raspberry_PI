#include "DHT22.h"

DHT22::DHT22()
{
    _pin = 0;
    _temperature = 0;
    _humidity = 0;
    _dewPoint = 0;
}
void DHT22::begin(uint8_t dhtPin)
{
    dhtPin = _pin;
}
DHT_STATUS DHT22::readValues()
{
}
float DHT22::getTemperature()
{
}
float DHT22::getHumidity()
{
}
float DHT22::getDewPoint()
{
}

DHT_STATUS DHT22::_startConversion()
{
}
uint8_t DHT22::_readByte()
{
}
float DHT22::_readDewPoint(float temp, float hum)
{
}