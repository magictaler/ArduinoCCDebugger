/*
  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */

#ifndef Messages_h
#define Messages_h

const char FIRMWARE_REV[] PROGMEM = "V1.1";
const char FIRMWARE_DATE[] PROGMEM = "20.06.16";
const char PRODUCT_NAME[] PROGMEM = "Arduino CC-Debugger for CC2540/41 SOC (BLE112, BLE113, HM-10)";
const char RC_OK[] PROGMEM = "OK";
const char SPACE_CHAR[] PROGMEM = " ";
const char OPENING_BRACKET[] PROGMEM = " [";
const char COMMA[] PROGMEM = ", ";
const char TICKED[] PROGMEM = "X";
const char KB[] PROGMEM = " Kb";
const char CHIP_ERASE_BUSY[] PROGMEM = "] CHIP_ERASE_BUSY";
const char PCON_IDLE[] PROGMEM = "] PCON_IDLE";
const char CPU_HALTED[] PROGMEM = "] CPU_HALTED";
const char PM_ACTIVE[] PROGMEM = "] PM_ACTIVE";
const char HALT_STATUS[] PROGMEM = "] HALT_STATUS";
const char DEBUG_LOCKED[] PROGMEM = "] DEBUG_LOCKED";
const char OSCILLATOR_STABLE[] PROGMEM = "] OSCILLATOR_STABLE";
const char STACK_OVERFLOW[] PROGMEM = "] STACK_OVERFLOW";
const char CHIP_INFORMATION[] PROGMEM = "Chip information:";
const char CHIP_ID[] PROGMEM =    "      Chip ID : 0x";
const char FLASH_SIZE[] PROGMEM = "   Flash size : ";
const char SRAM_SIZE[] PROGMEM =  "    SRAM size : ";
const char USB[] PROGMEM =        "          USB : ";
const char PC[] PROGMEM =         "           PC : ";
const char LICENSE[] PROGMEM =    "      License : ";
const char BT_ADDR[] PROGMEM =    "   BT Address : ";
const char HW_VER[] PROGMEM =     " Hardware Ver : ";
const char BLE_STORE[] PROGMEM =  "    BLE Store : ";
const char PAGES[] PROGMEM = " pages";
const char BYTES[] PROGMEM = " bytes";
const char YES[] PROGMEM = "Yes";
const char NO[] PROGMEM = "No";
const char BLANK[] PROGMEM = "N/A (BLANK)";
const char DEVICE_INFO[] PROGMEM = "Device information:";
const char IEEE_ADDR[] PROGMEM = " IEEE Address : ";
const char FW_INFO[] PROGMEM = "Firmware information:";
const char DBG_STATUS[] PROGMEM = "Debug status:";
const char DBG_CONFIG[] PROGMEM = "Debug config:";
const char SOFT_POWER_MODE[] PROGMEM = "] SOFT_POWER_MODE";
const char TIMERS_OFF[] PROGMEM = "] TIMERS_OFF";
const char DMA_PAUSE[] PROGMEM = "] DMA_PAUSE";
const char TIMER_SUSPEND[] PROGMEM = "] TIMER_SUSPEND";
const char ARDUINO_BUF_SZ[] PROGMEM =  "Arduino RX serial buffer size : ";

#endif

