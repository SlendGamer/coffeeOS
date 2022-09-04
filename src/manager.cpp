/*!
    @file     manager.cpp
    @author   Tim Prüß

    Contains initilizations of member functions.
*/

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

/*!
    @brief  Initializes an instance of class menu. Loads users and products.
*/
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

/*!
    @brief  Initializes the SPIF file system (SPIFFS) and formats it if necessary.
    @return True if successful, false if it failed to mount/format the file system
*/
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

/*!
    @brief  Serializes all users from the user_list to a single string.
    @return Serialized users string
*/
std::string manager::serialize_users() const {
    std::string serialized; // string to construct the whole serialized user list

    // append every serialized user of the map to the buffer string
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
            serialized += it->second.serialize() + ELEMENT_DELIMITER;
    }
    //serialized.erase(serialized.length() - 1, serialized.length());

    return serialized;
}

/*!
    @brief  Serializes all products from the product_list to a single string.
    @return Serialized products string
*/
std::string manager::serialize_products() const {
    std::string serialized; // string to construct the whole serialized user list

    // append every serialized user of the map to the buffer string
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
            serialized += it->second.serialize() + ELEMENT_DELIMITER;
    }
    //serialized.erase(serialized.length() - 1, serialized.length());

    return serialized;
}

/*!
    @brief  Saves all users from the user_list to the SPIFFS.
*/
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
/*!
    @brief  Saves all products from the product_list to the SPIFFS.
*/
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

/*!
    @brief  Loads all users into the user_list from the SPIFFS. 
*/
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

/*!
    @brief  Loads default users (e.g. if SPIFFS failed). 
*/
void manager::load_default_users() {
    user_list.insert({0, user(0, "Tim", "Pruess", 0, 0)});
    user_list.insert({1, user(1, "Olaf", "Assmann", 0, 0)});
    user_list.insert({2, user(2, "Holger", "nn", 0, 0)});
    user_list.insert({3, user(3, "Markus", "Krause", 0, 0)});
    user_list.insert({4, user(4, "Ronald", "Bonk", 0, 0)});

    DEBUG_PRINTLN("load_default_users(): loading complete");
}

/*!
    @brief  Loads all products into the product_list from the SPIFFS. 
*/
void manager::load_products() {
    load_default_products();
}

/*!
    @brief  Loads default products (e.g. if SPIFFS failed). 
*/
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

/*!
    @brief  Loads all users from the SPIFFS.
*/
void manager::add_user_amount(int id, double amount) {
    user_list.at(id).add_amount(amount);
}

/*!
    @brief  Prints all users to serial monitor.
*/
void manager::serial_print_users() const {
    for (auto it = user_list.begin(), end = user_list.end(); it != end; it++) {
        it->second.serial_print();
    }
}

/*!
    @brief  Prints all products to serial monitor.
*/
void manager::serial_print_products() const {
    for (auto it = product_list.begin(), end = product_list.end(); it != end; it++) {
        it->second.serial_print();
    }
}

/*!
    @brief  Gets the nfc card_id and compares it with the card_id in the user_list.
    @return User id of the corresponding user matching with the card_id. Return -1 when no card is detected.
*/
int manager::get_user_id_from_nfc() {
    int user = -1;

    // check for nfc card
    if (reader_disabled) {
        if (millis() - time_last_card_read > DELAY_BETWEEN_CARDS) {
            reader_disabled = false;
            start_listening_to_nfc();
        }

    } else {
        irqCurr = digitalRead(PN532_IRQ);

        // when the IRQ is pulled low, the reader has passively read a card
        if (irqCurr == LOW && irqPrev == HIGH) {  
            
            // find user card id matching with the read card id
            uint32_t card_id = get_card_id();
            for (int i = 0; i < user_list.size(); i++) {
                if (user_list.at(i).card_id == card_id) user = i;
            }
            
        }

        irqPrev = irqCurr;
    }

    return user;
}

/*!
    @brief  Activates the passive detection of nfc tags by the PN532 reader.
*/
void manager::start_listening_to_nfc() {
    Adafruit_PN532& nfc = CoffeeOS::instance().my_nfc_reader;

    // Reset our IRQ indicators
    irqPrev = irqCurr = HIGH;

    Serial.println("Waiting for an ISO14443A Card ...");
    nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
}

/*!
    @brief  Handles event that a card is detected by the PN532 reader and convert its array-like uid into a number for easier handling.
    @return Integer card_id
*/
uint32_t manager::get_card_id() {
    Adafruit_PN532& nfc = CoffeeOS::instance().my_nfc_reader;
    
    uint8_t success = false;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    uint32_t card_id = 0;

    // read card
    success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
    Serial.println(success ? "Read successful" : "Read failed (not a card?)");

    if (success) {
        // display some basic information about the card
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        
        // convert array uid to integer value for easier handling
        if (uidLength == 4) { 
            card_id = uid[0];
            card_id <<= 8;
            card_id |= uid[1];
            card_id <<= 8;
            card_id |= uid[2];
            card_id <<= 8;
            card_id |= uid[3]; 
            Serial.print("Mifare Classic card #");
            Serial.println(card_id);
        }

        // set when the card was last read
        time_last_card_read = millis();
    }

    // The reader will be enabled again after DELAY_BETWEEN_CARDS ms will pass.
    reader_disabled = true;
    return card_id;
}