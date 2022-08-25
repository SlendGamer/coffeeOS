#pragma once

#include "coffeeOS.h"

class User {
public:
    String name;
    int id;
    int nfcTag_id;
    Transaction **transactionList;

    String admin_password;
};