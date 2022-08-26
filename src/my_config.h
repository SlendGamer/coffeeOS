#pragma once

//#define ENABLE_WIFI

#define MY_CONFIG

#ifdef MY_CONFIG
    #define INITIAL_WLAN_SSID "owo-uwu-rawr"
    #define INITIAL_WLAN_PASS "Constant-Frightful-Straining-Lettuce-Sabotage"
#endif

#define OLED_SDA 21
#define OLED_SCL 22

#define PN532_SDA 16
#define PN532_SCL 17
#define PN532_IRQ 5
#define PN532_RST 18

#define ROT_CLK 32
#define ROT_DT 35
#define ROT_BUTTON 34
#define ROT_BUTTON_PRESS LOW

#define RESET_PIN -1
#define OLED_ADDR 0x3C
#define FLIP180 0
#define INVERT 0
#define USE_HW_I2C 0

#define MAX_USERS 20 // TODO delete this later if not needed
#define ARTICLES 18 // TODO delete this later if not needed