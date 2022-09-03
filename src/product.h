#pragma once

#include <string>

class product {
public:
    product(int, std::string, double);

    std::string serialize() const;
    void serial_print() const;

    int identifier;
    std::string name;
    double price;
};
