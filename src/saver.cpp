
#include "coffeeOS.h"

#include "saver.h"
#include "stringhelper.h"

#include <cmath>
#include <EEPROM.h>

void saver::init() {
    EEPROM.begin(EEPROM_SIZE);
}

void saver::saveUser(int id, double money) {
    int a = std::floor(money);
    int a_1 = std::floor((money-a)*10);
    int a_2 = std::floor((((money-a)*10)-a_1) * 10);
    writeIntIntoEEPROM(id*8, id);
    writeIntIntoEEPROM(id*8 + 2, a);
    writeIntIntoEEPROM(id*8 + 4, a_1);
    writeIntIntoEEPROM(id*6 + 6, a_2);
    EEPROM.commit();
}

// idk if needed
void saver::clearFlashMem() {
    // reset all flash mem
}

void saver::saveAllUser() {
    clearFlashMem();
    std::unordered_map<int, user> ul = CoffeeOS::instance().my_manager.get_user_list();
    for (int i = 0; i < ul.size(); i++) {
        saveUser(ul.at(i).identifier, ul.at(i).get_amount());
    }
}

double saver::get_user_amount(int id) {
    double a = readIntFromEEPROM(8*id + 2);
    double a_1 = readIntFromEEPROM(4*id + 4);
    double a_2 = readIntFromEEPROM(4*id + 6);

    a = (a == 65535) ? 0 : a;
    a_1 = (a_1 == 65535) ? 0 : a_1;
    a_2 = (a_2 == 65535) ? 0 : a_2;
    
    Serial.printf(("From Memory a: " + std::to_string(a)).c_str());
    Serial.printf(("From Memory a1: " + std::to_string(a_1)).c_str());
    Serial.println(("From Memory a2: " + std::to_string(a_2)).c_str());


    return (a + (a_1/10.0 + a_2/100.0));
}

void saver::setAllUserAmount() {
    std::unordered_map<int, user> ul = CoffeeOS::instance().my_manager.get_user_list();
    for (int i = 0; i < ul.size(); i++) {
        CoffeeOS::instance().my_manager.set_user_amount(i, get_user_amount(i));
    }
}

void saver::writeIntIntoEEPROM(int address, int number) { 
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

int saver::readIntFromEEPROM(int address) {
  byte byte1 = EEPROM.read(address);
  byte byte2 = EEPROM.read(address + 1);
  return (byte1 << 8) + byte2;
}
