//#include "cli.h" // command-line-interface library
#include "my_config.h"
#include "config.h"
#include "product.h"
#include "user.h"
#include "transaction.h"
#include "menu.h"

#include <Arduino.h>
#include <cstdint>
#include <cstring>

#include <Wire.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include <ESP32Encoder.h>
#include <OneBitDisplay.h>
#include <Adafruit_PN532.h>

#define DEBUG

// Variablen deklarieren/ initialisieren
unsigned char menuID = 0;
char sTemp[20];

unsigned long lastMillis = 0;
bool pressConfirm = false, onOff;

int16_t brightness = 255;
float balance;

static uint8_t ucBackBuffer[1024];

const char *initial_ssid = "FRITZ!Box 7490";
const char *initial_password = "+Feuerwehr-dbkt1";

String header;

typedef struct
{
  char c_name[10];
  uint8_t cardUID[4];
  uint8_t memory[18];
} user_t;

user_t *user;
//TaskHandle_t Task1;

// Menu-Items
char *menu0[] = {                  // Artikel-IDs
    (char *)"Kaffee",              // 0
    (char *)"Ristretto",           // 1
    (char *)"Espresso",            // 2
    (char *)"Espresso  Macchiato", // 3
    (char *)"Latte     Macchiato", // 4
    (char *)"Cappuccino",          // 5
    (char *)"CappuccinoItaliano",  // 6
    (char *)"Cafe      Latte",     // 7
    (char *)"Cafe      au lait",   // 8
    (char *)"Cafe      Americano", // 9
    (char *)"Flat      White",     // 10
    (char *)"Long      Black",     // 11
    (char *)"Heisse    Milch",     // 12
    (char *)"Milch-    schaum",    // 13
    (char *)"Schwarzer Tee",       // 14
    (char *)"Gruener   Tee",       // 15
    (char *)"Kraeuter- tee",       // 16
    (char *)"Fruechte- tee",       // 17
    (char *)"Settings",
    nullptr
};

// Preisliste
float prices[] = {
    1.000,
    1.500,
    1.200,
    0.500,
    10.10,
    20.00,
    1.750,
    1.230,
    2.500,
    3.500,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200,
    4.200
};

// declare instances of component objects
// Adafruit_PN532 nfc_reader(PN532_IRQ, PN532_RST);

#ifdef ENABLE_WIFI
WiFiServer server(80);
#endif
ESP32Encoder encoder;
//SIMPLEMENU menu;
OBDISP obd;



void confirm();

template<typename T>
int elementCount(T**);

class CoffeeOS;
class Config;
class User;
class Transaction;
class Product;

class CoffeeOS {
public:
    Config *config{};

    CoffeeOS();
    static CoffeeOS& instance()
    {
        //static CoffeeOS& instance();
        return instance();
    }

    // boot begins from here
    void setup();
    void loop();
    void reset();

    void wlan_init();
    void i2c_init();
    static void display_init();
    static void rotaryEncoder_init();
    static void nfcReader_init();
};

