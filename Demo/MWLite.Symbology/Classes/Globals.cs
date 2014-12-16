using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.Drawing;
using System.IO;
using System.Windows.Forms;
using System.Xml;
using MapWinGIS;
using MWLite.Symbology.Classes;
using MWLite.Symbology.LegendControl;

namespace MWLite.Symbology.Classes
{
    #region Enumerations
    /// <summary>
    /// Enumeration of supported layer types.
    /// </summary>
    public enum eLayerType
    {
        /// <summary>Invalid layer type</summary>
        Invalid = -1,
        /// <summary>Image layer</summary>
        Image = 0,
        /// <summary>Point shapefile layer</summary>
        PointShapefile = 1,
        /// <summary>Line shapefile layer</summary>
        LineShapefile = 2,
        /// <summary>Polygon shapefile layer</summary>
        PolygonShapefile = 3,
        /// <summary>Grid layer</summary>
        Grid = 4
    }
    #endregion

    internal static class Globals
    {
        #region Declarations
        // List of color schemes for layer
        internal static ColorSchemes LayerColors;

        // List of color schemes for charts
        internal static ColorSchemes ChartColors;
        #endregion

        /// <summary>
        /// Static constructor for Globals class
        /// </summary>
        static Globals()
        {
            LayerColors = new ColorSchemes(ColorSchemeType.Layer);
            XmlDocument doc = new XmlDocument();
            doc.LoadXml(Properties.Resources.colorschemes);
            LayerColors.LoadXML(doc);
        }

        /// <summary>
        /// A shortcut to show standard error window
        /// </summary>
        internal static void MessageBoxError(string message)
        {
            MessageBox.Show(message, "MapWindow GIS", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        /// <summary>
        /// A shortcut to show standard infromation window
        /// </summary>
        internal static void MessageBoxInformation(string message)
        {
            MessageBox.Show(message, "MapWindow GIS", MessageBoxButtons.OK, MessageBoxIcon.Error);
        }

        /// <summary>
        /// Returns 2D representation of shape type to simplify conditions
        /// </summary>
        public static MapWinGIS.ShpfileType ShapefileType2D(MapWinGIS.ShpfileType shpType)
        {
            if (shpType == ShpfileType.SHP_POLYGON || shpType == ShpfileType.SHP_POLYGONM || shpType == ShpfileType.SHP_POLYGONZ)
            {
                return ShpfileType.SHP_POLYGON;
            }
            else if (shpType == ShpfileType.SHP_POLYLINE || shpType == ShpfileType.SHP_POLYLINEM || shpType == ShpfileType.SHP_POLYLINEZ)
            {
                return ShpfileType.SHP_POLYLINE;
            }
            else if (shpType == ShpfileType.SHP_POINT || shpType == ShpfileType.SHP_POINTM || shpType == ShpfileType.SHP_POINTZ ||
                     shpType == ShpfileType.SHP_MULTIPOINT || shpType == ShpfileType.SHP_MULTIPOINTM || shpType == ShpfileType.SHP_MULTIPOINTZ)
            {
                return ShpfileType.SHP_POINT;
            }
            else
            {
                return ShpfileType.SHP_NULLSHAPE;
            }
        }

        /// <summary>
        /// Returns descriptions of the standard types of symbology (random and default)
        /// </summary>
        internal static string GetSymbologyDescription(SymbologyType symbologyType)
        {
            string s = "";
            if (symbologyType == SymbologyType.Default)
            {
                s = "Default options stored in the .mwsymb or .mwsr files";
            }
            else if (symbologyType == SymbologyType.Random)
            {
                s = "Options set randomly by MapWinGIS ActiveX control";
            }
            return s;
        }

        /// <summary>
        /// Build list of available options for the layer (.mwsymb, .mwsr files)
        /// </summary>
        internal static void FillSymbologyList(ListView listView, string filename, bool manager, ref bool noEvents)
        {
            noEvents = true;
            listView.Items.Clear();

            // always available
            if (!manager)
            {
                ListViewItem item = listView.Items.Add("[random]");
                item.Tag = SymbologyType.Random;
            }

            string path = filename + ".mwsymb";
            if (System.IO.File.Exists(path))
            {
                ListViewItem item = listView.Items.Add("[default]");
                item.Tag = SymbologyType.Default;
            }

            // cities.shp.default.mwsymb
            path = System.IO.Path.GetDirectoryName(filename);
            string[] names = System.IO.Directory.GetFiles(path, System.IO.Path.GetFileName(filename) + "*");
            for (int i = 0; i < names.Length; i++)
            {
                if (names[i].ToLower().EndsWith(".mwsymb"))
                {
                    string name = names[i].Substring(filename.Length);
                    if (name.ToLower() == ".mwsymb")
                    {
                        // was added before
                    }
                    else
                    {
                        name = name.Substring(1, name.Length - 8);
                        ListViewItem item = listView.Items.Add(name);
                        item.Tag = SymbologyType.Custom;
                    }
                }
            }

            if (listView.Items.Count > 0)
                listView.SelectedIndices.Add(0);
            noEvents = false;
        }

        private static Dictionary<int, SymbologySettings> _settings = new Dictionary<int, SymbologySettings>();

        #region Obsolete
        internal static SymbologySettings get_LayerSettings(int layerHandle)
        {
            
            // TODO: restore
            //SymbologySettings settings = null;
            //MWLite.Symbology.Layer layer = Legend.GetLayer(layerHandle);
            //if (layer != null)
            //{
            //    settings = (SymbologySettings)layer.GetCustomObject("SymbologyPluginSettings");
            //}
            if (_settings.ContainsKey(layerHandle))
            {
                return _settings[layerHandle];
            }
            else
            {
                var settings = new SymbologySettings();
                _settings[layerHandle] = settings;
                return settings;
            }
        }

        /// <summary>
        /// Saves symbology settings for the layer
        /// </summary>
        internal static void SaveLayerSettings(int layerHandle, SymbologySettings settings)
        {
            // TODO: restore
            //MWLite.Symbology.Layer layer = Legend.GetLayer(layerHandle);
            //if (layer != null)
            //{
            //    layer.SetCustomObject(settings, "SymbologyPluginSettings");
            //}
        }

        internal static void SaveLayerOptions(int LayerHandle)
        {
            // TODO: restore
            //if (mapWin.ApplicationInfo.SymbologyLoadingBehavior == MapWindow.Interfaces.SymbologyBehavior.DefaultOptions)
            //{
            //    AxMapWinGIS.AxMap map = Globals.Map;
            //    if (map != null)
            //    {
            //        map.SaveLayerOptions(LayerHandle, "", true, "");
            //    }
            //}
        }
        #endregion

        // move files to the project resources
        #region Path
        /// <summary>
        /// Returns path to the default directory with icons
        /// </summary>
        internal static string GetIconsPath()
        {
            string filename = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            //filename = Directory.GetParent(filename).FullName;
            return filename + "\\Styles\\Icons\\";
        }

        /// <summary>
        /// Returns path to the default directory with icons
        /// </summary>
        internal static string GetTexturesPath()
        {
            string filename = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            filename = Directory.GetParent(filename).FullName;
            return filename + "\\Styles\\Textures\\";
        }
        #endregion
    }

    #region Callbacks
    /// <summary>
    /// Implementation of callback interface to return progress information
    /// </summary>
    internal class Callback : MapWinGIS.ICallback
    {
        public void Error(string KeyOfSender, string ErrorMsg)
        {
            return;
        }
        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            if (string.IsNullOrEmpty(Message)) {
                //MapWinUtility.Logger.Progress(Percent, 100);
            }
            else
                //MapWinUtility.Logger.Progress(Message, Percent, 100);
                Debug.Print("{0}: {1}", Message, Percent);
        }
        public void Clear()
        {
            //MapWinUtility.Logger.Progress("", 100, 100);
        }
    }

