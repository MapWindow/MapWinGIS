using System;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // <summary>
        // This code calculates an area of polygons, writes it to the attribute table, and displays as labels.
        // </summary>
        public void CalculateArea(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;

            string filename = dataPath + "buildings.shp";

            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            var sf = new Shapefile();
            if (!sf.Open(filename, null))
                return;

            if (sf.ShapefileType != ShpfileType.SHP_POLYGON)
            {
                MessageBox.Show("Polygon shapefile is expected." + Environment.NewLine +
                                "Received: " + sf.ShapefileType);
            }
            else
            {
                int layerHandle = axMap1.AddLayer(sf, true);
                sf = axMap1.get_Shapefile(layerHandle);     // in case a copy of shapefile was created by GlobalSettings.ReprojectLayersOnAdding

                int fldIndex = sf.Table.FieldIndexByName["CalcArea"];
                if (fldIndex != -1)
                {
                    if (MessageBox.Show("The area field exists. Do you want to overwrite it?", "",
                        MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.No)
                    {
                        sf.Close();
                        return;
                    }
                }

                if (!sf.StartEditingTable(null))
                {
                    MessageBox.Show("Failed to start editing mode: " + sf.ErrorMsg[sf.LastErrorCode]);
                    sf.Close();
                }
                else
                {
                    // removing the field in case it is already present
                    if (fldIndex != -1)
                    {
                        if (!sf.EditDeleteField(fldIndex, null))
                        {
                            MessageBox.Show("Failed to delete field: " + sf.ErrorMsg[sf.LastErrorCode]);
                            sf.Close();
                            return;
                        }
                    }
                
                    //adding the new field in the end of the table
                    fldIndex = sf.EditAddField("CalcArea", FieldType.DOUBLE_FIELD, 9, 12);
                    if (fldIndex == -1)
                    {
                        MessageBox.Show("Failed to insert field: " + sf.ErrorMsg[sf.LastErrorCode]);
                        sf.Close();
                        return;
                    }

                    for (int i = 0; i < sf.NumShapes; i++)
                    {
                        Shape shp = sf.Shape[i];
                        sf.EditCellValue(fldIndex, i, shp.Area);
                    }

                    sf.Labels.Generate("[CalcArea] + \" sqr.m\"", tkLabelPositioning.lpCentroid, true);
                    sf.Labels.FrameVisible = true;
                }
            }
        }

        // <summary>
        // A shorter variant of procedure with less checks
        // </summary>
        public void CalculateAreaFast(Shapefile sf)
        {
            bool editing = sf.EditingTable;
        
            if (!sf.EditingTable)
            {
                if (sf.StartEditingTable(null))
                {
                    MessageBox.Show("Failed to open editing mode: " + sf.ErrorMsg[sf.LastErrorCode]);
                    return;
                }

                int fldIndex = sf.Table.FieldIndexByName["Area"];
                if (fldIndex == -1)
                {
                    MessageBox.Show("Field index doesn't exists");
                }
                else
                {
                    for (int i = 0; i < sf.NumShapes; i++)
                    {
                        Shape shp = sf.Shape[i];
                        sf.EditCellValue(fldIndex, i, shp.Area);
                    }
                
                    if (!editing)
                        sf.StopEditingTable(true, null);
                }
            }
        }
    }
}