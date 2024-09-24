#ifndef PCT2075_H
#define PCT2075_H

#include <Arduino.h>
#include <Wire.h>
#define R_config 0x01
#define R_temp 0x00

class PCT2075
{
private:
    /* data */
    float _temperature;
    uint8_t _adress;
public:
    PCT2075(/* args */);
    void begin(uint8_t adress);
    float get_temp();
};

#endif