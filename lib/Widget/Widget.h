#ifndef WIDGET_H
#define WIDGET_H

#define mPI 3.1415926535
#define mHALF_PI 1.5707963267
#define mTWO_PI 6.2831853071

#define chardegree (char)247
#include <Arduino.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

extern float TEMPERATURE;
extern float HUMIDITY;
extern float DEWPOINT;

#define MAX_Dewpoint 3000
#define longueur_gauge_Dewpoint 110
#define index_pixel_Dewpoint 2.2 // longeur_gauge_lux / MAX_LUX
#define index_pixel_hum 1             // longeur_gauge_lux / MAX_LUX


//*********************
// gauge widget temperature
//*********************
class Gauge
{

public:
    // constructor
    Gauge(GFXcanvas1 *pcanvas, uint16_t centerX, uint16_t centerY, uint16_t radius);
    void draw();
    float _calculX(float rayon, float angle);
    float _calculY(float rayon, float angle);

private:
    void _drawBackground();
    void _drawtemp();
    uint16_t _radius;
    uint16_t _centerX;
    uint16_t _centerY;
    float _temp;
    GFXcanvas1 *_canvas;
};

//*********************
//  widget Dewpoint 
//*********************
class Gauge_Dewpoint
{
public:
    // constructor
    Gauge_Dewpoint(GFXcanvas1 *pcanvas);
    void draw();

private:
    void _drawBackground();
    void _drawDewpoint();
    float _Dewpoint;
    GFXcanvas1 *_canvas;
};

class Gauge_hum
{
public:
    // constructor
    Gauge_hum(GFXcanvas1 *pcanvas);
    void draw();

private:
    void _drawBackground();
    void _drawhum();
    float _hum;
    GFXcanvas1 *_canvas;
};

#endif
