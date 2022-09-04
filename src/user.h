/*!
    @file     user.h
    @author   Tim Prüß

    Contains the declarations of class and members (functions).
*/

#pragma once

// standard library includes
#include <string>

/*!
    @brief  Contains all functions and variables for handling an individual user.
*/
class user {
public:
    user(int, std::string, std::string, double, uint32_t); 

    void add_amount(double);
    void reset_amount();
    double get_amount();
    
    std::string serialize() const;

    void print_to_oled() const;
    void serial_print() const;

    // public member variables
    int identifier; /*! Unique identifier of a user */
    std::string first_name; /*! First name of a user */
    std::string last_name; /*! Last name of a user */
    uint32_t card_id; /*! Unique nfc tag id associated with a user */
    
private:
    double amount = 0;
    void on_data_change();
};
