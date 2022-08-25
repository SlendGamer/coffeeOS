#pragma once

//#include "cli.h" // command-line-interface library
#include "const.h"
#include "config.h"
#include "product.h"
#include "user.h"
#include "transaction.h"

#include <Arduino.h>
#include <stdint.h>
#include <string.h>

#include <Wire.h>
#include <WiFi.h>
#include <ESPmDNS.h>
#include <Preferences.h>

#include <ESP32Encoder.h>
#include <OneBitDisplay.h>
#include <Adafruit_PN532.h>

#define DEBUG
//#define HOME

// Variablen deklarieren/ initialisieren
unsigned char menuID = 0;
char sTemp[20];

unsigned long lastMillis = 0;
bool pressConfirm = false, onOff;

int16_t brightness = 255;
float balance;

static uint8_t ucBackBuffer[1024];

#ifndef HOME
const char *initial_ssid = "FRITZ!Box 7490";
const char *initial_password = "+Feuerwehr-dbkt1";
#else
const char *initial_ssid = "Feldfunkstation 1";
const char *initial_password = "52837754105258160424";
#endif

String header;

typedef struct
{
  char c_name[10];
  uint8_t cardUID[4];
  uint8_t memory[18];
} user_t;

user_t *user;
TaskHandle_t Task1;

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
    (char *)"Settings", NULL};

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
    4.200};

// Settings-Menu
char *menu1[] = {
    (char *)"<<<<",
    (char *)"User-Info",
    (char *)"User-Reset",
    (char *)"Netzwerk-Info",
    (char *)"Helligkeit", NULL};

// neue Instantzen erzeugen
Adafruit_PN532 nfc(PN532_IRQ, PN532_RST);
Preferences pref;
WiFiServer server(80);
ESP32Encoder encoder;
SIMPLEMENU menu;
OBDISP obd;

void confirm();
void saveUsers(user_t *);
void loadUsers(user_t **);
int8_t getElements(user_t *, uint8_t *);
uint8_t centerOffset(char *, uint8_t);

template<typename T>
int elementCount(T**);

class CoffeeOS;
class Config;
class User;
class Transaction;
class Product;
class Menu;
class WLAN;

class CoffeeOS {
public:
    Config *config;
    Menu *menu;

    CoffeeOS();
    static CoffeeOS& instance()
    {
        static CoffeeOS& instance();
        return instance();
    }

    // boot begins from here
    void setup();
    void loop();
    void reset();

    void wlan_init();
    void i2c_init();
    void display_init();
    void rotaryEncoder_init();
    void nfcReader_init();
};

class Menu {
public:

    void startupScreen();
};

class WLAN {
public:
    String wlan_ssid;
    String wlan_pass;
};
