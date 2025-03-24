/**
 * File:       HD44780.c
 * Author:     Franklyn Dahlberg
 * Created:    23 March, 2025
 * Copyright:  2025 (c) Franklyn Dahlberg
 * License:    MIT License (see https://choosealicense.com/licenses/mit/)
 */

#include <string.h>
#include "HD44780.h"
#include "ch32fun.h"

int rows;
int columns;
HD44780_DISPLAY_MODE displayMode;
HD44780_FOUR_BIT_BUS *fourBus;
HD44780_EIGHT_BIT_BUS *eightBus;

// The integers here should really be passed as the pin definition enums from 
// CH32Fun (ie PD0, PD1, PC0, etc)
int enablePin;
int rsPin;

static uint32_t ONE_HUNDRED = 100;
static uint32_t TWENTY = 20;
static uint32_t VOLTAGE_CHANGE_DELAY_US = 5;
static uint32_t INSTRUCTION_DELAY_US = 70;

// 'Public' functions, designed for use by the main application

/**
 * Initializes the param four bit bus, and initializes the display
 * in four bit mode.
 * 
 * @param fourBitBus HD44780_FOUR_BIT_BUS to drive the display
 */
void HD44780_initFourBitBus(HD44780_FOUR_BIT_BUS *fourBitBus) {
    funGpioInitAll();

    displayMode = HD44780_FOUR_BIT_MODE;
    fourBus = fourBitBus;
    rows = fourBitBus->rows;
    columns = fourBitBus->columns;

    funPinMode(fourBitBus->D4, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(fourBitBus->D5, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(fourBitBus->D6, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(fourBitBus->D7, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(fourBitBus->E, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(fourBitBus->RS, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

    enablePin = fourBitBus->E;
    rsPin = fourBitBus->RS;

    HD44780_InitDisplay();
}

/**
 * Initializes the param eight bit bus, and initializes the display
 * in eight bit mode.
 * 
 * @param eightBitBus HD44780_EIGHT_BIT_BUS to drive the display
 */
void HD44780_initEightBitBus(HD44780_EIGHT_BIT_BUS *eightBitBus) {
    funGpioInitAll();

    displayMode = HD44780_EIGHT_BIT_MODE;
    eightBus = eightBitBus;
    rows = eightBitBus->rows;
    columns = eightBitBus->columns;

    // Set all pins on bus to OUTPUTs
    funPinMode(eightBitBus->D0, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D1, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D2, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D3, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D4, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D5, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D6, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->D7, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->E, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);
    funPinMode(eightBitBus->RS, GPIO_Speed_10MHz | GPIO_CNF_OUT_PP);

    // Keep E and RS as globals separately to make other functions easier
    enablePin = eightBitBus->E;
    rsPin = eightBitBus->RS;

    HD44780_InitDisplay();
}

/**
 * Prints the param string to the display
 * NOTE: This function does not check to see if the string will fit in the
 *       visible area of the display. It is expected that the calling 
 *       function takes care of that.
 * 
 * @param data String to draw on the display as a character array
 */
void HD44780_print(char* data) {
    int length = strlen(data);
    for(int i = 0; i < length; i++) {
        HD44780_SendData(data[i]);
    }
}

/**
 * Clears the entire display and sets the cursor back to 0, 0
 * NOTE: This instruction has two write to all DDRAM registers on the HD44780,
 *       so the delay is quite a bit longer then most other instructions.
 */
void HD44780_clear() {
    HD44780_SendInstruction(HD44780_DISP_CLEAR);
    Delay_Ms(TWENTY);
}

/**
 * Sets the position of the cursor back to home (0, 0).
 */
void HD44780_homeCursor() {
    HD44780_setCursorPos(0, 0);
}

/**
 * Creates a custom character at the param char slot, comprised of
 * the param data.  Up to eight custom characters can be held in
 * memory, with slots numbered 0-7.
 * 
 * @param slot Integer 0-7 of which "slot" of CGRAM to store the character in
 * @param data 
 */
void HD44780_createChar(int slot, uint8_t* data) {
    if (slot < 8) {
        HD44780_SendInstruction(HD44780_CGRAM_START + (slot * 8));
        for (int i = 0; i < 8; i++) {
            HD44780_SendData(data[i]);
        }
    }
}

/**
 * Prints the custom character held in the param char slot
 * 
 * @param slot Integer 0-7 of which "slot" of CGRAM the character to print is 
 *             stored in
 */
void HD44780_writeChar(int slot) {
    if (slot < 8) {
        HD44780_SendData(slot);
    }
}

/**
 * Shifts all characters in the display one space to the left
 */
void HD44780_shiftDispLeft() {
    HD44780_SendInstruction(HD44780_SHIFT_LEFT);
}

/**
 * Shifts all characters in the display one space to the right
 */
void HD44780_shiftDispRight() {
    HD44780_SendInstruction(HD44780_SHIFT_RIGHT);
}

/**
 * Sets the position of the cursor based on the param column (x) and row (y)
 * 
 * @param x column to set cursor to as an integer
 * @param y row to set cursor to as an integer
 */
void HD44780_setCursorPos(int x, int y) {
    // If position is out of range for display, just return
    if (x < 0 || y < 0 || x > columns || y > rows) {
        return;
    }

    // Set cursor based on row
    if (y == 0) {
        HD44780_SendInstruction(HD44780_SET_POSITION | (HD44780_ROW1_START + x));
    } else if (y == 1) {
        HD44780_SendInstruction(HD44780_SET_POSITION | (HD44780_ROW2_START + x));
    } else if (y == 2) {
        HD44780_SendInstruction(HD44780_SET_POSITION | (HD44780_ROW3_START + x));
    } else if (y == 3) {
        HD44780_SendInstruction(HD44780_SET_POSITION | (HD44780_ROW4_START + x));
    }
}

/**
 * Turns on the character cursor and sets it to blinking mode
 */
void HD44780_blink() {
    HD44780_SendInstruction(HD44780_CURSOR_BLINK);
}

/**
 * Turns on the character cursor and sets it to non blinking mode
 */
void HD44780_noBlink() {
    HD44780_SendInstruction(HD44780_CURSOR_ON);
}

/**
 * Turns on the character cursor and sets it to non blinking mode
 * NOTE: Functionally this is the same as HD445780_noBlink()
 */
void HD44780_cursor() {
    HD44780_SendInstruction(HD44780_CURSOR_ON);
}

/**
 * Turns off the character cursor
 */
void HD44780_noCursor() {
    HD44780_SendInstruction(HD44780_DISP_ON);
}

/**
 * Turns the display off entirely.
 * NOTE: As this library does not control the backlight, it is expected
 *       that toggling the backlight on/off is handled by the main application.
 */
void HD44780_dispOff() {
    HD44780_SendInstruction(HD44780_DISP_OFF);
}

/**
 * Turns the display on.
 * NOTE: Functionally this is the same as HD44780_noCursor()
 */
void HD44780_dispOn() {
    HD44780_SendInstruction(HD44780_DISP_ON);
}


// 'Private' functions designed for internal use

/**
 * Initializes the HD44780 character LCD in either 4 bit or 8 bit mode
 * depending on the value of the displayMode global enum.
 */
void HD44780_InitDisplay() {
    // NOTE: The SendInstruction method has a delay built in, but it
    //       is not long enough for some of the initialization steps.
    //       That is why some of these commands have a delay after, and
    //       some don't.
    Delay_Ms(ONE_HUNDRED);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    Delay_Ms(TWENTY);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    Delay_Ms(TWENTY);
    HD44780_Send4BitStartInstruction(HD44780_INIT_SEQ);
    Delay_Ms(TWENTY);

    // TODO: FLD 01FEB25 - Currently don't support one row or 5x10 displays, as I don't
    //                     have any to test with nor do I have an explicit need
    //                     for that.  The HD44780_TWO_ROWS constant here is 
    //                     slightly misleading, this does support common 4 row
    //                     displays, it's just that internally there are multiple
    //                     controllers in the display that are all set to two row mode.
    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send4BitStartInstruction(HD44780_FOUR_BIT_MODE);
        HD44780_SendInstruction(HD44780_FOUR_BIT_MODE | HD44780_TWO_ROWS | 
                                HD44780_FONT_5X8);
    } else {
        HD44780_SendInstruction(HD44780_EIGHT_BIT_MODE | HD44780_TWO_ROWS | 
                                HD44780_FONT_5X8);
    }

    HD44780_SendInstruction(HD44780_DISP_OFF);
    HD44780_SendInstruction(HD44780_DISP_CLEAR);
    Delay_Ms(TWENTY);
    HD44780_SendInstruction(HD44780_ENTRY_MODE);
    HD44780_SendInstruction(HD44780_DISP_ON);
}

/**
 * Pulses the 'E' clock pin
 */
void HD44780_Pulse_E() {
    funDigitalWrite(enablePin, 1);
    Delay_Us(VOLTAGE_CHANGE_DELAY_US);
    funDigitalWrite(enablePin, 0);
    Delay_Us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Set the bits on the four bit bus to the four upper bits of the param data byte.
 * 
 * @param data Byte to set
 */
void HD44780_SetUpperNibble(unsigned short int data) {
    int D7;
    int D6;
    int D5;
    int D4;

    if (displayMode == HD44780_EIGHT_BIT_MODE) {
        D7 = eightBus->D7;
        D6 = eightBus->D6;
        D5 = eightBus->D5;
        D4 = eightBus->D4;
    } else {
        D7 = fourBus->D7;
        D6 = fourBus->D6;
        D5 = fourBus->D5;
        D4 = fourBus->D4;
    }

    // Set D4-D7 if corresponding bit is set in the data
    funDigitalWrite(D7, (data & 0x80));
    funDigitalWrite(D6, (data & 0x40));
    funDigitalWrite(D5, (data & 0x20));
    funDigitalWrite(D4, (data & 0x10));

    Delay_Us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Sets the lower four bits on the bus (D0-D3)
 * NOTE: If the display is not set for eight bit mode, this function
 *       will just return
 * 
 * @param data byte containing the 4 LSB to send
 */
void HD44780_SetLowerNibble(unsigned short data) {
    if (displayMode != HD44780_EIGHT_BIT_MODE) {
        return;
    }

    funDigitalWrite(eightBus->D3, (data & 0x08));
    funDigitalWrite(eightBus->D2, (data & 0x04));
    funDigitalWrite(eightBus->D1, (data & 0x02));
    funDigitalWrite(eightBus->D0, (data & 0x01));

    Delay_Us(VOLTAGE_CHANGE_DELAY_US);
}

/**
 * Sends the four upper bits from the param data byte.
 * 
 * @param data byte containing the 4 MSB to send
 */
void HD44780_Send4BitsIn4BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();
    Delay_Us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param data byte in 8 bit mode.  In 8 bit mode the
 * entire byte is sent at once on D7-D0.
 * 
 * @param data byte to send
 */
void HD44780_Send8BitsIn8BitMode(unsigned short int data) {
    HD44780_SetUpperNibble(data);
    HD44780_SetLowerNibble(data);
    HD44780_Pulse_E();
    Delay_Us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param byte of data in 4 bit mode.  In 4 bit mode
 * the byte is sent only on D7-D4, and is sent in two steps
 * (4 MSB followed by 4 LSB).
 * 
 * @param data byte to send
 */
void HD44780_Send8BitsIn4BitMode(unsigned short int data) {
    // Send upper nibble
    HD44780_SetUpperNibble(data);
    HD44780_Pulse_E();

    // Send lower nibble
    HD44780_SetUpperNibble(data << 4);
    HD44780_Pulse_E();
    Delay_Us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param instruction to the HD44780 as a special four bit start
 * instruction.  This is used on initialization of the display to set the 
 * mode (4 or 8 bit).  Only the top nibble of the instruction is actually 
 * sent, and it's sent on D7-D4.
 * 
 * @param data Instruction to send
 */
void HD44780_Send4BitStartInstruction(unsigned short int data) {
    // RS low to write to instruction register
    funDigitalWrite(rsPin, 0);
    HD44780_Send4BitsIn4BitMode(data);
    Delay_Us(INSTRUCTION_DELAY_US);
}

/**
 * Sends the param instruction to the HD44780.
 * 
 * @param data Instruction to send
 */
void HD44780_SendInstruction(unsigned short int data) {
    // RS low to write to instruction register
    funDigitalWrite(rsPin, 0);

    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send8BitsIn4BitMode(data);
    } else {
        HD44780_Send8BitsIn8BitMode(data);
    }
}

/**
 * Sends the param character to the HD44780.
 * 
 * @param data Character to send
 */
void HD44780_SendData(unsigned short int data) {
    // RS high to write to data register
    funDigitalWrite(rsPin, 1);

    if (displayMode == HD44780_FOUR_BIT_MODE) {
        HD44780_Send8BitsIn4BitMode(data);
    } else {
        HD44780_Send8BitsIn8BitMode(data);
    }
}

