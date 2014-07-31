using System;
using System.IO;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        private double m_distance = 0.0;  // the distance passed
        private double m_step = 10.0;   // the distance to pass on a single timer event
        private Shape m_path = null;      // the shape which holds the path of vehicle
        private int m_count = 0;          // number of steps performed
        public Timer m_timer = new Timer();

        // <summary>
        // Loads the layers, registers event handlers
        // </summary>
        public void Tracking(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_NONE;
            axMap1.GrabProjectionFromData = true;

            axMap1.DisableWaitCursor = true;

            string filename1 = dataPath + "buildings.shp";
            string filename2 = dataPath + "roads.shp";
            string filename3 = dataPath + "path.shp";

            if (!File.Exists(filename1) || !File.Exists(filename2) || !File.Exists(filename3))
            {
                MessageBox.Show("Couldn't find the files (buildings.shp, roads.shp, path.shp): " + dataPath);
            }
            else
            {
                Shapefile sf = new Shapefile();
                sf.Open(filename1, null);
                axMap1.AddLayer(sf, true);

                sf = new Shapefile();
                sf.Open(filename2, null);
                sf.Labels.Generate("[Name]", tkLabelPositioning.lpLongestSegement, false);

                Utils utils = new Utils();
                LinePattern pattern = new LinePattern();
                pattern.AddLine(utils.ColorByName(tkMapColor.Brown), 10.0f, tkDashStyle.dsSolid);
                pattern.AddLine(utils.ColorByName(tkMapColor.Yellow), 9.0f, tkDashStyle.dsSolid);
                sf.DefaultDrawingOptions.LinePattern = pattern;
                sf.DefaultDrawingOptions.UseLinePattern = true;
                axMap1.AddLayer(sf, true);

                sf = new Shapefile();
                sf.Open(filename3, null);
                m_path  = sf.Shape[0];
                axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
                axMap1.CurrentScale = 5000.0;

                m_timer.Interval = 250;
                m_timer.Tick += TimerTick;
                m_timer.Start();
            }
        }

        // <summary>
        // Calculates the new position
        // </summary>
        void TimerTick(object sender, EventArgs e)
        {
            // moves car a step further
            m_distance += m_step;
            if (m_distance > m_path.Length)
                m_distance = m_path.Length - m_distance;

            //calculating the current position (x2, y2)
            double distance = 0.0;
            double x1, x2, y1, y2;
            x1= x2 = y1 = y2 = 0.0;
            for (int i = 1; i < m_path.NumPoints; i++)
            {
                m_path.get_XY(i, ref x2, ref y2);
                m_path.get_XY(i - 1, ref x1, ref y1);
                double val = Math.Sqrt(Math.Pow(x2 - x1, 2.0) + Math.Pow(y2 - y1, 2.0));
                if (distance + val > m_distance)
                {
                    double ratio = (m_distance - distance) / val;
                    x2 = x1 + (x2 - x1) * ratio;
                    y2 = y1 + (y2 - y1) * ratio;
                    //distance += val * ratio;
                    break;
                }
                if (distance + val < m_distance)
                {
                    distance += val;
                }
                else
                {
                    break;
                }
            }

            this.DrawPosition(x2, y2);
        }

        // <summary>
        // Displays the point in the current position
        // </summary>
        private void DrawPosition(double x, double y)
        {
            try
            {
                axMap1.ClearDrawings();

                Extents ext = axMap1.Extents as Extents;
                if (x < ext.xMin || x > ext.xMax || y < ext.yMin || y > ext.yMax)
                {
                    double width = (ext.xMax - ext.xMin) / 2.0;
                    double height = (ext.yMax - ext.yMin) / 2.0;

                    ext.SetBounds(x - width, y - height, 0.0, x + width, y + height, 0.0);
                    axMap1.Extents = ext;
                    Application.DoEvents();
                }

                if (m_count % 2 == 0)
                {
                    int handle = axMap1.NewDrawing(tkDrawReferenceList.dlScreenReferencedList);
                    double pxX = 0.0;
                    double pxY = 0.0;
                    axMap1.ProjToPixel(x, y, ref pxX, ref pxY);
                    axMap1.DrawCircleEx(handle, pxX, pxY, 5.0, 255, true);
                }
                m_count++;
            }
            catch
            {
                // the function can be called when the form is about to close
            }
        }
    }
}