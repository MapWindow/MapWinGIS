using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MWLite.GUI.Forms
{
    public partial class Nagscreen : Form
    {
        public Nagscreen()
        {
            InitializeComponent();
        }

        private void ShowOnStartupCheckbox_CheckedChanged(object sender, EventArgs e)
        {
            // Save to registry:
            Properties.Settings.Default.ShowNagScreen = ((CheckBox)sender).Checked;
            Properties.Settings.Default.Save();
        }

        private void webBrowser1_DocumentCompleted(object sender, WebBrowserDocumentCompletedEventArgs e)
        {
            // MessageBox.Show("DocumentCompleted");
            Application.DoEvents();
        }

        private void ProgressLabel_TextChanged(object sender, EventArgs e)
        {
            Application.DoEvents();
        }
    }
}
