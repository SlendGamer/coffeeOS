/*!
    @file     menu.h
    @author   Tim Prüß

    Contains the declarations of class and members (functions).
*/

#pragma once

// project includes
#include "product.h"

// standard library includes
#include <string>
#include <time.h>

/*!
    @brief  Contains all functions for the menu interaction.
*/
class menu {
public:
    void init();
    void exit_menu();
    void on_encoder_event();
    int get_menu_state() const;
    time_t get_timestamp() const;
    product get_current_product() const;

private:
    bool temp_turn; /*! Force stop after each turn: one turn click = one new product. */
    int menu_state; /*! The selected menu item */
    time_t time_stamp; /*! After (arbitrary amount of time) go back to home screen. */
    bool select_mode; /*! True when user is selecting product (for an arbitrary amount of time). */
    bool clicked; /*! True when user has selected product (for an arbitrary amount of time). */

    void on_left_rotation();
    void on_right_rotation();
    void on_click();
};
