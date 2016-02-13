/*
  Arduino CC-Debugger Command Processor
  
  Based on Python code by Ioannis Charalampidis
  This functionality doesn't suffer from significant performance issues which was the case for Python scripts
  
  As the original library, Arduino-CCDebugger targets only the CC2540 SoC which is used by BlueGiga's BLE112 & BLE113 modules.

  !!! WARNING !!!  
  The higher-level functions are *TAILORED* for CC2540/41 SOC (BLE112,BLE113). 
  DO NOT USE them for ANY other chip!

  
  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */
 
#include "CmdProc.h"

CmdProc::CmdProc( int pinRST, int pinDC, int pinDD_I, int pinDD_O ) : CCDebugger (pinRST, pinDC, pinDD_I, pinDD_O)
{
    // Enter debug mode
    enter();
}

void CmdProc::generateHeader(byte cmd, unsigned int total_sz)
{
    Serial.write((total_sz >> 8) & 0xFF); 
    Serial.write(total_sz & 0xFF);         
    Serial.write(cmd);
}

void CmdProc::generateFooter(unsigned int checksum)
{
    Serial.write((checksum >> 8) & 0xFF);
    Serial.write(checksum & 0xFF);
}


void CmdProc::sendOK(void)
{
    unsigned short checksum = ANS_OK;
    generateHeader(ANS_OK, 0x3);
    generateFooter(checksum);  
}

void CmdProc::sendError(byte errCode)
{
    unsigned short checksum = ANS_ERROR + errCode;
    generateHeader(ANS_ERROR, 0x4);
    Serial.write(errCode);
    generateFooter(checksum);  
}

bool CmdProc::read_cmd( byte block_num )
{
    if (block_num == 0)
    {
        //unsigned int flashSizeKb;
        if (!chip_info(flashSizeKb, false)) 
    {        
        return false;
    }
        total_blocks = (unsigned long)flashSizeKb * 1024L / IO_BUF_SZ;
    }
    
    if (block_num >= total_blocks)
    {
        return false;  
    }else
    {
        if (IO_BUF_SZ != readCODE( block_num * IO_BUF_SZ, IO_BUF_SZ, ioBuf )) 
        {
            return false;
        }

        unsigned short checksum = 0;
        
        // adding header + footer to accomodate 1 byte for packet type, 
        //1 byte block number, 1 byte for total blocks and 2 byte checksum
        unsigned short packet_len = IO_BUF_SZ + BIN_PACKET_HDR + BIN_PACKET_FTR; 
        
        generateHeader(CMD_READ, packet_len);
        checksum += CMD_READ;
        for (unsigned short i = 0; i < IO_BUF_SZ; i++)
        {
            Serial.write(ioBuf[i]);
            checksum += ioBuf[i];
        }
        Serial.write(block_num);
        Serial.write(total_blocks);        
        checksum += block_num;
        checksum += total_blocks;
        generateFooter(checksum);
    }
    return true;  
}

