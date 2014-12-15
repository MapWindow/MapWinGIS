//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License");
//you may not use this file except in compliance with the License. You may obtain a copy of the License at
//http://www.mozilla.org/MPL/
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and
//limitations under the License.
//
//The Original Code is MapWindow Open Source.
//
//The Initial Developer of this version of the Original Code is Daniel P. Ames using portions created by
//Utah State University and the Idaho National Engineering and Environmental Lab that were released as
//public domain in March 2004.
//
//Contributor(s): (Open source contributors should list themselves and their modifications here).
//
//********************************************************************************************************

using System;
using System.Collections;
using System.Collections.Generic;
using System.Drawing;
using MapWindow.Legend.Classes;
using MapWinGIS;

namespace MapWindow.Legend.Controls.Legend
{

    /// <summary>
    /// One layer within the legend
    /// </summary>
    public class Layer
    {
        #region Declarations
        public object Tag = null;
        
        // symbology settings
        private SymbologySettings m_symbologySettings = new SymbologySettings();
        private Size textSize = new Size(0, 0);
        private bool m_Expanded;
        private Legend m_Legend;
        private int m_height;
        private object m_Icon;
        private eLayerType m_LayerType;
        private bool m_UseDynamicVisibility;
        internal List<LayerElement> Elements;  // size and positions of elements
        internal bool m_smallIconWasDrawn;

        public ShapefileBinding ShapefileBinding;

        /// <summary>
        /// Color Scheme information for this layer
        /// </summary>
        protected internal ArrayList ColorLegend;

        /// <summary>
        /// Top of this Layer
        /// </summary>
        protected internal int Top;

        /// <summary>
        /// Allows you to force the expansion box option to be shown, e.g. you're planning to use ExpansionBoxCustomRenderFunction.
        /// </summary>
        public bool ExpansionBoxForceAllowed = false;

        /// <summary>
        /// Allows you to render the expanded region of a layer yourself. Useful with ExpansionBoxForceAllowed=true.
        /// If you use this, you must also set ExpansionBoxCustomHeightFunction.
        /// </summary>
        public ExpansionBoxCustomRenderer ExpansionBoxCustomRenderFunction = null;

        /// <summary>
        /// Tells the legend how high your custom rendered legend will be, so that it can
        /// arrange items around it.
        /// </summary>
        public ExpansionBoxCustomHeight ExpansionBoxCustomHeightFunction = null;

        /// <summary>
        /// Handle to this Layer (within the MapWinGIS.Map)
        /// </summary>
        protected internal int m_Handle;//handle of the associate layer from the Map Object

        /// <summary>
        /// Stores custom objects associated with layer
        /// </summary>
        public Hashtable m_CustomObjects = null;

        /// <summary>
        /// Returns custom object for specified key
        /// </summary>
        public object GetCustomObject(string key)
        {
            return m_CustomObjects[key];
        }

        /// <summary>
        /// Sets custom object associated with layer
        /// </summary>
        public void SetCustomObject(object obj, string key)
        {
            m_CustomObjects[key] = obj;
        }

        /// <summary>
        /// If an image layer, this tells us if the layer contains transparency
        /// </summary>
        protected internal bool HasTransparency;

        /// <summary>
        /// Indicates whether to hide this layer when drawing the legend.
        /// Added by Chris M in May 2006.
        /// </summary>
        private bool m_HideFromLegend;

        /// <summary>
        /// Indicates what field index should be used for displaying map tooltips.
        /// </summary>
        public int MapTooltipFieldIndex = -1;

        /// <summary>
        /// Indicates whether map tooltips should be shown for this layer.
        /// </summary>
        public bool MapTooltipsEnabled = false;

        /// <summary>
        /// (Doesn't apply to line shapefiles)
        /// Indicates whether the vertices of a line or polygon are visible.
        /// </summary>
        public bool VerticesVisible = false;

        /// <summary>
        /// If you wish to display a caption (e.g. "State Name") above the legend items for a coloring scheme, set this.
        /// Set to "" to disable.
        /// </summary>
        public string StippleSchemeFieldCaption = "";

