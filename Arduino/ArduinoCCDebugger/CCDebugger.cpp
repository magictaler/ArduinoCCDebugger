/**
 * Modified Jan 2016 by Dmitry Pakhomenko.
 * Improved performance by replacing standard DigitalWrite, DigitalRead and PinMode
 * dmitryp@magictale.com
 * http://magictale.com
 *
 * CC-Debugger Protocol Library for Arduino
 * Copyright (c) 2014 Ioannis Charalampidis
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *  
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "CCDebugger.h"

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////                CONSTRUCTOR & CONFIGURATORS                  ////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/**
 * Initialize CC Debugger class
 */
CCDebugger::CCDebugger( int pinRST, int pinDC, int pinDD ) 
{

  // Keep references
  this->pinRST  = pinRST;
  this->pinDC   = pinDC;
  this->pinDD   = pinDD;
  
  // Prepare CC Pins
  pinMode(pinDC,        OUTPUT);
  pinMode(pinDD,        OUTPUT);
  pinMode(pinRST,       OUTPUT);
  digitalWrite(pinDC,   LOW);
  digitalWrite(pinDD,   LOW);
  digitalWrite(pinRST,  LOW);

  // Prepare default direction
  setDDDirection(INPUT);

  // We are active by default
  active = true;
  
  this->portRST_Out = portOutputRegister(digitalPinToPort(pinRST));
  this->portDC_Out = portOutputRegister(digitalPinToPort(pinDC));
  this->portDD_Out = portOutputRegister(digitalPinToPort(pinDD));
  this->portDD_In = portInputRegister(digitalPinToPort(pinDD));
  this->portDD_Mode = portModeRegister(digitalPinToPort(pinDD));
  
  this->bitRST = digitalPinToBitMask(pinRST);
  this->bitDC = digitalPinToBitMask(pinDC);  
  this->bitDD = digitalPinToBitMask(pinDD);  
};

inline void CCDebugger::pinModeFast(volatile uint8_t* portOut, volatile uint8_t* portModeReg, uint8_t bit, uint8_t mode)
{
  if (mode == INPUT) { 
    uint8_t oldSREG = SREG;
    cli();
    *portModeReg &= ~bit;
    *portOut &= ~bit;
    SREG = oldSREG;
  } else if (mode == INPUT_PULLUP) {
    uint8_t oldSREG = SREG;
    cli();
    *portModeReg &= ~bit;
    *portOut |= bit;
    SREG = oldSREG;
  } else {
    uint8_t oldSREG = SREG;
    cli();
    *portModeReg |= bit;
    SREG = oldSREG;
  }
}


inline void CCDebugger::digitalWriteFast( volatile uint8_t* portOut, uint8_t bit, uint8_t val)
{
  uint8_t oldSREG = SREG;
  cli();

  if (val == LOW) {
    *portOut &= ~bit;
  } else {
    *portOut |= bit;
  }

  SREG = oldSREG;
}

inline int CCDebugger::digitalReadFast( volatile uint8_t* portIn, uint8_t bit)
{
  if (*portIn & bit) return HIGH;
  return LOW;  
}


/**
 * Activate/Deactivate debugger
 */
void CCDebugger::setActive( boolean on ) 
{

  // Reset error flag
  errorFlag = 0;

  // Continue only if active
  if (on == this->active) return;
  this->active = on;

  if (on) {

    // Prepare CC Pins
    pinMode(pinDC,        OUTPUT);
    pinMode(pinDD,        OUTPUT);
    pinMode(pinRST,       OUTPUT);
    digitalWrite(pinDC,   LOW);
    digitalWrite(pinDD,   LOW);
    digitalWrite(pinRST,  LOW);

    // Default direction is INPUT
    setDDDirection(INPUT);
    
  } else {

    // Before deactivating, exit debug mode
    if (inDebugMode)
      this->exit();

    // Put everything in inactive mode
    pinMode(pinDC,        INPUT);
    pinMode(pinDD,        INPUT);
    pinMode(pinRST,       INPUT);
    digitalWrite(pinDC,   LOW);
    digitalWrite(pinDD,   LOW);
    digitalWrite(pinRST,  LOW);

  }
}

/**
 * Return the error flag
 */
