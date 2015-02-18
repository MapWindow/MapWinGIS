using System;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Adds randomly positioned labels to the image layer.
        // </summary>
        public void ImageLabels(AxMap axMap1)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            Image img = new Image();
            OpenFileDialog dlg = new OpenFileDialog {Filter = img.CdlgFilter};
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                img.Open(dlg.FileName, ImageType.USE_FILE_EXTENSION, false, null);
                axMap1.AddLayer(img, true);

                Labels lbl = img.Labels;
                lbl.FontSize = 12;
                lbl.FontBold = true;

                lbl.FontOutlineVisible = true;
                lbl.FontOutlineColor = (255 << 16) + (255 << 8) + 255;  //white
                lbl.FontOutlineWidth = 4;

                LabelCategory cat = lbl.AddCategory("Red");
                cat.FontColor = 255;

                cat = lbl.AddCategory("Blue");
                cat.FontColor = 255 << 16;

                cat = lbl.AddCategory("Yellow");
                cat.FontColor = 255 + 255 << 8;

                Extents ext = img.Extents;
                double xRange = ext.xMax - ext.xMin;
                double yRange = ext.yMax - ext.yMin;
                Random rnd = new Random();

                for (int i = 0; i < 100; i++)
                {
                    double x = xRange * rnd.NextDouble();
                    double y = yRange * rnd.NextDouble();

                    int categoryIndex = i % 3;
                    lbl.AddLabel("label" + Convert.ToString(i),
                        ext.xMin + x, ext.yMin + y, i * 3.6, categoryIndex);
                }

                axMap1.Redraw();
            }
        }
    }
}