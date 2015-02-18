using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.IO;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using System.Diagnostics;

namespace TestGrids
{
    public partial class TestForm : Form
    {
        public TestForm()
        {
            InitializeComponent();
        }

        private void button1_Click(object sender, EventArgs e)
        {
            //string filename = @"d:\mw\TestingScripts\ScriptData\General\Images\Formats\HFA-Erdas-Imagine-Image.img";
            string filename = @"d:\mw\TestingScripts\ScriptData\General\Grids\Formats\grid.tif";
            axMap1.AddLayerFromFilename(filename, MapWinGIS.tkFileOpenStrategy.fosAutoDetect, true);
        }
    }
}