        /// <summary>
        /// If you wish to display a caption (e.g. "Region") above the legend items for a stipple scheme, set this.
        /// Set to "" to disable.
        /// </summary>
        public string ColorSchemeFieldCaption = "";

        /// <summary>
        /// If you wish to display a caption (e.g. "State Name") above the legend items for a point image scheme, set this.
        /// Set to "" to disable.
        /// </summary>
        public string PointImageFieldCaption = "";

        #endregion "Member Variables"

        #region Constructor
        /// <summary>
        /// Constructor
        /// </summary>
        public Layer(Legend leg)
        {
            //The next line MUST GO FIRST in the constructor
            m_Legend = leg;
            //The previous line MUST GO FIRST in the constructor

            Expanded = true; //(m_Legend.m_Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology);
            //Expanded = false;

            ColorLegend = new ArrayList();
            m_Handle = -1;
            m_Icon = null;
            m_LayerType = eLayerType.Invalid;
            m_UseDynamicVisibility = false;
            HasTransparency = false;

            Elements = new List<LayerElement>();
            ShapefileBinding = new ShapefileBinding();

            //_symbologySettings = new SymbologySettings();
            m_CustomObjects = new Hashtable();
            m_smallIconWasDrawn = false;
            //m_height = 0;
        }
        #endregion

        #region Properties
        /// <summary>
        /// Gets a snapshot (bitmap) of the layer
        /// </summary>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot()
        {
            return m_Legend.LayerSnapshot(this.Handle);
        }

        /// <summary>
        /// Gets a snapshot (bitmap) of the layer
        /// </summary>
        /// <param name="imgWidth">Desired width in pixels of the snapshot</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(int imgWidth)
        {
            return m_Legend.LayerSnapshot(this.Handle, imgWidth);
        }

        /// <summary>
        /// Returns the underlying object, either image or shapefile
        /// </summary>
        public object GetObject()
        {
            return this.m_Legend.Map.get_GetObject(this.Handle);
        }

        /// <summary>
        /// Settings of the symbology dialogs for the layer
        /// </summary>
        internal SymbologySettings SymbologySettings
        {
            get { return m_symbologySettings; }
            set { m_symbologySettings = value; }
        }

        /// <summary>
        /// Returns filename of the layer source
        /// </summary>
        public string FileName
        {
            get { return this.m_Legend.Map.get_LayerFilename(this.Handle); }
        }