byte CCDebugger::error()
{
  return errorFlag;
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////                     LOW LEVEL FUNCTIONS                     ////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////

/**
 * Delay a particular number of cycles
 */
inline void cc_delay( unsigned char d )
{
  volatile unsigned char i = d;
  while( i-- );
}

/**
 * Enter debug mode
 */
byte CCDebugger::enter() 
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  // =============

  // Reset error flag
  errorFlag = 0;

  // Enter debug mode
  digitalWriteFast( portRST_Out, bitRST, LOW);
  cc_delay(200);
  digitalWriteFast( portDC_Out, bitDC, HIGH);
  cc_delay(3);
  digitalWriteFast( portDC_Out, bitDC, LOW);  
  cc_delay(3);
  digitalWriteFast( portDC_Out, bitDC, HIGH);  
  cc_delay(3);
  digitalWriteFast( portDC_Out, bitDC, LOW);  
  cc_delay(200);
  digitalWriteFast( portRST_Out, bitRST, HIGH);  
  cc_delay(200);

  // We are now in debug mode
  inDebugMode = 1;

  // =============

  // Success
  return 0;

};


inline byte CCDebugger::writeFast( byte data ) 
{
  byte cnt;
  
  // Sent bytes
  for (cnt = 8; cnt; cnt--) {

    // First put data bit on bus
    if (data & 0x80)
      digitalWriteFast( portDD_Out, bitDD, HIGH);  
    else
      digitalWriteFast( portDD_Out, bitDD, LOW);  

    // Place clock on high (other end reads data)
    digitalWriteFast( portDC_Out, bitDC, HIGH);      

    // Shift & Delay
    data <<= 1;

    // Place clock down
    digitalWriteFast( portDC_Out, bitDC, LOW);
  }

  return 0;
}


/**
 * Wait until input is ready for reading
 */
inline byte CCDebugger::switchRead()
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte cnt;
  byte didWait = 0;

  // Switch to input
  setDDDirection(INPUT);

  // Wait at least 83 ns before checking state t(dir_change)
  cc_delay(2);

  // Wait for DD to go LOW (Chip is READY)
  //while (digitalRead(pinDD_I) == HIGH) {  
  while (digitalReadFast( portDD_In, bitDD) == HIGH) {

    // Do 8 clock cycles
    for (cnt = 8; cnt; cnt--) {
      digitalWriteFast( portDC_Out, bitDC, HIGH);      
      digitalWriteFast( portDC_Out, bitDC, LOW);      
    }

    // Let next function know that we did wait
    didWait = 1;
  }

  // Wait t(sample_wait) 
  if (didWait) cc_delay(2);

  return 0;
}

/**
 * Switch to output
 */
inline byte CCDebugger::switchWrite()
{
  setDDDirection(OUTPUT);
  return 0;
}

inline byte CCDebugger::readFast()
{
  byte cnt;
  byte data = 0;

  // Send 8 clock pulses if we are HIGH
  for (cnt = 8; cnt; cnt--) {
    digitalWriteFast( portDC_Out, bitDC, HIGH);      

    // Shift and read
    data <<= 1;

//pinModeFast(portDD_Out, portDD_Mode, bitDD, INPUT);    
    if (digitalReadFast(portDD_In, bitDD) == HIGH)
      data |= 0x01;

//pinModeFast(portDD_Out, portDD_Mode, bitDD, OUTPUT);      

    digitalWriteFast( portDC_Out, bitDC, LOW);      
  }

  return data;
}


/**
 * Switch reset pin
 */
inline void CCDebugger::setDDDirection( byte direction )
{

  // Switch direction if changed
  if (direction == ddIsOutput) return;
  ddIsOutput = direction;
  
  // Handle new direction
  //digitalWriteFast(portDD_Out, bitDD, LOW);        
  if (ddIsOutput) {
    //pinMode(pinDD_O, OUTPUT);   // Enable output
    pinModeFast(portDD_Out, portDD_Mode, bitDD, OUTPUT);
  } else {
    //pinMode(pinDD_O, INPUT);    // Disable output
    pinModeFast(portDD_Out, portDD_Mode, bitDD, INPUT);
  }
  //digitalWriteFast(portDD_Out, bitDD, LOW);            
}

/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////
////                    HIGH LEVEL FUNCTIONS                     ////
/////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////


/**
 * Exit from debug mode
 */
byte CCDebugger::exit() 
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x48 ); // RESUME
  switchRead();
  bAns = readFast(); // debug status
  switchWrite(); 

  inDebugMode = 0;

  return 0;
}
/**
 * Get debug configuration
 */
