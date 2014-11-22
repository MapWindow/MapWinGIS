using System;
using System.Collections.Generic;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace MWLite.Core
{
    public static class WinApi
    {
        [DllImport("user32.dll", CharSet = CharSet.Auto, CallingConvention = CallingConvention.Winapi)]
        internal static extern IntPtr GetFocus();

        public static Control GetFocusedControl()
        {
            IntPtr focusedHandle = GetFocus();
            return focusedHandle != IntPtr.Zero ? Control.FromHandle(focusedHandle) : null;
        }
    }
}
