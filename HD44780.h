/**
 * File:       HD44780.h
 * Author:     Franklyn Dahlberg
 * Created:    20 March, 2025
 * Copyright:  2025 (c) Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */
#include <stdint.h>

#pragma once

typedef enum _displayMode {
    HD44780_FOUR_BIT_MODE,
    HD44780_EIGHT_BIT_MODE
} HD44780_DISPLAY_MODE;

typedef struct _fourBitBus {
    int rows;
    int columns;

    // The ints here should really be passed as the pin definition enums from CH32Fun
    // (ie PD0, PD1, PC0, etc)
    int D4;
    int D5;
    int D6;
    int D7;
    int RS;
    int E;
} HD44780_FOUR_BIT_BUS;

typedef struct _eightBitBus {
    int rows;
    int columns;

    // The ints here should really be passed as the pin definition enums from CH32Fun
    // (ie PD0, PD1, PC0, etc)
    int D0;
    int D1;
    int D2;
    int D3;
    int D4;
    int D5;
    int D6;
    int D7;
    int RS;
    int E;
} HD44780_EIGHT_BIT_BUS;

// 'Private' methods designed for internal use
void HD44780_InitDisplay();

void HD44780_Pulse_E();

void HD44780_SetUpperNibble(unsigned short int data);

void HD44780_SetLowerNibble(unsigned short int data);

void HD44780_Send4BitsIn4BitMode(unsigned short int data);

void HD44780_Send8BitsIn4BitMode(unsigned short int data);

void HD44780_Send4BitStartInstruction(unsigned short int data);

void HD44780_SendInstruction(unsigned short int data);

void HD44780_SendData(unsigned short int data);


// Public methods designed for the user to call
void HD44780_initFourBitBus(HD44780_FOUR_BIT_BUS *bus);

void HD44780_initEightBitBus(HD44780_EIGHT_BIT_BUS *bus);

void HD44780_print(char* data);

void HD44780_clear();

void HD44780_setCursorPos(int col, int row);

void HD44780_homeCursor();

void HD44780_createChar(int slot, uint8_t* data);

void HD44780_writeChar(int slot);

void HD44780_shiftDispLeft();

void HD44780_shiftDispRight();

void HD44780_blink();

void HD44780_noBlink();

void HD44780_cursor();

void HD44780_noCursor();

void HD44780_dispOff();

void HD44780_dispOn();

// HD44780 Instruction Definitions
#define HD44780_INIT_SEQ        0x30
#define HD44780_DISP_CLEAR      0x01
#define HD44780_DISP_OFF        0x08
#define HD44780_DISP_ON         0x0C
#define HD44780_CURSOR_ON       0x0E
#define HD44780_CURSOR_BLINK    0x0F
#define HD44780_RETURN_HOME     0x02
#define HD44780_ENTRY_MODE      0x06
#define HD44780_FOUR_BIT_MODE   0x20
#define HD44780_EIGHT_BIT_MODE  0x30
#define HD44780_SET_POSITION    0x80
#define HD44780_SHIFT_RIGHT     0x1C
#define HD44780_SHIFT_LEFT      0x18

// Bitmasks for various instructions
#define HD44780_TWO_ROWS        0x08
#define HD44780_FONT_5X8        0x00
#define HD44780_FONT_5X10       0x40

// Constants for calculations
#define HD44780_ROW1_START      0x00
#define HD44780_ROW2_START      0x40
#define HD44780_ROW3_START      0x14
#define HD44780_ROW4_START      0x54
#define HD44780_CGRAM_START     0x40
