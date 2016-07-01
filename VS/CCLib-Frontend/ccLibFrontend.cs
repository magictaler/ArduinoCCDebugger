
/*
  Frontend for Arduino CC-Debugger

  Copyright (c) 2016 Dmitry Pakhomenko.
  dmitryp@magictale.com
  http://magictale.com
 
  This example code is in the public domain.
 */

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using System.IO;
using System.Threading;
using System.Reflection;
using LibGIS.Net;

namespace CCLibFrontend
{
    public partial class ccLibFrontend : Form
    {
        public byte CHIP_CC2540_SIGNATURE = 0x8D;
        public byte CHIP_CC2541_SIGNATURE = 0x41;

        private StreamWriter _LogFile;
        private BinaryWriter _BinFile;
        private StreamReader _HexFileReader;
        private int _blockNum;
        private int _totalBlocks;
        private MODE _mode;
        private byte[] _image;
        private long _flashSize;
        private ushort _arduino_ser_buf_size;
        private DateTime _startTime;

        private bool _hwVersionBlank;
        private bool _btAddrBlank;
        private bool _licenceBlank;

        public ccLibFrontend()
        {
            InitializeComponent();
        }

        private void Form1_Load(object sender, EventArgs e)
        {
            
            string revision = string.Format("{0}.{1}.{2}",
                Assembly.GetExecutingAssembly().GetName().Version.Major,
                Assembly.GetExecutingAssembly().GetName().Version.Minor,
                Assembly.GetExecutingAssembly().GetName().Version.Build
                );


            this.Text = string.Format("{0} Rev.{1}", this.Text, revision);

            serialPort1.PortName = global::CCLibFrontend.Properties.Settings.Default.SerialPort;
            serialPort1.BaudRate = global::CCLibFrontend.Properties.Settings.Default.Baudrate;
            chipIDTBox.Text = "";
            flashSizeTBox.Text = "";
            sramSizeTBox.Text = "";
            usbTBox.Text = "";
            ieeeAddrTBox.Text = "";
            btAddrTBox.Text = "";
            hwVersionTBox.Text = "";
            licenseTBox.Text = "";
            statusLabel.Text = "";

            try
            {
                serialPort1.Open();
                _LogFile = new StreamWriter(Path.Combine(
                    global::CCLibFrontend.Properties.Settings.Default.LogPath,
                    "CCLib-FrontendLog.txt"), true);

                btnInfo.Enabled = true;
                btnRead.Enabled = true;
                btnWrite.Enabled = true;

            }
            catch (Exception ex)
            {
                MessageBox.Show(ex.ToString());
                Application.Exit();
            }
        }


