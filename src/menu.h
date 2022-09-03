#pragma once

#include <string>
#include <time.h>

// true when user clicks the turn wheel (for 10s)

class menu {
public:
    void init();
    void exit_menu();
    void on_encoder_event();
    int get_menu_state() const;
    time_t get_timestamp() const;
    bool has_clicked() const;
    bool is_turned() const;
    void set_clicked(bool);

    product get_current_product() const;

    // nfc functions
    void start_listening_to_nfc();
    int get_user_id_from_nfc();
    uint32_t get_card_id();

private:
    bool temp_turn;              // force stop after each turn: one turn click -> one new product
    int menu_state;              // the selected menu item
    time_t time_stamp;           // after 10s go back to home screen
    bool select_mode;            // true when user is selecting product (for 10s)
    bool clicked;                // true when user has selected product (for 10s)

    const int DELAY_BETWEEN_CARDS = 500;
    long time_last_card_read = 0;
    bool reader_disabled = false;
    int irqCurr;
    int irqPrev;

    void on_left_rotation();
    void on_right_rotation();
    void on_click();
};
