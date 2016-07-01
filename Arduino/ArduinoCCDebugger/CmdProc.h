/*
  Arduino CC-Debugger Command Processor
  
  Based on Python code by Ioannis Charalampidis
  This functionality doesn't suffer from significant performance issues which was the case for Python scripts
  
  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */

#ifndef CMDPROC_H
#define CMDPROC_H

#include "ArduinoCCDebugger.h"

#define MAX_BURST_WRITE 2048
#define BIN_PACKET_HDR 1
#define BIN_PACKET_SMPL_FTR 2
#define BIN_PACKET_FTR 5
#define DEVICE_INFO_ADDR 0x780E
#define DEVICE_INFO_SZ 6
#define BLUEGIGA_INFO_SZ 0x40
#define CHIP_INFO_ADDR 0x6276

#define CHIP_ERASE_BUSY_BIT 7
#define PCON_IDLE_BIT 6
#define CPU_HALTED_BIT 5
#define PM_ACTIVE_BIT 4
#define HALT_STATUS_BIT 3
#define DEBUG_LOCKED_BIT 2
#define OSCILLATOR_STABLE_BIT 1
#define STACK_OVERFLOW_BIT 0
#define SOFT_POWER_MODE_BIT 4
#define TIMERS_OFF_BIT 3
#define DMA_PAUSE_BIT 2
#define TIMER_SUSPEND_BIT 1
#define LICENSE_START_BYTE 7
#define LICENSE_END_BYTE 39
#define BT_ADDR_START_BYTE 42
#define BT_ADDR_END_BYTE 48
#define HW_VER_BYTE 39
#define FLASH_SZ_KB_MASK 0x70
#define SRAM_SZ_MASK 0x7
#define USB_PRESENT_MASK 0x8
#define XDATA_BANK_ADDR 0xC7
#define XDATA_BANK_MASK 0x7
#define PAUSE_DMA_BIT 2
#define DMA_CONFIG_AREA 8
#define DMA_VLEN_MASK 0x7
#define DMA_VLEN_SHIFT_L 5
#define DMA_TLEN_MASK 0x1F
#define DMA_TLEN_SHIFT_R 0x8
#define DMA_NWORD_BIT 7
#define DMA_TRANSFER_MODE_BIT 5
#define DMA_TRIGGER_MASK 0x1F
#define DMA_SRC_INC_MASK 0x3
#define DMA_DST_INC_MASK 0x3
#define DMA_SRC_INC_SHIFT_L 6
#define DMA_DST_INC_SHIFT_L 4
#define DMA_IRQ_BIT 3
#define DMA_M8_BIT 2
#define DMA_PRIORITY_MASK 0x3
#define DMA0CFGL 0xD4
#define DMA0CFGH 0xD5
#define DMA1CFGL 0xD2
#define DMA1CFGH 0xD3
#define DMAARM 0xD6
#define DMAIRQ 0xD1
#define FLASH_STAT_ADDR 0x6270
#define BANK_SIZE 0x8000
#define BLE_STORE_SZ_ADDR 0x1F7EF
#define FLASH_PAGE_SIZE 0x800L
#define BLE_STORE_ADDR 0x18000L

class CmdProc : public CCDebugger {
public:

  ////////////////////////////
  // Configuration
  ////////////////////////////

  /**
   * Initialize CmdProc class
   */
  CmdProc(int pinRST, int pinDC, int pinDD);

  ////////////////////////////
  // High-Level interaction
  ////////////////////////////
  bool info_cmd(void);
  bool chip_info(unsigned int& flashSizeKb, bool console_output);
  bool read_cmd(byte block_num);
  bool ble_store_cmd(byte block_num);
  bool ieee_addr_cmd(void);
  bool license_cmd(void);
  bool bt_addr_cmd(void);
  bool hw_ver_cmd(void);
  bool chip_info_cmd(void);
  bool chip_erase_cmd(void);  
  bool in_buf_sz(void);
  bool write_cmd(byte inCmd, byte c1, byte c2, byte c3, bool forced_flush = false);
  void renderDebugStatus(byte status);
  void renderDebugConfig(byte cfg);
  /*byte error()
  {
      if (dbg) return dbg->error();  
  }*/
  
  /////////////////////////////////
  // Serial communication functions
  /////////////////////////////////
  void generateHeader(byte cmd, unsigned int total_sz);
  void generateFooter(unsigned int checksum);
  void sendOK(void);
  void sendError(byte errCode);

  ////////////////////////////
  // Register access functions
  ////////////////////////////
  byte getRegister( byte reg );
  byte setRegister( byte reg, byte v );
  byte selectXDATABank( byte bank );

  ////////////////////////////
  // DMA related functions
  ////////////////////////////
  void pauseDMA(bool pause);
  void configDMAChannel(byte index, unsigned short srcAddr,  unsigned short dstAddr, byte trigger, 
    byte vlen = 0, unsigned short tlen = 1, bool word = false, byte transferMode = 0, byte srcInc = 0, 
    byte dstInc = 0, bool interrupt = false, bool m8 = true, byte priority = 0,  
    unsigned short memBase = 0x1000);
  void armDMAChannel(byte index);
  void disarmDMAChannel(byte index);
  bool isDMAIRQ(byte index);
  void clearDMAIRQ(byte index);

  ////////////////////////////
  // Flash related functions
  ////////////////////////////
  bool clearFlashStatus( void );
  bool setFlashErase( void );
  bool setFlashWrite( void );
  bool isFlashBusy( void );
  bool isFlashAbort( void );

  ////////////////////////////
  // Block memory operations
  ////////////////////////////
  unsigned short writeXDATA(unsigned short offset, unsigned short sz, byte* buf);
  unsigned short readXDATA(unsigned short offset, unsigned short sz, byte* buf);
  bool verifyXDATA(unsigned short offset, unsigned short sz, byte* buf);
  bool verifyCODE(unsigned int offset, unsigned short sz, byte* buf);
  unsigned short burstWrite(unsigned short offset, unsigned short sz, byte* buf);
  unsigned short writeCODE(unsigned long offset, unsigned short sz, byte* buf, bool erase = false, bool verify = false);
  unsigned short readCODE(unsigned long offset, unsigned short sz, byte* buf);

private:

  ////////////////////////////
  // Private/Helper parts
  ////////////////////////////
  //CCDebugger * dbg;
  unsigned short total_blocks;
  unsigned long bytes_to_send;
  unsigned int flashSizeKb;

};

#endif

