using MapWinGIS;
using MWLite.Core.UI;
using MWLite.GUI.Classes;
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
    public partial class BingApiKeyForm : Form
    {
        public BingApiKeyForm()
        {
            InitializeComponent();
            textBox1.Text = AppSettings.Instance.BingApiKey;
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            var gs = new GlobalSettings();
            string key = textBox1.Text;
            if (!gs.TestBingApiKey(key))
            {
                MessageHelper.Warn("Failed to initialize Bing maps provider.");
            }
            else
            {
                AppSettings.Instance.BingApiKey = key;
                DialogResult = DialogResult.OK;
            }
        }
    }
}
