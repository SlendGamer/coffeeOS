#pragma once

#include "coffeeOS.h"

class Transaction {
public:
    int id;
    uint32_t unix_timestamp;
    int user_id;
    int product_id;
    float amount_paid;
};