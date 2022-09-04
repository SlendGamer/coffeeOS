/**************************************************************************
 * Created on May 23 2022
 * Copyright (c) 2022 by Tim Pruess
 **************************************************************************/
/*!
    @file     coffeeOS.cpp
    @author   Tim Prüß

    Contains initilizations of member functions.
*/

// project includes
#include "coffeeOS.h"
#include "product.h"
#include "wifi_handler.h"

// extern includes
#include <Arduino.h>
#include <Wire.h>
#include <SPI.h>
#include <Adafruit_PN532.h>
#include <ESP32Encoder.h>

// standard library includes
#include <string>
#include <chrono>
#include <unordered_map>

bool reset = true;

/*!
    @brief  Gets executed once at boot to run necessary setup functions.

    Contains initilization functions of class CoffeeOS and other classes that execute
    necessary init functions (e.g. Serial.begin() for serial communication with a computer).
*/
void CoffeeOS::setup() {
    DEBUG_SERIAL_BEGIN(115200);
    while(!Serial) delay(10); // wait for serial connection

    Wire.begin(SDA_PIN, SCL_PIN);

    display_init();
    rotary_encoder_init();
    nfc_reader_init();

    my_menu.init();
    my_manager.init();
    my_wifi.init();

    boot_screen();
}

/*!
    @brief  Gets executed repeatedly after the setup function. Contains the actual runtime code.

*/
void CoffeeOS::loop() {
    my_wifi.wifi_client();

    std::string prev_prod_name = my_menu.get_current_product().name;
    my_menu.on_encoder_event();

    if ((std::time(nullptr) - my_menu.get_timestamp()) > 10) {
        if (reset) return; // check if start up was already drawn

        my_menu.exit_menu();
        boot_screen();

        reset = true;
    } 
    else {
        std::string curr_prod_name = my_menu.get_current_product().name;
        if (prev_prod_name == curr_prod_name) return; // check if current name was already drawn

        int xShift = (15 - strlen(curr_prod_name.c_str()));
        obdFill(&oled_display, 0, 1);
        obdWriteString(&oled_display, 0, 8*(xShift/2), 4*8, (char *)curr_prod_name.c_str(), FONT_8x8, 0, 1);

        reset = false;
    }
}

/*!
    @brief  Sets up the rotary encoder for user input.
*/
void CoffeeOS::rotary_encoder_init() {
    pinMode(ROT_BUTTON, INPUT_PULLUP);
    pinMode(ROT_CLK, INPUT);
    pinMode(ROT_DT, INPUT);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    my_encoder.attachFullQuad(ROT_CLK, ROT_DT);

    DEBUG_PRINTLN("rotary_encoder_init(): encoder attached");
    obdFill(&oled_display, 0, 1);
    obdWriteString(&oled_display, 0, 0, 1*8, (char*) "Encoder attached", FONT_6x8, 0, 1);

    delay(1000);

    my_encoder.clearCount();
    my_encoder.pauseCount();
}

/*!
    @brief  Initializes the OLED display (using I2C) for displaying of user-relevant information and instructions.
*/
void CoffeeOS::display_init() {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};

    int checkDisplay = obdI2CInit(&oled_display, OLED_128x64, -1, OLED_FLIP_180, OLED_INVERT, OLED_USE_HW_I2C, OLED_SDA, OLED_SCL, -1, 800000L);

    DEBUG_PRINTLN("display_init(): display initialized");

    obdSetContrast(&oled_display, brightness_t);
    obdSetBackBuffer(&oled_display, ucBuffer);
    obdSetTextWrap(&oled_display, 1);

    obdFill(&oled_display, 0, 1);
    obdWriteString(&oled_display, 0, 0, 1*8, msgs[checkDisplay], FONT_6x8, 0, 1);

    delay(1000);

    obdFill(&oled_display, 0, 1);
}

/*!
    @brief  Initializes the PN532 nfc reader (using I2C).
*/
void CoffeeOS::nfc_reader_init() {

    my_nfc_reader.begin();

    obdFill(&oled_display, 0, 1);
    uint32_t versiondata = my_nfc_reader.getFirmwareVersion();
    if (!versiondata) {
        DEBUG_PRINTLN("nfc_reader_init(): PN532 not found");
        obdWriteString(&oled_display, 0, 0, 1*8, (char *) "PN532 not found!", FONT_6x8, 0, 1);

        delay(1000);
    }
    else {
        DEBUG_PRINTLN("nfc_reader_init(): PN532 found");
        obdWriteString(&oled_display, 0, 0, 1*8, (char *) "PN532 init", FONT_6x8, 0, 1);

        //my_nfc_reader.setPassiveActivationRetries(0xFF); // set maximum retries
        my_nfc_reader.SAMConfig();

        DEBUG_PRINTLN("nfc_reader_init(): PN532 configured");
        obdWriteString(&oled_display, 0, 0, 2*8, (char *) "PN532 config", FONT_6x8, 0, 1);
    }
    
    delay(1000);
}

/*!
    @brief  Displays a boot screen with the project name on the OLED-display.
*/
void CoffeeOS::boot_screen() {
    obdFill(&oled_display, 0, 1);
    obdWriteString(&oled_display, 0, 1, 1*8, (char *) "ESP32", FONT_12x16, 0, 1);
    obdWriteString(&oled_display, 0, 1, 3*8, (char *) "Coffee-", FONT_12x16, 0, 1);
    obdWriteString(&oled_display, 0, 1, 5*8, (char *) "inator", FONT_12x16, 0, 1);
}

/*!
    @brief  Restarts the software of the microcontroller.
*/
void CoffeeOS::restart() {
    DEBUG_PRINT("reset(): restarting in 5s...");
    delay(5000);
    ESP.restart();
}
