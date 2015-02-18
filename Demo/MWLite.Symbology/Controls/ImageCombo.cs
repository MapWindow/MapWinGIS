// ********************************************************************************************************
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
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MapWinGIS;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Controls
{
    /// <summary>
    ///  Available styles for image combo. All apart from common have fixed set of icons.
    /// </summary>
    public enum ImageComboStyle
    {
        Common = 0,
        LineStyle = 1,
        LineWidth = 2,
        LinearGradient = 3,
        FrameType = 4,
        PointShape = 5,
        HatchStyle = 6,
        ColorSchemeGraduated = 7,
        ColorSchemeRandom = 8,
        HatchStyleWithNone = 9,
    }

    /// <summary>
    /// Image combo to store the icons for symbology plug-in
    /// </summary>
    internal class ImageCombo : ComboBox
    {
        #region Member variables
        // The list of icons 
        private ImageList m_list = new ImageList();
        
        // The style combobox (particular list of images to store)
        private ImageComboStyle m_style = ImageComboStyle.Common;
        
        // The first color to fill contents
        private Color _color1 = Color.Gray;
        
        // The second color to fill contents
        private Color _color2 = Color.Honeydew;
        
        // The color for contents outlines
        private Color _outlineColor = Color.Black;
        
        // Part of the image free from drawing by X axis
        private int m_paddX = 1;
        
        // Part of the image free free from drawing by Y axis
        private int m_paddY = 1;
        
        // Number of items for a given styp of combo
        private int _itemCount = 0;
        
        // The list of color schemes to display
        private ColorSchemes _colorSchemes;
        #endregion

        #region Contructors
        /// <summary>
        /// Constructor. Common type of combo will be used
        /// </summary>
        public ImageCombo():this(ImageComboStyle.Common){}
        
        /// <summary>
        /// Constructor. Sets the style of combo.
        /// </summary>
        public ImageCombo(ImageComboStyle style) : this(style, Color.Gray, Color.Gray) { }
        
        /// <summary>
        /// Constructor. Sets the style of combo and fill color.
        /// </summary>
        public ImageCombo(ImageComboStyle style, Color color1):this(style, color1, color1) { }
        
        /// <summary>
        /// Constructor. Sets the style of combo and 2 both fill colors.
        /// </summary>
        public ImageCombo(ImageComboStyle style, Color color1, Color color2)
        {
            m_list.ColorDepth = ColorDepth.Depth24Bit;
            this.OutlineColor = Color.Black;
            this.DrawMode = DrawMode.OwnerDrawFixed;
            this.DropDownStyle = ComboBoxStyle.DropDownList;
            m_style = style;
            _color1 = color1;
            _color2 = color2;
            EnabledChanged += (s, e) => RefreshImageList();
        }
        #endregion

        #region Properties
        /// <summary>
        /// The main  color to fill contents
        /// </summary>
        public Color Color1
        {
            get { return _color1; }
            set 
            { 
                _color1 = value;
                RefreshImageList();
                this.Invalidate();
            }
        }

        /// <summary>
        /// The second color to fill the contents
        /// </summary>
        public Color Color2
        {
            get { return _color2; }
            set { _color2 = value; }
        }

        /// <summary>
        /// The color to draw outline of the content
        /// </summary>
        public Color OutlineColor
        {
            get { return _outlineColor; }
            set { _outlineColor = value; }
        }

        /// <summary>
        ///  Setting the number of items for a given combo style
        /// </summary>
        public ImageComboStyle ComboStyle
        {
            get { return m_style; }
            set 
            {
                m_style = value; 
                GenerateItems(value);
            }
        }

        /// <summary>
        ///  Gets or sets bound ImageList
        /// </summary>
        public ImageList ImageList
        {
            get { return m_list; }
            set { m_list = value; }
        }

        /// <summary>
        /// Sets or gets the list of color schemes
        /// </summary>
        public ColorSchemes ColorSchemes
        {
            get { return _colorSchemes; }
            set
            {
                // preserving index
                int index = this.SelectedIndex;

                _colorSchemes = value;
                this.GenerateItems(this.ComboStyle);

                // restoring index
                if (index < this.Items.Count)
                {
                    this.SelectedIndex = index;
                }
                else if (this.Items.Count > 0)
                {
                    this.SelectedIndex = 0;
                }
            }
        }
        #endregion

        #region Item generation
        /// <summary>
        /// Generates items for the given combo style
        /// </summary>
        private void GenerateItems(ImageComboStyle style)
        {
            this.Items.Clear();

            // choosing number of items
            switch (style)
            {
                case ImageComboStyle.FrameType:             _itemCount = 3; break;
                case ImageComboStyle.LinearGradient:        _itemCount = 4; break;
                case ImageComboStyle.LineStyle:             _itemCount = 5; break;
                case ImageComboStyle.LineWidth:             _itemCount = 10; break;
                case ImageComboStyle.PointShape:            _itemCount = 6; break;
                case ImageComboStyle.HatchStyle:            _itemCount = 53; break;
                case ImageComboStyle.HatchStyleWithNone:    _itemCount = 54; break;
                case ImageComboStyle.ColorSchemeGraduated:
                case ImageComboStyle.ColorSchemeRandom:
                                                            {
                                                                if (_colorSchemes != null)
                                                                    _itemCount = _colorSchemes.List.Count;
                                                                else
                                                                    _itemCount = 0;
                                                                break;
                                                            }
            }

            // adding items
            string str = string.Empty;
            for (int i = 0; i < _itemCount; i++)
            {
                switch (style)
                {
                    case ImageComboStyle.FrameType:
                    {
                        if (i == 0) str = "ftRectangle";
                        else if (i == 1) str = "ftRounded rectangle";
                        else if (i == 2) str = "ftPointed rectangle";
                        break;
                    }
                    case ImageComboStyle.LinearGradient:
                    {
                        // TODO: temporary
                        //str = ((tkLinearGradientMode)i).ToString();
                        str = "  Style " + (i + 1).ToString();
                        break;
                    }
                    case ImageComboStyle.LineStyle:
                    {
                        str = "ls";
                        break;
                    }
                    case ImageComboStyle.LineWidth:
                    {
                        str = "wd";
                        break;
                    }
                    case ImageComboStyle.PointShape:
                    {
                        // TODO: temporary
                        //str = ((tkPointShapeType)i).ToString();
                        //str = str.Substring(5, str.Length - 5);
                        
                        str = "  Style " + (i + 1).ToString();
                        break;
                    }
                    case ImageComboStyle.HatchStyle:
                    {
                        // TODO: temporary
                        //str = ((tkGDIPlusHatchStyle)i).ToString();
                        str = "  Style " + (i + 1).ToString();
                        break;
                    }
                    case ImageComboStyle.HatchStyleWithNone:
                    {
                        // TODO: temporary
                        //if (i == 0)
                        //{
                        //    str = "None";
                        //}
                        //else
                        //{
                        //    str = ((tkGDIPlusHatchStyle)i - 1).ToString();
                        //}
                        str = "  Style " + (i + 1).ToString();
                        break;
                    }
                    case ImageComboStyle.ColorSchemeGraduated:
                    case ImageComboStyle.ColorSchemeRandom:
                    {
                        str = "cl";
                        break;
                    }
                }

                // getting rid of prefix
                str = str.Substring(2, str.Length - 2); 
                
                this.Items.Add(new ImageComboItem(str, i));
            }

            // adds images
            RefreshImageList();
        }
        #endregion

        #region Generation of icons
        /// <summary>
        /// Fills the image list with icons according to the selected colors
        /// </summary>
        public void RefreshImageList()
        {
            if (m_style == ImageComboStyle.Common) return;

            m_list.Images.Clear();

            int width;
            if (m_style == ImageComboStyle.PointShape)
            {
                width = 20;
            }
            else if (m_style == ImageComboStyle.ColorSchemeGraduated || m_style == ImageComboStyle.ColorSchemeRandom)
            {
                width = this.Width - 24;
            }
            else
            {
                width = 64;
            }

            Size sz = new Size(width, 16);
            m_list.ImageSize = sz;

            int _imgHeight = m_list.ImageSize.Height;
            int _imgWidth = m_list.ImageSize.Width;

            Rectangle rect = new Rectangle(m_paddX, m_paddY, _imgWidth - 1 - m_paddX * 2, _imgHeight - 1 - m_paddY * 2);

            Color foreColor = this.Enabled ? Color.Black : Color.Gray;

            for (int i = 0; i < _itemCount; i++)
            {
                Bitmap img = new Bitmap(_imgWidth, _imgHeight, System.Drawing.Imaging.PixelFormat.Format32bppArgb);
                Graphics g = Graphics.FromImage(img);
                
                switch (m_style)
                {
                    // frame type combo
                    case ImageComboStyle.FrameType:
                    {
                        if (i == 0 )
                        {
                            g.FillRectangle(new SolidBrush(_color1), rect);
                            g.DrawRectangle(new Pen(foreColor), rect);
                        }
                        else if (i == 1)
                        {

                            g.FillEllipse(new SolidBrush(_color1), rect);
                            g.DrawEllipse(new Pen(foreColor), rect);
                        }
                        else if  (i == 2)
                        {
                            float left = rect.X;
                            float right = rect.X + rect.Width;
                            float top = rect.Y;
                            float bottom = rect.Y + rect.Height;

                            GraphicsPath path = new GraphicsPath();
                            path.StartFigure();
                            path.AddLine(left + (rect.Height / 4), top, right - (rect.Height / 4), top);

                            path.AddLine(right - (rect.Height / 4), top, right, (top + bottom) / 2);
                            path.AddLine(right, (top + bottom) / 2, right - (rect.Height / 4), bottom);

                            path.AddLine(right - (rect.Height / 4), bottom, left + (rect.Height / 4), bottom);

                            path.AddLine(left + (rect.Height / 4), bottom, left, (top + bottom) / 2);
                            path.AddLine(left, (top + bottom) / 2, left + (rect.Height / 4), top);

                            path.CloseFigure();
                            g.FillPath(new SolidBrush(_color1), path);
                            g.DrawPath(new Pen(foreColor), path);
                            path.Dispose();
                            break;
                        }
                        break;
                    }

                    // linear gradient combo
                    case ImageComboStyle.LinearGradient:
                    {
                        if ((tkLinearGradientMode)i == tkLinearGradientMode.gmNone)
                        {
                            g.FillRectangle(new SolidBrush(_color1), rect);
                            g.DrawRectangle(new Pen(_outlineColor), rect);
                        }
                        else
                        {
                            LinearGradientBrush lgb = new LinearGradientBrush(rect, _color1, _color2, (LinearGradientMode)i);
                            g.FillRectangle(lgb, rect);
                            g.DrawRectangle(new Pen(_outlineColor), rect);
                            lgb.Dispose();
                        }
                        break;
                    }

                    //  line style combo
                    case ImageComboStyle.LineStyle: 
                    {
                        Pen pen = new Pen(_outlineColor);
                        pen.DashStyle = (DashStyle)i;
                        pen.Width = 2;
                        g.DrawLine(pen, new System.Drawing.Point(rect.Left, rect.Top + rect.Height / 2),
                                                            new System.Drawing.Point(rect.Right, rect.Top + rect.Height / 2));
                        break;
                    }

                    //  line width combo
                    case ImageComboStyle.LineWidth: 
                    {
                        Pen pen = new Pen(_outlineColor);
                        pen.Width = i + 1;
                        g.DrawLine(pen, new System.Drawing.Point(rect.Left, rect.Top + rect.Height / 2),
                                                            new System.Drawing.Point(rect.Right, rect.Top + rect.Height / 2));
                        break;
                    }
                    case ImageComboStyle.PointShape:
                    {
                        ShapeDrawingOptions sdo = new ShapeDrawingOptions();
                        sdo.FillColor = Colors.ColorToUInteger(this._color1);
                        sdo.LineColor = Colors.ColorToUInteger(this._outlineColor);
                        sdo.PointShape = (tkPointShapeType)i;
                        sdo.PointType = tkPointSymbolType.ptSymbolStandard;
                        sdo.PointSize = 12;
                        if (sdo.PointShape == tkPointShapeType.ptShapeStar)
                        {
                            sdo.PointSidesCount = 5;
                            sdo.PointRotation = 17;
                            sdo.PointSize = 14;
                        }
                        else if (sdo.PointShape == tkPointShapeType.ptShapeArrow)
                        {
                            sdo.PointSize = 14;
                            sdo.PointRotation = 0;
                        }
                        else
                        {
                            sdo.PointSidesCount = 4;
                            sdo.PointRotation = 0;
                            sdo.PointSize = 12;
                        }
                        
                        IntPtr ptr = g.GetHdc();
                        sdo.DrawPoint(ptr, 0.0f, 0.0f, _imgWidth, _imgHeight, Colors.ColorToUInteger(this.BackColor));
                        g.ReleaseHdc(ptr);
                        break;
                    }
                    case ImageComboStyle.HatchStyle:
                    {
                        HatchBrush br = new HatchBrush((HatchStyle)i, _color1, Color.Transparent);
                        g.FillRectangle(br, rect);
                        g.DrawRectangle(new Pen(_outlineColor), rect);
                        br.Dispose();
                        break;
                    }
                    case ImageComboStyle.HatchStyleWithNone:
                    {
                        if (i == 0)
                        {
                            g.FillRectangle(new SolidBrush(_color1), rect);
                            g.DrawRectangle(new Pen(_outlineColor), rect);
                        }
                        else
                        {
                            HatchBrush br = new HatchBrush((HatchStyle)(i - 1), _color1, Color.Transparent);
                            g.FillRectangle(br, rect);
                            g.DrawRectangle(new Pen(_outlineColor), rect);
                            br.Dispose();
                        }
                        break;
                    }
                    case ImageComboStyle.ColorSchemeGraduated:
                    {
                        if (_colorSchemes != null)
                        {
                            
                            ColorBlend blend = (_colorSchemes.List[i] as ColorBlend);
                            if (blend != null)
                            {
                                LinearGradientBrush lgb = new LinearGradientBrush(rect, Color.White, Color.White, 0.0f);
                                lgb.InterpolationColors = blend;
                                g.FillRectangle( lgb, rect );
                                g.DrawRectangle( new Pen(_outlineColor), rect);
                                lgb.Dispose();
                                break;
                            }
                        }
                        break;
                    }
                    case ImageComboStyle.ColorSchemeRandom:
                    {
                        if (_colorSchemes != null)
                        {
                            ColorBlend blend = (_colorSchemes.List[i] as ColorBlend);
                            if (blend != null)
                            {
                                MapWinGIS.ColorScheme scheme = ColorSchemes.ColorBlend2ColorScheme(blend);
                                if (scheme != null)
                                {
                                    int n = 0;
                                    Random rnd = new Random();
                                    while (n < _imgWidth)
                                    {
                                        Color clr = Colors.UintToColor(scheme.get_RandomColor(rnd.NextDouble()));
                                        SolidBrush brush = new SolidBrush(clr);
                                        Rectangle rectTemp = new Rectangle(rect.X + n, rect.Y, 8, rect.Height);
                                        g.FillRectangle(brush, rectTemp);
                                        g.DrawRectangle(new Pen(_outlineColor), rectTemp);
                                        brush.Dispose();
                                        n += 8;
                                    }
                                }
                            }
                        }
                        break;
                    }
                    default: return;
                }
                // adding an image
                m_list.Images.Add(img);
            }
        }
        #endregion

        #region Item drawing
        /// <summary>
        /// Drawing procedure of a single item of list
        /// </summary>
        protected override void OnDrawItem(DrawItemEventArgs e)
        {
            // we don't want to draw ites when combo is disabled
            if ((this.ComboStyle == ImageComboStyle.ColorSchemeGraduated ||
                this.ComboStyle == ImageComboStyle.ColorSchemeRandom) && !this.Enabled)
            {
                return;
            }
            
            // check if it is an item from the Items collection
            if (e.Index < 0)
            {
                // not an item, draw the text (indented)
                e.Graphics.DrawString(this.Text, e.Font, new SolidBrush(e.ForeColor), e.Bounds.Left + m_list.ImageSize.Width, e.Bounds.Top);
            }
            else
            {
                // check if item is an ImageComboItem
                if (this.Items[e.Index].GetType() == typeof(ImageComboItem))
                {
                    // get item to draw
                    ImageComboItem item = (ImageComboItem)this.Items[e.Index];

                    if (this.Enabled)
                    {
                        e.Graphics.FillRectangle(new SolidBrush(this.BackColor), e.Bounds);
                    }
                    else
                    {
                        e.DrawBackground();
                    }

                    Color textColor = this.Enabled ? Color.Black : Color.Gray;
                    Color forecolor = item.ForeColor;
                    Font font = item.Mark ? new Font(e.Font, FontStyle.Bold) : e.Font;

                    if (item.ImageIndex != -1 && item.ImageIndex < this.ImageList.Images.Count)
                    {
                        // draw image
                        this.ImageList.Draw(e.Graphics, e.Bounds.Left, e.Bounds.Top, item.ImageIndex);
                        // draw text (indented)
                        e.Graphics.DrawString(item.Text, font, new SolidBrush(textColor), e.Bounds.Left + m_list.ImageSize.Width + 3 /*offset*/, e.Bounds.Top);
                    }
                    else
                    {
                        e.Graphics.DrawString(item.Text, font, new SolidBrush(forecolor), e.Bounds.Left + m_list.ImageSize.Width, e.Bounds.Top);
                    }

                    if (((e.State & DrawItemState.Selected) != 0) && ((e.State & DrawItemState.ComboBoxEdit) == 0))
                    {
                        Pen pen = new Pen(textColor);
                        pen.DashStyle = DashStyle.Dot;
                        e.Graphics.DrawRectangle(pen, 0, e.Bounds.Top, e.Bounds.Width - 1, e.Bounds.Height - 1);
                    }
                }
                else
                {
                    // it is not an ImageComboItem, draw it
                    e.Graphics.DrawString(this.Items[e.Index].ToString(), e.Font, new SolidBrush(e.ForeColor), e.Bounds.Left + m_list.ImageSize.Width, e.Bounds.Top);
                }
            }
        }
        #endregion
    }

    #region Image combo item
    /// <summary>
    /// An item of ImageComboBox
    /// </summary>
    public class ImageComboItem : object
    {
        // forecolor: transparent = inherit
        private Color forecolor = Color.FromKnownColor(KnownColor.Transparent);
        private bool mark = false;
        private int imageindex = -1;
        private object tag = null;
        private string text = null;        
        
        /// <summary>
        /// Constructor
        /// </summary>
         public ImageComboItem()
        {
        }

        public ImageComboItem(string Text) 
        {
            text = Text;    
        }

        public ImageComboItem(string Text, int ImageIndex)
        {
            text = Text;
            imageindex = ImageIndex;
        }

        public ImageComboItem(string Text, int ImageIndex, bool Mark)
        {
            text = Text;
            imageindex = ImageIndex;
            mark = Mark;
        }

        public ImageComboItem(string Text, int ImageIndex, 
            bool Mark, Color ForeColor)
        {
            text = Text;
            imageindex = ImageIndex;
            mark = Mark;
            forecolor = ForeColor;
        }

        public ImageComboItem(string Text, int ImageIndex, 
               bool Mark, Color ForeColor, object Tag)
        {
            text = Text;
            imageindex = ImageIndex;
            mark = Mark;
            forecolor = ForeColor;
            tag = Tag;
        }

        /// <summary>
        /// Gets or sets fore color
        /// </summary>
        public Color ForeColor 
        {
            get {return forecolor;}
            set {forecolor = value;}
        }

        /// <summary>
        /// Index of image for the item
        /// </summary>
        public int ImageIndex 
        {
            get {return imageindex;}
            set {imageindex = value;}
        }

        /// <summary>
        /// Marks the itme in bold font
        /// </summary>
         public bool Mark
        {
            get{return mark;}
            set{mark = value;}
        }

        /// <summary>
        /// Gets a sets an object to describe an itme
        /// </summary>
        public object Tag
        {
            get {return tag;}
            set {tag = value;}
        }

        /// <summary>
        /// Gets or sets the text of the item
        /// </summary>
         public string Text 
        {
            get {return text;}
            set {text = value;}
        }
        
        /// <summary>
        ///  Returns text property of the item
        /// </summary>
        public override string ToString() 
        {
            return text;
        }
    }
    #endregion
}