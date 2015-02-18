using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // Split a shapefile into several ones according the values the specified attribute.
        // </summary>
        public void SplitByAttribute(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;
            
            string filename = dataPath + "natural.shp";
            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
            }
            else
            {
                Shapefile sf = new Shapefile();
                sf.Open(filename, null);

                int fieldIndex = sf.Table.FieldIndexByName["type"];
                sf.Categories.Generate(fieldIndex, tkClassificationType.ctUniqueValues, 0);
                sf.Categories.ApplyExpressions();

                ColorScheme scheme = new ColorScheme();
                scheme.SetColors2(tkMapColor.White, tkMapColor.Black);

                for (int i = 0; i < sf.Categories.Count; i++)
                {
                    Shapefile sfNew = sf.Clone();
                    int layerHandle = axMap1.AddLayer(sfNew, true);

                    for (int shapeIndex = 0; shapeIndex < sf.NumShapes; shapeIndex++)
                    {
                        if (sf.ShapeCategory[shapeIndex] == i)
                        {
                            Shape shape = sf.Shape[shapeIndex].Clone();
                            int index = sfNew.NumShapes;
                            sfNew.EditInsertShape(shape, ref index); 
                        }
                    }

                    ShapefileCategory category = sf.Categories.Item[i];
                    string name = category.Name.Substring(category.Name.IndexOf("=") + 1);

                    uint color = scheme.get_RandomColor((i + 1) / sf.Categories.Count);
                    sfNew.DefaultDrawingOptions.FillColor = color;
                    
                    axMap1.set_LayerName(layerHandle, name);
                
                    //sfNew.SaveAs(path + name + ".shp", null);    // saves shapefile
                }
                ShowLegend();
                axMap1.ZoomToMaxExtents();
                axMap1.Redraw();

            }
        }

        // <summary>
        // Shows the names of layers on the drawing layer
        // </summary>
        private void ShowLegend()
        {
            int width = 40;
            int height = 20;
            int padding = 5;

            int drawHandle = axMap1.NewDrawing(tkDrawReferenceList.dlScreenReferencedList);
            Labels labels = axMap1.get_DrawingLabels(drawHandle);
            if (labels != null)
                labels.Alignment = tkLabelAlignment.laBottomRight;

            Shapefile sf = new Shapefile();
            string message = "";
            for (int i = 0; i < axMap1.NumLayers; i++)
            {
                int layerHandle = axMap1.get_LayerHandle(i);
                sf = axMap1.get_Shapefile(layerHandle);

                // adds rectangle
                object x, y;
                int top = padding + i * (height + padding);
                this.getRectange(padding, top, width, height, out x, out y);
                axMap1.DrawPolygonEx(drawHandle, ref x, ref y, 4, sf.DefaultDrawingOptions.FillColor, true);

                // adds text
                string text = axMap1.get_LayerName(layerHandle) + ".shp";
                var dlbls = axMap1.get_DrawingLabels(drawHandle);
                if (dlbls != null)
                    dlbls.AddLabel(text, padding * 2 + width, top + padding);

                // the position of text (for debugging)
                axMap1.DrawPointEx(drawHandle, padding * 2 + width, top, 2, 255);
            }
        }

        // <summary>
        // Returns coordinates of the rectangle with specified size and position
        // </summary>
        private void getRectange(int left, int top, int width, int height, out object xArr, out object yArr)
        {
            double[] x = new double[4];
            double[] y = new double[4];

            x[0] = left;
            x[1] = left;
            x[2] = left + width;
            x[3] = left + width;

            y[0] = top;
            y[1] = top + height;
            y[2] = top + height;
            y[3] = top;

            xArr = x; yArr = y;
        }
    }
}