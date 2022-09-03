// ! deprecated file
#include "coffeeOS.h"

#include "saver.h"

#include <cmath>
#include <EEPROM.h>

#define EEPROM_SIZE 512

void saver::init() {
    EEPROM.begin(EEPROM_SIZE);
}

void saver::save_user(int id, double money) {
    int a = std::floor(money);
    int a_1 = std::floor((money-a)*10);
    int a_2 = std::floor((((money-a)*10)-a_1) * 10);
    write_int_to_eeprom(id*8, id);
    write_int_to_eeprom(id*8 + 2, a);
    write_int_to_eeprom(id*8 + 4, a_1);
    write_int_to_eeprom(id*6 + 6, a_2);
    EEPROM.commit();
}

// idk if needed
void saver::clear_flash_mem() {
    // reset all flash mem
}

void saver::save_all_users() {
    clear_flash_mem();
    std::unordered_map<int, user> ul = CoffeeOS::instance().my_manager.get_user_list();
    for (int i = 0; i < ul.size(); i++) {
        save_user(ul.at(i).identifier, ul.at(i).get_amount());
    }
}

double saver::get_user_amount(int id) {
    double a = read_int_from_eeprom(8*id + 2);
    double a_1 = read_int_from_eeprom(4*id + 4);
    double a_2 = read_int_from_eeprom(4*id + 6);

    a = (a == 65535) ? 0 : a;
    a_1 = (a_1 == 65535) ? 0 : a_1;
    a_2 = (a_2 == 65535) ? 0 : a_2;
    
    Serial.printf(("From Memory a: " + std::to_string(a)).c_str());
    Serial.printf(("From Memory a1: " + std::to_string(a_1)).c_str());
    Serial.println(("From Memory a2: " + std::to_string(a_2)).c_str());


    return (a + (a_1/10.0 + a_2/100.0));
}

void saver::write_int_to_eeprom(int address, int number) { 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int saver::read_int_from_eeprom(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}
