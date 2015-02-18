using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;

namespace MWLite.GUI.Forms
{
    public partial class ImagePropertiesForm : Form
    {
        public ImagePropertiesForm(int layerHandle)
        {
            InitializeComponent();
            
            Text = "GDAL info: " + App.Map.get_LayerName(layerHandle);
            
            var utils = new Utils();
            richTextBox1.Text = utils.GDALInfo(App.Map.get_LayerFilename(layerHandle), "");
        }
    }
}
