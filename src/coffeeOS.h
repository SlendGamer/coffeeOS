/*!
    @file     coffeeOS.h
    @author   Tim Prüß

    Contains the declarations of class, members (functions) and pre-processor defines.
*/

#pragma once

// project includes
#include "menu.h"
#include "manager.h"
#include "wifi_handler.h"

// extern includes
#include <ESP32Encoder.h>
#include <OneBitDisplay.h>
#include <Adafruit_PN532.h>

// I2C defines
#define SDA_PIN 21
#define SCL_PIN 22

// PN532 defines
#define PN532_IRQ 5
#define PN532_RST 18
#define PN532_MOSI 13
#define PN532_MISO 6
#define PN532_SCK 14
#define PN532_CS 15

// rotary encoder defines
#define ROT_CLK 32
#define ROT_DT 35
#define ROT_BUTTON 34
#define ROT_BUTTON_PRESS LOW

// OLED defines
#define OLED_SDA SDA_PIN
#define OLED_SCL SCL_PIN
#define OLED_RESET_PIN -1
#define OLED_ADDR 0x3C
#define OLED_FLIP_180 0
#define OLED_INVERT 0
#define OLED_USE_HW_I2C 1

// build defines
#define DEBUG
#define USE_MY_CONFIG

#ifdef USE_MY_CONFIG
#   define INITIAL_WLAN_SSID "owo-uwu-rawr"
#   define INITIAL_WLAN_PASS "Constant-Frightful-Straining-Lettuce-Sabotage"
#endif

#ifdef DEBUG
#   define DEBUG_SERIAL_BEGIN(x)    Serial.begin(x)
#   define DEBUG_PRINTLN(x)         Serial.println(x)
#   define DEBUG_PRINT(x)           Serial.print(x)
#else
#   define DEBUG_SERIAL_BEGIN(x)    ()
#   define DEBUG_PRINTLN(x)         ()
#   define DEBUG_PRINT(x)           ()
#endif

/*!
    @brief  Main class for the firmware. Contains important setup and loop functions.
*/
class CoffeeOS {
public:
    void setup(); // gets executed once at boot
    void loop(); // gets executed repeatedly after setup
    void restart(); // reset the microcontroller

    /*!
        @brief Constructs an Object of class CoffeeOS with given parameters.
        @param reader Instance of class Adafruit_PN532
        @param wifi Instance of class wifi_handler
    */
    CoffeeOS(Adafruit_PN532 reader, wifi_handler wifi) : 
        my_nfc_reader(reader),
        my_wifi(wifi) {
    }

    /*!
    @brief  Initializes a static instance of class CoffeeOS that can be called everywhere in the program.
    */
    static CoffeeOS& instance() {
        static CoffeeOS instance(Adafruit_PN532(PN532_IRQ, PN532_RST), wifi_handler(INITIAL_WLAN_SSID, INITIAL_WLAN_PASS));
        return instance;
    }

    // config variables
    int brightness_t = 255; /*! Stores brightness (= contrast) of the OLED-display. */
    uint8_t ucBuffer[1024]{}; /*! Stores image buffer data for the OLED-display. */

    int last_product; /*! Stores which product was purchased most recent. */
    int last_user; /*! Stores which user recently purchased something. */ 

    // component module instances
    Adafruit_PN532 my_nfc_reader; /*! Object of class Adafruit_PN532 */ 
    ESP32Encoder my_encoder; /*! Object of class ESP32Encoder */ 
    OBDISP oled_display{}; /*! Instanace of struct OBDISP */ 

    // firmware module instances
    wifi_handler my_wifi; /*! Object of class wifi_handler */ 
    manager my_manager; /*! Object of class manager */ 
    menu my_menu; /*! Object of class menu */ 

private:
    void display_init();
    void rotary_encoder_init();
    void nfc_reader_init();
    void boot_screen();

};

