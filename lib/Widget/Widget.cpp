#include <Widget.h>

Gauge::Gauge(GFXcanvas1 *pcanvas, uint16_t centerX, uint16_t centerY, uint16_t radius)
{
    _radius = radius;
    _centerX = centerX;
    _centerY = centerY;
    _canvas = pcanvas;
    _temp = 0;
}

void Gauge ::_drawBackground()
{
    float xx = 0;
    float yy = 0;
    uint8_t i = 60;

    /* affichage 0 de gauge */
    _canvas->setCursor(0, 57);
    _canvas->setTextSize(1);
    _canvas->setTextColor(WHITE);
    _canvas->println(0);
    /* affichage 0 de gauge */

    for (double teta = 0.0; teta > -1.0 * PI; teta -= PI / 12.0) // if i use float teta only 0*PI works in the rest numbers are off.
    {
        xx = _calculX(_radius, teta);
        yy = _calculY(_radius, teta);

        if (i < 0)
        {
            i = 60;
        }

        else if (i % 10)
        {
            _canvas->drawCircle(xx, yy, 1, WHITE);
        }
        else
        {
            if (i == 60)
            {
                _canvas->setCursor(xx - 8, yy - 8);
                _canvas->setTextSize(1);
                _canvas->setTextColor(WHITE);
                _canvas->println(i);
            }
            else
            {
                _canvas->setCursor(xx - 3, yy - 4);
                _canvas->setTextSize(1);
                _canvas->setTextColor(WHITE);
                _canvas->println(i);
            }
        }
        i -= 5;
    }
    _canvas->fillCircle(_centerX, _centerY - 1, 25, WHITE);

    _canvas->setCursor(_centerX + 6, _centerY - 14);
    _canvas->setTextSize(1);
    _canvas->setTextColor(BLACK);
    _canvas->print((char)247);
    _canvas->setTextSize(2);
    _canvas->setCursor(_centerX + 12, _centerY - 14);
    _canvas->print("C");
}

void Gauge ::draw()
{
    _canvas->fillScreen(0);
    _drawBackground();
    _drawtemp();
}

void Gauge ::_drawtemp()
{

    float temp = TEMPERATURE;

    float angle_s = PI + (PI / 60) * temp;
    // _canvas->drawLine(_centerX, _centerY, _calculX(_radius, angle_s), _calculY(_radius, angle_s), WHITE);

    _canvas->fillTriangle(_calculX(_radius, angle_s), _calculY(_radius, angle_s),
                          _calculX(5, angle_s + PI / 2), _calculY(5, -angle_s + PI / 2),
                          _calculX(5, angle_s - PI / 2), _calculY(5, angle_s - PI / 2), WHITE);

    _canvas->setCursor(_centerX + -22, _centerY - 8);
    _canvas->setTextSize(1);
    _canvas->setTextColor(BLACK);
    _canvas->print(temp, 2);
}
float Gauge ::_calculX(float rayon, float angle)
{
    return _centerX + (rayon * cos(angle));
}
float Gauge ::_calculY(float rayon, float angle)
{
    return _centerY + (rayon * sin(angle));
}

Gauge_LUX::Gauge_LUX(GFXcanvas1 *pcanvas)
{
    _lux = 0;
    _canvas = pcanvas;
}

void Gauge_LUX ::_drawBackground()
{
    _canvas->setCursor(43, 0);
    _canvas->setTextSize(1);
    _canvas->setTextColor(WHITE);
    _canvas->print("Dewpoint");

    _canvas->drawRect(9, 10, 110, 15, WHITE);

    _canvas->setRotation(1);
    for (int i = 0; i <= 50; i += 5)
    {

        _canvas->setCursor(28, 116 - (11 * (i / 5)));
        // Position verticale ajustée pour chaque itération       
        _canvas->setTextSize(1);
        _canvas->print(String(i)); // Conversion de l'entier en chaîne de caractères pour l'affichage       
        if (i < 50)
        {
            _canvas->drawLine(10, 118 - (11 * (i / 5)), 24, 118 - (11 * (i / 5)), WHITE);
        }
    }
    _canvas->setRotation(0);
}

void Gauge_LUX ::draw()
{
    _canvas->fillScreen(0);
    _drawBackground();
    _drawlux();
}

void Gauge_LUX ::_drawlux()
{
    float hum = 0;
    hum = DEWPOINT;

    int longueur = (index_pixel_lux)*hum;

    _canvas->fillRect(9, 10, longueur, 15, WHITE);

    _canvas->setCursor(49, 56);
    _canvas->setTextSize(1);
    _canvas->setTextColor(WHITE);
    _canvas->print(hum);
}


Gauge_hum::Gauge_hum(GFXcanvas1 *pcanvas)
{
    _hum = 0;
    _canvas = pcanvas;
}

void Gauge_hum ::_drawBackground()
{
    _canvas->setCursor(43, 0);
    _canvas->setTextSize(1);
    _canvas->setTextColor(WHITE);
    _canvas->print("HUMIDITY");

    _canvas->drawRect(9, 10, 110, 15, WHITE);

    _canvas->setRotation(1);
    for (int i = 0; i <= 100; i += 10)
    {

        _canvas->setCursor(28, 116 - (11 * (i / 10)));
        // Position verticale ajustée pour chaque itération       
        _canvas->setTextSize(1);
        _canvas->print(String(i)); // Conversion de l'entier en chaîne de caractères pour l'affichage       
        if (i < 100)
        {
            _canvas->drawLine(10, 118 - (11 * (i / 10)), 24, 118 - (11 * (i / 10)), WHITE);
        }
    }
    _canvas->setRotation(0);
}

void Gauge_hum ::draw()
{
    _canvas->fillScreen(0);
    _drawBackground();
    _drawhum();
}

void Gauge_hum ::_drawhum()
{
    float dewpoint = 0;
    dewpoint = HUMIDITY;

    int longueur = (index_pixel_hum)*dewpoint;

    _canvas->fillRect(9, 10, longueur, 15, WHITE);

    _canvas->setCursor(49, 56);
    _canvas->setTextSize(1);
    _canvas->setTextColor(WHITE);
    _canvas->print(dewpoint);
}