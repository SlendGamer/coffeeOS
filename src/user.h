#pragma once

// standard library includes
#include <string>

class user {
public:
    // public member variables
    int identifier;
    std::string first_name;
    std::string last_name;
    uint32_t card_id;

    // class constructor
    user(int, std::string, std::string, double, uint32_t); 

    void add_amount(double);
    void reset_amount();
    double get_amount();
    
    std::string serialize() const;

    void print_to_oled() const;
    void serial_print() const;
    
private:
    double amount = 0;
    void on_data_change();
};
