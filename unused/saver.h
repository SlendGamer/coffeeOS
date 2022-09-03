// ! deprecated file
#pragma once

#include <unordered_map>

class saver {
public:
    void init();

    void save_user(int, double);
    void save_all_users();
    
    double get_user_amount(int);
    //void set_all_user_amount();

private:
    void clear_flash_mem();
    void write_int_to_eeprom(int, int);
    int read_int_from_eeprom(int);
};
