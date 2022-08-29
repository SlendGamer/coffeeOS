
#include "coffeeOS.h"

#include "product.h"
#include "stringhelper.h"

#include <iostream>

// p:id;name;price
product::product(int identifier, std::string name, double price) {
    this->identifier = identifier;
    this->name = std::move(name);
    this->price = price;
}

std::string product::toString() const {
    std::string product_string = "p:";
    product_string = product_string + std::to_string(identifier) + ";";
    product_string = product_string + name + ";";
    product_string = product_string + std::to_string(price);
    return product_string;
}

void product::printData() const {
    std::cout << "Identifier: " + std::to_string(this->identifier) << std::endl;
    std::cout << "Name: " + this->name << std::endl;
    std::cout << "Price: " + std::to_string(this->price) << std::endl;
}
