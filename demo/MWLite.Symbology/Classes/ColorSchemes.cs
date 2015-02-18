// ********************************************************************************************************
/// ********************************************************************************************************
// <copyright file="MWLite.Symbology.cs" company="MapWindow.org">
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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.IO;
using System.Reflection;
using System.Xml;
using MapWinGIS;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Classes
{
    /// <summary>
    /// Color scheme lists used in the plug-in
    /// </summary>
    public enum ColorSchemeType
    {
        Layer = 0,
        Charts = 1,
    }

    /// <summary>
    /// This class is wrapper for a list of color schemes associated with layer, labels or charts
    /// </summary>
    internal class ColorSchemes
    {
        // The list of color schemes used in image combos
        public List<ColorBlend> List;
        
        // The type of color schemes        
        private ColorSchemeType _type;

        /// <summary>
        /// Returns the type of the color scheme
        /// </summary>
        public ColorSchemeType Type
        {
            get { return _type; }
        }

        /// <summary>
        /// Creates a new instance of the ColorSchemes class
        /// </summary>
        /// <param name="type">The type of color scheme to create</param>
        public ColorSchemes(ColorSchemeType type)
        {
            _type = type;
            List = new List<ColorBlend>();
        }

        /// <summary>
        /// Returns the path to the specified style file, in case the file doesn't exist - creates it.
        /// </summary>
        private string GetFilename()
        {
            string filename = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location);
            filename = Directory.GetParent(filename).FullName;
            if (_type == ColorSchemeType.Layer)
            {
                filename += "\\Styles\\colorschemes.xml";
            }
            else if (_type == ColorSchemeType.Charts)
            {
                filename += "\\Styles\\chartcolors.xml";
            }
            return filename;
        }

        /// <summary>
        /// Saves the list of color schemes to XML file (currently C:\colorschemes.xml is used)
        /// </summary>
        /// <param name="schemes"></param>
        public void Serialize2Xml()
        {
            XmlDocument xmlDoc = new XmlDocument();
            xmlDoc.LoadXml("<MapWindow version= '" + "'></MapWindow>");     // TODO: add version
            XmlElement xelRoot = xmlDoc.DocumentElement;

            XmlElement xelSchemes = xmlDoc.CreateElement("ColorSchemes");

            // the first scheme must not be saved
            int j = _type == ColorSchemeType.Layer ? 1 : 0;
            
            for (; j < List.Count; j++)
            {
                ColorBlend scheme = (ColorBlend)List[j];
                XmlElement xelScheme = xmlDoc.CreateElement("ColorScheme");
                for (int i = 0; i < scheme.Colors.Length; i++)
                {
                    XmlElement xel = xmlDoc.CreateElement("Break");
                    XmlAttribute attrColor = xmlDoc.CreateAttribute("Color");
                    XmlAttribute attrValue = xmlDoc.CreateAttribute("Value");

                    attrColor.InnerText = scheme.Colors[i].ToArgb().ToString();
                    attrValue.InnerText = scheme.Positions[i].ToString(System.Globalization.CultureInfo.InvariantCulture);

                    xel.Attributes.Append(attrColor);
                    xel.Attributes.Append(attrValue);
                    xelScheme.AppendChild(xel);
                }
                xelSchemes.AppendChild(xelScheme);
            }
            xelRoot.AppendChild(xelSchemes);

            string filename = this.GetFilename();
            string path = Path.GetDirectoryName(filename);
            if (!Directory.Exists(path))
            {
                try
                {
                    Directory.CreateDirectory(path);
                }
                catch(Exception ex)
                {
                    Globals.MessageBoxError("Failed to create directory: " + path + Environment.NewLine + ex.Message);
                    return;
                }
            }

            if (Directory.Exists(path))
            {
                try
                {
                    xmlDoc.Save(filename);
                }
                catch(Exception ex)
                {
                    Globals.MessageBoxError("Failed to save color schemes: " + path + Environment.NewLine + ex.Message);
                }
            }
        }

        /// <summary>
        /// Reads the list of color schemes
        /// </summary>
        public void ReadFromXML()
        {
            this.List.Clear();

            XmlDocument xmlDoc = new XmlDocument();
            string filename = this.GetFilename();

            // reading from the file
            if (System.IO.File.Exists(filename))
            {
                xmlDoc.Load(filename);
                this.LoadXML(xmlDoc);
            }

            // creating default ones
            if (this.List.Count == 0)
            {
                ColorBlend blend = null;
                if (_type == ColorSchemeType.Layer)
                {
                    // dummy single color blend must be always the first for shapefile
                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.White; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.White; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.LightBlue; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Orange; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.Yellow; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Orange; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    for (int i = 0; i < 2; i++)
                    {
                        MapWinGIS.ColorScheme sch = new ColorScheme();
                        if (i == 0) sch.SetColors4(PredefinedColorScheme.FallLeaves);
                        if (i == 1) sch.SetColors4(PredefinedColorScheme.DeadSea);
                        blend = ColorSchemes.ColorScheme2ColorBlend(sch);
                        this.List.Add(blend);
                    }

                    // adding to 2 color schemes, as there can be none in the list
                    blend = new ColorBlend(7);
                    blend.Colors[0] = Color.Red; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Orange; blend.Positions[1] = 1.0f / 6.0f;
                    blend.Colors[2] = Color.Yellow; blend.Positions[2] = 2.0f / 6.0f;
                    blend.Colors[3] = Color.LightGreen; blend.Positions[3] = 3.0f / 6.0f;
                    blend.Colors[4] = Color.LightBlue; blend.Positions[4] = 4.0f / 6.0f;
                    blend.Colors[5] = Color.Blue; blend.Positions[5] = 5.0f / 6.0f;
                    blend.Colors[6] = Color.BlueViolet; blend.Positions[6] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.LightGray; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Gray; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.Pink; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.LightYellow; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);
                }
                else if (_type == ColorSchemeType.Charts)
                {
                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.Yellow; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Orange; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.LightBlue; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Pink; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);

                    blend = new ColorBlend(2);
                    blend.Colors[0] = Color.LightGreen; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Yellow; blend.Positions[1] = 1.0f;
                    this.List.Add(blend);
                }
            }
        }

        /// <summary>
        /// Initiliazes color schemes from XML document
        /// </summary>
        /// <returns></returns>
        public bool LoadXML(XmlDocument xmlDoc)
        {
            XmlElement xelSchemes = xmlDoc.DocumentElement["ColorSchemes"];
            if (xelSchemes != null)
            {
                // dummy single color blend 
                if (_type == ColorSchemeType.Layer)
                {
                    ColorBlend blend = new ColorBlend(2);
                    blend.Colors[0] = Color.Black; blend.Positions[0] = 0.0f;
                    blend.Colors[1] = Color.Black; blend.Positions[1] = 1.0f;
                    List.Add(blend);
                }

                foreach (XmlNode nodeScheme in xelSchemes.ChildNodes)
                {
                    int i = 0;
                    ColorBlend scheme = new ColorBlend(nodeScheme.ChildNodes.Count);
                    foreach (XmlNode nodeBreak in nodeScheme.ChildNodes)
                    {
                        XmlAttribute attrValue = nodeBreak.Attributes["Value"];
                        XmlAttribute attrColor = nodeBreak.Attributes["Color"];
                        if (attrColor != null && attrValue != null)
                        {
                            double valDouble; int valInt;
                            bool parsed = false;
                            string s = attrValue.InnerText;

                            parsed = double.TryParse(s, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out valDouble);
                            if (!parsed)
                            {
                                s = s.Replace(',', '.');
                                parsed = double.TryParse(s, System.Globalization.NumberStyles.Float, System.Globalization.CultureInfo.InvariantCulture, out valDouble);
                            }

                            if (parsed && int.TryParse(attrColor.InnerText, out valInt))
                            {
                                scheme.Positions[i] = (float)valDouble;
                                scheme.Colors[i] = Color.FromArgb(valInt);
                                i++;
                            }
                        }
                    }
                    if (scheme.Colors.Length == i)
                    {
                        this.List.Add(scheme);
                    }
                }
            }
            return true;
        }

        /// <summary>
        /// Converts color blend to MapWinGis color scheme
        /// </summary>
        public static MapWinGIS.ColorScheme ColorBlend2ColorScheme(ColorBlend blend)
        {
            if (blend == null)
            {
                return null;
            }

            if (blend.Positions.Length == 0 || (blend.Positions.Length != blend.Colors.Length))
            {
                return null;
            }

            MapWinGIS.ColorScheme scheme = new MapWinGIS.ColorScheme();
            for (int i = 0; i < blend.Positions.Length; i++)
            {
                scheme.AddBreak((double)blend.Positions[i], Colors.ColorToUInteger(blend.Colors[i]));
            }
            return scheme;
        }

        /// <summary>
        /// Converts MapWinGis color scheme to color blend
        /// </summary>
        public static ColorBlend ColorScheme2ColorBlend(MapWinGIS.ColorScheme scheme)
        {
            if (scheme == null)
            {
                return null;
            }

            if (scheme.NumBreaks == 0)
                return null;

            ColorBlend blend = new ColorBlend(scheme.NumBreaks);
            for (int i = 0; i < scheme.NumBreaks; i++)
            {
                blend.Positions[i] = (float)scheme.get_BreakValue(i);
                uint color = scheme.get_BreakColor(i);
                blend.Colors[i] = Colors.UintToColor(color);
            }
            return blend;
        }

        /// <summary>
        /// Sets dummy color scheme based on the shapefile default color. The color schemes is the first in the list.
        /// </summary>
        public void SetDefaultColorScheme(Shapefile sf)
        {
            // settings dummy color scheme
            ColorBlend blend = null;
            if (List.Count <= 0)
            {
                blend = new ColorBlend(2);
                blend.Colors[0] = Color.Black; blend.Positions[0] = 0.0f;
                blend.Colors[1] = Color.Black; blend.Positions[1] = 1.0f;
                List.Add(blend);
            }

            blend = (ColorBlend)List[0];
            ShpfileType shpType = Globals.ShapefileType2D(sf.ShapefileType);
            if (shpType == ShpfileType.SHP_POINT || shpType == ShpfileType.SHP_POLYGON || shpType == ShpfileType.SHP_MULTIPOINT)
            {
                blend.Colors[0] = Colors.UintToColor(sf.DefaultDrawingOptions.FillColor);
                blend.Colors[1] = Colors.UintToColor(sf.DefaultDrawingOptions.FillColor);
            }
            else if (shpType == ShpfileType.SHP_POLYLINE)
            {
                blend.Colors[0] = Colors.UintToColor(sf.DefaultDrawingOptions.LineColor);
                blend.Colors[1] = Colors.UintToColor(sf.DefaultDrawingOptions.LineColor);
            }
        }

        /// <summary>
        /// Sets first color schemes which is a single color one
        /// </summary>
        /// <param name="color"></param>
        public void SetFirstColorScheme(uint color)
        {
            SetFirstColorScheme(Colors.UintToColor(color));
        }

        public void SetFirstColorScheme(Color color)
        {
            // settings dummy color scheme
            ColorBlend blend = null;
            if (List.Count <= 0)
            {
                blend = new ColorBlend(2);
                blend.Colors[0] = Color.Black; blend.Positions[0] = 0.0f;
                blend.Colors[1] = Color.Black; blend.Positions[1] = 1.0f;
                List.Add(blend);
            }

            blend = (ColorBlend)List[0];

            blend.Colors[0] = color;
            blend.Colors[1] = color;
        }
    }
}
