using System;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.UI;

namespace MWLite.Databases.Forms
{
    public partial class OgrImportShapefile : Form
    {
        public OgrImportShapefile(string layerName)
        {
            InitializeComponent();
            txtLayerName.Text = layerName;
        }

        public string LayerName
        {
            get { return txtLayerName.Text; }
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(txtLayerName.Text))
            {
                MessageHelper.Info("Layer name is empty");
                return;
            }
            DialogResult = DialogResult.OK;
        }
    }
}
