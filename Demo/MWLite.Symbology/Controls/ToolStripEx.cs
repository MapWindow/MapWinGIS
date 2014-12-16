using System.Windows.Forms;

namespace MWLite.Symbology.Controls
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
