/*
  Frontend for Arduino CC-Debugger

  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */


using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace CCLibFrontend
{

    enum FLD_OFFSETS
    {
        HW_VER_OFFSET = 25,
        BT_ADDR = 22,
        LICENSE = 57
    }

    enum FLD_SIZES
    {
        HW_VER_OFFSET = 1,
        BT_ADDR = 6,
        LICENSE = 32
    }
    

    enum MODE
    {
        NONE,
        CONSOLE_INFO,
        CHIP_INFO,
        READ,
        WRITE,
        BLE_STORE,
        ERASE,
        IEEE_ADDR,
        HW_VER,
        LICENSE,
        BT_ADDR
    };

    enum COMMANDS
    {
        CONSOLE_CMD_INFO = 0x61,
        CMD_READ = 0x2,
        CMD_WRITE = 0x3,
        CMD_BLE_STORE = 0x4,
        CMD_ERASE = 0x5,
        CMD_IEEE_ADDR = 0x6,
        CMD_LICENSE = 0x7,
        CMD_BT_ADDR = 0x8,
        CMD_HW_VER = 0x9,
        CMD_CHIP_INFO = 0xA,
        CMD_IN_BUF_SZ = 0xB,

        CMD_RESPONSE_OK = 0x81,
        CMD_RESPONSE_ERR = 0x82,
        CMD_RESPONSE_RDY = 0x83
    };

    enum RESPONSES
    {
        ANS_OK = 0x1,
        ANS_ERROR = 0x2,
        ANS_READY = 0x3
    };

    public class CCPacket
    {
        private int cmd;
        public int Cmd
        {
            get
            {
                return cmd;
            }
        }

        private UInt16 total_len;
        public UInt16 TotalLen
        {
            get
            {
                return total_len;
            }
        }

        private byte[] payload;
        public byte[] Payload
        {
            get
            {
                return payload;
            }
        }

        public UInt16 ChecksumCalc
        {
            get
            {
                UInt16 checksum = 0;
                checksum += (byte)cmd;

                foreach (byte bt in payload)
                {
                    checksum += bt;
                }

                return checksum;
            }
        }


        private UInt16 checksum_received;
        public UInt16 ChecksumReceived
        {
            get
            {
                return checksum_received;
            }
        }

        public bool checksum_valid
        {
            get
            {
                return ChecksumCalc == checksum_received;
            }
        }

        public bool Read(System.IO.Ports.SerialPort prt)
        {
            if (prt == null) return false;


            total_len = (UInt16)((prt.ReadByte() << 8 & 0xFF00) | (prt.ReadByte() & 0xFF));
            cmd = prt.ReadByte();

            payload = new byte[TotalLen - 3];
            for (UInt16 i = 0; i < payload.Length; i++)
            {
                payload[i] = (byte)prt.ReadByte();
            }

            checksum_received = (UInt16)((prt.ReadByte() << 8 & 0xFF00) | (prt.ReadByte() & 0xFF));

            return checksum_valid;
        }
    }
}
