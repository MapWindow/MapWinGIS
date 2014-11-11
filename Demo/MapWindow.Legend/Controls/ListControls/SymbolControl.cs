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
using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Controls.ListControls
{
    [ToolboxItem(true)]
    internal partial class SymbolControl : ListControl
    {
        List<MapWinGIS.ShapeDrawingOptions> _icons = new List<MapWinGIS.ShapeDrawingOptions>();
        
        /// <summary>
        ///  Creates a new instance of the PointSymbolControl.
        /// </summary>
        public SymbolControl()
        {
            InitializeComponent();
            this.ItemCount = 17;
            this.CellWidth = 24;
            this.CellHeight = 24;

            for (int i = 0; i < this.ItemCount; i++)
            {
                MapWinGIS.ShapeDrawingOptions sdo = new MapWinGIS.ShapeDrawingOptions();
                sdo.SetDefaultPointSymbol((MapWinGIS.tkDefaultPointSymbol)i);
                sdo.PointSize = 0.8f * this.CellWidth;
                sdo.FillColor = Colors.ColorToUInteger(Color.Orange);
                _icons.Add(sdo);
            }
            this.OnDrawItem += new OnDrawItemDelegate(PointSymbolControl_OnDrawItem);
        }

        public override Color ForeColor
        {
            get
            {
                return base.ForeColor;
            }
            set
            {
                base.ForeColor = value;
                for (int i = 0; i < _icons.Count; i++)
                {
                    _icons[i].FillColor = Colors.ColorToUInteger(value);
                }
                base.Redraw();
            }
        }

        /// <summary>
        /// Draws an item from the list
        /// </summary>
        void PointSymbolControl_OnDrawItem(System.Drawing.Graphics graphics, System.Drawing.RectangleF rect, int itemIndex, bool selected)
        {
            IntPtr ptr = graphics.GetHdc();
            MapWinGIS.ShapeDrawingOptions sdo = _icons[itemIndex];
            sdo.DrawPoint(ptr, rect.X + 1.0f, rect.Y +1.0f, (int)rect.Width -2, (int)rect.Height -2, Colors.ColorToUInteger(this.BackColor));
            graphics.ReleaseHdc();
       }
    }
}
