#include <Arduino.h>
#include "Ticker.h"
#include "DHT22.h"
#include "PCT2075.h"
#include "MATRIX.h"
#include "ArduinoGraphics.h"
#include <ArduinoJson.h>
#include <ArduinoMqttClient.h>

#include <Wire.h>
#include "Adafruit_SSD1306.h"
#include "Adafruit_GFX.h"
#include "Widget.h"
#include <WiFiS3.h>

// *************************** const ***************************
#define LED_R 3 // led bouton
const uint8_t LED_Button = LED_R;
#define LED_G 4 // led wifi
#define LED_B 5
#define PERIOD 3000
#define A_PCT2075 0x37

// mise en place JSON
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

#define actionPeriod 3000

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define A_OLED 0x3C
#define OLED_RESET -1

//*  Constantes wifi
const uint8_t LED_WIFI_PIN = LED_G;
const char *ssid = "Linksys01370";
const char *pwd = "3fanq5w4pb";

// mqtt params
const char *mqtt_server = "192.168.1.125";
const int MQTT_PORT = 1883;
const char *mqtt_topic_led = "Uno413/led";
const char *mqtt_topic_datas = "Uno413/datas";

//* ************************************* Variables **************************************
float TEMPERATURE;
float HUMIDITY;
float DEWPOINT;
String DHT_STATUS;
uint8_t condition;

bool isWifiConnection = false; // Variable gobale pour le wifi
String matrix_text;            // référence au film

WiFiClient wifiClient;
MqttClient mqttClient(wifiClient);
//* ************************************* prototypes *************************************
void action();
void create_json();
void display_matix();
/**
 * mettre en loop parcqu'on les donnees peuvent arriver a n'imp quel moment
 */
void led_deserialize();

void connectWifi();
void display_TextOnMatrix(String text);

void mqttConnection();
void mqttMessageHandler(int messageSize);
void mqttSendMessage();

//* ************************************* obj **************************************
DHT22 dht22;
PCT2075 pct2075;
Ticker timer4(action, actionPeriod);
ArduinoLEDMatrix matrix;
Adafruit_SSD1306 oled(SCREEN_WIDTH, SCREEN_HEIGHT);
GFXcanvas1 canvas(SCREEN_WIDTH, SCREEN_HEIGHT);
Gauge gauge(&canvas, 64, 64, 60);
Gauge_LUX gauge_lux(&canvas);
Gauge_hum gauge_hum(&canvas);

// on va remplir le json
JsonDocument json; // json
JsonDocument json_led;
JsonDocument mqtt_msg; // json msg mqtt recu

//* ********************************** SETUP **************************************
void setup()
{
  Serial.begin(115200);
  Wire.begin();
  matrix.begin();

  pinMode(LED_R, OUTPUT);
  pinMode(LED_G, OUTPUT);
  pinMode(LED_B, OUTPUT);

  digitalWrite(LED_G, 0);
  digitalWrite(LED_R, 0);
  digitalWrite(LED_B, 1);

  //
  connectWifi();
  delay(2000);

  if (isWifiConnection)
  {
    mqttConnection();
  }

  oled.begin(SSD1306_SWITCHCAPVCC, A_OLED);
  oled.display();
  delay(2000);
  oled.clearDisplay();

  dht22.begin(DHT_PIN);
  pct2075.begin(A_PCT2075);

  timer4.start();

  delay(2000);
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

  if (isWifiConnection)
  {
    mqttClient.poll();
  }
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

  create_json();

  static uint8_t condition = 0; // initialisation en variable statique de l'état de l'écran OLED

  switch (condition)
  {
  case 0:
    gauge.draw();
    condition++;
    break;
  case 1:
    gauge_hum.draw();
    condition++;
    break;
  case 2:
    gauge_lux.draw();
    condition = 0; // On revient à 0 à partir de ce moment
    break;

  default:
    condition = 0; // Valeur par défaut
    break;
  }

  // gauge_hum.draw();
  // gauge_lux.draw();

  oled.drawBitmap(0, 0, canvas.getBuffer(), SCREEN_WIDTH, SCREEN_HEIGHT, 1, 0);
  oled.display();

  mqttSendMessage();
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

  // serializeJson(json, Serial);
  // Serial.println();
}

void display_matix() // not used
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

void display_TextOnMatrix(String text)
{

  matrix.beginDraw();
  matrix.stroke(0xFFFFFFFF);
  matrix.textScrollSpeed(50);
  matrix.textFont(Font_5x7);
  matrix.beginText(11, 1, 0xFFFFFF);
  matrix.println(text);
  matrix.endText(SCROLL_LEFT);
  matrix.endDraw();
}

void led_deserialize()
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

void connectWifi()
{
  uint8_t cmp = 0;
  int status = WL_IDLE_STATUS;
  //
  digitalWrite(LED_WIFI_PIN, 0);
  isWifiConnection = false;
  // check wifi module
  if (WiFi.status() == WL_NO_MODULE)
  {
    display_TextOnMatrix("No Wifi module, impossible connection");
  }
  //
  display_TextOnMatrix("Try wifi connection ");
  status = WiFi.begin(ssid, pwd);

  while (status != WL_CONNECTED)
  {
    Serial.print(".");
    delay(1000);
    cmp++;
    if (cmp > 15)
    {
      display_TextOnMatrix("Failed Wifi connection !");
      isWifiConnection = false;
      return;
    }
  }
  //
  IPAddress ip = WiFi.localIP();
  display_TextOnMatrix(WiFi.localIP().toString());
  digitalWrite(LED_WIFI_PIN, 1);
  isWifiConnection = true;
}

void mqttConnection()
{
  if (isWifiConnection == true)
  {
    mqttClient.setId("uno413");
    Serial.println("try to connect to the MQTT Broker");
    //
    if (mqttClient.connect(mqtt_server, MQTT_PORT) == false)
    {
      Serial.println("Failed MQTT Connection:");
      Serial.println(mqttClient.connectError());
      return;
    }
    //
    Serial.println("Connection MQTT OK");
    mqttClient.onMessage(mqttMessageHandler);
    mqttClient.subscribe(mqtt_topic_led);
  }
  else
  {
    Serial.println("Error MQTT no wifi connection");
  }
}

void mqttMessageHandler(int messageSize)
{
  Serial.println("Received message");
  Serial.println(mqttClient.messageTopic());
  String msg = mqttClient.readString();
  Serial.print(msg);
  // todo with msg
  DeserializationError error = deserializeJson(mqtt_msg, msg);

  if (error)
  {
    Serial.print("deserializeJson() failed: ");
    Serial.println(error.c_str());
    return;
  }
  String LED_STATUS = mqtt_msg["LED_STATUS"]; // "ON"
  const char *LED_PIN = mqtt_msg["LED_PIN"];  // "3"
  if (LED_STATUS.equals("ON"))
  {
    digitalWrite(LED_Button, 1);
  }
  else
  {
    digitalWrite(LED_Button, 0);
  }
}

void mqttSendMessage()
{
  char thebuffer[200] = {"\0"};

  json["board"] = "uno413";
  json["dhtStatus"] = DHT_STATUS;
  json["temperature"] = (int)TEMPERATURE;
  json["humidity"] = (int)HUMIDITY;
  json["dewPoint"] = (int)DEWPOINT;
  json["ledstate"] = digitalRead(LED_G);
  json.shrinkToFit(); // optional
  serializeJson(json, thebuffer);
  // todo create the json with ArduinoJson
  //  place the result in thebuffer
  // send the msg
  mqttClient.beginMessage(mqtt_topic_datas);
  mqttClient.print(thebuffer);
  mqttClient.endMessage();
}