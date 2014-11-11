using MapWinGIS;
using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Linq;
using System.Runtime.InteropServices;
using System.Text;
using System.Windows.Forms;

namespace MWLite.GUI.Classes
{
    [ComVisible(true)]
    public class MapCallback: ICallback
    {
        ToolStripProgressBar _progress;
        ToolStripLabel _progressLabel;
        StatusStrip _statusStrip;

        public MapCallback(StatusStrip status, ToolStripProgressBar progress, ToolStripLabel progressLabel)
        {
            _statusStrip = status;
            _progress = progress;
            _progressLabel = progressLabel;
        }

        #region ICallback interface

        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            if (Percent == 0)
            { 
                _progress.Visible = false;
                _progressLabel.Text = "";
            }
            else
            { 
               _progress.Visible = true;
               _progress.Value = Percent;
               _progressLabel.Text = Message;
            }
            _statusStrip.Refresh();
        }

        public void Error(string KeyOfSender, string ErrorMsg)
        {
            Debug.Print("OCX error callback: " + ErrorMsg);
        }

        #endregion
    }
}
