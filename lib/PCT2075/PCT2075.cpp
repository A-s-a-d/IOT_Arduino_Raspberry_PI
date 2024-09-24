#include "PCT2075.h"

PCT2075::PCT2075(/* args */)
{
    _temperature = 0;
    _adress = 0;
}

void PCT2075::begin(uint8_t adress)
{
    _adress = adress;
    Wire.begin();
}

float PCT2075::get_temp()
{
    uint8_t MSB;
    uint8_t LSB;
    uint16_t TwoBytes;
    TwoBytes = 0;
    int sign = +1;
    float temperature = 0;

    Wire.beginTransmission(_adress);
    Wire.write(R_temp);
    Wire.endTransmission();
    //
    Wire.requestFrom(_adress, byte(2));
    MSB = Wire.read();
    LSB = Wire.read();
    Wire.endTransmission();

    TwoBytes = ((MSB << 8)+ LSB) >> 5;
    if ((MSB & 0x80) != 0)
    {
        temperature = TwoBytes ^ 0xffff;
        temperature = -(temperature + 0x01) * 0.125;
    }
    else
    {
        temperature = TwoBytes * 0.125;
    }
    _temperature = temperature;
    return temperature;
}
