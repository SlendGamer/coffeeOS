

#include "coffeeOS.h"

#include "menu.h"
#include "config.h"
#include "product.h"
#include "stringhelper.h"

#include <unordered_map>
#include <Adafruit_PN532.h>

void menu::on_encoder_event() {
    if (clicked) return; // check if user has selected an item and clicked on it
    if (digitalRead(ROT_BUTTON) == LOW) {
        on_click();
        return;
    }
    if (digitalRead(ROT_CLK) == LOW) {
        select_mode = true;
        on_left_rotation();
        return;
    }
    if (digitalRead(ROT_DT) == LOW) {
        select_mode = true;
        on_right_rotation();
        return;
    }
    temp_turn = false;
}

void menu::on_left_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == 0) menu_state = CoffeeOS::instance().my_manager.get_product_list().size();
    menu_state = menu_state - 1;
    temp_turn = true;
}

void menu::on_right_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == CoffeeOS::instance().my_manager.get_product_list().size()-1) menu_state = 0;
    menu_state = menu_state + 1;
    temp_turn = true;
}

void menu::on_click() {
    if (!select_mode) return;
    time_stamp = time(nullptr);
    OBDISP obd_t = CoffeeOS::instance().oled_display;
    std::string name = CoffeeOS::instance().my_manager.get_product_list().at(menu_state).name;

    obdFill(&obd_t, 0, 1);
    obdWriteString(&obd_t, 0, 0, 2*8, (char *)"You selected: ", FONT_8x8, 0, 1);
    obdWriteString(&obd_t, 0, 0, 4*8, (char *)name.c_str(), FONT_8x8, 0, 1);

    // set precision of double to 2 digits
    std::string num_text = std::to_string(CoffeeOS::instance().my_manager.get_product_list().at(menu_state).price);
    std::string rounded = num_text.substr(0, num_text.find('.')+3);
    obdWriteString(&obd_t, 0, 0, 6*8, (char *)("Price: " + rounded + " Euro").c_str(), FONT_8x8, 0, 1);
    clicked = true;
}

void menu::exitMenu() {
    clicked = false;
    temp_turn = false;
    select_mode = false;
}

int menu::getMenuState() const {
    return menu_state;
}

time_t menu::getTimeStamp() const {
    return time_stamp;
}

bool menu::isTurned() const {
    return temp_turn;
}

bool menu::hasClicked() const {
    return clicked;
}

void menu::setClicked(bool c) {
    clicked = c;
}

std::string menu::getCurrentProduct() const {
    return CoffeeOS::instance().my_manager.get_product_list().at(menu_state).name;
}

void menu::init() {
    temp_turn = false;                      // force stop after each turn: one turn click -> one new product
    menu_state = 0;                          // the selected menu item
    time_stamp = time(nullptr);     // after 10s go back to home screen
    select_mode = false;                    // true when user is selecting product (for 10s)
    clicked = false;
}

