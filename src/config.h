#pragma once

#include "coffeeOS.h"

class Config {
public:
    WLAN *wlanConfig;
    Product **productList;
    User **userList;

    /* OLED-Config */
    uint8_t oled_sda; // OLED SDA-pin (serial data)
    uint8_t oled_scl; // OLED SCL-pin (serial clock)
    uint8_t oled_addr; // OLED I2C-Bus-Address
    bool flip180Vert; // flip contents of OLED vertically
    bool oled_invert; // invert pixels of OLED
    bool oled_useHWi2c; // use Hardware- or Software-I2C

    /* RFID-Config */
    uint8_t rfid_sda;
    uint8_t rfid_scl;
    uint8_t rfid_irq;
    uint8_t rfid_rst;

    /* Rotary-Encoder-Config */
    uint8_t rotaryEnc_clk;
    uint8_t rotaryEnc_btn;
    bool rotaryEnc_press;

    void export();
    void import();
};