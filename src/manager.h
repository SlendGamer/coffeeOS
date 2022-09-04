/*!
    @file     manager.h
    @author   Tim Prüß

    Contains the declarations of class and members (functions).
*/

#pragma once

// project includes
#include "product.h"
#include "user.h"

// standard library includes
#include <unordered_map>

#define ITEM_DELIMITER ";"
#define ELEMENT_DELIMITER "#"

/*!
    @brief  Contains all functions for managing the user and product list.
*/
class manager {
public:
    void init();

    void add_user_amount(int id, double amount);

    std::string serialize_users() const;
    std::string serialize_products() const;

    void save_users() const;
    void save_products() const;

    void load_default_users();
    void load_users();

    void load_default_products();
    void load_products();

    void serial_print_users() const;
    void serial_print_products() const;

    // nfc functions
    void start_listening_to_nfc();
    int get_user_id_from_nfc();
    uint32_t get_card_id();

    // nfc function variables
    const int DELAY_BETWEEN_CARDS = 500; /*! Amount of time between card reads */
    long time_last_card_read = 0; /*! Timestamp (milliseconds) of last card read */ 
    bool reader_disabled = false; /*! If true, reader will not be called */
    int irqCurr; /*! Current value of the IRQ line of the PN532 that acts like an interrupt for the microcontroller */
    int irqPrev; /*! Previous value of the IRQ line of the PN532 that acts like an interrupt for the microcontroller */

    std::unordered_map<int, product> product_list; /*! Stores every product and associates each with an identifier */
    std::unordered_map<int, user> user_list; /*! Stores every user and associates each with an identifier */

private:
    bool file_system_init() const;
};
