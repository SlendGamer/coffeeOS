
#include "coffeeOS.h"

#include "user.h"
#include "stringhelper.h"

user::user(int identifier, std::string firstName, std::string lastName, double amount) {
    this->identifier = identifier;
    this->firstName = firstName;
    this->lastName = lastName;
    this->amount = amount;
}

void user::print_to_oled() const {
    OBDISP oled = CoffeeOS::instance().oled_display;
    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 0*8, (char* ) ("Identifier: " + std::to_string(this->identifier)).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char* ) ("First-Name: " + this->firstName).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 2*8, (char* ) ("Last-Name: " + this->lastName).c_str(), FONT_8x8, 0, 1);
    obdWriteString(&oled, 0, 0, 3*8, (char* ) ("Amount: " + std::to_string(this->amount)).c_str(), FONT_8x8, 0, 1);
}
// Serialize format:
// u:id;firstname;lastname;amount
std::string user::serialize() const {
    std::string user_string = "u:";
    user_string = user_string + std::to_string(identifier) + ";";
    user_string = user_string + firstName + ";";
    user_string = user_string + lastName + ";";
    user_string = user_string + std::to_string(amount);
    return user_string;
}

void user::add_amount(double value) {
    this->amount = this->amount + value;
    on_data_change();
}

// getter/setter functions
double user::get_amount() {
    return this->amount;
}

void user::init_set_amount(double amount) {
    this->amount = amount;
}

void user::reset_amount() {
    this->amount = 0;
    on_data_change();
}

void user::on_data_change() {

}