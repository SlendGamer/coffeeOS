#include "coffeeOS.h"

void setup() {
    CoffeeOS::instance().setup();
}

void loop() {
    CoffeeOS::instance().loop();
}
