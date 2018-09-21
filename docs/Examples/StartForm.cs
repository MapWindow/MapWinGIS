using System;
using System.Windows.Forms;

namespace Examples
{
    public partial class StartForm : Form
    {
        public StartForm()
        {
            InitializeComponent();

            Shown += StartForm_Shown;
        }

        private void StartForm_Shown(object sender, EventArgs e)
        {
            using (var form = new TestForm())
            {
                form.ShowDialog(this);
                form.Close();
            }

            GC.Collect();
            GC.WaitForPendingFinalizers();
            Close();
        }
    }
}