    /// <summary>
    /// Implementation of callback interface to return progress information
    /// </summary>
    internal class CallbackLocal : MapWinGIS.ICallback
    {
        ProgressBar _progress = null;
        public CallbackLocal(ProgressBar progress)
        {
            _progress = progress;
        }
        public void Error(string KeyOfSender, string ErrorMsg)
        {
            return;
        }
        public void Progress(string KeyOfSender, int Percent, string Message)
        {
            if (!_progress.Visible)
            {
                _progress.Visible = true;
            }
            _progress.Value = Percent;
            Application.DoEvents();
            if (Percent == 100)
            {
                this.Clear();
            }
        }
        public void Clear()
        {
            _progress.Value = 0;
            _progress.Visible = false;
            Application.DoEvents();
        }
    }
    #endregion

    #region Globals
    internal class globals
    {
        //public static MapWinGIS.Map map = null;
        //public static MapWinGIS.m_legend Legend = null;
        public static string LastError;

        private struct POINTAPI
        {
            public int x;
            public int y;
        }

        [System.Runtime.InteropServices.DllImport("user32.dll")]
        private static extern void GetCursorPos(ref POINTAPI lpPoint);

        public static System.Drawing.Point GetCursorLocation()
        {
            POINTAPI pnt = new POINTAPI();
            GetCursorPos(ref pnt);
            return new System.Drawing.Point(pnt.x, pnt.y);
        }

        public static bool IsSupportedPicture(object picture)
        {
            if (picture == null)
                return true;

            System.Type picType = picture.GetType();
            if (typeof(Icon) == picType)
                return true;
            if (typeof(MapWinGIS.Image) == picType)
                return true;
            if (typeof(Bitmap) == picType)
                return true;

            return false;
        }
    }
    #endregion

    #region Colors
    public static class Colors
    {

        public static Color UintToColor(uint val)
        {
            int r, g, b;

            GetRGB((int)val, out r, out g, out b);

            return Color.FromArgb(255, r, g, b);
        }

        public static void GetRGB(int Color, out int r, out int g, out int b)
        {
            if (Color < 0)
                Color = 0;

            r = (int)(Color & 0xFF);
            g = (int)(Color & 0xFF00) / 256;	//shift right 8 bits
            b = (int)(Color & 0xFF0000) / 65536; //shift right 16 bits
        }

