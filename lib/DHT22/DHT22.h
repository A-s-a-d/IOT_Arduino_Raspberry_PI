#ifndef DHT22_H
#define DHT22_H

#include <Arduino.h>

enum DHT_STATUS
{
    DHT_OK = 0,
    DHT_ERROR_RESPONSE,
    DHT_ERROR_CHECK_SUM,
    DHT_ERROR_TIMEOUT
};
class DHT22
{
public:
    DHT22();
    void begin(uint8_t dhtPin);
    DHT_STATUS readValues();
    float getTemperature();
    float getHumidity();
    float getDewPoint();

private:
    DHT_STATUS _startConversion();
    uint8_t _readByte();
    float _readDewPoint(float temp, float hum);
    uint8_t _pin;
    float _temperature;
    float _humidity;
    float _dewPoint;
};

#endif