bool CmdProc::info_cmd( void )
{
    //unsigned int flashSizeKb;
    if (!chip_info(flashSizeKb, true)) return false;

    // Get device information from the read-only section
    Serial.println();    
    serial_println_p(DEVICE_INFO);
    
    byte ieeeAddr[DEVICE_INFO_SZ];
    if (sizeof(ieeeAddr) != readXDATA(DEVICE_INFO_ADDR, sizeof(ieeeAddr), ieeeAddr)) return false;
    serial_print_p(IEEE_ADDR);
    for (byte i = 0; i < sizeof(ieeeAddr); i++)
    {
        Serial.print(ieeeAddr[i], HEX);
        serial_print_p(SPACE_CHAR);
    }
    Serial.println();
    
    unsigned short s1 = getPC();
    if (error()) return false;
    serial_print_p(PC);
    Serial.println(s1, HEX);

    
    //Return the translated Bluegiga information struct (last 64 bytes)
    if (BLUEGIGA_INFO_SZ != readCODE( flashSizeKb * 1024L - BLUEGIGA_INFO_SZ, BLUEGIGA_INFO_SZ, ioBuf )) return false;
    Serial.println();
    serial_println_p(FW_INFO);
    
    serial_print_p(LICENSE);
    for (byte i = LICENSE_START_BYTE; i < LICENSE_END_BYTE; i++)
    {
        Serial.print(ioBuf[i], HEX);
        serial_print_p(SPACE_CHAR);
    }
    Serial.println();

    serial_print_p(BT_ADDR);
    for (byte i = BT_ADDR_START_BYTE; i < BT_ADDR_END_BYTE; i++)
    {
        Serial.print(ioBuf[i], HEX);
        serial_print_p(SPACE_CHAR);
    }
    Serial.println();
    
    serial_print_p(HW_VER);
    Serial.print(ioBuf[HW_VER_BYTE], HEX);
    Serial.println();


    serial_print_p(BLE_STORE);
    if (1 != readCODE(BLE_STORE_SZ_ADDR, 1, ioBuf )) return false;
    if (ioBuf[0] > (unsigned long)flashSizeKb * 1024L / FLASH_PAGE_SIZE)
    {
      serial_print_p(BLANK);
    }else
    {
      unsigned long ble_store_pages = (unsigned long)ioBuf[0];
      unsigned long ble_store_size = (unsigned long)ioBuf[0] * FLASH_PAGE_SIZE;

      Serial.print(ble_store_pages, DEC);
      serial_print_p(PAGES);        
      serial_print_p(COMMA);    
      Serial.print(ble_store_size, DEC);    
      serial_print_p(BYTES);            
      Serial.println();
    }
    
    // Get status
    Serial.println();    
    serial_println_p(DBG_STATUS);    
    byte bAns = getStatus();
    renderDebugStatus(bAns);
    
    // Get config
    Serial.println();    
    serial_println_p(DBG_CONFIG);    
    bAns = getConfig();
    renderDebugConfig(bAns);

    Serial.println();    
    serial_print_p(ARDUINO_BUF_SZ);
    Serial.print(SERIAL_RX_BUFFER_SIZE, DEC);    
    serial_print_p(BYTES);            
    Serial.println();

    return true;
}

bool CmdProc::chip_info(unsigned int& flashSizeKb, bool console_output)
{
    // ChipID
    unsigned short s1 = getChipID();
    if (error()) return false;
    
    if (console_output)
    {
        Serial.println();
        serial_println_p(CHIP_INFORMATION);
        serial_print_p(CHIP_ID);
        Serial.println(s1, HEX);
    }
    
    // ChipInfo
    byte chipInfo[2];
    if (sizeof(chipInfo) != readXDATA(CHIP_INFO_ADDR, sizeof(chipInfo), chipInfo)) return false;
    
    flashSizeKb = powint(2, 4 + ((chipInfo[0] & FLASH_SZ_KB_MASK) >> 4));
    
    if (console_output)
    {
        serial_print_p(FLASH_SIZE);
        Serial.print(flashSizeKb, DEC);
        serial_println_p(KB);
        
        serial_print_p(SRAM_SIZE);
        Serial.print((chipInfo[1] & SRAM_SZ_MASK) + 1, DEC);
        serial_println_p(KB);

        serial_print_p(USB);    
        if ((chipInfo[0] & USB_PRESENT_MASK) != 0)
            serial_println_p(YES);
        else
            serial_println_p(NO);  
    }
        
    return true;
}

