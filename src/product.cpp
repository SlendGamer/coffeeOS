/*!
    @file     manager.cpp
    @author   Tim Prüß

    Contains initilizations of member functions.
*/

// project includes
#include "coffeeOS.h"
#include "product.h"

// standard library includes
#include <iostream>

/*!
    @brief Constructs an Object of class user with the given parameters.
    @param identifier Unique number for product identification
    @param name Product name to specify the product
    @param price Product price the user has to pay
*/
product::product(int identifier, std::string name, double price) {
    this->identifier = identifier;
    this->name = std::move(name);
    this->price = price;
}

/*!
    @brief Serializes the product instance to a string in the format of: ID;NAME;PRICE
    @return Serialized product string
*/
std::string product::serialize() const {
    std::string product_string;
    product_string += std::to_string(identifier) + ITEM_DELIMITER;
    product_string += name + ITEM_DELIMITER;
    product_string += std::to_string(price);
    return product_string;
}


/*!
    @brief Prints product to the serial monitor with a specific format.
*/
void product::serial_print() const {
    Serial.printf("\nproduct-id: %u\n|-name: %s\n|-price: %f\n", identifier, name.c_str(), price);
}
