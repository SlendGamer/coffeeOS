#pragma once

#include <string>

class product {
public:
    int identifier;
    std::string name;
    double price;

    product(int, std::string, double);

    std::string toString() const;
    void printData() const;
};
