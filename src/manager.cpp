
// project includes 
#include "coffeeOS.h"
#include "manager.h"
#include "product.h"
#include "user.h"

// extern includes
#include <SPI.h>
#include <SPIFFS.h>

// standard library includes
#include <unordered_map>
#include <string>

void manager::init() {
    OBDISP& oled = CoffeeOS::instance().oled_display;

    if(file_system_init() == true) {
        load_users();
        load_products();
    }
    else {
        load_default_users();
        load_default_products();
    }

    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char*) "manager init", FONT_6x8, 0, 1);
    delay(1000);
}

bool manager::file_system_init() const {
    DEBUG_PRINTLN("file_system_init(): initiating SPI");
    SPI.begin();

    DEBUG_PRINTLN("file_system_init(): initiating SPIFFS");
    bool init_success = SPIFFS.begin();

    if (init_success == false) SPIFFS.format();

    init_success = SPIFFS.begin();
    if (init_success == false) DEBUG_PRINTLN("file_system_init(): error mounting the file system");
    else DEBUG_PRINTLN("file_system_init(): file system mounted with success");

    return init_success;
}

std::string manager::serialize_users() const {
    std::string serialized; // string to construct the whole serialized user list

    // append every serialized user of the map to the buffer string
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
            serialized += it->second.serialize() + ELEMENT_DELIMITER;
    }
    //serialized.erase(serialized.length() - 1, serialized.length());

    return serialized;
}

std::string manager::serialize_products() const {
    std::string serialized; // string to construct the whole serialized user list

    // append every serialized user of the map to the buffer string
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
            serialized += it->second.serialize() + ELEMENT_DELIMITER;
    }
    //serialized.erase(serialized.length() - 1, serialized.length());

    return serialized;
}

void manager::save_users() const {
    int bytes_written;
    File user_file;

    user_file = SPIFFS.open("/user_config", "w");

    if(!user_file) {
        DEBUG_PRINTLN("save_users(): error opening file");
        return;
    }

    String users_string = String(serialize_users().c_str()); // convert std::string to String because print function has String as parameter
    DEBUG_PRINT("save_users(): ");
    DEBUG_PRINTLN(users_string);
    bytes_written = user_file.print(users_string);

    if(bytes_written > 0) {
        DEBUG_PRINTLN("save_users(): file was written");
        DEBUG_PRINTLN(bytes_written);
    }
    else DEBUG_PRINTLN("save_users(): error writing");

    user_file.close();
}

void manager::save_products() const {
    int bytes_written;
    File products_file;

    products_file = SPIFFS.open("/product_config", "w");

    if(!products_file) {
        DEBUG_PRINTLN("save_products(): error opening file");
        return;
    }

    String products_string = String(serialize_products().c_str()); // convert std::string to String because print function has String as parameter
    DEBUG_PRINT("save_products(): ");
    DEBUG_PRINTLN(products_string);

    bytes_written = products_file.print(products_string);

    if(bytes_written > 0) {
        DEBUG_PRINTLN("save_products(): file was written");
        DEBUG_PRINTLN(bytes_written);
    }
    else DEBUG_PRINTLN("save_products(): error writing");

    products_file.close();
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
    uint32_t card_id;

    DEBUG_PRINT("load_users(): file size in byte: ");
    DEBUG_PRINTLN(file_size);

    if(file_size <= 0) {
        DEBUG_PRINTLN("load_users(): error loading: file empty");
        DEBUG_PRINTLN("load_users(): loading default users instead");
        load_default_users();
        return;
    }

    const char* temp_string = user_file.readString().c_str();
    users_string = std::string(temp_string);
    DEBUG_PRINT("load_users(): ");
    DEBUG_PRINTLN(temp_string);
    user_file.close();

    identifier = 0;
    size_t pos = 0;

    DEBUG_PRINTLN("load_users(): deserializing string");
    while ((pos = users_string.find(element_delimiter)) != std::string::npos) {
        // split string based on item delimiters and initialize values
            pos = users_string.find(item_delimiter); 
        first_name = users_string.substr(0, pos);
            users_string.erase(0, pos + item_delimiter.length());

            pos = users_string.find(item_delimiter);
        last_name = users_string.substr(0, pos);
            users_string.erase(0, pos + item_delimiter.length());

            pos = users_string.find(element_delimiter);
        amount = std::stod(users_string.substr(0, pos));
            users_string.erase(0, pos + element_delimiter.length());

            pos = users_string.find(element_delimiter);
        card_id = std::stod(users_string.substr(0, pos));
            users_string.erase(0, pos + element_delimiter.length());

        // insert each user in the user list
        user_list.insert({identifier, user(identifier, first_name, last_name, amount, card_id)});

        Serial.printf("load_users(): loaded user %i\n", identifier);

        // next user element position in string
        identifier++;
    }
}

void manager::load_default_users() {
    user_list.insert({0, user(0, "Tim", "Pruess", 0, 0)});
    user_list.insert({1, user(1, "Olaf", "Assmann", 0, 0)});
    user_list.insert({2, user(2, "Holger", "nn", 0, 0)});
    user_list.insert({3, user(3, "Markus", "Krause", 0, 0)});
    user_list.insert({4, user(4, "Ronald", "Bonk", 0, 0)});

    DEBUG_PRINTLN("load_default_users(): loading complete");
}

void manager::load_products() {
    load_default_products();
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

    DEBUG_PRINTLN("load_default_products(): loading complete");
    // product_list.insert({3, product(3, "Espresso Macchiato", 0.5)});
    // product_list.insert({6, product(6, "Cappuccino Italiano", 1.75)});
}

void manager::add_user_amount(int id, double amount) {
    user_list.at(id).add_amount(amount);
}

void manager::serial_print_users() const {
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
        it->second.serial_print();
    }
}

void manager::serial_print_products() const {
    for (auto it = product_list.begin(), end = product_list.end(); it != end; it++) {
        it->second.serial_print();
    }
}
