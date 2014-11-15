using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.MapLegend
{
    public partial class LegendDockForm : DockContent
    {
        private LegendDispatcher _dispatcher = null;

        public LegendDockForm()
        {
            InitializeComponent();
            _dispatcher = new LegendDispatcher(legend1);
            _dispatcher.InitMenu(contextMenuStrip1.Items);
            legend1.LayerMouseUp += legend1_LayerMouseUp;
        }

        public MapWindow.Legend.Controls.Legend.Legend Legend
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
                        _dispatcher.LayerHandle = Handle;
                        contextMenuStrip1.Show(Control.MousePosition);        
                    }
                }
            }
        }
    }
}
