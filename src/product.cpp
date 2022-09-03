
// project includes
#include "coffeeOS.h"
#include "product.h"

// standard library includes
#include <iostream>

product::product(int identifier, std::string name, double price) {
    this->identifier = identifier;
    this->name = std::move(name);
    this->price = price;
}

// serialize format: id;name;price
std::string product::serialize() const {
    std::string product_string;
    product_string += std::to_string(identifier) + ITEM_DELIMITER;
    product_string += name + ITEM_DELIMITER;
    product_string += std::to_string(price);
    return product_string;
}

void product::serial_print() const {
    Serial.printf("\nproduct-id: %u\n|-name: %s\n|-price: %f\n", identifier, name.c_str(), price);
}
