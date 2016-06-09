using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MWLite.GUI.Forms
{
    public partial class LoaderForm : Form
    {
        public LoaderForm()
        {
            InitializeComponent();
            Shown += LoaderForm_Shown;
        }

        void LoaderForm_Shown(object sender, EventArgs e)
        {
            using (var form = new MainForm())
            { 
                form.ShowDialog(this);
            }
            GC.Collect();
            GC.WaitForPendingFinalizers();
            var utils = new MapWinGIS.Utils();
            Debug.WriteLine(utils.ComUsageReport[true]);
            this.Close();
        }
    }
}
