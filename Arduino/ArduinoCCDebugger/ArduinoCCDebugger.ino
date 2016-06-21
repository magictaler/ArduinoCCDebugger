/*
  Arduino CC-Debugger

  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */

#include "ArduinoCCDebugger.h"

// Pinout configuration
int CC_RST   = 6;
int CC_DD    = 4;
int CC_DC    = 3;

byte inCmd;
byte c1, c2, c3;
CmdProc * cmdProc;
byte ioBuf[IO_BUF_SZ];

/**
 * Initialize debugger
 */
void setup() 
{
    // Initialize serial port
    Serial.begin(115200);
    cmdProc = new CmdProc( CC_RST, CC_DC, CC_DD );
}

/**
 * Main program loop
 */
void loop() 
{
    // Wait for incoming data frame
    if (Serial.available() < 4) return;
  
    // Read input frame
    inCmd = Serial.read();
        c1 = Serial.read();
        c2 = Serial.read();
        c3 = Serial.read();  
  
    // Handle commands
    if (inCmd == CONSOLE_CMD_INFO) 
    {
        serial_print_p(PRODUCT_NAME);
        serial_print_p(SPACE_CHAR);
        serial_print_p(FIRMWARE_REV);
        serial_print_p(SPACE_CHAR);
        serial_println_p(FIRMWARE_DATE);

        if (!cmdProc->info_cmd()) 
          cmdProc->sendError(cmdProc->error());      

    } else if (inCmd == CMD_READ) 
    {
        if (!cmdProc->read_cmd(c1)) cmdProc->sendError(cmdProc->error());      
    } else if (inCmd == CMD_WRITE) 
    {
         cmdProc->write_cmd(inCmd, c1, c2, c3);
    } else if (inCmd == CMD_BLE_STORE)
    {
        if (!cmdProc->ble_store_cmd(c1)) cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_ERASE)
    {
        if (cmdProc->chip_erase_cmd())
        {
            unsigned short checksum = CMD_ERASE;
            cmdProc->generateHeader(CMD_ERASE, BIN_PACKET_SMPL_FTR + BIN_PACKET_HDR);
            cmdProc->generateFooter(checksum);  
        }
        else cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_IEEE_ADDR)
    {
        if (!cmdProc->ieee_addr_cmd()) cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_LICENSE)
    {
        if (!cmdProc->license_cmd()) cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_BT_ADDR)
    {
        if (!cmdProc->bt_addr_cmd()) cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_HW_VER)
    {
        if (!cmdProc->hw_ver_cmd()) cmdProc->sendError(cmdProc->error());
    } else if (inCmd == CMD_CHIP_INFO)
    {
        if (!cmdProc->chip_info_cmd()) cmdProc->sendError(cmdProc->error());
    }
    else if (inCmd == CMD_IN_BUF_SZ)
    {
        if (!cmdProc->in_buf_sz()) cmdProc->sendError(cmdProc->error());
    }else
    {
      cmdProc->sendError(ERR_OTHER);
    }
}
