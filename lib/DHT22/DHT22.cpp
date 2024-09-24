#include "DHT22.h"

DHT22::DHT22()
{
    _pin = 0;
    _temperature = 0;
    _humidity = 0;
    _dewPoint = 0;
    _a = 17.27; // On initialise dans le constructeur les constantes a et b pour calculer le point de rosée (Dewpoint)
    _b = 237.7;
}

void DHT22::begin(uint8_t dhtPin)
{
    _pin = dhtPin;
    pinMode(_pin, INPUT);
    delay(2000);
}

DHT_STATUS DHT22::readValues()
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

    status = _startConversion();
    if (status == 0)
    {
        hum_upper = _readByte();
        hum_lower = _readByte();
        temp_upper = _readByte();
        temp_lower = _readByte();
        checksum = _readByte();
    }
    operation = hum_upper + hum_lower + temp_upper + temp_lower;

    if (operation == checksum)
    {
        status = DHT_OK;
        hum = (hum_upper << 8) | hum_lower;
        temp = (temp_upper << 8) | temp_lower;
        _temperature = temp / 10.0;
        _humidity = hum / 10.0;
    }
    else
    {
        return DHT_ERROR_CHECK_SUM;
    }
    return status;
}

float DHT22::getTemperature()
{
    return _temperature;
}
float DHT22::getHumidity()
{
    return _humidity;
}
float DHT22::getDewPoint()
{
    _dewPoint = _readDewPoint(_temperature, _humidity);
    return _dewPoint;
}

DHT_STATUS DHT22::_startConversion()
{
    // var loc
    DHT_STATUS status = DHT_OK;
    // uc output signal
    pinMode(_pin, OUTPUT);
    digitalWrite(_pin, 0);
    delayMicroseconds(1200);
    digitalWrite(_pin, 1);
    delayMicroseconds(30);

    // en mode entree
    pinMode(_pin, INPUT);
    delayMicroseconds(40);

    if (digitalRead(_pin) == 0)
    {
        delayMicroseconds(80);
        if (digitalRead(_pin) == 1)
        {
            status = DHT_OK;
        }
        else
        {
            status = DHT_ERROR_RESPONSE;
        }
    }
    // verification 1
    uint32_t now = millis();
    while (digitalRead(_pin) == 1)
    {
        if ((millis() - now) > 20)
        {
            return DHT_ERROR_TIMEOUT;
        }
    }
    return status;
}

uint8_t DHT22::_readByte()
{
    uint8_t res = 0x00;
    for (int i = 0; i < 8; i++)
    {
        while (digitalRead(_pin) == 0)
        {
        }
        delayMicroseconds(35);
        if (digitalRead(_pin) == 1)
        {
            res = res | 1 << (7 - i);
            while (digitalRead(_pin) == 1)
            {
            }
        }
    }
    return res;
}

float DHT22::_readDewPoint(float temp, float hum)
{
    float psy = 0;
    float res = 0;
    if ((temp > 0 && temp < 60) && (hum > 0 && hum < 100))
    {
        psy = ((_a * temp) / (_b + temp));
        psy = psy + log(hum / 100.0);
        res = (_b * psy) / (_a - psy);
    }
    else
    {
        res = -1;
    }
    return res;
}