using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWindow.Legend.Controls.Legend;
using MapWindow.Legend.Forms;
using MapWindow.Legend.Forms.Labels;
using MapWinGIS;
using MWLite.Core.UI;

namespace MWLite.GUI.Forms.Dock
{
    internal class LegendCommandMap : CommandMap<LegendCommand>
    {
        internal int LayerHandle = -1;
        private Legend _legend = null;

        public LegendCommandMap(Legend legend)
        {
            _legend = legend;
        }

        public override void Run(LegendCommand command)
        {
            Shapefile sf = null;
            var layer = _legend.Layers.ItemByHandle(LayerHandle);
            if (layer != null)
            {
                sf = layer.GetObject() as Shapefile;
            }

            switch (command)
            {
                case LegendCommand.ZoomToLayer:
                    _legend.Map.ZoomToLayer(LayerHandle);
                    break;
                case LegendCommand.Labels:
                    {
                        using (Form form = new LabelStyleForm(_legend, sf, LayerHandle))
                        {
                            form.ShowDialog(MainForm.Instance);
                            _legend.Refresh();
                        }
                    }
                    break;
                case LegendCommand.Properties:
                    {
                        using (var form = new frmSymbologyMain(_legend, LayerHandle))
                        {
                            form.ShowDialog(MainForm.Instance);
                            _legend.Refresh();
                        }
                    }
                    break;
            }
        }

        protected override void CommandNotFound(ToolStripItem item) { }
    }
}
