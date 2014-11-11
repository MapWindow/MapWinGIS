using System.Drawing;
using System.Windows.Forms;

namespace MapWindow.Legend.Controls
{
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
