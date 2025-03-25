/**
 * File:       Example_Count.c
 * Author:     Franklyn Dahlberg
 * Created:    24 March, 2025
 * Copyright:  2025 (c) Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */

/**
 * Example for a 2x16 HD44780 display that shows a integer counting upwards.
 * Designed to show basic HD44780 functionality.
 */
#include "../../HD44780.h"
#include "ch32fun.h"
#include <string.h>

void intToStr(int N, char *str);

int main() {
    int COUNT_DELAY = 200;
    HD44780_FOUR_BIT_BUS bus = { 2, 16, PC3, PC4, PC5, PC6, PC1, PC2 };

    SystemInit();
    
    HD44780_initFourBitBus(&bus);

    int count = 0;
    char str[16];

	while (1) {
        HD44780_homeCursor();
        intToStr(count, str);
        HD44780_print(str);
        count++;
        Delay_Ms(COUNT_DELAY);
	}
}

/**
 * Converts the param Integer to a String.
 * 
 * @param N Integer to convert
 * @param str Character array to fill out the String to
 */
void intToStr(int N, char *str) {
    int i = 0;
    
    while (N > 0) {
        str[i++] = N % 10 + '0';
        N /= 10;
    }

    str[i] = '\0';

    for (int j = 0, k = i - 1; j < k; j++, k--) {
        char temp = str[j];
        str[j] = str[k];
        str[k] = temp;    
    }
}
