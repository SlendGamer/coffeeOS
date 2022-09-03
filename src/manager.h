#pragma once

// project includes
#include "product.h"
#include "user.h"

// standard library includes
#include <unordered_map>

#define ITEM_DELIMITER ";"
#define ELEMENT_DELIMITER "#"

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

    std::unordered_map<int, product> product_list;
    std::unordered_map<int, user> user_list;

private:
    bool file_system_init() const;
};
