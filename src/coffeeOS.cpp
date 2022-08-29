
// project includes
#include "coffeeOS.h"
#include "config.h"
#include "product.h"
#include "stringhelper.h"

// extern includes
#include <Arduino.h>
#include <Adafruit_PN532.h>
#include <ESP32Encoder.h>
#include <chrono>
#include <unordered_map>

bool reset = true;

/*! @brief This function initializes the rotary encoder.
 */
void CoffeeOS::rotary_encoder_init() {
    pinMode(ROT_BUTTON, INPUT_PULLUP);
    pinMode(ROT_CLK, INPUT);
    pinMode(ROT_DT, INPUT);

    ESP32Encoder::useInternalWeakPullResistors = UP;
    my_encoder.attachFullQuad(ROT_CLK, ROT_DT);
    my_encoder.clearCount();
    my_encoder.pauseCount();
}

void CoffeeOS::display_init() {
    char *msgs[] = {(char *)"SSD1306 @ 0x3C", (char *)"SSD1306 @ 0x3D", (char *)"SH1106 @ 0x3C", (char *)"SH1106 @ 0x3D"};

    int checkDisplay = obdI2CInit(&oled_display, OLED_128x64, -1, 0, 0, 1, -1, -1, -1, 800000L);
    obdSetContrast(&oled_display, brightness_t);
    obdSetBackBuffer(&oled_display, ucBuffer);
    obdSetTextWrap(&oled_display, 1);

    obdFill(&oled_display, 0, 1);
    obdWriteString(&oled_display, 0, 0, 1*8, msgs[checkDisplay], FONT_8x8, 0, 1);
    delay(4000);
    obdFill(&oled_display, 0, 1);
}

void CoffeeOS::nfc_reader_init() {
    Adafruit_PN532 reader = CoffeeOS::instance().my_nfc_reader;
    reader.begin();

    uint32_t versiondata = reader.getFirmwareVersion();
    if (!versiondata) {
        DEBUG_PRINTLN("PN532 not found!");
        obdWriteString(&oled_display, 0, 0, 2*8, (char *) "PN532 not found!", FONT_6x8, 0, 1);
        delay(2000);
        return;
    }

    DEBUG_PRINTLN("PN532 found");
    obdWriteString(&oled_display, 0, 0, 2*8, (char *)"PN532 init", FONT_6x8, 0, 1);

    reader.setPassiveActivationRetries(0xFF); // set maximum retries
    reader.SAMConfig();
    DEBUG_PRINTLN("PN532 configured");
    obdWriteString(&oled_display, 0, 0, 3*8, (char *)"PN532 config", FONT_6x8, 0, 1);
}

void CoffeeOS::boot_screen() {
    obdFill(&oled_display, 0, 1);
    obdWriteString(&oled_display, 0, 10, 2*8, (char *)"ESP32 - ", FONT_8x8, 0, 1);
    obdWriteString(&oled_display, 0, 10, 4*8, (char *)"Coffee - ", FONT_8x8, 0, 1);
    obdWriteString(&oled_display, 0, 10, 6*8, (char *)"inator  ", FONT_8x8, 0, 1);
}

void CoffeeOS::setup() {

    DEBUG_SERIAL_BEGIN(115200);

    rotary_encoder_init();
	display_init();
    nfc_reader_init();

	boot_screen();

    mySaver.saver_init();
    myMenu.setup();
    myManager.init();

    user u = myManager.get_user_list().at(0);
    Serial.print(("User: " + u.firstName + "; To pay: " + std::to_string(u.get_amount())).c_str());

}

void CoffeeOS::loop() {
    std::string prev_prod_name = myMenu.getCurrentProduct();
    myMenu.onRotateEvent();

    if ((std::time(nullptr) - myMenu.getTimeStamp()) > 10) {
        if (reset) return; // check if start up was already drawn

        myMenu.exitMenu();
        boot_screen();

        reset = true;
    } else {

        std::string curr_prod_name = myMenu.getCurrentProduct();
        if (prev_prod_name == curr_prod_name) return; // check if current name was already drawn

        int xShift = (15 - strlen(curr_prod_name.c_str()));
        obdFill(&oled_display, 0, 1);
        obdWriteString(&oled_display, 0, 8*(xShift/2), 4*8, (char *)curr_prod_name.c_str(), FONT_8x8, 0, 1);

        reset = false;
    }
}

