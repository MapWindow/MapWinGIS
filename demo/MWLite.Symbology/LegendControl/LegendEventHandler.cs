using System;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MWLite.Symbology.Forms;
using MWLite.Symbology.Forms.Labels;
using MapWinGIS;

namespace MWLite.Symbology.LegendControl
{
    /// <summary>
    /// A class provider default handling of the Legend events
    /// </summary>
    public class LegendEventHandler
    {
        private Legend m_legend = null;
        
        /// <summary>
        /// Creates a new instance of the LegendEventHandler
        /// </summary>
        /// <param name="legend"></param>
        public LegendEventHandler(Legend legend)
        {
            if (legend == null)
                throw new NullReferenceException("Reference to the legend wasn't passed");
            
            m_legend = legend;
            m_legend.LayerColorboxClicked += m_legend_LayerColorboxClicked;
            m_legend.LayerLabelsClicked += m_legend_LayerLabelsClicked;
            m_legend.LayerCategoryClicked += m_legend_LayerCategoryClicked;
        }

        /// <summary>
        /// Handles the clicking on the layer color box
        /// </summary>
        /// <param name="Handle"></param>
        void m_legend_LayerColorboxClicked(int Handle)
        {
            if (m_legend == null || m_legend.m_Map == null)
                return;

            MapWinGIS.Shapefile sf = m_legend.m_Map.get_Shapefile(Handle);
            if (sf != null)
            {
                Form form = FormHelper.GetSymbologyForm(m_legend, Handle, sf.ShapefileType, sf.DefaultDrawingOptions, false);
                if (form.ShowDialog() == DialogResult.OK)
                {
                    // do something
                }
                form.Dispose();
            }
        }

        /// <summary>
        /// Handles the clicking on the labels for a layer
        /// </summary>
        /// <param name="Handle"></param>
        void m_legend_LayerLabelsClicked(int Handle)
        {
            if (m_legend == null || m_legend.m_Map == null)
                return;

            MapWinGIS.Shapefile sf = m_legend.m_Map.get_Shapefile(Handle);
            if (sf != null)
            {
                using (Form form = new LabelStyleForm(m_legend, sf, Handle))
                { 
                    if (form.ShowDialog() == DialogResult.OK)
                    {
                        m_legend.Refresh();
                    }
                }
            }
        }

        /// <summary>
        /// Handling the clicking on the category preview
        /// </summary>
        void m_legend_LayerCategoryClicked(int Handle, int Category)
        {
            if (m_legend == null || m_legend.m_Map == null)
                    return;

            MapWinGIS.Shapefile sf = m_legend.m_Map.get_Shapefile(Handle);
            if (sf != null)
            {
                MapWinGIS.ShapefileCategory cat = sf.Categories.get_Item(Category);
                if (cat != null)
                {
                    Form form = FormHelper.GetSymbologyForm(m_legend, Handle, sf.ShapefileType, cat.DrawingOptions, false);
                    if (form.ShowDialog() == DialogResult.OK)
                    {
                        // do something
                    }
                    form.Dispose();
                }
            }

        }
    }
}
