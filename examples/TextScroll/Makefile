all : flash

TARGET:=text_scroll
ADDITIONAL_C_FILES+=../../HD44780.c

TARGET_MCU?=CH32V003
include ../../ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

