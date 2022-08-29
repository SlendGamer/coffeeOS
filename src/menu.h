#pragma once

#include <string>
#include <time.h>

// true when user clicks the turn wheel (for 10s)

class menu {
public:

    void setup();
    void exitMenu();

    void onRotateEvent();

    std::string getCurrentProduct() const;

    int getMenuState() const;
    time_t getTimeStamp() const;
    bool hasClicked() const;
    bool isTurned() const;
    void setClicked(bool);

private:
    bool temp_turn;              // force stop after each turn: one turn click -> one new product
    int menu_state;              // the selected menu item
    time_t time_stamp;           // after 10s go back to home screen
    bool select_mode;            // true when user is selecting product (for 10s)
    bool clicked;                // true when user has selected product (for 10s)

    void onLeftEvent();
    void onRightEvent();
    void onClickEvent();
};
