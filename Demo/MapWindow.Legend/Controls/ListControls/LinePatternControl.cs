// ********************************************************************************************************
// <copyright file="MapWindow.Legend.cs" company="MapWindow.org">
// Copyright (c) MapWindow.org. All rights reserved.
// </copyright>
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http:// Www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
// limitations under the License. 
// 
// The Initial Developer of this version of the Original Code is Sergei Leschinski
// 
// Contributor(s): (Open source contributors should list themselves and their modifications here). 
// Change Log: 
// Date            Changed By      Notes
// ********************************************************************************************************

using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.IO;
using System.Xml;
using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Controls.ListControls
{
    /// <summary>
    /// A control to show the list of available line pattern styles
    /// </summary>
    [ToolboxItem(true)]
    internal partial class LinePatternControl : ListControl
    {
        #region Declaration
        // The list of icons
        List<MapWinGIS.LinePattern> _patterns = new List<MapWinGIS.LinePattern>();

        #endregion

        #region Initialization
        /// <summary>
        /// Creates a new instance of the LinePatternControl
        /// </summary>
        public LinePatternControl()
        {
            this.CellWidth = 64;
            this.CellHeight = 24;
            this.OnDrawItem += new OnDrawItemDelegate(Control_OnDrawItem);

            this.AddDefaultPatterns();
        }

        /// <summary>
        /// Adds default patterns to the list
        /// </summary>
        private void AddDefaultPatterns()
        {
            _patterns.Clear();

            MapWinGIS.LinePattern pattern = new MapWinGIS.LinePattern();
            MapWinGIS.Utils utils = new MapWinGIS.Utils();
            pattern.AddLine(utils.ColorByName(MapWinGIS.tkMapColor.Red), 1, MapWinGIS.tkDashStyle.dsSolid);
            _patterns.Add(pattern);

            // TODO: mode patterns can be added

            this.ItemCount = _patterns.Count;
        }
        
        /// <summary>
        /// Adds pattern to the list
        /// </summary>
        internal void AddPattern(MapWinGIS.LinePattern pattern)
        {
            if (pattern != null && pattern.Count > 0)
            {
                _patterns.Add(pattern);
                this.ItemCount = _patterns.Count;
            }
        }

        /// <summary>
        /// Removes given pattern from the list
        /// </summary>
        internal void RemovePattern(int index)
        {
            if (index >= 0 && index < _patterns.Count)
            {
                _patterns.RemoveAt(index);
                this.ItemCount = _patterns.Count;
            }
        }

        /// <summary>
        /// Gets the selected pattern or null if there is no one
        /// </summary>
        internal MapWinGIS.LinePattern SelectedPattern
        {
            get
            {
                if (this.ItemCount == 0 || this.SelectedIndex < 0)
                {
                    return null;
                }
                else
                {
                    if (this.SelectedIndex < this.ItemCount)
                    {
                        return _patterns[this.SelectedIndex];
                    }
                    else
                    {
                        return null;
                    }
                }
            }
        }
        #endregion

        #region Serialization
        /// <summary>
        /// Saves list of styles to XML
        /// </summary>
        public bool SaveToXML()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml("<MapWindow version= '" + "'></MapWindow>");     // TODO: add version
            
            XmlElement xelRoot = xmlDoc.DocumentElement;

            XmlAttribute attr = xmlDoc.CreateAttribute("FileVersion");
            attr.InnerText = "0";
            xelRoot.Attributes.Append(attr);

            attr = xmlDoc.CreateAttribute("FileType");
            attr.InnerText = "LinePatterns";
            xelRoot.Attributes.Append(attr);

            XmlElement xelSchemes = xmlDoc.CreateElement("LinePatters");

            foreach (MapWinGIS.LinePattern pattern in _patterns)
            {
                XmlElement xelPattern = xmlDoc.CreateElement("Pattern");
                xelPattern.InnerText = pattern.Serialize();
                xelSchemes.AppendChild(xelPattern);
            }
            xelRoot.AppendChild(xelSchemes);

            string filename = this.get_FileName();
            string path = Path.GetDirectoryName(filename);
            if (!Directory.Exists(path))
            {
                try
                {
                    Directory.CreateDirectory(path);
                }
                catch (Exception ex)
                {
                    Globals.MessageBoxError("Failed to create directory: " + path + Environment.NewLine + ex.Message);
                    return false;
                }
            }

            if (Directory.Exists(path))
            {
                try
                {
                    xmlDoc.Save(filename);
                }
                catch (Exception ex)
                {
                    Globals.MessageBoxError("Failed to save line patterns: " + path + Environment.NewLine + ex.Message);
                    return false;
                }
            }

            // TEMP
            _patterns.Clear();
            this.ItemCount = _patterns.Count;

            return true;
        }
        
        /// <summary>
        /// Loads all the icons form the current path
        /// </summary>
        /// <param name="path"></param>
        public void LoadFromXML()
        {
            _patterns.Clear();

            XmlDocument xmlDoc = new XmlDocument();
            string filename = this.get_FileName();

            // reading from the file
            if (System.IO.File.Exists(filename))
            {
                xmlDoc.Load(filename);

                XmlElement xelSchemes = xmlDoc.DocumentElement["LinePatters"];
                if (xelSchemes != null)
                {
                    foreach (XmlNode nodePatter in xelSchemes.ChildNodes)
                    {
                        MapWinGIS.LinePattern pattern = new MapWinGIS.LinePattern();
                        pattern.Deserialize(nodePatter.InnerText);
                        if (pattern.Count > 0)
                        {
                            _patterns.Add(pattern);
                        }
                    }
                }
            }
            
            // load some default ones if none were loaded
            if (_patterns.Count == 0)
            {
                this.AddDefaultPatterns();
            }

            this.ItemCount = _patterns.Count;

            if (this.ItemCount > 0)
                this.SelectedIndex = 0;
        }

        /// <summary>
        /// Retuns the name of file to serialize patterns in
        /// </summary>
        private string get_FileName()
        {
            string filename = Path.GetDirectoryName(System.Reflection.Assembly.GetExecutingAssembly().Location);
            return Directory.GetParent(filename).FullName + "\\Styles\\linepatterns.xml";
        }
        #endregion

        #region Drawing
        /// <summary>
        /// Draws the next icon from the list
        /// </summary>
        void Control_OnDrawItem(Graphics graphics, RectangleF rect, int itemIndex, bool selected)
        {
            IntPtr ptr = graphics.GetHdc();
            MapWinGIS.LinePattern pattern = _patterns[itemIndex];
            pattern.Draw(ptr, rect.X + 1.0f, rect.Y + 1.0f, (int)rect.Width - 2, (int)rect.Height - 2, Colors.ColorToUInteger(this.BackColor));
            graphics.ReleaseHdc();
        }
        #endregion
    }
}