byte CCDebugger::getConfig() {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x20 ); // RD_CONFIG
  switchRead();
  bAns = readFast(); // Config
  switchWrite(); 

  return bAns;
}

/**
 * Set debug configuration
 */
byte CCDebugger::setConfig( byte config ) {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x18 ); // WR_CONFIG
  writeFast( config );
  switchRead();
  bAns = readFast(); // Config
  switchWrite();

  return bAns;
}

/**
 * Invoke a debug instruction with 1 opcode
 */
byte CCDebugger::exec( byte oc0 )
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  // Make sure DD is on output
  setDDDirection(OUTPUT);

  writeFast( 0x51 ); // DEBUG_INSTR + 1b
  writeFast( oc0 );
  switchRead();
  
  // Switch to input
  setDDDirection(INPUT);
  bAns = readFast(); // Accumulator
  
  switchWrite(); 

  return bAns;
}

/**
 * Invoke a debug instruction with 2 opcodes
 */
byte CCDebugger::exec( byte oc0, byte oc1 )
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  // Make sure DD is on output
  setDDDirection(OUTPUT);

  writeFast( 0x52 ); // DEBUG_INSTR + 2b
  writeFast( oc0 );
  writeFast( oc1 );
  switchRead();

  // Switch to input
  setDDDirection(INPUT);
  bAns = readFast(); // Accumulator
  
  switchWrite(); 

  return bAns;
}

/**
 * Invoke a debug instruction with 3 opcodes
 */
byte CCDebugger::exec( byte oc0, byte oc1, byte oc2 )
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  // Make sure DD is on output
  setDDDirection(OUTPUT);

  writeFast( 0x53 ); // DEBUG_INSTR + 3b
  writeFast( oc0 );
  writeFast( oc1 );
  writeFast( oc2 );
  
  switchRead();

  // Switch to input
  setDDDirection(INPUT);
  bAns = readFast(); // Accumulator
  
  switchWrite(); 

  return bAns;
}

/**
 * Invoke a debug instruction with 1 opcode + 16-bit immediate
 */
byte CCDebugger::execi( byte oc0, unsigned short c0 )
{
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;
  
  // Make sure DD is on output
  setDDDirection(OUTPUT);
  
  writeFast( 0x53 ); // DEBUG_INSTR + 3b
  writeFast( oc0 );
  writeFast( (c0 >> 8) & 0xFF );
  writeFast(  c0 & 0xFF );
  
  switchRead();
  
  // Switch to input
  setDDDirection(INPUT);
  bAns = readFast(); // Accumulator
  
  switchWrite(); 

  return bAns;
}

/**
 * Return chip ID
 */
unsigned short CCDebugger::getChipID() {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  unsigned short bAns;
  byte bRes;

  writeFast( 0x68 ); // GET_CHIP_ID
  switchRead();
  bRes = readFast(); // High order
  bAns = bRes << 8;
  bRes = readFast(); // Low order
  bAns |= bRes;
  switchWrite(); 

  return bAns;
}

/**
 * Return PC
 */
unsigned short CCDebugger::getPC() {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  unsigned short bAns;
  byte bRes;

  writeFast( 0x28 ); // GET_PC
  switchRead();
  bRes = readFast(); // High order
  bAns = bRes << 8;
  bRes = readFast(); // Low order
  bAns |= bRes;
  switchWrite(); 

  return bAns;
}

/**
 * Return debug status
 */
byte CCDebugger::getStatus() {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x30 ); // READ_STATUS
  switchRead();
  bAns = readFast(); // debug status
  switchWrite(); 

  return bAns;
}

/**
 * Step instruction
 */
byte CCDebugger::step() {
  if (!active) {
    errorFlag = 1;
    return 0;
  }
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x58 ); // STEP_INSTR
  switchRead();
  bAns = readFast(); // Accumulator
  switchWrite(); 

  return bAns;
}

/**
 * Mass-erase all chip configuration & Lock Bits
 */
byte CCDebugger::chipErase()
{
  if (!active) {
    errorFlag = 1;
    return 0;
  };
  if (!inDebugMode) {
    errorFlag = 2;
    return 0;
  }

  byte bAns;

  writeFast( 0x10 ); // CHIP_ERASE
  switchRead();
  bAns = readFast(); // Debug status
  switchWrite(); 

  return bAns;
};
