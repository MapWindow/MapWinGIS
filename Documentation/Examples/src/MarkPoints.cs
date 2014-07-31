using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        // the handle of the layer with markers
        private int m_layerHandle = -1;
    
        // <summary>
        // Loads the layers and registers event handlers
        // </summary>
        public void MarkPoints(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            string filename = dataPath + "buildings.shp";
            if (!File.Exists(filename))
            {
                MessageBox.Show("Couldn't file the file: " + filename);
                return;
            }

            var sf = new Shapefile();
            sf.Open(filename, null);
            m_layerHandle = axMap1.AddLayer(sf, true);
            sf = axMap1.get_Shapefile(m_layerHandle);     // in case a copy of shapefile was created by AxMap.ProjectionMismatchBehavior

            sf = new Shapefile();
            if (!sf.CreateNewWithShapeID("", ShpfileType.SHP_POINT))
            {
                MessageBox.Show("Failed to create shapefile: " + sf.ErrorMsg[sf.LastErrorCode]);
                return;
            }

            m_layerHandle = axMap1.AddLayer(sf, true);

            ShapeDrawingOptions options = sf.DefaultDrawingOptions;
            options.PointType = tkPointSymbolType.ptSymbolPicture;
            options.Picture = this.OpenMarker();
            sf.CollisionMode = tkCollisionMode.AllowCollisions;

            axMap1.SendMouseDown = true;
            axMap1.CursorMode = tkCursorMode.cmNone;
            MapEvents.MouseDownEvent += AxMap1MouseDownEvent;   // change MapEvents to axMap1
        }

        // <summary>
        // Opens a marker from the file
        // </summary>
        private Image OpenMarker()
        {
            string path = @"..\..\icons\marker.png";
            if (!File.Exists(path))
            {
                MessageBox.Show("Can't find the file: " + path);
            }
            else
            {
                Image img = new Image();
                if (!img.Open(path, ImageType.USE_FILE_EXTENSION, true, null))
                {
                    MessageBox.Show(img.ErrorMsg[img.LastErrorCode]);
                    img.Close();
                }
                else
                    return img;
            }
            return null;
        }

        // <summary>
        // Handles mouse down event and adds the marker
        // </summary>
        public void AxMap1MouseDownEvent(object sender, _DMapEvents_MouseDownEvent e)
        {
            if (e.button == 1)          // left button
            {
                Shapefile sf = axMap1.get_Shapefile(m_layerHandle);

                Shape shp = new Shape();
                shp.Create(ShpfileType.SHP_POINT);

                Point pnt = new Point();
                double x = 0.0;
                double y = 0.0;
                axMap1.PixelToProj(e.x, e.y, ref x, ref y);
                pnt.x = x;
                pnt.y = y;
                int index = shp.NumPoints;
                shp.InsertPoint(pnt, ref index);

                index = sf.NumShapes;
                if (!sf.EditInsertShape(shp, ref index))
                {
                    MessageBox.Show("Failed to insert shape: " + sf.ErrorMsg[sf.LastErrorCode]);
                    return;
                }
                axMap1.Redraw();
            }
        }
    }
}