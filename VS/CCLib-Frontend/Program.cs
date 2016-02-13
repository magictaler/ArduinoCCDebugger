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
using System.Windows.Forms;

namespace CCLibFrontend
{
    static class Program
    {
        /// <summary>
        /// The main entry point for the application.
        /// </summary>
        [STAThread]
        static void Main()
        {
            Application.EnableVisualStyles();
            Application.SetCompatibleTextRenderingDefault(false);
            Application.Run(new ccLibFrontend());
        }
    }
}
