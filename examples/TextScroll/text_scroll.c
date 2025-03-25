#include "../../HD44780.h"
#include "ch32fun.h"

int main() {
	static int SHIFT_DELAY = 500;
    HD44780_FOUR_BIT_BUS bus = { 2, 16, PC3, PC4, PC5, PC6, PC1, PC2 };

    SystemInit();
    
    HD44780_initFourBitBus(&bus);

    uint8_t smileyChar[8] = {
        0b00000,
        0b01010,
        0b01010,
        0b00000,
        0b10001,
        0b10001,
        0b01110,
        0b00000
    };

    uint8_t invertSmileyChar[8] = {
        0b11111,
        0b10101,
        0b10101,
        0b11111,
        0b01110,
        0b01110,
        0b10001,
        0b11111
    };

	HD44780_createChar(0, smileyChar);
	HD44780_createChar(1, invertSmileyChar);

    HD44780_setCursorPos(3, 0);
    HD44780_print("CH32V003");
    HD44780_setCursorPos(3, 1);
	HD44780_writeChar(0);
    HD44780_print(" test ");
	HD44780_writeChar(1);

	for (int i = 0; i < 5; i++) {
		HD44780_shiftDispRight();
		Delay_Ms(SHIFT_DELAY);
	}

	for (int i = 0; i < 8; i++) {
		HD44780_shiftDispLeft();
		Delay_Ms(SHIFT_DELAY);
	}

	while (1) {
		for (int i = 0; i < 8; i++) {
			HD44780_shiftDispRight();
			Delay_Ms(SHIFT_DELAY);
		}

		for (int i = 0; i < 8; i++) {
			HD44780_shiftDispLeft();
			Delay_Ms(SHIFT_DELAY);
		}
	}
}
