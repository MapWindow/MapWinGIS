using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Windows.Forms;
using AxMapWinGIS;
using MapWinGIS;
using System.Diagnostics;

namespace Examples
{
    public partial class MapExamples
    {
        private ContextMenuStrip _menu = null;  // a menu to be displayed
        private int _fieldIndex = -1;           // the index of the [Name] field
    
        // <summary>
        // Loads layers, builds the lists of unique names for the [Name] field, registers event handler
        // </summary>
        public void ZoomToValues(AxMap axMap1, string dataPath)
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

            sf.Labels.Generate("[Name]", tkLabelPositioning.lpCenter, false);
            sf.Labels.TextRenderingHint = tkTextRenderingHint.SystemDefault;

            _fieldIndex = sf.Table.FieldIndexByName["Name"];
            var names = new Dictionary<string, int>();

            for (int i = 0; i < sf.NumShapes; i++)
            {
                string name = (string)sf.Table.CellValue[_fieldIndex, i];
                if (names.ContainsKey(name))
                {
                    names[name]++;
                }
                else
                {
                    names.Add(name, 1);
                }
            }

            IEnumerable<string> list = names.Where(val => val.Value == 1).Select(val => val.Key);
            _menu = new ContextMenuStrip();

            // there can be to much names to show in a single dropdown, 
            // therefore let's add letters of alphabet as the first level of the menu
            string s = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
            for (int i = 0; i < s.Length; i++)
            {
                ToolStripMenuItem item = new ToolStripMenuItem();
                item.Name = s[i].ToString();
                item.Text = s[i].ToString();
                _menu.Items.Add(item);
            }

            // adding names under particular letters
            foreach (string name in list)
            {
                string ch = name[0].ToString();
                foreach (ToolStripMenuItem item in _menu.Items)
                {
                    if (item.Name == ch)
                    {
                        ToolStripMenuItem subItem = new ToolStripMenuItem();
                        subItem.Click += new EventHandler(ItemClick);
                        subItem.Text = name;
                        subItem.Tag = "zoomable";
                        item.DropDownItems.Add(subItem);
                        break;
                    }
                }
            }
        
            // now let's remove the unused letters
            for (int i = _menu.Items.Count - 1; i >= 0; i--)
            {
                ToolStripMenuItem item = _menu.Items[i] as ToolStripMenuItem;
                if (item.DropDownItems.Count == 0)
                    _menu.Items.Remove(item);
            }
       
            axMap1.SendMouseDown = true;
            axMap1.CursorMode = tkCursorMode.cmNone;
            axMap1.MapUnits = tkUnitsOfMeasure.umMeters;
            MapEvents.MouseDownEvent += AxMap1MouseDownEvent4;  // change MapEvents to axMap1
        }

        // <summary>
        // Handles mouse click event. Shows the context menu with the names.
        // </summary>
        private void AxMap1MouseDownEvent4(object sender, _DMapEvents_MouseDownEvent e)
        {
            if (e.button == 2)          // right button
            {
                System.Drawing.Point pnt = axMap1.PointToScreen(new System.Drawing.Point(e.x, e.y));
                _menu.Show(pnt.X, pnt.Y);
            }
        }

        // <summary>
        // Zooms map to the object after user clicks the item on context menu.
        // </summary>
        private void ItemClick(object sender, EventArgs e)
        {
            ToolStripMenuItem item = (sender as ToolStripMenuItem);
            if (item == null) return;
            if (item.Tag.ToString() == "zoomable")
            {
                Shapefile sf = axMap1.get_Shapefile(m_layerHandle);
                if (sf != null)
                {
                    for (int i = 0; i <sf.NumShapes; i++)
                    {
                        string s = sf.get_CellValue(_fieldIndex, i).ToString();
                        Debug.Print(s);
                        if (s == item.Text)
                        {
                            axMap1.ZoomToShape(m_layerHandle, i);
                            axMap1.CurrentScale = 2000;
                            break;
                        }
                    }
                }
            }
        }
    }
}