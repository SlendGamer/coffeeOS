/*!
    @file     menu.cpp
    @author   Tim Prüß

    Contains initilizations of member functions.
*/

// project includes
#include "coffeeOS.h"
#include "menu.h"
#include "config.h"
#include "product.h"

// extern includes
#include <Adafruit_PN532.h>

// standard library includes
#include <unordered_map>

/*!
    @brief  Initializes an instance of class menu.
*/
void menu::init() {
    OBDISP& oled = CoffeeOS::instance().oled_display;
    
    temp_turn = false; // force stop after each turn: one turn click -> one new product
    menu_state = 0; // the selected menu item
    time_stamp = time(nullptr); // after 10s go back to home screen
    select_mode = false; // true when user is selecting product (for 10s)
    clicked = false;

    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char*) "menu init", FONT_6x8, 0, 1);
    delay(1000);
}

/*!
    @brief  Detects whether an encoder event (turn, press) took place and handles further steps.
*/
void menu::on_encoder_event() {
    if (clicked) return; // check if user has selected an item and clicked on it
    if (digitalRead(ROT_BUTTON) == LOW) {
        on_click();
        clicked = true;
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

/*!
    @brief  Handles next actions on a left rotation of the rotary encoder.
*/
void menu::on_left_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == 0) menu_state = CoffeeOS::instance().my_manager.product_list.size();
    menu_state = menu_state - 1;
    temp_turn = true;
}

/*!
    @brief  Handles next ctions on a right rotation of the rotary encoder.
*/
void menu::on_right_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == CoffeeOS::instance().my_manager.product_list.size() - 1) menu_state = 0;
    menu_state = menu_state + 1;
    temp_turn = true;
}

/*!
    @brief  Handles next actions on a click of the rotary encoder button.
*/
void menu::on_click() {
    if (!select_mode) return;
    time_stamp = time(nullptr);

    OBDISP& obd_t = CoffeeOS::instance().oled_display;
    const char* name = CoffeeOS::instance().my_manager.product_list.at(menu_state).name.c_str();
    double& price = CoffeeOS::instance().my_manager.product_list.at(menu_state).price;
    int& last_user = CoffeeOS::instance().last_user;
    int& last_product = CoffeeOS::instance().last_product;

    obdFill(&obd_t, 0, 1);
    obdWriteString(&obd_t, 0, 0, 2*8, (char *) "You selected: ", FONT_8x8, 0, 1);
    obdWriteString(&obd_t, 0, 0, 4*8, (char *) name, FONT_8x8, 0, 1);

    // set precision of double to 2 digits
    std::string num_text = std::to_string(price);
    const char* price_rounded = ("Price: " + num_text.substr(0, num_text.find('.') + 3) + " Euro").c_str();
    obdWriteString(&obd_t, 0, 0, 6*8, (char *) price_rounded, FONT_8x8, 1, 1);

    last_product = menu_state;
    last_user = CoffeeOS::instance().my_manager.get_user_id_from_nfc();

    if (last_user == -1) return;
    CoffeeOS::instance().my_manager.add_user_amount(last_user, price);
}

/*!
    @brief  Sets variables to exit the menu.
*/
void menu::exit_menu() {
    clicked = false;
    temp_turn = false;
    select_mode = false;
}

/*!
    @brief  Handles actions on a left rotation of the rotary encoder.
    @return Menu state (integer) of current menu point. 
*/
int menu::get_menu_state() const {
    return menu_state;
}

/*!
    @brief  Get function for the timestamp member variable.
    @return Timestamp.
*/
time_t menu::get_timestamp() const {
    return time_stamp;
}

/*!
    @brief  Get function for the current product.
    @return Object of class product.
*/
product menu::get_current_product() const {
    return CoffeeOS::instance().my_manager.product_list.at(menu_state);
}
