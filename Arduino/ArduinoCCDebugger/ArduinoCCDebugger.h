/*
  Arduino CC-Debugger
  
  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */

#ifndef ARDUINO_CC_DEBUGGER_H
#define ARDUINO_CC_DEBUGGER_H

#include <Arduino.h>
#include <avr/pgmspace.h>
#include <avr/eeprom.h>
#include <util/delay.h>
#include "CCDebugger.h"
#include "CmdProc.h"
#include "Messages.h"                    //Collection of string constants
#include "Utils.h"                       //Conversion utils and helpers

// Command constants
#define   CONSOLE_CMD_INFO  byte(0x61)
#define   CMD_READ          byte(0x2)
#define   CMD_WRITE         byte(0x3)
#define   CMD_BLE_STORE     byte(0x4)
#define   CMD_ERASE         byte(0x5)
#define   CMD_IEEE_ADDR     byte(0x6)
#define   CMD_LICENSE       byte(0x7)
#define   CMD_BT_ADDR       byte(0x8)
#define   CMD_HW_VER        byte(0x9)
#define   CMD_CHIP_INFO     byte(0xA)
#define   CMD_IN_BUF_SZ     byte(0xB)

// Response constants
#define   ANS_OK            byte(0x81)
#define   ANS_ERROR         byte(0x82)

#define   ERR_OTHER         byte(0xFF)
#define   ERR_CHECKSUM      byte(0xFE)
#define   ERR_FLASHING      byte(0xFD)

#define IO_BUF_SZ 1024L

extern byte ioBuf[IO_BUF_SZ];

#endif
