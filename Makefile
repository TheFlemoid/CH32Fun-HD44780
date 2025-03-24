all : flash

TARGET:=textTest

ADDITIONAL_C_FILES+=HD44780.c

#TARGET:=HD44780

TARGET_MCU?=CH32V003
include ./ch32fun/ch32fun.mk

flash : cv_flash
clean : cv_clean