        public static int ColorToInt(Color c)
        {
            int retval = ((int)c.B) << 16;
            retval += ((int)c.G) << 8;
            return retval + ((int)c.R);
        }

        public static UInt32 ColorToUInteger(Color c)
        {
            int retval = ((int)c.B) << 16;
            retval += ((int)c.G) << 8;
            return Convert.ToUInt32(retval + ((int)c.R));
        }

        public static int ColorToaInt(Color c)
        {
            int retval = ((int)c.B) << 16;
            retval += ((int)c.G) << 8;
            return retval + ((int)c.R);
        }

        public static Color HSLtoColor(float Hue, float Sat, float Lum)
        {

            double r = 0, g = 0, b = 0;

            double temp1, temp2;



            if (Lum == 0)
            {

                r = g = b = 0;

            }

            else
            {

                if (Sat == 0)
                {

                    r = g = b = Lum;

                }

                else
                {

                    temp2 = ((Lum <= 0.5) ? Lum * (1.0 + Sat) : Lum + Sat - (Lum * Sat));

                    temp1 = 2.0 * Lum - temp2;



                    double[] t3 = new double[] { Hue + 1.0 / 3.0, Hue, Hue - 1.0 / 3.0 };

                    double[] clr = new double[] { 0, 0, 0 };

                    for (int i = 0; i < 3; i++)
                    {

                        if (t3[i] < 0)

                            t3[i] += 1.0;

                        if (t3[i] > 1)

                            t3[i] -= 1.0;



                        if (6.0 * t3[i] < 1.0)

                            clr[i] = temp1 + (temp2 - temp1) * t3[i] * 6.0;

                        else if (2.0 * t3[i] < 1.0)

                            clr[i] = temp2;

                        else if (3.0 * t3[i] < 2.0)

                            clr[i] = (temp1 + (temp2 - temp1) * ((2.0 / 3.0) - t3[i]) * 6.0);

                        else

                            clr[i] = temp1;

                    }

                    r = clr[0];

                    g = clr[1];

                    b = clr[2];

                }

            }
            return Color.FromArgb((int)(255 * r), (int)(255 * g), (int)(255 * b));
        }



        public static void GetHSL(Color c, out float Hue, out float Sat, out float Lum)
        {
            Hue = c.GetHue() / 360f;
            Sat = c.GetSaturation();
            Lum = c.GetBrightness();
        }
    }


    internal class Constants
    {
        public static int ITEM_HEIGHT = 18;
        public static int ITEM_PAD = 4;
        public static int ITEM_RIGHT_PAD = 5;
        //  TEXT
        public static int TEXT_HEIGHT = 14;
        public static int TEXT_TOP_PAD = 3;
        public static int TEXT_LEFT_PAD = 30;
        public static int TEXT_RIGHT_PAD = 25;
        public static int TEXT_RIGHT_PAD_NO_ICON = 8;
        //  CHECK BOX
        public static int CHECK_TOP_PAD = 4;
        public static int CHECK_LEFT_PAD = 15;
        public static int CHECK_BOX_SIZE = 12;
        //  EXPANSION BOX
        public static int EXPAND_BOX_TOP_PAD = 5;
        public static int EXPAND_BOX_LEFT_PAD = 3;
        public static int EXPAND_BOX_SIZE = 8;
        //  GROUP
        public static int GRP_INDENT = 3;
        //	LIST ITEMS
        public static int LIST_ITEM_INDENT = 18;
        public static int ICON_RIGHT_PAD = 25;
        public static int ICON_TOP_PAD = 3;
        public static int ICON_SIZE = 13;

        //	CONNECTION LINES FROM GROUPS TO SUB ITEMS
        public static int VERT_LINE_INDENT = (GRP_INDENT + 7);
        public static int VERT_LINE_GRP_TOP_OFFSET = 14;
        //	COLOR SCHEME CONSTANTS
        public static int CS_ITEM_HEIGHT = 14;
        public static int CS_TOP_PAD = 1;
        public static int CS_PATCH_WIDTH = 15;
        public static int CS_PATCH_HEIGHT = 12;
        public static int CS_PATCH_LEFT_INDENT = (CHECK_LEFT_PAD);
        public static int CS_TEXT_LEFT_INDENT = (CS_PATCH_LEFT_INDENT + CS_PATCH_WIDTH + 3);
        public static int CS_TEXT_TOP_PAD = 3;
        //	SCROLLBAR
        public static int SCROLL_WIDTH = 15;
        //	MISC
        // DROP_TOLERANCE 4

        public static int INVALID_INDEX = -1;

        // constants for the new symbology
        public static int ICON_WIDTH = 24;
        public static int ICON_HEIGHT = 13;

        //*******************************************************
        //Visual Basic Related constants
        //*******************************************************
        public static int VB_SHIFT_BUTTON = 1;
        public static int VB_LEFT_BUTTON = 1;
        public static int VB_RIGHT_BUTTON = 2;
    }
    #endregion
}
