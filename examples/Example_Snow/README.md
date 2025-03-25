## CH32Fun HD44780 Snow Example

An example application showing a rudimentary snow effect on a two row, 16 column HD44780 based
character display.  Mostly here to prove that this libary works.

In order to build this project, simply run `make clean all` from within this directory.
This will clean the build environment, compile the project, and attempt to flash the
compiled binary to the CH32 via any attached WCH-LinkE programmer it can find.

In terms of physical connection, this project is by default designed to be run in HD44780 
four bit mode, and should be set up as follows.

| CH32V003 Pin | HD44780 Pin |
| :---: | :---: |
| PC3 | D4 |
| PC4 | D5 |
| PC5 | D6 |
| PC6 | D7 |
| PC1 | RS |
| PC2 | E |

On the display: 
- Pin 1 should be connected to ground and pin 2 connected to 5V.  
- Pin 3 is the contrast control, and needs to be connected to a voltage divider for tuning.  
- Typically, connect pin 3 to the center (wiper) pin of a 10K potentiometer, and connect 
one side to 5V and the other to ground (making an adjustable voltage divider).  
- Connect pin 5 (RW) to ground, to ensure that
the HD44780 is in write mode for all operations.
