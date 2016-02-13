namespace CCLibFrontend
{
    partial class ccLibFrontend
    {
        /// <summary>
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.serialPort1 = new System.IO.Ports.SerialPort(this.components);
            this.btnInfo = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.btnRead = new System.Windows.Forms.Button();
            this.btnWrite = new System.Windows.Forms.Button();
            this.openFileDialog1 = new System.Windows.Forms.OpenFileDialog();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.usbTBox = new System.Windows.Forms.TextBox();
            this.label4 = new System.Windows.Forms.Label();
            this.sramSizeTBox = new System.Windows.Forms.TextBox();
            this.flashSizeTBox = new System.Windows.Forms.TextBox();
            this.label3 = new System.Windows.Forms.Label();
            this.label2 = new System.Windows.Forms.Label();
            this.chipIDTBox = new System.Windows.Forms.TextBox();
            this.label1 = new System.Windows.Forms.Label();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.cbHideValues = new System.Windows.Forms.CheckBox();
            this.licenseTBox = new System.Windows.Forms.TextBox();
            this.label9 = new System.Windows.Forms.Label();
            this.btAddrTBox = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.hwVersionTBox = new System.Windows.Forms.TextBox();
            this.label7 = new System.Windows.Forms.Label();
            this.ieeeAddrTBox = new System.Windows.Forms.TextBox();
            this.label6 = new System.Windows.Forms.Label();
            this.linkLabel1 = new System.Windows.Forms.LinkLabel();
            this.label5 = new System.Windows.Forms.Label();
            this.statusLbl = new System.Windows.Forms.Label();
            this.statusLabel = new System.Windows.Forms.Label();
            this.runtimeLabel = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.groupBox1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.SuspendLayout();
            // 
            // serialPort1
            // 
            this.serialPort1.BaudRate = 57600;
            this.serialPort1.DataReceived += new System.IO.Ports.SerialDataReceivedEventHandler(this.serialPort1_DataReceived);
            // 
            // btnInfo
            // 
            this.btnInfo.Enabled = false;
            this.btnInfo.Location = new System.Drawing.Point(58, 15);
            this.btnInfo.Name = "btnInfo";
            this.btnInfo.Size = new System.Drawing.Size(91, 30);
            this.btnInfo.TabIndex = 7;
            this.btnInfo.Text = "Info";
            this.btnInfo.UseVisualStyleBackColor = true;
            this.btnInfo.Click += new System.EventHandler(this.btnInfo_Click);
            // 
            // progressBar1
            // 
            this.progressBar1.Location = new System.Drawing.Point(12, 280);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(425, 23);
            this.progressBar1.TabIndex = 8;
            // 
            // btnRead
            // 
            this.btnRead.Enabled = false;
            this.btnRead.Location = new System.Drawing.Point(58, 51);
            this.btnRead.Name = "btnRead";
            this.btnRead.Size = new System.Drawing.Size(91, 30);
            this.btnRead.TabIndex = 9;
            this.btnRead.Text = "Read";
            this.btnRead.UseVisualStyleBackColor = true;
            this.btnRead.Click += new System.EventHandler(this.btnRead_Click);
            // 
            // btnWrite
            // 
            this.btnWrite.Enabled = false;
            this.btnWrite.Location = new System.Drawing.Point(58, 87);
            this.btnWrite.Name = "btnWrite";
            this.btnWrite.Size = new System.Drawing.Size(91, 30);
            this.btnWrite.TabIndex = 10;
            this.btnWrite.Text = "Write";
            this.btnWrite.UseVisualStyleBackColor = true;
            this.btnWrite.Click += new System.EventHandler(this.btnWrite_Click);
            // 
            // openFileDialog1
            // 
            this.openFileDialog1.Filter = "*.hex|*.bin";
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.usbTBox);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.sramSizeTBox);
            this.groupBox1.Controls.Add(this.flashSizeTBox);
            this.groupBox1.Controls.Add(this.label3);
            this.groupBox1.Controls.Add(this.label2);
            this.groupBox1.Controls.Add(this.chipIDTBox);
            this.groupBox1.Controls.Add(this.label1);
            this.groupBox1.Location = new System.Drawing.Point(222, 12);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(215, 128);
            this.groupBox1.TabIndex = 11;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Chip Info";
            // 
            // usbTBox
            // 
            this.usbTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.usbTBox.Location = new System.Drawing.Point(90, 96);
            this.usbTBox.Name = "usbTBox";
            this.usbTBox.ReadOnly = true;
            this.usbTBox.Size = new System.Drawing.Size(31, 20);
            this.usbTBox.TabIndex = 17;
            this.usbTBox.TabStop = false;
            this.usbTBox.Text = "Yes";
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(57, 98);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(32, 13);
            this.label4.TabIndex = 16;
            this.label4.Text = "USB:";
            // 
            // sramSizeTBox
            // 
            this.sramSizeTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.sramSizeTBox.Location = new System.Drawing.Point(140, 75);
            this.sramSizeTBox.Name = "sramSizeTBox";
            this.sramSizeTBox.ReadOnly = true;
            this.sramSizeTBox.Size = new System.Drawing.Size(39, 20);
            this.sramSizeTBox.TabIndex = 15;
            this.sramSizeTBox.TabStop = false;
            this.sramSizeTBox.Text = "1";
            this.sramSizeTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // flashSizeTBox
            // 
            this.flashSizeTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.flashSizeTBox.Location = new System.Drawing.Point(140, 51);
            this.flashSizeTBox.Name = "flashSizeTBox";
            this.flashSizeTBox.ReadOnly = true;
            this.flashSizeTBox.Size = new System.Drawing.Size(39, 20);
            this.flashSizeTBox.TabIndex = 15;
            this.flashSizeTBox.TabStop = false;
            this.flashSizeTBox.Text = "123";
            this.flashSizeTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(57, 77);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(83, 13);
            this.label3.TabIndex = 14;
            this.label3.Text = "SRAM Size, Kb:";
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(57, 53);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(77, 13);
            this.label2.TabIndex = 14;
            this.label2.Text = "Flash Size, Kb:";
            // 
            // chipIDTBox
            // 
            this.chipIDTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.chipIDTBox.Location = new System.Drawing.Point(108, 29);
            this.chipIDTBox.Name = "chipIDTBox";
            this.chipIDTBox.ReadOnly = true;
            this.chipIDTBox.Size = new System.Drawing.Size(50, 20);
            this.chipIDTBox.TabIndex = 13;
            this.chipIDTBox.TabStop = false;
            this.chipIDTBox.Text = "0x0000";
            this.chipIDTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(57, 32);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(45, 13);
            this.label1.TabIndex = 12;
            this.label1.Text = "Chip ID:";
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnInfo);
            this.groupBox2.Controls.Add(this.btnRead);
            this.groupBox2.Controls.Add(this.btnWrite);
            this.groupBox2.Location = new System.Drawing.Point(12, 12);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(204, 128);
            this.groupBox2.TabIndex = 12;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Action";
            // 
            // groupBox3
            // 
            this.groupBox3.Controls.Add(this.cbHideValues);
            this.groupBox3.Controls.Add(this.licenseTBox);
            this.groupBox3.Controls.Add(this.label9);
            this.groupBox3.Controls.Add(this.btAddrTBox);
            this.groupBox3.Controls.Add(this.label8);
            this.groupBox3.Controls.Add(this.hwVersionTBox);
            this.groupBox3.Controls.Add(this.label7);
            this.groupBox3.Controls.Add(this.ieeeAddrTBox);
            this.groupBox3.Controls.Add(this.label6);
            this.groupBox3.Location = new System.Drawing.Point(12, 146);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(425, 107);
            this.groupBox3.TabIndex = 13;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "License Info";
            // 
            // cbHideValues
            // 
            this.cbHideValues.AutoSize = true;
            this.cbHideValues.Location = new System.Drawing.Point(339, 43);
            this.cbHideValues.Name = "cbHideValues";
            this.cbHideValues.Size = new System.Drawing.Size(83, 17);
            this.cbHideValues.TabIndex = 22;
            this.cbHideValues.Text = "Hide Values";
            this.cbHideValues.UseVisualStyleBackColor = true;
            // 
            // licenseTBox
            // 
            this.licenseTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.licenseTBox.Location = new System.Drawing.Point(6, 79);
            this.licenseTBox.Name = "licenseTBox";
            this.licenseTBox.ReadOnly = true;
            this.licenseTBox.Size = new System.Drawing.Size(413, 20);
            this.licenseTBox.TabIndex = 21;
            this.licenseTBox.TabStop = false;
            this.licenseTBox.Text = "001122334455667788990011223344556677889900112233445566778899001122";
            this.licenseTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(7, 63);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(47, 13);
            this.label9.TabIndex = 20;
            this.label9.Text = "License:";
            // 
            // btAddrTBox
            // 
            this.btAddrTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.btAddrTBox.Location = new System.Drawing.Point(270, 14);
            this.btAddrTBox.Name = "btAddrTBox";
            this.btAddrTBox.ReadOnly = true;
            this.btAddrTBox.Size = new System.Drawing.Size(106, 20);
            this.btAddrTBox.TabIndex = 19;
            this.btAddrTBox.TabStop = false;
            this.btAddrTBox.Text = "00:11:22:33:44:55";
            this.btAddrTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(197, 16);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(65, 13);
            this.label8.TabIndex = 18;
            this.label8.Text = "BT Address:";
            // 
            // hwVersionTBox
            // 
            this.hwVersionTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.hwVersionTBox.Location = new System.Drawing.Point(79, 40);
            this.hwVersionTBox.Name = "hwVersionTBox";
            this.hwVersionTBox.ReadOnly = true;
            this.hwVersionTBox.Size = new System.Drawing.Size(30, 20);
            this.hwVersionTBox.TabIndex = 17;
            this.hwVersionTBox.TabStop = false;
            this.hwVersionTBox.Text = "0x00";
            this.hwVersionTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(6, 42);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(67, 13);
            this.label7.TabIndex = 16;
            this.label7.Text = "HW Version:";
            // 
            // ieeeAddrTBox
            // 
            this.ieeeAddrTBox.Font = new System.Drawing.Font("Courier New", 8.25F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)), true);
            this.ieeeAddrTBox.Location = new System.Drawing.Point(79, 14);
            this.ieeeAddrTBox.Name = "ieeeAddrTBox";
            this.ieeeAddrTBox.ReadOnly = true;
            this.ieeeAddrTBox.Size = new System.Drawing.Size(106, 20);
            this.ieeeAddrTBox.TabIndex = 15;
            this.ieeeAddrTBox.TabStop = false;
            this.ieeeAddrTBox.Text = "00:11:22:33:44:55";
            this.ieeeAddrTBox.TextAlign = System.Windows.Forms.HorizontalAlignment.Right;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(6, 16);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(75, 13);
            this.label6.TabIndex = 14;
            this.label6.Text = "IEEE Address:";
            // 
            // linkLabel1
            // 
            this.linkLabel1.AutoSize = true;
            this.linkLabel1.Location = new System.Drawing.Point(237, 306);
            this.linkLabel1.Name = "linkLabel1";
            this.linkLabel1.Size = new System.Drawing.Size(106, 13);
            this.linkLabel1.TabIndex = 14;
            this.linkLabel1.TabStop = true;
            this.linkLabel1.Text = "http://magictale.com";
            this.linkLabel1.LinkClicked += new System.Windows.Forms.LinkLabelLinkClickedEventHandler(this.linkLabel1_LinkClicked);
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(105, 306);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(126, 13);
            this.label5.TabIndex = 15;
            this.label5.Text = "For more information visit ";
            // 
            // statusLbl
            // 
            this.statusLbl.AutoSize = true;
            this.statusLbl.Location = new System.Drawing.Point(12, 264);
            this.statusLbl.Name = "statusLbl";
            this.statusLbl.Size = new System.Drawing.Size(40, 13);
            this.statusLbl.TabIndex = 16;
            this.statusLbl.Text = "Status:";
            // 
            // statusLabel
            // 
            this.statusLabel.AutoSize = true;
            this.statusLabel.Location = new System.Drawing.Point(53, 264);
            this.statusLabel.Name = "statusLabel";
            this.statusLabel.Size = new System.Drawing.Size(31, 13);
            this.statusLabel.TabIndex = 17;
            this.statusLabel.Text = "        ";
            // 
            // runtimeLabel
            // 
            this.runtimeLabel.AutoSize = true;
            this.runtimeLabel.Location = new System.Drawing.Point(368, 264);
            this.runtimeLabel.Name = "runtimeLabel";
            this.runtimeLabel.Size = new System.Drawing.Size(31, 13);
            this.runtimeLabel.TabIndex = 19;
            this.runtimeLabel.Text = "        ";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(310, 264);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(52, 13);
            this.label11.TabIndex = 18;
            this.label11.Text = "Run time:";
            // 
            // ccLibFrontend
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(454, 328);
            this.Controls.Add(this.runtimeLabel);
            this.Controls.Add(this.label11);
            this.Controls.Add(this.statusLabel);
            this.Controls.Add(this.statusLbl);
            this.Controls.Add(this.label5);
            this.Controls.Add(this.linkLabel1);
            this.Controls.Add(this.groupBox3);
            this.Controls.Add(this.groupBox2);
            this.Controls.Add(this.groupBox1);
            this.Controls.Add(this.progressBar1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "ccLibFrontend";
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Arduino CC-Debugger for BLE112, BLE113 ";
            this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.ccLibFrontend_FormClosing);
            this.Load += new System.EventHandler(this.Form1_Load);
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.IO.Ports.SerialPort serialPort1;
        private System.Windows.Forms.Button btnInfo;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.Button btnRead;
        private System.Windows.Forms.Button btnWrite;
        private System.Windows.Forms.OpenFileDialog openFileDialog1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TextBox chipIDTBox;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.TextBox flashSizeTBox;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.TextBox sramSizeTBox;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TextBox usbTBox;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.LinkLabel linkLabel1;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.TextBox ieeeAddrTBox;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.TextBox hwVersionTBox;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.TextBox btAddrTBox;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox licenseTBox;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label statusLbl;
        private System.Windows.Forms.Label statusLabel;
        private System.Windows.Forms.Label runtimeLabel;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.CheckBox cbHideValues;
    }
}

