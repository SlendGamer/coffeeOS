#pragma once

#include <unordered_map>

class saver {
public:
    void saver_init();

    void saveUser(int, double);
    void saveAllUser();
    
    double get_user_amount(int);
    void setAllUserAmount();

private:
    void clearFlashMem();
    void writeIntIntoEEPROM(int, int);
    int readIntFromEEPROM(int);
    
};
