using System.ComponentModel;
using System.Drawing;
using System.Windows.Forms;

namespace MWLite.Symbology.Controls
{
    [ToolboxItem(false)]
    public partial class TransparentControl : UserControl
    {
        public TransparentControl()
        {
            InitializeComponent();
            SetStyle(ControlStyles.SupportsTransparentBackColor, true);
            this.BackColor = Color.Transparent;
        }
    }
}
