using MapWinGIS;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;

namespace TestApplication
{
    public partial class StartForm : Form
    {
        public StartForm()
        {
            InitializeComponent();

            Shown += StartForm_Shown;
        }

        void StartForm_Shown(object sender, EventArgs e)
        {
            using (var form =new Form1())
            {
                form.ShowDialog(this);
                form.Close();
            }

            GC.Collect();
            GC.WaitForPendingFinalizers();

            var utils = new Utils();
            Debug.Print(utils.get_ComUsageReport(true));

            this.Close();
        }
    }
}
