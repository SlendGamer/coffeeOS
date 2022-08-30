#pragma once

#include <string>

#define ITEM_DELIMITER     ";"
#define ITEM_DELIMITER_LENGTH 1
#define ELEMENT_DELIMITER   "#"
#define ELEMENT_DELIMITER_LENGTH 1

class user {
public:
    // public member variables
    int identifier;
    std::string nfcIdentifier;
    std::string firstName;
    std::string lastName;

    user();
    user(int, std::string, std::string, double); // constructor

    void add_amount(double);
    void reset_amount();
    double get_amount();
    void init_set_amount(double); // only for init on reboot!! change is not saved in mem

    void print_to_oled() const;
    std::string serialize() const;


private:

    double amount = 0;
    void on_data_change();
};
