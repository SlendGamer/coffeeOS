#pragma once

#include "product.h"
#include "user.h"

#include <unordered_map>

class manager {
public:
    void init();

    void set_user_amount(int, double);
    void add_user_amount(int id, double amount);

    std::unordered_map<int, product> &get_product_list();
    std::unordered_map<int, user> &get_user_list();

    std::string serialize_users() const;
    //std::string serialize_all_products() const;

    void save_users() const;

    void load_default_users();
    void load_users();

    //void save_all_products();
    void load_default_products();
    void load_products();

private:
    std::unordered_map<int, product> product_list;
    std::unordered_map<int, user> user_list;

    bool file_system_init();
};