        /// <summary>
        /// Returns the name of the layer
        /// </summary>
        public string Name
        {
            get
            {
                return this.m_Legend.Map.get_LayerName(this.Handle); 
            }
            set
            {
                this.m_Legend.Map.set_LayerName(this.Handle, value);
            }
        }

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font, int maxWidth)
        {
            return g.MeasureString(this.Name, font, maxWidth);
        }

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font, int maxWidth, string otherName, StringFormat format)
        {
            return g.MeasureString(otherName, font, maxWidth, format);
        }

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font)
        {
            return g.MeasureString(this.Name, font);
        }

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font, string otherName)
        {
            return g.MeasureString(otherName, font);
        }

        /// <summary>
        /// Gets the Handle for this layer
        /// </summary>
        public int Handle { get { return m_Handle; } }

        /// <summary>
        /// Gets or sets the data type of the layer.
        /// Note:  This property should only be set when specifying a
        /// grid layer.  Shapefile layers and image layers are automatically
        /// set to the correct value
        /// </summary>
        public eLayerType Type
        {
            get { return m_LayerType; }
            set { m_LayerType = value; }
        }

        /// <summary>
        /// Specifies whether or not the layer should be using dynamic visibility.  The legend will draw the check box grey if the layer is using dynamic visibility.
        /// </summary>
        public bool UseDynamicVisibility
        {
            get
            {
                return m_Legend.m_Map.get_LayerDynamicVisibility(this.Handle);
            }
            set
            {
                m_Legend.m_Map.set_LayerDynamicVisibility(this.Handle, value);
            }
        }

        /// <summary>
        /// Gets or sets the maximum scale at which the layer is still visible when dynamic visibility is used
        /// </summary>
        public double MaxVisibleScale
        {
            get { return m_Legend.m_Map.get_LayerMaxVisibleScale(this.m_Handle); }
            set { m_Legend.m_Map.set_LayerMaxVisibleScale(this.m_Handle, value); }
        }

        /// <summary>
        /// Gets or sets the maximum zoom at which the layer is still visible when dynamic visibility is used
        /// </summary>
        public int MaxVisibleZoom
        {
            get { return m_Legend.m_Map.get_LayerMaxVisibleZoom(this.m_Handle); }
            set { m_Legend.m_Map.set_LayerMaxVisibleZoom(this.m_Handle, value); }
        }

        /// <summary>
        /// Gets or sets the minimum scale at which the layer is still visible when dynamic visibility is used
        /// </summary>
        public double MinVisibleScale
        {
            get { return m_Legend.m_Map.get_LayerMinVisibleScale(this.m_Handle); }
            set { m_Legend.m_Map.set_LayerMinVisibleScale(this.m_Handle, value); }
        }

        /// <summary>
        /// Gets or sets the minimum zoom at which the layer is still visible when dynamic visibility is used
        /// </summary>
        public int MinVisibleZoom
        {
            get { return m_Legend.m_Map.get_LayerMinVisibleZoom(this.m_Handle); }
            set { m_Legend.m_Map.set_LayerMinVisibleZoom(this.m_Handle, value); }
        }

        /// <summary>
        /// Gets or sets key of a layer
        /// </summary>
        public string GuidKey
        {
            get { return m_Legend.m_Map.get_LayerKey(this.m_Handle); }
            set { m_Legend.m_Map.set_LayerKey(this.m_Handle, value); }
        }
        #endregion

        /// <summary>
        /// Regenerates the Color Scheme associate with this layer and
        /// causes the control to redraw itself.
        /// </summary>
        public void Refresh()
        {
            //NewColorLegend = m_Legend.m_Map.GetColorScheme(this.Handle);
            m_Legend.Redraw();
        }

        /// <summary>
        /// Gets or sets the icon that appears next to this layer in the legend.
        /// Setting this value to null(nothing) removes the icon from the legend
        /// and sets it back to the default icon.
        /// </summary>
        public object Icon
        {
            get
            {
                return m_Icon;
            }
            set
            {
                if (globals.IsSupportedPicture(value))
                {
                    m_Icon = value;
                }
                else
                {
                    throw new System.Exception("Legend Error: Invalid Group Icon type");
                }
            }
        }

        /// <summary>
        /// Calculates the height of the layer
        /// </summary>
        /// <param name="UseExpandedHeight">If True, the height returned is the expanded height. Otherwise, the height is the displayed height of the layer</param>
        /// <returns>Height of layer(depends on 'Expanded' state of the layer)</returns>
        protected internal int CalcHeight(bool UseExpandedHeight)
        {
            // to affect drawing of the expansion box externally
            if (m_Expanded && ExpansionBoxCustomHeightFunction != null)
            {
                int ht = Constants.ITEM_HEIGHT;
                bool Handled = false;
                ExpansionBoxCustomHeightFunction(m_Handle, m_Legend.Width, ref ht, ref Handled);
                if (Handled)
                    return ht + Constants.ITEM_HEIGHT + Constants.EXPAND_BOX_TOP_PAD * 2;
                else
                    return Constants.ITEM_HEIGHT;
            }

            int ret = 0;

            if (this.Type == eLayerType.Grid || this.Type == eLayerType.Image)
            {
                // Our own calculation
                if (UseExpandedHeight == false && (m_Expanded == false || ColorLegend.Count == 0)) //|| (this.Type == eLayerType.Image))
                    ret = Constants.ITEM_HEIGHT;
                else
                    ret = Constants.ITEM_HEIGHT + (ColorLegend.Count * Constants.CS_ITEM_HEIGHT) + 2;

                // Add in caption space
                if (UseExpandedHeight || m_Expanded)
                    ret += (ColorSchemeFieldCaption.Trim() != "" ? Constants.CS_ITEM_HEIGHT : 0) + (StippleSchemeFieldCaption.Trim() != "" ? Constants.CS_ITEM_HEIGHT : 0);
            }
            else
            {
                var sf = m_Legend.m_Map.get_Shapefile(this.Handle);

                if ((UseExpandedHeight || m_Expanded) && sf != null)
                {
                    ret = Constants.ITEM_HEIGHT + 2;                            // layer name

                    // height of symbology or label
                    int val1, val2;
                    val1 = (GetCategoryHeight(sf.DefaultDrawingOptions) + 2);  // default symbology

                    if (sf.Labels.Count == 0 || sf.Labels.Visible == false || true) // labels aren't drawn currently
                    {
                        ret += val1;
                    }
                    else
                    {
                        // label preview is present
                        LabelStyle style = new LabelStyle(sf.Labels.Options);
                        System.Drawing.Bitmap img = new System.Drawing.Bitmap(500, 200);
                        Graphics g = Graphics.FromImage(img);
                        Size size = style.MeasureString(g, "String", 30);
                        val2 = size.Height + 2;
                        ret += val1 > val2 ? val1 : val2;
                    }

                    if (sf.Categories.Count > 0)
                    {
                        ret += Constants.CS_ITEM_HEIGHT + 2;   // caption

                        MapWinGIS.ShapefileCategories categories = sf.Categories;
                        if (this.Type == eLayerType.LineShapefile || this.Type == eLayerType.PolygonShapefile)
                        {
                            ret += sf.Categories.Count * (Constants.CS_ITEM_HEIGHT + 2);
                        }
                        else
                        {
                            for (int i = 0; i < sf.Categories.Count; i++)
                            {
                                ret += GetCategoryHeight(categories.get_Item(i).DrawingOptions);
                            }
                        }
                        ret += 2;
                    }

                    if (sf.Charts.Count > 0 && sf.Charts.NumFields > 0 && sf.Charts.Visible)
                    {
                        ret += (Constants.CS_ITEM_HEIGHT + 2);   // caption
                        ret += sf.Charts.IconHeight;
                        ret += 2;

                        ret += (sf.Charts.NumFields * (Constants.CS_ITEM_HEIGHT + 2));
                    }
                }
                else
                    ret = Constants.ITEM_HEIGHT;

                // TODO: Add caption space
                //if (UseExpandedHeight || m_Expanded)
                //    ret += (ColorSchemeFieldCaption.Trim() != "" ? Constants.CS_ITEM_HEIGHT : 0) + (StippleSchemeFieldCaption.Trim() != "" ? Constants.CS_ITEM_HEIGHT : 0);
            }

            m_height = ret;  // caching height here to get it of recalculation when there are lots of categories

            return ret;
        }

        /// <summary>
        /// Calculates the height of the given category
        /// </summary>
        public int GetCategoryHeight(MapWinGIS.ShapeDrawingOptions options)
        {
            if (this.Type == eLayerType.PolygonShapefile || this.Type == eLayerType.LineShapefile)
            {
                return Constants.CS_ITEM_HEIGHT + 2;
            }
            else if (this.Type == eLayerType.PointShapefile)
            {
                switch(options.PointType)
                {
                    case tkPointSymbolType.ptSymbolPicture:
                        return options.Picture.Height * options.PictureScaleY + 2 <= Constants.CS_ITEM_HEIGHT || options.Picture == null ? Constants.CS_ITEM_HEIGHT + 2 : (int)(options.Picture.Height * options.PictureScaleY + 2);
                    case tkPointSymbolType.ptSymbolFontCharacter:
                        double ratio = options.FrameVisible ? 1.4 : 0.9;
                        return (options.PointSize * ratio) + 2 <= Constants.CS_ITEM_HEIGHT ? Constants.CS_ITEM_HEIGHT : (int)(options.PointSize * ratio);
                    default:
                        return options.PointSize + 2 <= Constants.CS_ITEM_HEIGHT ? Constants.CS_ITEM_HEIGHT + 2: (int)options.PointSize + 2;
                }
            }
            else
                return 0;
        }

        /// <summary>
        /// Returns the width of icon for specified set of options
        /// </summary>
        public int GetCategoryWidth(ShapeDrawingOptions options)
        {
            const int maxWidth = 100;
            if (this.Type == eLayerType.PolygonShapefile || this.Type == eLayerType.LineShapefile)
            {
                return Constants.ICON_WIDTH;
            }
            else if (this.Type == eLayerType.PointShapefile)
            {
                int width = 0;
                switch (options.PointType)
                {
                    case tkPointSymbolType.ptSymbolPicture:
                        width = options.Picture.Width*options.PictureScaleX <= Constants.ICON_WIDTH || options.Picture == null ? Constants.ICON_WIDTH: (int) (options.Picture.Width*options.PictureScaleX);
                        break;
                    case tkPointSymbolType.ptSymbolFontCharacter:
                        double ratio = options.FrameVisible ? 1.4 : 1.0;
                        width = options.PointSize * ratio <= Constants.ICON_WIDTH ? Constants.ICON_WIDTH : (int)(options.PointSize * ratio);
                        break;
                    default:
                        width = options.PointSize <= Constants.ICON_WIDTH ? Constants.ICON_WIDTH : (int) options.PointSize;
                        break;

                }
                return width <= maxWidth ? width : maxWidth;
            }
            else
                return 0;
        }

        /// <summary>
        /// Calculates the maximium width of the icon for the layer going through all categories
        /// </summary>
        /// <returns></returns>
        public int get_MaxIconWidth(MapWinGIS.Shapefile sf)
        {
            if (sf == null)
                return 0;
            int maxWidth = GetCategoryWidth(sf.DefaultDrawingOptions);
            for (int i = 0; i < sf.Categories.Count; i++)
            {
                int width = GetCategoryWidth(sf.Categories.get_Item(i).DrawingOptions);
                if (width > maxWidth)
                    maxWidth = width;
            }
            return maxWidth;
        }

        /// <summary>
        /// Calculates the height of a layer
        /// </summary>
        /// <returns>Height of layer(depends on Expanded state of the layer)</returns>
        protected internal int CalcHeight()
        {
            return CalcHeight(this.Expanded);
        }

        public int Height
        {
            get
            {
                return CalcHeight();
            }
        }

        /// <summary>
        /// Gets or sets whether or not the Layer is expanded.  This shows or hides the
        /// layer's Color Scheme (if one exists).
        /// </summary>
        public bool Expanded
        {
            get
            {
                return m_Expanded;
            }
            set
            {
                m_Expanded = value;
                m_Legend.Redraw();
            }
        }

        /// <summary>
        /// Gets or Sets the visibility of the layer
        /// </summary>
        public bool Visible
        {
            get
            {
                return m_Legend.m_Map.get_LayerVisible(m_Handle);
            }
            set
            {
                bool oldVal = m_Legend.m_Map.get_LayerVisible(m_Handle);
                if (oldVal != value)
                {
                    m_Legend.m_Map.set_LayerVisible(m_Handle, value);
                    m_Legend.Redraw();
                }
            }
        }

        /// <summary>
        /// Indicates whether to skip over the layer when saving a project.
        /// </summary>
        public bool SkipOverDuringSave
        {
            get
            {
                return m_Legend.m_Map.get_LayerSkipOnSaving(this.Handle);
                //return m_SkipDuringSave;
            }
            set
            {
                m_Legend.m_Map.set_LayerSkipOnSaving(this.Handle, value);
                //m_SkipDuringSave = value;
            }
        }

        /// <summary>
        /// Indicates whether to skip over the layer when drawing the legend.
        /// </summary>
        public bool HideFromLegend
        {
            get
            {
                return m_HideFromLegend;
            }
            set
            {
                m_HideFromLegend = value;
            }
        }
    }
}