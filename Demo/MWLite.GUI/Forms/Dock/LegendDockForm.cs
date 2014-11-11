using System;
using System.Windows.Forms;
using MapWindow.Legend;
using MapWindow.Legend.Controls.Legend;
using MapWindow.Legend.Forms;
using MapWindow.Legend.Forms.Categories;
using MapWindow.Legend.Forms.Labels;
using MWLite.Core;
using MWLite.Core.UI;
using MWLite.GUI.Events;
using MWLite.GUI.Helpers;
using WeifenLuo.WinFormsUI.Docking;
using MapWinGIS;

namespace MWLite.GUI.Forms.Dock
{
    public partial class LegendDockForm : DockContent
    {
        private LegendCommandMap _commandMap = null;

        public LegendDockForm()
        {
            InitializeComponent();
            _commandMap = new LegendCommandMap(legend1);
            _commandMap.InitMenu(contextMenuStrip1.Items);
            legend1.LayerMouseUp += legend1_LayerMouseUp;
        }

        public Legend Legend
        {
            get { return legend1;  }
        }

        private void legend1_LayerMouseUp(int Handle, MouseButtons button)
        {
            if (button == MouseButtons.Right && Handle != -1)
            {
                var layer = legend1.Layers.ItemByHandle(Handle);
                if (layer != null)
                {
                    var sf = layer.GetObject() as MapWinGIS.Shapefile;
                    if (sf != null)
                    {
                        _commandMap.LayerHandle = Handle;
                        contextMenuStrip1.Show(Control.MousePosition);        
                    }
                }
            }
        }
    }
}
