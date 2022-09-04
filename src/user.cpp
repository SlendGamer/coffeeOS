/*!
    @file     user.cpp
    @author   Tim Prüß

    Contains initilizations of member functions.
*/

// project includes
#include "coffeeOS.h"
#include "user.h"
#include "manager.h"

/*!
    @brief Constructs an Object of class user with the given parameters.
    @param identifier Unique number for user identification
    @param first_name First name of the user
    @param last_name Last name of the user
    @param amount Amount of money that the user has to pay
    @param card_id Unique nfc tag id associated with the user  
*/
user::user(int identifier, std::string first_name, std::string last_name, double amount, uint32_t card_id) {
    this->identifier = identifier;
    this->first_name = first_name;
    this->last_name = last_name;
    this->amount = amount;
    this->card_id = card_id;
}

/*!
    @brief Prints user to the OLED-display with a specific format.
*/
void user::print_to_oled() const {
    OBDISP oled = CoffeeOS::instance().oled_display;
    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 0*8, (char* ) ("Identifier: " + std::to_string(identifier)).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char* ) ("First-Name: " + first_name).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 2*8, (char* ) ("Last-Name: " + last_name).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 3*8, (char* ) ("Amount: " + std::to_string(amount)).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 3*8, (char* ) ("Amount: " + std::to_string(card_id)).c_str(), FONT_8x8, 0, 1);
}

/*!
    @brief Prints user to the serial monitor with a specific format.
*/
void user::serial_print() const {
    Serial.printf("\nid: %u\n|-first name: %s\n|-last name: %s\n|-amount: %f\n", identifier, first_name.c_str(), last_name.c_str(), amount);
}

/*!
    @brief Serializes the user instance to a string in the format of: ID;FIRSTNAME;LASTNAME;AMOUNT
    @return Serialized user string
*/
std::string user::serialize() const {
    std::string user_string;
    user_string += first_name + ITEM_DELIMITER;
    user_string += last_name + ITEM_DELIMITER;
    user_string += std::to_string(amount);
    return user_string;
}

/*!
    @brief Adds an value to the user amount.
    @param value Value to be added to the user amount
*/
void user::add_amount(double value) {
    amount += value;
    on_data_change();
}

/*!
    @brief Get function for user amount.
    @return User amount
*/
double user::get_amount() {
    return amount;
}

/*!
    @brief Set the user amount to 0 after user has paid the amount.
*/
void user::reset_amount() {
    amount = 0;
    on_data_change();
}

/*!
    @brief Save the users when some member gets changed during runtime.
*/
void user::on_data_change() {
    CoffeeOS::instance().my_manager.save_users();
}
