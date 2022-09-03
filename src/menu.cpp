
// project includes
#include "coffeeOS.h"
#include "menu.h"
#include "config.h"
#include "product.h"

// extern includes
#include <Adafruit_PN532.h>

// standard library includes
#include <unordered_map>

void menu::init() {
    OBDISP& oled = CoffeeOS::instance().oled_display;
    
    temp_turn = false; // force stop after each turn: one turn click -> one new product
    menu_state = 0; // the selected menu item
    time_stamp = time(nullptr); // after 10s go back to home screen
    select_mode = false; // true when user is selecting product (for 10s)
    clicked = false;

    obdFill(&oled, 0, 1);
    obdWriteString(&oled, 0, 0, 1*8, (char*) "menu init", FONT_6x8, 0, 1);
    delay(1000);
}

void menu::on_encoder_event() {
    if (clicked) return; // check if user has selected an item and clicked on it
    if (digitalRead(ROT_BUTTON) == LOW) {
        on_click();
        clicked = true;
        return;
    }
    if (digitalRead(ROT_CLK) == LOW) {
        select_mode = true;
        on_left_rotation();
        return;
    }
    if (digitalRead(ROT_DT) == LOW) {
        select_mode = true;
        on_right_rotation();
        return;
    }
    temp_turn = false;
}

void menu::on_left_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == 0) menu_state = CoffeeOS::instance().my_manager.product_list.size();
    menu_state = menu_state - 1;
    temp_turn = true;
}

void menu::on_right_rotation() {
    if (temp_turn) return;
    time_stamp = time(nullptr);
    if (menu_state == CoffeeOS::instance().my_manager.product_list.size() - 1) menu_state = 0;
    menu_state = menu_state + 1;
    temp_turn = true;
}

void menu::on_click() {
    if (!select_mode) return;
    time_stamp = time(nullptr);

    OBDISP& obd_t = CoffeeOS::instance().oled_display;
    const char* name = CoffeeOS::instance().my_manager.product_list.at(menu_state).name.c_str();
    double& price = CoffeeOS::instance().my_manager.product_list.at(menu_state).price;
    int& last_user = CoffeeOS::instance().last_user;
    int& last_product = CoffeeOS::instance().last_product;

    obdFill(&obd_t, 0, 1);
    obdWriteString(&obd_t, 0, 0, 2*8, (char *) "You selected: ", FONT_8x8, 0, 1);
    obdWriteString(&obd_t, 0, 0, 4*8, (char *)name, FONT_8x8, 0, 1);

    // set precision of double to 2 digits
    std::string num_text = std::to_string(price);
    const char* price_rounded = ("Price: " + num_text.substr(0, num_text.find('.') + 3) + " Euro").c_str();
    obdWriteString(&obd_t, 0, 0, 6*8, (char *) price_rounded, FONT_8x8, 1, 1);

    last_product = menu_state;
    last_user = get_user_id_from_nfc();

    if (last_user == -1) return;
    CoffeeOS::instance().my_manager.add_user_amount(last_user, price);
}

void menu::exit_menu() {
    clicked = false;
    temp_turn = false;
    select_mode = false;
}

int menu::get_menu_state() const {
    return menu_state;
}

time_t menu::get_timestamp() const {
    return time_stamp;
}

bool menu::is_turned() const {
    return temp_turn;
}

bool menu::has_clicked() const {
    return clicked;
}

void menu::set_clicked(bool c) {
    clicked = c;
}

product menu::get_current_product() const {
    return CoffeeOS::instance().my_manager.product_list.at(menu_state);
}

int menu::get_user_id_from_nfc() {
    int user = -1;

    // check for nfc card
    if (reader_disabled) {
        if (millis() - time_last_card_read > DELAY_BETWEEN_CARDS) {
            reader_disabled = false;
            start_listening_to_nfc();
        }

    } else {
        irqCurr = digitalRead(PN532_IRQ);

        // when the IRQ is pulled low, the reader has passively read a card
        if (irqCurr == LOW && irqPrev == HIGH) {  
            
            // find user card id matching with the read card id
            uint32_t card_id = get_card_id();
            for (int i = 0; i < CoffeeOS::instance().my_manager.user_list.size(); i++) {
                if (CoffeeOS::instance().my_manager.user_list.at(i).card_id == card_id) user = i;
            }
            
        }

        irqPrev = irqCurr;
    }

    return user;
}

void menu::start_listening_to_nfc() {
    Adafruit_PN532& nfc = CoffeeOS::instance().my_nfc_reader;

    // Reset our IRQ indicators
    irqPrev = irqCurr = HIGH;

    Serial.println("Waiting for an ISO14443A Card ...");
    nfc.startPassiveTargetIDDetection(PN532_MIFARE_ISO14443A);
}

uint32_t menu::get_card_id() {
    Adafruit_PN532& nfc = CoffeeOS::instance().my_nfc_reader;
    
    uint8_t success = false;
    uint8_t uid[] = { 0, 0, 0, 0, 0, 0, 0 };  // Buffer to store the returned UID
    uint8_t uidLength;                        // Length of the UID (4 or 7 bytes depending on ISO14443A card type)
    uint32_t card_id = 0;

    // read card
    success = nfc.readDetectedPassiveTargetID(uid, &uidLength);
    Serial.println(success ? "Read successful" : "Read failed (not a card?)");

    if (success) {
        // display some basic information about the card
        Serial.println("Found an ISO14443A card");
        Serial.print("  UID Length: ");Serial.print(uidLength, DEC);Serial.println(" bytes");
        Serial.print("  UID Value: ");
        nfc.PrintHex(uid, uidLength);
        
        // convert array uid to integer value for easier handling
        if (uidLength == 4) { 
            card_id = uid[0];
            card_id <<= 8;
            card_id |= uid[1];
            card_id <<= 8;
            card_id |= uid[2];
            card_id <<= 8;
            card_id |= uid[3]; 
            Serial.print("Mifare Classic card #");
            Serial.println(card_id);
        }

        // set when the card was last read
        time_last_card_read = millis();
    }

    // The reader will be enabled again after DELAY_BETWEEN_CARDS ms will pass.
    reader_disabled = true;
    return card_id;
}
