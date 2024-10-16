#include <Arduino.h>
#include "Ticker.h"
#include "DHT22.h"
#include "PCT2075.h"
#include "MATRIX.h"
#include "ArduinoGraphics.h"
#include <ArduinoJson.h>
#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"

// *************************** const ***************************
#define LED_R 3
#define LED_G 4
#define PERIOD 3000
#define A_PCT2075 0x37

/* ---------------------- mise en place JSON ----------------------*/
#define ARDUINOJSON_SLOT_ID_SIZE 1
#define ARDUINOJSON_STRING_LENGTH_SIZE 1
#define ARDUINOJSON_USE_DOUBLE 0
#define ARDUINOJSON_USE_LONG_LONG 0
const uint8_t DHT_PIN = 7;
#define MAX_INPUT_LENGTH 50

byte frame[8][12] = {
    {1, 1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0},
    {1, 0, 1, 0, 0, 0, 0, 0, 0, 1, 1, 0},
    {0, 1, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 1, 0, 1, 0, 0, 0, 1, 0, 0, 0},
    {0, 0, 0, 1, 0, 1, 1, 0, 0, 1, 1, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0},
    {0, 0, 0, 0, 0, 1, 1, 0, 0, 0, 0, 0}};

#define actionPeriod 1000

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define A_OLED 0x3C
#define OLED_RESET -1

//* ************************************* Variables **************************************
float TEMPERATURE;
float HUMIDITY;
float DEWPOINT;
String DHT_STATUS;

//* ************************************* prototypes *************************************
void action();
void create_json();
void display_matix();
/**
 * mettre en loop parcqu'on les donnees peuvent arriver a n'omp quel moment
 */
void led_deserialize();

//* ************************************* obj **************************************
DHT22 dht22;
PCT2075 pct2075;
Ticker timer4(action, actionPeriod);
ArduinoLEDMatrix matrix;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
Gauge gauge(&canvas, 64, 64, 60);

// on va remplir le json
JsonDocument json; // json
JsonDocument json_led;

//* ********************************** SETUP **************************************
void setup()
{
  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  digitalWrite(LED_G, 0);
  digitalWrite(LED_R, 1);
  Serial.begin(115200);
  Wire.begin();

  dht22.begin(DHT_PIN);
  pct2075.begin(A_PCT2075);

  timer4.start();

  matrix.begin();
  display();

  if (!oled.begin(SSD1306_SWITCHCAPVCC, A_OLED))
  {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;)
      ; // Don't proceed, loop forever
  }
}

//* ************************** MAIN LOOP *******************************

void loop()
{
  timer4.update();
  // display_matix();
  // while (Serial.available()) //Tant que les données sont disponibles
  // {
  //   led_deserialize();
  // }
}
//* ************************** fonction **************************
void action()
{
  uint8_t status = 0;
  status = dht22.readValues();
  TEMPERATURE = dht22.getTemperature() + pct2075.get_temp();
  TEMPERATURE /= 2.0;
  HUMIDITY = dht22.getHumidity();
  DEWPOINT = dht22.getDewPoint();

  if (status > 0)
  {
    DHT_STATUS = "DHT_ERROR";
    TEMPERATURE = 0;
    HUMIDITY = 0;
    DEWPOINT = 0;
  }
  else
  {
    DHT_STATUS = "DHT_OK";
  }

  // digitalWrite(LED_G, !digitalRead(LED_G));
  // digitalWrite(LED_R, !digitalRead(LED_R));

  matrix.renderBitmap(frame, 8, 12);

  create_json();
  gauge.draw();
}

void create_json()
{
  json["board"] = "uno413";
  json["dhtStatus"] = DHT_STATUS;
  json["temperature"] = (int)TEMPERATURE;
  json["humidity"] = (int)HUMIDITY;
  json["dewPoint"] = (int)DEWPOINT;
  json["ledstate"] = digitalRead(LED_G);

  json.shrinkToFit(); // optional

  serializeJson(json, Serial);
  Serial.println();
}

void display_matix()
{
  matrix.beginDraw();

  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);

  char text[50];

  sprintf(text, "     T : %.2f °C    H : %.2f     D : %.2f ", TEMPERATURE, HUMIDITY, DEWPOINT);
  matrix.textFont(Font_5x7);
  matrix.beginText(0, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);

  matrix.endDraw();
}

void led_deserialize() // On voulait mettre la fonction dans le ticker "action", mais il faut le mettre dans un endroit où on est sur qu'il ne perdra pas les caractères envoyés pour lui. Donc, on mettra la fonction dans le loop.
{

  char input[MAX_INPUT_LENGTH];

  DeserializationError error = deserializeJson(json_led, input, MAX_INPUT_LENGTH);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }

  const char *LED_STATUS = json_led["LED_STATUS"]; // "ON"
  const char *LED_PIN = json_led["LED_PIN"];       // "0"

  if (LED_STATUS == "ON")
  {
    digitalWrite(LED_G, 1);
  }
  else if (LED_STATUS == "OFF")
  {
    digitalWrite(LED_G, 0);
  }
}