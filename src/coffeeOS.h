#pragma once

// project includes
#include "menu.h"
#include "manager.h"
#include "saver.h"

// extern includes
#include <ESP32Encoder.h>
#include <OneBitDisplay.h>
#include <Adafruit_PN532.h>

#define EEPROM_SIZE 512

#define DEBUG

#ifdef DEBUG
#   define DEBUG_SERIAL_BEGIN(x)    Serial.begin(x)
#   define DEBUG_PRINTLN(x)         Serial.println(x)
#   define DEBUG_PRINT(x)           Serial.print(x)
#else
#   define DEBUG_SERIAL_BEGIN(x)    ()
#   define DEBUG_PRINTLN(x)         ()
#   define DEBUG_PRINT(x)           ()
#endif

// coffeeOS class: microcontroller firmware for the project
class CoffeeOS {
public:
    // config variables
    int brightness_t = 255;
    uint8_t ucBuffer[1024]{};

    // component modules instances
    Adafruit_PN532 my_nfc_reader;
    ESP32Encoder my_encoder;
    OBDISP oled_display{};

    // firmware modules instances
    menu myMenu;
    saver mySaver;
    manager myManager;

    void setup(); // gets executed once at boot
    void loop(); // gets repeatedly executed after setup

    explicit CoffeeOS(Adafruit_PN532 my_nfc_reader) : my_nfc_reader(my_nfc_reader) {
        this->my_nfc_reader = my_nfc_reader;
    }

    static CoffeeOS &instance() {
        static CoffeeOS instance({2, 3});
        return instance;
    }

private:
    void display_init();
    void rotary_encoder_init();
    void nfc_reader_init();
    void boot_screen();
};