void CmdProc::renderDebugStatus(byte status)
{
    // Visualize debug status

    serial_print_p(OPENING_BRACKET);
    if ((status & (1 << CHIP_ERASE_BUSY_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(CHIP_ERASE_BUSY);

    serial_print_p(OPENING_BRACKET);
    if ((status & (1 << PCON_IDLE_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(PCON_IDLE);

    serial_print_p(OPENING_BRACKET);
    if ((status & (1 << CPU_HALTED_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(CPU_HALTED);
    
    serial_print_p(OPENING_BRACKET);      
    if ((status & (1 << PM_ACTIVE_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(PM_ACTIVE);

    serial_print_p(OPENING_BRACKET);      
    if ((status & (1 << HALT_STATUS_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(HALT_STATUS);
      
    serial_print_p(OPENING_BRACKET);      
    if ((status & (1 << DEBUG_LOCKED_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(DEBUG_LOCKED);

    serial_print_p(OPENING_BRACKET);
    if ((status & (1 << OSCILLATOR_STABLE_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(OSCILLATOR_STABLE);

    serial_print_p(OPENING_BRACKET);        
    if ((status & (1 << STACK_OVERFLOW_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(STACK_OVERFLOW);
}

void CmdProc::renderDebugConfig(byte cfg)
{
    // Visualize debug config

    serial_print_p(OPENING_BRACKET);
    if ((cfg & (1 << SOFT_POWER_MODE_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(SOFT_POWER_MODE);

    serial_print_p(OPENING_BRACKET);
    if ((cfg & (1 << TIMERS_OFF_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(TIMERS_OFF);
        
    serial_print_p(OPENING_BRACKET);
    if ((cfg & (1 << DMA_PAUSE_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(DMA_PAUSE);

    serial_print_p(OPENING_BRACKET);
    if ((cfg & (1 << TIMER_SUSPEND_BIT)) != 0)
        serial_print_p(TICKED);
    else
        serial_print_p(SPACE_CHAR);
    serial_println_p(TIMER_SUSPEND);

}

unsigned short CmdProc::writeXDATA(unsigned short offset, unsigned short sz, byte* buf)
{
    byte bAns = execi( 0x90, offset ); // MOV DPTR,#data16
    if (error()) return 0;

    unsigned short i;
    for (i = 0; i < sz; i++)
    {
        bAns = exec( 0x74, buf[i] );  // MOV A,#data
        if (error()) return 0;

        bAns = exec( 0xF0 );  // MOVX @DPTR,A
        if (error()) return 0;                
        
        bAns = exec( 0xA3 );  // INC DPTR
        if (error()) return 0;        
    }
    return i;
}

unsigned short CmdProc::readXDATA(unsigned short offset, unsigned short sz, byte* buf)
{
    byte bAns = execi( 0x90, offset ); // MOV DPTR,#data16
    if (error()) return 0;

    unsigned short i;
    for (i = 0; i < sz; i++)
    {
        bAns = exec( 0xE0 );  // MOVX A,@DPTR
        if (error()) return 0;
        buf[i] = bAns;      
        bAns = exec( 0xA3 );  // INC DPTR
        if (error()) return 0;        
    }
    return i;
}

byte CmdProc::getRegister( byte reg )
{
    byte res = exec( 0xE5, reg );
    if (error()) return 0;
    return res;
}

byte CmdProc::setRegister( byte reg, byte v )
{
    byte res = exec( 0x75, reg, v );
    if (error()) return 0;
    return res;
}

byte CmdProc::selectXDATABank( byte bank )
{
    byte a = getRegister( XDATA_BANK_ADDR );
    a = (a & 0xF8) | (bank & XDATA_BANK_MASK);
    return setRegister( 0xC7, a );
}

void CmdProc::pauseDMA(bool pause)
{
    //Pause/Unpause DMA in debug mode
    byte a = getConfig();
    if (pause)
        a |= (1 << PAUSE_DMA_BIT);
    else
        a &= ~(1 << PAUSE_DMA_BIT);
    setConfig(a);
}

void CmdProc::configDMAChannel(byte index, unsigned short srcAddr,  unsigned short dstAddr, byte trigger, 
  byte vlen, unsigned short tlen, bool word, byte transferMode, byte srcInc, 
  byte dstInc, bool interrupt, bool m8, byte priority, unsigned short memBase)
{
    // Create a DMA buffer and place it in memory
    
    // Calculate numeric flags
    byte nword = 0;
    if (word)  nword = 1;
        
    byte nirq = 0;
    if (interrupt) nirq = 1;
    
    byte nm8 = 1;
    if (m8) nm8 = 0;
   
    // Prepare DMA configuration bytes
    byte config[DMA_CONFIG_AREA];
    config[0] = (srcAddr >> 8) & 0xFF;                                 // 0: SRCADDR[15:8]
    config[1] = (srcAddr & 0xFF);                                      // 1: SRCADDR[7:0]              
    config[2] = (dstAddr >> 8) & 0xFF;                                 // 2: DESTADDR[15:8]
    config[3] = (dstAddr & 0xFF);                                      // 3: DESTADDR[7:0]
    config[4] = (vlen & DMA_VLEN_MASK) << DMA_VLEN_SHIFT_L |           // 4: VLEN[2:0]
                ((tlen >> DMA_TLEN_SHIFT_R) & DMA_TLEN_MASK);          //    | TLEN[12:8]
    config[5] = (tlen & 0xFF);                                         // 5: LEN[7:0]
    config[6] = (nword << DMA_NWORD_BIT) | 
                (transferMode << DMA_TRANSFER_MODE_BIT) |              // 6: WORDSIZE | TMODE[1:0] |
                (trigger & DMA_TRIGGER_MASK);                          //    TRIG[4:0]
    config[7] = ((srcInc & DMA_SRC_INC_MASK) << DMA_SRC_INC_SHIFT_L) | // 7: SRCINC[1:0] |
                ((dstInc & DMA_DST_INC_MASK) << DMA_DST_INC_SHIFT_L) | //    DESTINC[1:0] |
                (nirq << DMA_IRQ_BIT) |                                //    IRQMASK |
                (nm8 << DMA_M8_BIT) |                                  //    M8 |
                (priority & DMA_PRIORITY_MASK);                        //    PRIORITY[1:0]

    // Pick an offset in memory to store the configuration
    unsigned short memAddr = memBase + index * DMA_CONFIG_AREA;
    writeXDATA( memAddr, DMA_CONFIG_AREA, config );

    // Split address in high/low
    byte cHigh = (memAddr >> 8) & 0xFF;
    byte cLow = (memAddr & 0xFF);

    // Update DMA registers
    if (index == 0)
    {
        exec( 0x75, DMA0CFGL, cLow  ); // MOV direct,#data @ DMA0CFGL
        exec( 0x75, DMA0CFGH, cHigh ); // MOV direct,#data @ DMA0CFGH
    }else{
        // For DMA1+ they reside one after the other, starting
        // on the base address of the first in DMA1CFGH:DMA1CFGL
      	memAddr = memBase + 8;
        cHigh = (memAddr >> 8) & 0xFF;
        cLow = (memAddr & 0xFF);

        exec( 0x75, DMA1CFGL, cLow  ); // MOV direct,#data @ DMA1CFGL
        exec( 0x75, DMA1CFGH, cHigh ); // MOV direct,#data @ DMA1CFGH
    }
}

void CmdProc::armDMAChannel(byte index)
{
    // Arm a DMA channel (index in 0-4)

    // Get DMAARM state
    byte a = getRegister(DMAARM); // MOV A,direct @ DMAARM
  
    // Set given flag
    a |= (1 << index);
    
    // Update DMAARM state
    setRegister(DMAARM, a); // MOV direct,#data @ DMAARM

    _delay_ms(10);
}

void CmdProc::disarmDMAChannel(byte index)
{
    //Disarm a DMA channel (index in 0-4)

    // Get DMAARM state
    byte a = getRegister( DMAARM );
    
    // Unset given flag
    a &= ~ (1 << index);

    // Update DMAARM state
    setRegister( DMAARM, a );
}

bool CmdProc::isDMAIRQ(byte index)
{
    // Check if DMA IRQ flag is set (index in 0-4)

    // Get DMAIRQ state
    byte a = getRegister(DMAIRQ);

    // Lookup IRQ bit
    byte bt = (1 << index);

    // Check if IRQ bit is set
    return ((a & bt) != 0);
}

void CmdProc::clearDMAIRQ(byte index)
{
    // Clear DMA IRQ flag (index in 0-4)

    // Get DMAIRQ state
    byte a = getRegister(DMAIRQ);

    // Unset given flag
    a &= ~ (1 << index);
    
    // Update DMAIRQ state
    setRegister( DMAIRQ, a );
}

bool CmdProc::clearFlashStatus( void )
{
    // Clear the flash status register
    byte flashStat[1];
    if (sizeof(flashStat) != readXDATA(FLASH_STAT_ADDR, sizeof(flashStat), flashStat)) return false;
    flashStat[0] &= 0x1F;
    return writeXDATA(FLASH_STAT_ADDR, 1, flashStat);
}

bool CmdProc::setFlashErase( void )
{
    // Set the ERASE bit in the flash control register
    byte flashStat[1];    
    if (sizeof(flashStat) != readXDATA(FLASH_STAT_ADDR, sizeof(flashStat), flashStat)) return false;    
    flashStat[0] |= 0x01;
    return writeXDATA(FLASH_STAT_ADDR, 1, flashStat);
}

bool CmdProc::setFlashWrite( void )
{
    // Set the WRITE bit in the flash control register
    byte flashStat[1];
    if (sizeof(flashStat) != readXDATA(FLASH_STAT_ADDR, sizeof(flashStat), flashStat)) return false;    
    flashStat[0] |= 0x02;
    return writeXDATA(FLASH_STAT_ADDR, 1, flashStat);
}

bool CmdProc::isFlashBusy( void )
{
    // Check if the BUSY bit is set in the flash register
    byte flashStat[1];    
    if (sizeof(flashStat) != readXDATA(FLASH_STAT_ADDR, sizeof(flashStat), flashStat)) return false;    
    return (flashStat[0] & 0x80 != 0);
}

bool CmdProc::isFlashAbort( void )
{
    // Check if the ABORT bit is set in the flash register
    byte flashStat[1];    
    if (sizeof(flashStat) != readXDATA(FLASH_STAT_ADDR, sizeof(flashStat), flashStat)) return false;    
    return (flashStat[0] & 0x20 != 0);
}

bool CmdProc::verifyXDATA(unsigned short offset, unsigned short sz, byte* buf)
{
    byte bAns = execi( 0x90, offset ); // MOV DPTR,#data16
    if (error()) return false;

    unsigned short i;
    for (i = 0; i < sz; i++)
    {
        bAns = exec( 0xE0 );  // MOVX A,@DPTR
        if (error()) return 0;
        if (buf[i] != bAns) return false;
        bAns = exec( 0xA3 );  // INC DPTR
        if (error()) return 0;        
    }
    return true;
}

bool CmdProc::verifyCODE(unsigned int offset, unsigned short sz, byte* buf)
{
    //Verify any size of buffer from the XDATA+0x8000 (code-mapped) region
     
    //Pick the code bank this code chunk belongs to
    int fBank = int(offset / BANK_SIZE );
    selectXDATABank( fBank );
    if (error()) return false;

    // Recalibrate offset
    offset -= fBank * BANK_SIZE;

    // Verify XDATA-mapped CODE region
    return verifyXDATA( BANK_SIZE + offset, sz, buf );
}

unsigned short CmdProc::burstWrite(unsigned short offset, unsigned short sz, byte* buf)
{
    if (sz > MAX_BURST_WRITE) return 0;
    
    // Prepare for burst-write      
    writeFast( 0x80 | ((sz >> 8) & 0x07) ); // High-order bits
    writeFast( sz & 0xFF ); // Low-order bits

    unsigned short i = 0;
    for (i = 0; i < sz; i++)
    {
      writeFast( buf[offset + i] );
      if (error()) break;
    }
      
    switchRead();
    byte bAns = readFast(); //ignore it as we return number of bytes written
    switchWrite();
    
    return i;
}

unsigned short CmdProc::writeCODE(unsigned long offset, unsigned short sz, byte* buf, bool erase, bool verify)
{
    // Fully automated function for writing the Flash memory.

    //WARNING: This requires DMA operations to be unpaused ( use: pauseDMA(False) )

    // Prepare DMA-0 for DEBUG -> RAM (using DBG_BW trigger)
    configDMAChannel( 0, 0x6260, 0x0000, 0x1F, 0, IO_BUF_SZ, false, 0, 0, 1, true, true, 1);
    // Prepare DMA-1 for RAM -> FLASH (using the FLASH trigger)
    configDMAChannel( 1, 0x0000, 0x6273, 0x12, 0, IO_BUF_SZ, false, 0, 1, 0, true, true, 2);
    
    // Reset flags
    if (!clearFlashStatus()) return false;
    clearDMAIRQ(0);
    clearDMAIRQ(1);
    disarmDMAChannel(0);
    disarmDMAChannel(1);
    byte flashRetries = 0;

    unsigned short iOfs = 0;
    while (iOfs < sz)
    {
      // Get next page
      unsigned short iLen = min( sz - iOfs, IO_BUF_SZ );

      // Update DMA configuration if we have less than bulk-block size data 
      if (iLen < IO_BUF_SZ)
      {
          configDMAChannel( 0, 0x6260, 0x0000, 0x1F, 0, iLen, false, 0, 0, 1, true, true, 1);
          configDMAChannel( 1, 0x0000, 0x6273, 0x12, 0, iLen, false, 0, 1, 0, true, true, 2);
      }

      // Upload to RAM through DMA-0
      armDMAChannel(0);
      burstWrite( 0, iLen, buf );
      
      // Wait until DMA-0 raises interrupt
      while (!isDMAIRQ(0))
      {
          _delay_ms(100);
      }
      
      // Clear DMA IRQ flag
      clearDMAIRQ(0);

      // Calculate the page where this data belong to
      unsigned long fAddr = offset + iOfs;
      unsigned short fPage = fAddr / FLASH_PAGE_SIZE;
      
      // Calculate FLASH address High/Low bytes
      // for writing (addressable as 32-bit words)
      unsigned short fWordOffset = fAddr / 4;
      
      byte addr[2];
      
      addr[0] = fWordOffset & 0xFF;      
      addr[1] = (fWordOffset >> 8) & 0xFF;
      
      writeXDATA( 0x6271, 2, addr );
      
      // Check if we should erase page first
      if (erase && (offset % 2048 == 0))
      {
          // Select the page to erase using FADDRH[7:1]
          //
          // NOTE: Specific to (CC2530, CC2531, CC2540, and CC2541),
          //       the CC2533 uses FADDRH[6:0]
          //
          addr[0] = 0;          
          addr[1] = (fPage << 1);
          writeXDATA( 0x6271, 2, addr );
             
          // Set the erase bit
          setFlashErase();
          // Wait until flash is not busy any more
          while (isFlashBusy())
          {                
              _delay_ms(100);
          }
      }
      
      // Upload to FLASH through DMA-1
      armDMAChannel(1);
      setFlashWrite();

      // Wait until DMA-1 raises interrupt
      while (!isDMAIRQ(1))
      {
          // Also check for errors
          if (isFlashAbort())
          {
              disarmDMAChannel(1);
              //raise IOError("Flash page 0x%02x is locked!" % fPage)
              return 0;
          }
          _delay_ms(100);
      }

      // Clear DMA IRQ flag
      clearDMAIRQ(1);

      // Check if we should verify
      if (verify)
      {
          byte flashRetries = 0;
          bool verified = false;
          while (flashRetries < 3)
          {
              if (!verifyCODE(fAddr, iLen, buf))
              {
                  verified = true;
                  break;  
              }
          }
          if (!verified) return 0;
      }
              
      flashRetries = 0;

      // Forward to next page
      iOfs += iLen;
   }
   return iOfs;
}

unsigned short CmdProc::readCODE(unsigned long offset, unsigned short sz, byte* buf)
{
    //Read any size of buffer from the XDATA+0x8000 (code-mapped) region
     
    //Pick the code bank this code chunk belongs to
    unsigned int fBank = offset / BANK_SIZE;
    selectXDATABank( fBank );
    if (error()) return 0;

    // Recalibrate offset
    offset -= fBank * BANK_SIZE;

    // Read XDATA-mapped CODE region
    return readXDATA( BANK_SIZE + offset, sz, buf );
}

bool CmdProc::ble_store_cmd(byte block_num)
{
    if (block_num == 0)
    {
        //unsigned int flashSizeKb;
        if (!chip_info(flashSizeKb, false)) return false;

        if (1 != readCODE(BLE_STORE_SZ_ADDR, 1, ioBuf )) return false;
    
        if (ioBuf[0] > (unsigned long)flashSizeKb * 1024L / FLASH_PAGE_SIZE) 
        {
          //No valid length, empty response (not an error as it could be just an erased (blank) flash)
          unsigned short checksum = 0;
          unsigned short packet_len = BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
          generateHeader(CMD_BLE_STORE, packet_len);
          checksum += CMD_BLE_STORE;
          generateFooter(checksum);          
          return true;
        }

        bytes_to_send = (unsigned long)ioBuf[0] * FLASH_PAGE_SIZE;    
        total_blocks = bytes_to_send / IO_BUF_SZ;
    }
    
    if (block_num >= total_blocks)
        return false;
    else
    {
        unsigned long block_size = bytes_to_send - IO_BUF_SZ * block_num;
        if (block_size > IO_BUF_SZ) block_size = IO_BUF_SZ;
        
        if (block_size != readCODE( BLE_STORE_ADDR + block_num * IO_BUF_SZ, block_size, ioBuf )) 
        {
            return false;
        }

        unsigned short checksum = 0;
        
        // adding header + footer to accomodate 1 byte for packet type, 
        //1 byte block number, 1 byte for total blocks and 2 byte checksum
        unsigned short packet_len = block_size + BIN_PACKET_HDR + BIN_PACKET_FTR; 

        //TODO: should we include starting addr for consistency sake on frontend side?        
        
        generateHeader(CMD_BLE_STORE, packet_len);
        checksum += CMD_BLE_STORE;
        for (unsigned short i = 0; i < block_size; i++)
        {
            Serial.write(ioBuf[i]);
            checksum += ioBuf[i];
        }
        Serial.write(block_num);
        Serial.write(total_blocks);        
        checksum += block_num;
        checksum += total_blocks;
        generateFooter(checksum);
    }    
    return true;
}


bool CmdProc::ieee_addr_cmd(void)
{
    if (DEVICE_INFO_SZ != readXDATA(DEVICE_INFO_ADDR, DEVICE_INFO_SZ, ioBuf)) return false;
    
    unsigned short packet_len = DEVICE_INFO_SZ + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;
    
    generateHeader(CMD_IEEE_ADDR, packet_len);
    
    checksum += CMD_IEEE_ADDR;
    for (unsigned short i = 0; i < DEVICE_INFO_SZ; i++)
    {
        Serial.write(ioBuf[i]);
        checksum += ioBuf[i];
    }
    generateFooter(checksum);
    return true;
}

bool CmdProc::license_cmd(void)
{
    //unsigned int flashSizeKb;
    if (!chip_info(flashSizeKb, false)) return false;
  
    //Return the translated Bluegiga information struct (last 64 bytes)
    if (BLUEGIGA_INFO_SZ != readCODE( flashSizeKb * 1024L - BLUEGIGA_INFO_SZ, BLUEGIGA_INFO_SZ, ioBuf )) return false;

    unsigned short packet_len = LICENSE_END_BYTE - LICENSE_START_BYTE + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;
    
    generateHeader(CMD_LICENSE, packet_len);
    
    checksum += CMD_LICENSE;
    for (byte i = LICENSE_START_BYTE; i < LICENSE_END_BYTE; i++)
    {
        Serial.write(ioBuf[i]);
        checksum += ioBuf[i];
    }
    generateFooter(checksum);
    return true;
}

bool CmdProc::bt_addr_cmd(void)
{
    //unsigned int flashSizeKb;
    if (!chip_info(flashSizeKb, false)) return false;
  
    //Return the translated Bluegiga information struct (last 64 bytes)
    if (BLUEGIGA_INFO_SZ != readCODE( flashSizeKb * 1024L - BLUEGIGA_INFO_SZ, BLUEGIGA_INFO_SZ, ioBuf )) return false;

    unsigned short packet_len = BT_ADDR_END_BYTE - BT_ADDR_START_BYTE + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;
    
    generateHeader(CMD_BT_ADDR, packet_len);
    
    checksum += CMD_BT_ADDR;
    for (byte i = BT_ADDR_START_BYTE; i < BT_ADDR_END_BYTE; i++)
    {
        Serial.write(ioBuf[i]);
        checksum += ioBuf[i];
    }
    generateFooter(checksum);
    return true;
}

bool CmdProc::hw_ver_cmd(void)
{
    //unsigned int flashSizeKb;
    if (!chip_info(flashSizeKb, false)) return false;
  
    //Return the translated Bluegiga information struct (last 64 bytes)
    if (BLUEGIGA_INFO_SZ != readCODE( flashSizeKb * 1024L - BLUEGIGA_INFO_SZ, BLUEGIGA_INFO_SZ, ioBuf )) return false;

    unsigned short packet_len = 1 + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;
    
    generateHeader(CMD_HW_VER, packet_len);
    
    checksum += CMD_HW_VER;
    Serial.write(ioBuf[HW_VER_BYTE]);
    checksum += ioBuf[HW_VER_BYTE];
    generateFooter(checksum);
    return true;
}

bool CmdProc::in_buf_sz(void)
{
    unsigned short buf_sz = SERIAL_RX_BUFFER_SIZE;
    unsigned short packet_len = sizeof (buf_sz) + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;

    generateHeader(CMD_IN_BUF_SZ, packet_len);
    checksum += CMD_IN_BUF_SZ;;

    Serial.write((buf_sz >> 8) & 0xFF);
    checksum += (buf_sz >> 8) & 0xFF;
    
    Serial.write(buf_sz & 0xFF);
    checksum += buf_sz & 0xFF;

    generateFooter(checksum);
    
    return true;
}

bool CmdProc::chip_info_cmd(void)
{
    // ChipID
    unsigned short chip_id = getChipID();
    if (error()) return false;
    
    // ChipInfo
    byte chipInfo[2];
    if (sizeof(chipInfo) != readXDATA(CHIP_INFO_ADDR, sizeof(chipInfo), chipInfo)) return false;
    
    unsigned short packet_len = sizeof (chip_id) + 3 + BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR; 
    unsigned short checksum = 0;
    
    generateHeader(CMD_CHIP_INFO, packet_len);
    checksum += CMD_CHIP_INFO;
    
    Serial.write((chip_id >> 8) & 0xFF);
    checksum += (chip_id >> 8) & 0xFF;
    
    Serial.write(chip_id & 0xFF);
    checksum += chip_id & 0xFF;
    
    byte flashSizeKb  = (byte)(powint(2, 4 + ((chipInfo[0] & FLASH_SZ_KB_MASK) >> 4)));
    Serial.write(flashSizeKb);
    checksum += flashSizeKb;
    
    byte sramSizeKb = (byte)((chipInfo[1] & SRAM_SZ_MASK) + 1);
    Serial.write(sramSizeKb);
    checksum += sramSizeKb;
    
    byte usbPresent = 0;
    if ((chipInfo[0] & USB_PRESENT_MASK) != 0)
      usbPresent = 1;
      
    Serial.write(usbPresent);
    checksum += usbPresent;
    
    generateFooter(checksum);
    
    return true;
}

bool CmdProc::chip_erase_cmd(void)
{
    enter();
    
    chipErase();
    
    byte bAns = getStatus();
    while (( bAns & 0x80 ) != 0)
    {
      _delay_ms(100);
      bAns = getStatus();
    }
    
    return ((bAns & 0x80) == 0);
}

bool CmdProc::write_cmd(byte inCmd, byte c1, byte c2, byte c3)
{
    bool flash_result = true;
    unsigned short block_num = (unsigned short)c1 << 8;      
    block_num |= c2;
    if (block_num == 0) 
    {
          pauseDMA(false);        
    }
      
    unsigned short payload_len = (unsigned short)c3 << 8;
    while (Serial.available() < 1)
          ;//Wait for extra byte in header
    uint8_t c4 = Serial.read();
    payload_len |= c4;
        
    unsigned short calc_checksum = 0;
    calc_checksum += inCmd;
    calc_checksum += c1;
    calc_checksum += c2;
    calc_checksum += c3;
    calc_checksum += c4;        

    unsigned long addr = ((unsigned long)block_num * (SERIAL_RX_BUFFER_SIZE - 7));
    unsigned long wr_addr = 0;
    while (addr > IO_BUF_SZ)
    {
        wr_addr += IO_BUF_SZ;
        addr -= IO_BUF_SZ;
    }
        
    for (unsigned short i = 0; i < payload_len; i++)
    {
        if (addr == IO_BUF_SZ) 
        {
             flash_result = writeCODE(wr_addr, IO_BUF_SZ, ioBuf, false, false);
             wr_addr += IO_BUF_SZ;
             addr -= IO_BUF_SZ;
        }

        while (Serial.available() < 1)
            ;
            
        ioBuf[addr] = Serial.read();
        calc_checksum += ioBuf[addr];
          
        addr++;            
    }
    
    //Flash remainder if it is the last block
    unsigned long total_len = wr_addr + addr + payload_len;
    if (total_len >= flashSizeKb * 1024L) 
        flash_result = writeCODE(wr_addr, IO_BUF_SZ, ioBuf, false, false);

    while (Serial.available() < 2) 
        ;
          
    unsigned short rec_checksum = (unsigned short)Serial.read() << 8;
    rec_checksum |= Serial.read();
        
    if (rec_checksum != calc_checksum) 
        sendError(ERR_CHECKSUM);
    else if (!flash_result) 
        sendError(ERR_FLASHING);
    else
    {
        unsigned short checksum = CMD_WRITE;
        generateHeader(CMD_WRITE, BIN_PACKET_HDR + BIN_PACKET_SMPL_FTR);
        generateFooter(checksum);  
    }
}



