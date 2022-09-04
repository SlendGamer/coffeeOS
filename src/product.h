/*!
    @file     manager.h
    @author   Tim Prüß

    Contains the declarations of class and members (functions).
*/

#pragma once

// standard library includes
#include <string>

/*!
    @brief  Contains all functions and variables for handling a product.
*/
class product {
public:
    product(int, std::string, double);

    std::string serialize() const;
    void serial_print() const;

    int identifier; /*! Unique identifier of a product */
    std::string name; /*! Product name */
    double price; /*! Product price */
};