        delegate void EnableInfoBtnCallback();
        public void EnableInfoBtn()
        {
            if (this.btnInfo.InvokeRequired)
            {
                EnableInfoBtnCallback cb = new EnableInfoBtnCallback(EnableInfoBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnInfo.Enabled = true;
            }
        }

        delegate void EnableReadBtnCallback();
        public void EnableReadBtn()
        {
            if (this.btnRead.InvokeRequired)
            {
                EnableReadBtnCallback cb = new EnableReadBtnCallback(EnableReadBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnRead.Enabled = true;
            }
        }

        delegate void EnableWriteBtnCallback();
        public void EnableWriteBtn()
        {
            if (this.btnWrite.InvokeRequired)
            {
                EnableWriteBtnCallback cb = new EnableWriteBtnCallback(EnableWriteBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnWrite.Enabled = true;
            }
        }

        public void EnableButtons()
        {
            EnableInfoBtn();
            EnableReadBtn();
            EnableWriteBtn();
        }

        delegate void DisableWriteBtnCallback();
        public void DisableWriteBtn()
        {
            if (this.btnWrite.InvokeRequired)
            {
                DisableWriteBtnCallback cb = new DisableWriteBtnCallback(DisableWriteBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnWrite.Enabled = false;
            }
        }

        delegate void DisableReadBtnCallback();
        public void DisableReadBtn()
        {
            if (this.btnRead.InvokeRequired)
            {
                DisableReadBtnCallback cb = new DisableReadBtnCallback(DisableReadBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnRead.Enabled = false;
            }
        }

        delegate void DisableInfoBtnCallback();
        public void DisableInfoBtn()
        {
            if (this.btnInfo.InvokeRequired)
            {
                DisableInfoBtnCallback cb = new DisableInfoBtnCallback(DisableInfoBtn);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                btnInfo.Enabled = false;
            }
        }

        public void DisableButtons()
        {
            DisableInfoBtn();
            DisableReadBtn();
            DisableWriteBtn();
        }

        delegate void SetStatusLabelCallback(string msg);
        public void SetStatusLabel(string msg)
        {
            if (this.statusLabel.InvokeRequired)
            {
                SetStatusLabelCallback cb = new SetStatusLabelCallback(SetStatusLabel);
                this.Invoke(cb, new Object[] { msg });
            }
            else
            {
                statusLabel.Text = msg;
            }
        }



        delegate void SetHwVersionTBoxCallback(byte hwver);
        public void SetHwVersionTBox(byte hwver)
        {
            if (this.hwVersionTBox.InvokeRequired)
            {
                SetHwVersionTBoxCallback cb = new SetHwVersionTBoxCallback(SetHwVersionTBox);
                this.Invoke(cb, new Object[] { hwver });
            }
            else
            {
                hwVersionTBox.Text = String.Format("0x{0}", hwver.ToString("X2"));
            }
        }


        delegate void SetLicenseTBoxCallback(byte[] license);
        public void SetLicenseTBox(byte[] license)
        {
            if (this.licenseTBox.InvokeRequired)
            {
                SetLicenseTBoxCallback cb = new SetLicenseTBoxCallback(SetLicenseTBox);
                this.Invoke(cb, new Object[] { license });
            }
            else
            {
                string txtLicense = "";

                if (cbHideValues.Checked)
                {
                    for (int i = 0; i < 2; i++)
                    {
                        txtLicense = string.Format("{0}{1}", txtLicense, license[i].ToString("X2"));
                    }

                    for (int i = 2; i < license.Length - 2; i++)
                    {
                        txtLicense = string.Format("{0}{1}", txtLicense, "XX");
                    }
                }
                else
                {
                    for (int i = 0; i < license.Length; i++)
                    {
                        txtLicense = string.Format("{0}{1}", txtLicense, license[i].ToString("X2"));
                    }
                }
                licenseTBox.Text = txtLicense;
            }
        }


        delegate void SetBtAddrTBoxCallback(byte[] btAddr);
        public void SetBtAddrTBox(byte[] btAddr)
        {
            if (this.btAddrTBox.InvokeRequired)
            {
                SetBtAddrTBoxCallback cb = new SetBtAddrTBoxCallback(SetBtAddrTBox);
                this.Invoke(cb, new Object[] { btAddr });
            }
            else
            {
                string txtAddr = "";
                if (cbHideValues.Checked)
                {
                    for (int i = 0; i < 2; i++)
                    {
                        txtAddr = string.Format("{0}", btAddr[btAddr.Length - 1 - i].ToString("X2"));
                    }
                    for (int i = 1; i < btAddr.Length - 1; i++)
                    {
                        txtAddr = string.Format("{0}:{1}", txtAddr, "XX");
                    }
                }
                else
                {
                    for (int i = 0; i < btAddr.Length; i++)
                    {
                        if (i == 0)
                            txtAddr = string.Format("{0}", btAddr[btAddr.Length - 1 - i].ToString("X2"));
                        else
                            txtAddr = string.Format("{0}:{1}", txtAddr, btAddr[btAddr.Length - 1 - i].ToString("X2"));
                    }
                }
                btAddrTBox.Text = txtAddr;
            }
        }


        delegate void SetIeeeAddrTBoxCallback(byte[] ieeeAddr);
        public void SetIeeeAddrTBox(byte[] ieeeAddr)
        {
            if (this.ieeeAddrTBox.InvokeRequired)
            {
                SetIeeeAddrTBoxCallback cb = new SetIeeeAddrTBoxCallback(SetIeeeAddrTBox);
                this.Invoke(cb, new Object[] { ieeeAddr });
            }
            else
            {
                string txtAddr = "";
                for (int i = 0; i < ieeeAddr.Length; i++)
                {
                    if (i== 0)
                        txtAddr = string.Format("{0}", ieeeAddr[ieeeAddr.Length - 1 - i].ToString("X2"));
                    else
                        txtAddr = string.Format("{0}:{1}", txtAddr, ieeeAddr[ieeeAddr.Length - 1 - i].ToString("X2"));
                }
                    ieeeAddrTBox.Text = txtAddr;
            }
        }

        delegate void SetRuntimeCallback();
        public void SetRuntime()
        {
            if (this.runtimeLabel.InvokeRequired)
            {
                SetRuntimeCallback cb = new SetRuntimeCallback(SetRuntime);
                this.Invoke(cb, new Object[] { });
            }
            else
            {
                runtimeLabel.Text = String.Format("{0} secs", (long)(DateTime.Now - _startTime).TotalSeconds);
            }
        }


        delegate void SetUsbTBoxCallback(bool usb);
        public void SetUsbTBox(bool usb)
        {
            if (this.usbTBox.InvokeRequired)
            {
                SetUsbTBoxCallback cb = new SetUsbTBoxCallback(SetUsbTBox);
                this.Invoke(cb, new Object[] { usb });
            }
            else
            {
                usbTBox.Text = usb?"Yes":"No";
            }
        }


        delegate void SetSramSizeTBoxCallback(byte sramSize);
        public void SetSramSizeTBox(byte sramSize)
        {
            if (this.sramSizeTBox.InvokeRequired)
            {
                SetSramSizeTBoxCallback cb = new SetSramSizeTBoxCallback(SetSramSizeTBox);
                this.Invoke(cb, new Object[] { sramSize });
            }
            else
            {
                sramSizeTBox.Text = String.Format("{0}", sramSize.ToString());
            }
        }


        delegate void SetFlashSizeTBoxCallback(int flashSize);
        public void SetFlashSizeTBox(int flashSize)
        {
            if (this.flashSizeTBox.InvokeRequired)
            {
                SetFlashSizeTBoxCallback cb = new SetFlashSizeTBoxCallback(SetFlashSizeTBox);
                this.Invoke(cb, new Object[] { flashSize });
            }
            else
            {
                flashSizeTBox.Text = String.Format("{0}", flashSize.ToString());
            }
        }


        delegate void SetChipIDTBoxCallback(UInt16 chipID);
        public void SetChipIDTBox(UInt16 chipID)
        {
            if (this.chipIDTBox.InvokeRequired)
            {
                SetChipIDTBoxCallback cb = new SetChipIDTBoxCallback(SetChipIDTBox);
                this.Invoke(cb, new Object[] {chipID});
            }
            else
            {
                chipIDTBox.Text = String.Format("0x{0}", chipID.ToString("X4"));
            }
        }

        delegate void UpdateProgressCallback(int maxVal, int currVal);
        public void UpdateProgress(int maxVal, int currVal)
        {
            if (this.progressBar1.InvokeRequired)
            {
                UpdateProgressCallback cb = new UpdateProgressCallback(UpdateProgress);
                this.Invoke(cb, new object[] { maxVal, currVal });

            }
            else
            {
                this.progressBar1.Maximum = maxVal;
                this.progressBar1.Value = currVal;
                this.progressBar1.Update();
            }

        }

        private void serialPort1_DataReceived(object sender, System.IO.Ports.SerialDataReceivedEventArgs e)
        {
            if (_mode == MODE.CONSOLE_INFO)
            {
                _LogFile.Write(serialPort1.ReadExisting());
                _LogFile.Flush();
            }
            else
            {
                CCPacket packet = new CCPacket();
                if (packet.Read(serialPort1))
                {
                    if (packet.Cmd == (int)COMMANDS.CMD_RESPONSE_ERR)
                    {
                        _mode = MODE.NONE;
                        SetStatusLabel("Error");
                        MessageBox.Show("Error");
                        EnableButtons();
                    }
                    else
                    {
                        switch (_mode)
                        {
                            case MODE.CHIP_INFO:
                                {
                                    SetChipIDTBox((ushort)((packet.Payload[0] << 8) | packet.Payload[1]));
                                    SetFlashSizeTBox((packet.Payload[2] << 8) | packet.Payload[3]);
                                    SetSramSizeTBox(packet.Payload[4]);
                                    SetUsbTBox(packet.Payload[5] != 0);

                                    reqLicense(MODE.LICENSE);
                                }
                                break;

                            case MODE.LICENSE:
                                {
                                    SetLicenseTBox(packet.Payload);

                                    reqIeeeAddr(MODE.IEEE_ADDR);
                                }
                                break;

                            case MODE.IEEE_ADDR:
                                {
                                    SetIeeeAddrTBox(packet.Payload);

                                    reqBtAddr(MODE.BT_ADDR);
                                }
                                break;

                            case MODE.BT_ADDR:
                                {
                                    SetBtAddrTBox(packet.Payload);

                                    reqHwVer(MODE.HW_VER);
                                }
                                break;

                            case MODE.HW_VER:
                                {
                                    SetHwVersionTBox(packet.Payload[0]);
                                    EnableButtons();
                                }
                                break;


                            case MODE.READ:
                                {
                                    if (packet.Cmd == (int)COMMANDS.CMD_CHIP_INFO)
                                    {
                                        SetChipIDTBox((ushort)((packet.Payload[0] << 8) | packet.Payload[1]));
                                        SetFlashSizeTBox((packet.Payload[2] << 8) | packet.Payload[3]);
                                        SetSramSizeTBox(packet.Payload[4]);
                                        SetUsbTBox(packet.Payload[5] != 0);

                                        _flashSize = 1024L * (packet.Payload[2] << 8) | packet.Payload[3];

                                        reqHwVer(MODE.READ);
                                    }
                                    else if (packet.Cmd == (int)COMMANDS.CMD_HW_VER)
                                    {
                                        SetHwVersionTBox(packet.Payload[0]);
                                        reqIeeeAddr(MODE.READ);
                                    }
                                    else if (packet.Cmd == (int)COMMANDS.CMD_IEEE_ADDR)
                                    {
                                        SetIeeeAddrTBox(packet.Payload);
                                        reqBtAddr(MODE.READ);
                                    }
                                    else if (packet.Cmd == (int)COMMANDS.CMD_BT_ADDR)
                                    {
                                        SetBtAddrTBox(packet.Payload);
                                        reqLicense(MODE.READ);
                                    }
                                    else if (packet.Cmd == (int)COMMANDS.CMD_LICENSE)
                                    {
                                        SetLicenseTBox(packet.Payload);
                                        byte[] writeBuf = new byte[] { (byte)COMMANDS.CMD_READ, (byte)_blockNum, 0, 0 };
                                        serialPort1.Write(writeBuf, 0, writeBuf.Length);
                                    }
                                    else if (packet.Cmd == (int)COMMANDS.CMD_READ)
                                    {
                                        if (_blockNum == 0)
                                        {
                                            string readFilePath = Path.Combine(
                                                global::CCLibFrontend.Properties.Settings.Default.LogPath,
                                                "read.bin");
                                            try
                                            {
                                                if (File.Exists(readFilePath)) File.Delete(readFilePath);
                                            }
                                            catch
                                            {
                                                ;
                                            }

                                            _BinFile = new BinaryWriter(new FileStream(readFilePath, FileMode.CreateNew));

                                            UpdateProgress(100, 0);
                                        }

                                        _BinFile.Write(packet.Payload, 0, packet.Payload.Length - 3);
                                        _BinFile.Flush();

                                        _blockNum = packet.Payload[packet.Payload.Length - 3]; //third last byte in payload
                                        _blockNum++;
                                        int totalBlocks = (packet.Payload[packet.Payload.Length - 2] << 8) | (packet.Payload[packet.Payload.Length - 1]); //last and second last byte in payload

                                        UpdateProgress(totalBlocks, _blockNum);


                                        if (totalBlocks > _blockNum)
                                        {
                                            byte[] writeBuf = new byte[] { (byte)COMMANDS.CMD_READ, (byte)_blockNum, 0, 0 };
                                            serialPort1.Write(writeBuf, 0, writeBuf.Length);
                                        }
                                        else
                                        {
                                            _BinFile.Close();
                                            _mode = MODE.NONE;
                                            SetRuntime();
                                            MessageBox.Show("Reading Done");
                                            SetStatusLabel("Reading Done");
                                            EnableButtons();
                                        }
                                    }
                                }
                                break;

                            case MODE.WRITE:

                                if (packet.Cmd == (int)COMMANDS.CMD_CHIP_INFO)
                                {
                                    SetChipIDTBox((ushort)((packet.Payload[0] << 8) | packet.Payload[1]));
                                    SetFlashSizeTBox((packet.Payload[2] << 8) | packet.Payload[3]);
                                    SetSramSizeTBox(packet.Payload[4]);
                                    SetUsbTBox(packet.Payload[5] != 0);

                                    _flashSize = 1024L * (packet.Payload[2] << 8) | packet.Payload[3];

                                    if (packet.Payload[0] != CHIP_CC2540_SIGNATURE && packet.Payload[0] != CHIP_CC2541_SIGNATURE)
                                    {
                                        SetStatusLabel("Incorrect chip.");
                                        MessageBox.Show("Incorrect chip ID or missing target platform", "Incorrect chip", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                        EnableButtons();

                                    }else if (_flashSize < _image.Length)
                                    {
                                        SetStatusLabel("Unable to flash.");
                                        MessageBox.Show("Image is too big to fit in chip's memory", "Not enough flash space", MessageBoxButtons.OK, MessageBoxIcon.Error);
                                        EnableButtons();
                                    }
                                    else
                                    {
                                        SetStatusLabel("Reading hardware version...");
                                        reqHwVer(MODE.WRITE);
                                    }
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_HW_VER)
                                {
                                    if (isBlank(packet.Payload))
                                    {
                                        MessageBox.Show("HW version is blank, skipping copying it from the chip");
                                        _hwVersionBlank = true;
                                    }
                                    else
                                    {
                                        SetHwVersionTBox(packet.Payload[0]);
                                        _image[_flashSize - (int)FLD_OFFSETS.HW_VER_OFFSET] = packet.Payload[0];
                                        _hwVersionBlank = false;
                                    }

                                    SetStatusLabel("Reading IEEE address...");
                                    reqIeeeAddr(MODE.WRITE);
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_IEEE_ADDR)
                                {
                                    SetIeeeAddrTBox(packet.Payload);

                                    SetStatusLabel("Reading BT address...");
                                    reqBtAddr(MODE.WRITE);
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_BT_ADDR)
                                {
                                    if (isBlank(packet.Payload))
                                    {
                                        MessageBox.Show("BT address is blank, skipping copying it from the chip");
                                        _btAddrBlank = true;
                                    }
                                    else
                                    {
                                        SetBtAddrTBox(packet.Payload);

                                        for (int i = 0; i < packet.Payload.Length; i++)
                                        {
                                            _image[_flashSize - (int)FLD_OFFSETS.BT_ADDR + i] = packet.Payload[i];
                                        }
                                        _btAddrBlank = false;
                                    }

                                    SetStatusLabel("Reading license...");
                                    reqLicense(MODE.WRITE);
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_LICENSE)
                                {
                                    if (isBlank(packet.Payload))
                                    {
                                        _licenceBlank = true;
                                        MessageBox.Show("License is blank, skipping copying it from the chip");
                                    }
                                    else
                                    {

                                        SetLicenseTBox(packet.Payload);

                                        for (int i = 0; i < packet.Payload.Length; i++)
                                        {
                                            _image[_flashSize - (int)FLD_OFFSETS.LICENSE + i] = packet.Payload[i];
                                        }

                                        _licenceBlank = false;
                                    }

                                    bool noBLEStore = false;
                                    if (_licenceBlank && _btAddrBlank && _hwVersionBlank)
                                    {
                                        if (MessageBox.Show("Is it HM-10 module?", "Module autodetection", MessageBoxButtons.YesNo, MessageBoxIcon.Question) != System.Windows.Forms.DialogResult.Yes)
                                            break;
                                        else
                                            noBLEStore = true;
                                    }
                                    else
                                    {

                                        if (!restoreFromBackup()) break;
                                    }

                                    if (MessageBox.Show("This is going to ERASE and REPROGRAM the chip. Are you sure?",
                                        "Ready for programming", MessageBoxButtons.YesNo, MessageBoxIcon.Question) ==
                                        System.Windows.Forms.DialogResult.Yes)
                                    {
                                        if (noBLEStore)
                                        {
                                            //possibly blank chip, skip back up operation
                                            writeMergedBin();
                                            reqErase(MODE.WRITE);
                                        }
                                        else
                                        {
                                            SetStatusLabel("Backing up BLE store...");

                                            _blockNum = 0;
                                            byte[] writeBuf = new byte[] { (byte)COMMANDS.CMD_BLE_STORE, (byte)_blockNum, 0, 0 };
                                            serialPort1.Write(writeBuf, 0, writeBuf.Length);
                                            UpdateProgress(100, 0);
                                        }
                                    }

                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_BLE_STORE)
                                {
                                    if (packet.Payload.Length == 0)
                                    {
                                        //possibly blank chip, skip back up operation
                                        writeMergedBin();
                                        reqErase(MODE.WRITE);
                                    }
                                    else
                                    {

                                        _blockNum = packet.Payload[packet.Payload.Length - 3]; //third last byte in payload
                                        long startAddr = 0x18000 + _blockNum * (packet.Payload.Length - 3);

                                        for (int i = 0; i < packet.Payload.Length - 2; i++)
                                        {
                                            _image[startAddr + i] = packet.Payload[i];
                                        }

                                        _blockNum++;

                                        //int totalBlocks = packet.Payload[packet.Payload.Length - 1]; //last byte in payload
                                        int totalBlocks = (packet.Payload[packet.Payload.Length - 2] << 8) | (packet.Payload[packet.Payload.Length - 1]); //last and second last byte in payload

                                        UpdateProgress(totalBlocks, _blockNum);

                                        if (totalBlocks > _blockNum)
                                        {
                                            byte[] writeBuf = new byte[] { (byte)COMMANDS.CMD_BLE_STORE, (byte)_blockNum, 0, 0 };
                                            serialPort1.Write(writeBuf, 0, writeBuf.Length);
                                        }
                                        else
                                        {
                                            writeMergedBin();
                                            reqErase(MODE.WRITE);
                                        }
                                    }
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_ERASE)
                                {
                                    reqInBufSz(MODE.WRITE);

                                }else if (packet.Cmd == (int)COMMANDS.CMD_IN_BUF_SZ)
                                {
                                    _arduino_ser_buf_size = ((ushort)((packet.Payload[0] << 8) | packet.Payload[1]));
                                    _arduino_ser_buf_size -= 7; //compensate for header and footer
                                    
                                    _blockNum = 0;
                                    SetStatusLabel("Flasing image...");
                                    _totalBlocks = _image.Length / _arduino_ser_buf_size;
                                    UpdateProgress(100, 0);
                                    reqWrite(_blockNum, _arduino_ser_buf_size, _arduino_ser_buf_size);
                                }
                                else if (packet.Cmd == (int)COMMANDS.CMD_WRITE)
                                {
                                    _blockNum++;

                                    if (_totalBlocks >= _blockNum)
                                        UpdateProgress(_totalBlocks, _blockNum);

                                    if (_blockNum < _totalBlocks) reqWrite(_blockNum, _arduino_ser_buf_size, _arduino_ser_buf_size);
                                    else
                                    {
                                        //Is there any remainder?
                                        if (_blockNum == _totalBlocks)
                                        {
                                            int remainder = _image.Length - _blockNum * _arduino_ser_buf_size;
                                            if (remainder > 0)
                                                reqWrite(_blockNum, _arduino_ser_buf_size, remainder, true);
                                        }
                                        else
                                        {

                                            _mode = MODE.NONE;
                                            SetRuntime();
                                            SetStatusLabel("Flashing Done.");
                                            MessageBox.Show("Flashing Done.");
                                            EnableButtons();
                                        }
                                    }
                                }
                                break;

                            default:

                                break;
                        }
                    }
                }
            }
        }

        private bool writeMergedBin()
        {
            bool res = false;
            string readFilePath = Path.Combine(
                 Path.GetDirectoryName(openFileDialog1.FileName),
                 "image_merged.bin");
            try
            {
                if (File.Exists(readFilePath)) File.Delete(readFilePath);
            }
            catch
            {
                res = false;
                return res;
            }

            try
            {
                _BinFile = new BinaryWriter(new FileStream(readFilePath, FileMode.CreateNew));
                _BinFile.Write(_image, 0, _image.Length);
                _BinFile.Flush();
                _BinFile.Close();
                res = true;
            }catch
            {
                res = false;
            }

            return res;
        }

        private bool restoreFromBackup()
        {
            //Verify the validity of the fields
            bool bt_addr_blank = true;
            int startIdx = _image.Length - (int)FLD_OFFSETS.BT_ADDR;
            int endIdx = startIdx + (int)FLD_SIZES.BT_ADDR;
            for (int i = startIdx; i < endIdx; i++)
            {
                if (_image[i] != 0xFF)
                {
                    bt_addr_blank = false;
                    break;
                }
            }

            bool license_blank = true;
            startIdx = _image.Length - (int)FLD_OFFSETS.LICENSE;
            endIdx = startIdx + (int)FLD_SIZES.LICENSE;
            for (int i = startIdx; i < endIdx; i++)
            {
                if (_image[i] != 0xFF)
                {
                    license_blank = false;
                    break;
                }
            }

            bool hw_ver_blank = true;
            startIdx = _image.Length - (int)FLD_OFFSETS.HW_VER_OFFSET;
            endIdx = startIdx + (int)FLD_SIZES.HW_VER_OFFSET;
            for (int i = startIdx; i < endIdx; i++)
            {
                if (_image[i] != 0xFF)
                {
                    hw_ver_blank = false;
                    break;
                }
            }

            if (hw_ver_blank || license_blank || bt_addr_blank)
            {
                MessageBox.Show("Some field values (BT address, HW version, License) are blank. Flashing process is aborted");
                return false;

                /*if (MessageBox.Show("Some field values (BT address, HW version, License) are blank. Try to restore them from a backed up binary file?",
                    "Blank fields", MessageBoxButtons.YesNo) == System.Windows.Forms.DialogResult.Yes)
                {
                    openFileDialog1.FileName = "*.bin";
                    if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
                    {
                        int total_size = 1024 * 128;
                        BinaryReader br = new BinaryReader(File.Open(openFileDialog1.FileName, FileMode.Open));
                        byte[] tmp_image = new byte[total_size];
                        if (total_size == br.Read(tmp_image, 0, total_size))
                        {
                            Array.Copy(tmp_image, total_size - (int)FLD_OFFSETS.BT_ADDR,
                                       _image, total_size - (int)FLD_OFFSETS.BT_ADDR, (int)FLD_SIZES.BT_ADDR);

                            Array.Copy(tmp_image, total_size - (int)FLD_OFFSETS.HW_VER_OFFSET,
                                       _image, total_size - (int)FLD_OFFSETS.HW_VER_OFFSET, (int)FLD_SIZES.HW_VER_OFFSET);

                            Array.Copy(tmp_image, total_size - (int)FLD_OFFSETS.LICENSE,
                                       _image, total_size - (int)FLD_OFFSETS.LICENSE, (int)FLD_SIZES.LICENSE);


                            //TODO: display the merged values

                            return true;
                        }
                        else
                        {
                            MessageBox.Show("Wrong backup file size, flashing process is aborted");
                            return false;
                        }
                    }
                    else
                    {
                        MessageBox.Show("No backup file is opened, flashing process is aborted");
                        return false;
                    }

                }
                else
                {
                    MessageBox.Show("Some fields are still blank, flashing process is aborted");
                    return false;
                }*/
            }else
                return true;
        }

        private void reqErase(MODE mode)
        {
            _mode = mode;
            SetStatusLabel("Erasing...");
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_ERASE, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }


        private void reqInBufSz(MODE mode)
        {
            _mode = mode;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_IN_BUF_SZ, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }

        private void reqWrite(int block, int arduino_ser_buf_size, int payload_len, bool flush_buf = false)
        {
            UInt16 checksum = 0;
            byte cmd = flush_buf ? (byte)COMMANDS.CMD_WR_AND_FLUSH : (byte)COMMANDS.CMD_WRITE;
            byte[] frame = new byte[5] { cmd, 
                                         (byte)((block & 0xFF00) >> 8), 
                                         (byte)(block & 0xFF),
                                         (byte)((payload_len & 0xFF00) >> 8), 
                                         (byte)(payload_len & 0xFF) };

            for (int i = 0; i < frame.Length; i++) checksum += frame[i];
            serialPort1.Write(frame, 0, frame.Length);


            int addr = arduino_ser_buf_size * block;

            for (int i = addr; i < addr + payload_len; i++) checksum += _image[i];
            serialPort1.Write(_image, addr, payload_len);

            byte[] checkSumArr = new byte[] { (byte)((checksum & 0xFF00) >> 8), (byte)(checksum & 0xFF) };
            serialPort1.Write(checkSumArr, 0, checkSumArr.Length);


            _LogFile.WriteLine(string.Format("Writing block {0} at {1:x}", block, addr));
        }

        private void reqHwVer(MODE mode)
        {
            _mode = mode;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_HW_VER, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }

        private void reqBtAddr(MODE mode)
        {
            _mode = mode;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_BT_ADDR, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }

        private void reqIeeeAddr(MODE mode)
        {
            _mode = mode;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_IEEE_ADDR, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }

        private void reqLicense(MODE mode)
        {
            _mode = mode;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_LICENSE, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);
        }


        private void btnInfo_Click(object sender, EventArgs e)
        {
            _startTime = DateTime.Now;
            DisableButtons();
            _mode = MODE.CHIP_INFO;
            byte[] frame = new byte[4] { (byte)COMMANDS.CMD_CHIP_INFO, 0, 0, 0 };
            serialPort1.Write(frame, 0, frame.Length);

        }

        private void btnRead_Click(object sender, EventArgs e)
        {
            _startTime = DateTime.Now;
            DisableButtons();
            _blockNum = 0;
            _mode = MODE.READ;
            SetStatusLabel("Reading image...");
            byte[] writeBuf = new byte[] { (byte)COMMANDS.CMD_CHIP_INFO, (byte)_blockNum, 0, 0 };
            serialPort1.Write(writeBuf, 0, writeBuf.Length);
        }

        private void btnWrite_Click(object sender, EventArgs e)
        {
            if (openFileDialog1.ShowDialog() == System.Windows.Forms.DialogResult.OK)
            {
                _startTime = DateTime.Now;
                DisableButtons();

                if (Path.GetExtension(openFileDialog1.FileName).Equals(".hex"))
                {
                    _HexFileReader = new StreamReader(openFileDialog1.FileName);
                    IntelHex irec = new IntelHex();
                    IntelHexStructure irecs = irec.Read(_HexFileReader);
                    int blocksRead = 0;
                    long totalLength = 0;
                    while (irecs != null)
                    {
                        blocksRead++;
                        if (irecs.type == 1)
                        {
                            //EOF
                        }
                        else if (irecs.type == 0)
                        {
                            //data
                            totalLength += irecs.dataLen;
                        }
                        else if (irecs.type == 4)
                        {
                            //extended linear address
                        }
                        irecs = irec.Read(_HexFileReader);
                    }
                    _HexFileReader.Close();

                    _image = new byte[totalLength];

                    _HexFileReader = new StreamReader(openFileDialog1.FileName);
                    irec = new IntelHex();
                    irecs = irec.Read(_HexFileReader);
                    blocksRead = 0;
                    long extLinearAddr = 0;
                    while (irecs != null)
                    {
                        blocksRead++;
                        if (irecs.type == 1)
                        {
                            //EOF
                        }
                        else if (irecs.type == 0)
                        {
                            //data
                            for (long i = 0; i < irecs.dataLen; i++)
                            {
                                _image[extLinearAddr + irecs.address + i] = irecs.data[i];
                            }
                        }
                        else if (irecs.type == 4)
                        {
                            //extended linear address
                            extLinearAddr = (UInt16)((irecs.data[1] << 8 & 0xFF00) | (irecs.data[0] & 0xFF));
                            extLinearAddr = extLinearAddr << 8;
                        }
                        irecs = irec.Read(_HexFileReader);
                    }
                    _HexFileReader.Close();
                }
                else
                {
                    //int total_size = 1024 * 128;
                    //BinaryReader br = new BinaryReader(File.Open(openFileDialog1.FileName, FileMode.Open));
                    //_image = new byte[total_size];
                    //if (total_size != br.Read(_image, 0, total_size))
                    //{
                        //MessageBox.Show("Incorrect binary file size");
                        //EnableButtons();
                    //}

                    _image = File.ReadAllBytes(openFileDialog1.FileName);

                    //byte[] tmpimage = File.ReadAllBytes(openFileDialog1.FileName);
                    //round up to the whole number of Kb as there on Arduino side writings are done in bursts of 1K
                    //TODO: implement better logic to commit incomplete bursts
                    //int image_sz_kb = tmpimage.Length / 1024;
                    //if (image_sz_kb * 1024 < tmpimage.Length) image_sz_kb+=2;
                    
                    //_image = new byte[image_sz_kb * 1024];
                    //Array.Copy(tmpimage, _image, tmpimage.Length);
                }

                string readFilePath = Path.Combine(
                     Path.GetDirectoryName(openFileDialog1.FileName),
                     "image.bin");
                try
                {
                    if (File.Exists(readFilePath)) File.Delete(readFilePath);
                }
                catch
                {
                    ;
                }

                _BinFile = new BinaryWriter(new FileStream(readFilePath, FileMode.CreateNew));
                _BinFile.Write(_image, 0, _image.Length);
                _BinFile.Flush();
                _BinFile.Close();

                _mode = MODE.WRITE;
                byte[] frame = new byte[4] { (byte)COMMANDS.CMD_CHIP_INFO, 0, 0, 0 };
                serialPort1.Write(frame, 0, frame.Length);

                SetStatusLabel("Getting chip info...");
            }
        }

        private bool isBlank(byte[] array)
        {
            for (int i = 0; i < array.Length; i++)
            {
                if (array[i] != 0xFF) return false;
            }
            return true;
        }

        private void linkLabel1_LinkClicked(object sender, LinkLabelLinkClickedEventArgs e)
        {
            this.linkLabel1.LinkVisited = true;
            System.Diagnostics.Process.Start("http://magictale.com");
        }

        private void ccLibFrontend_FormClosing(object sender, FormClosingEventArgs e)
        {
            try
            {
                serialPort1.Close();
            }
            catch (Exception ex)
            {
                ;
            }
        }
    }
}
