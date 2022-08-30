
#include "coffeeOS.h"

#include "manager.h"
#include "product.h"
#include "user.h"

#include "stringhelper.h"

#include <SPI.h>
#include <FS.h> // SPIFFS Library
#include <SPIFFS.h>
#include <unordered_map>
#include <string>

bool manager::file_system_init() {
    SPI.begin();

    bool init_success = SPIFFS.begin();

    if (init_success == false) SPIFFS.format();

    init_success = SPIFFS.begin();
    if (init_success) DEBUG_PRINTLN("File system mounted with success");
    else DEBUG_PRINTLN("Error mounting the file system");

    return init_success;
}

void manager::save_users() const {
    int bytes_written;
    File user_file;

    user_file = SPIFFS.open("/user_config", "w");

    if(!user_file) {
        DEBUG_PRINTLN("Error opening file for writing");
        return;
    }

    String user_string = String(serialize_users().c_str()); // convert std::string to String because print function has String as parameter
    bytes_written = user_file.print(user_string);

    if(bytes_written > 0) {
        DEBUG_PRINTLN("File was written");
        DEBUG_PRINTLN(bytes_written);
    }

    user_file.close();
}

std::string manager::serialize_users() const {
    std::string serialized_user_list; // string to construct the whole serialized user list

    // append every serialized user of the map to the buffer string
    for(auto it = user_list.begin(); it != user_list.end(); it++) {
        serialized_user_list += it->second.serialize() + ELEMENT_DELIMITER;
    }

    return serialized_user_list;
}

void manager::load_users() {
    File user_file = SPIFFS.open("/user_config", "r"); // read from user config
    int file_size = user_file.size();
    std::string users_string;
    std::string item_delimiter = ITEM_DELIMITER;
    std::string element_delimiter = ELEMENT_DELIMITER;

    int identifier;
    std::string first_name, last_name;
    double amount;

    DEBUG_PRINT("File size in byte: ");
    DEBUG_PRINTLN(file_size);

    if(file_size <= 0) {
        DEBUG_PRINTLN("Error loading: file empty");
        DEBUG_PRINTLN("Loading default users instead");
        load_default_users();
        return;
    }

    // delete all elements from user list before loading users again
    //user_list.clear();

    users_string = std::string(user_file.readString().c_str());
    DEBUG_PRINTLN(user_file.readString());
    user_file.close();

    identifier = 0;
    size_t pos = 0;

    while ((pos = users_string.find(element_delimiter)) != std::string::npos) {
        // split string based on item delimiters and initialize values
        users_string.erase(0, pos + item_delimiter.length()); // dont parse identifier

        pos = users_string.find(item_delimiter); 
        first_name = users_string.substr(0, pos);
        users_string.erase(0, pos + item_delimiter.length());

        pos = users_string.find(item_delimiter);
        last_name = users_string.substr(0, pos);
        users_string.erase(0, pos + item_delimiter.length());

        pos = users_string.find(item_delimiter);
        amount = std::stod(users_string.substr(0, pos));
        users_string.erase(0, pos + item_delimiter.length());

        // insert each user in the user list
        user_list.insert({identifier, user(identifier, first_name, last_name, amount)});

        // next user element position in string
        users_string.erase(0, pos + element_delimiter.length());
        identifier++;
    }
}

void manager::load_default_users() {
    user_list.insert({0, user(0, "Tim", "Pruess", 0.0)});
    user_list.insert({1, user(1, "Olaf", "nn", 0.0)});
    user_list.insert({2, user(2, "Holger", "nn", 0.0)});
    user_list.insert({3, user(3, "Markus", "nn", 0.0)});
    user_list.insert({4, user(4, "Ronald", "nn", 0.0)});
}

void manager::load_default_products() {
    product_list.insert({0, product(0, "Kaffee", 1.0)});
    product_list.insert({1, product(1, "Ristretto", 1.5)});
    product_list.insert({2, product(2, "Espresso", 1.2)});
    product_list.insert({3, product(3, "Latte Macchiato", 10.1)});
    product_list.insert({4, product(4, "Cappuccino", 20.0)});
    product_list.insert({5, product(5, "Cafe Latte", 1.23)});
    product_list.insert({6, product(6, "Cafe au lait", 2.5)});
    product_list.insert({7, product(7, "Cafe Americano", 3.5)});
    product_list.insert({8, product(8, "Flat White", 4.2)});
    product_list.insert({9, product(9, "Long Black", 4.2)});
    product_list.insert({10, product(10, "Heisse Milch", 4.2)});
    product_list.insert({11, product(11, "Milchschaum", 4.2)});
    product_list.insert({12, product(12, "Schwarzer Tee", 4.2)});
    product_list.insert({13, product(13, "Gruener Tee", 4.2)});
    product_list.insert({14, product(14, "Kraeutertee", 4.2)});
    product_list.insert({15, product(15, "Fruechtetee", 4.2)});

    // product_list.insert({3, product(3, "Espresso Macchiato", 0.5)});
    // product_list.insert({6, product(6, "Cappuccino Italiano", 1.75)});
}

void manager::load_products() {
    product_list.insert({0, product(0, "Kaffee", 1.0)});
    product_list.insert({1, product(1, "Ristretto", 1.5)});
    product_list.insert({2, product(2, "Espresso", 1.2)});
    product_list.insert({3, product(3, "Latte Macchiato", 10.1)});
    product_list.insert({4, product(4, "Cappuccino", 20.0)});
    product_list.insert({5, product(5, "Cafe Latte", 1.23)});
    product_list.insert({6, product(6, "Cafe au lait", 2.5)});
    product_list.insert({7, product(7, "Cafe Americano", 3.5)});
    product_list.insert({8, product(8, "Flat White", 4.2)});
    product_list.insert({9, product(9, "Long Black", 4.2)});
    product_list.insert({10, product(10, "Heisse Milch", 4.2)});
    product_list.insert({11, product(11, "Milchschaum", 4.2)});
    product_list.insert({12, product(12, "Schwarzer Tee", 4.2)});
    product_list.insert({13, product(13, "Gruener Tee", 4.2)});
    product_list.insert({14, product(14, "Kraeutertee", 4.2)});
    product_list.insert({15, product(15, "Fruechtetee", 4.2)});

    // product_list.insert({3, product(3, "Espresso Macchiato", 0.5)});
    // product_list.insert({6, product(6, "Cappuccino Italiano", 1.75)});
}

void manager::set_user_amount(int id, double amount) {
    user_list.at(id).init_set_amount(amount);
}

void manager::add_user_amount(int id, double amount) {
    user_list.at(id).add_amount(amount);
}

void manager::init() {
    file_system_init();
    load_users();
    load_products();
}

std::unordered_map<int, product> &manager::get_product_list() {
    return product_list;
}

std::unordered_map<int, user> &manager::get_user_list() {
    return user_list;
}
