using System.Windows.Forms;

namespace MapWindow.Legend.Controls
{
    class ToolStripEx: ToolStrip
    {
        protected override CreateParams CreateParams
        {
            get
            {
                CreateParams cp = base.CreateParams;
                cp.ExStyle |= 0x20; // WS_EX_TRANSPARENT
                return cp;
            }
        }
    }
}
