/**
 * File:       Example_Snow.c
 * Author:     Franklyn Dahlberg
 * Created:    24 March, 2025
 * Copyright:  2025  Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */

/**
 * Prints two lines of inverted * characters to make a crude snow animation.
 * Designed to show basic HD44780 functionality.
 */
#include "../../HD44780.h"
#include "ch32fun.h"

int main() {
    HD44780_FOUR_BIT_BUS bus = { 2, 16, PC3, PC4, PC5, PC6, PC1, PC2 };
    SystemInit();

    HD44780_initFourBitBus(&bus);

    uint8_t pattern = 0;

    while (1) {
        HD44780_clear();

        for (int i = 0; i < 20; i+=2) {
            HD44780_setCursorPos(i, (pattern == 0));
            HD44780_print("*");
        }

        for (int i = 1; i < 20; i+=2) {
            HD44780_setCursorPos(i, (pattern == 1));
            HD44780_print("*");
        }

        pattern = (pattern == 0) ? 1 : 0;

        Delay_Ms(500);
    }
}

