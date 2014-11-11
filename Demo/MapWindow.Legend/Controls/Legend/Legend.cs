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
//Christian Degrassi 2010-02-25: Implemented LayerAdded/LayerRemoved GroupAdded/GroupRemoved Events
//Christian Degrassi 2010-03-12: Refactoring MoveLayer method to fix issue 1641
//Christian Degrassi 2010-03-12: Refactoring MoveLayer method to fix issues 1642, 1643
//********************************************************************************************************

using System;
using System.Collections;
using System.Drawing;
using System.Drawing.Drawing2D;
using System.Windows.Forms;
using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Controls.Legend
{

    #region "Event Delegate definitions"

    /// <summary>
    /// Handler for the event when a layer item is double clicked
    /// </summary>
    public delegate void LayerDoubleClick(int Handle);
    /// <summary>
    /// Handler for the event when a mouse down event occurs on a Layer item
    /// </summary>
    public delegate void LayerMouseDown(int Handle, MouseButtons button);

    /// <summary>
    /// Handler for the event when a Mouse up event occurs on a Layer item
    /// </summary>
    public delegate void LayerMouseUp(int Handle, MouseButtons button);

    /// <summary>
    /// Handler for the event when a new layer is selected
    /// </summary>
    public delegate void LayerSelected(int Handle);

    //Christian Degrassi 2010-02-25: This fixes issue 1622
    /// <summary>
    /// Handler for the event when a new layer is added
    /// </summary>
    public delegate void LayerAdded(int Handle);

    //Christian Degrassi 2010-02-25: This fixes issue 1622
    /// <summary>
    /// Handler for the event when a layer is removed
    /// </summary>
    public delegate void LayerRemoved(int Handle);

    /// <summary>
    /// Handler for the event when a Double Click Event Occurs on a Group
    /// </summary>
    public delegate void GroupDoubleClick(int Handle);

    /// <summary>
    /// Handler for the event when a Mouse down event occurs on a group
    /// </summary>
    public delegate void GroupMouseDown(int Handle, MouseButtons button);

    /// <summary>
    /// Handler for the event when a Mouse up event occurs on a group
    /// </summary>
    public delegate void GroupMouseUp(int Handle, MouseButtons button);

    /// <summary>
    /// Handler for the event when a Mouse click event occurs the control (not on a group nor a layer)
    /// </summary>
    public delegate void LegendClick(MouseButtons button, Point Location);

    /// <summary>
    /// Handler for the event when a Layer is moved
    /// </summary>
    public delegate void LayerPositionChanged(int Handle);

    /// <summary>
    /// Handler for the event when a layer's visibility changes due to a mouse click
    /// </summary>
    public delegate void LayerVisibleChanged(int Handle, bool NewState, ref bool Cancel);

    /// <summary>
    /// Handler for the event when a Group (and all of its Layers) changes positions
    /// </summary>
    public delegate void GroupPositionChanged(int Handle);

    //Christian Degrassi 2010-02-25: This fixes issue 1622
    /// <summary>
    /// Handler for the event when a Group is Added
    /// </summary>
    public delegate void GroupAdded(int Handle);

    //Christian Degrassi 2010-02-25: This fixes issue 1622
    /// <summary>
    /// Handler for the event when a Group is Removed
    /// </summary>
    public delegate void GroupRemoved(int Handle);

    /// <summary>
    /// Handler for the event when a group's expaned property changes.
    /// </summary>
    public delegate void GroupExpandedChanged(int Handle, bool Expanded);

    /// <summary>
    /// Fired when a group checkbox is clicked
    /// </summary>
    /// <param name="Handle"></param>
    /// <param name="State"></param>
    public delegate void GroupCheckboxClicked(int Handle, VisibleStateEnum State);

    /// <summary>
    /// Delegate definition for ExpansionBoxCustomRenderFunction.
    /// </summary>
    /// <param name="Handle">The layer handle of the layer whose expanded area is being drawn.</param>
    /// <param name="Bounds">The bounds you're allowed to draw in. Note that HEIGHT is the maximum, you don't have to use it all; please set to what you used. GOH</param>
    /// <param name="G">The graphics object you should draw with.</param>
    /// <param name="Handled">If you rendered an expansion box for this layer, set this to true!</param>
    public delegate void ExpansionBoxCustomRenderer(int Handle, System.Drawing.Rectangle Bounds, ref System.Drawing.Graphics G, ref bool Handled);

    /// <summary>
    /// Indicates how high your ExpansionBoxCustomRenderFunction will draw, so that
    /// other layer items can render around it.
    /// </summary>
    /// <param name="Handle">The layer handle being rendered/</param>
    /// <param name="CurrentWidth">The width of the area you will be allowed to draw in.</param>
    /// <param name="HeightToDraw">How high, based on the width, you plan to draw. This must be exact to look good!</param>
    /// <param name="Handled">If you handled this function call for this handle, set Handled to true.</param>
    public delegate void ExpansionBoxCustomHeight(int Handle, int CurrentWidth, ref int HeightToDraw, ref bool Handled);

    /// <summary>
    /// Fired when a layer checkbox is clicked
    /// </summary>
    /// <param name="Handle"></param>
    /// <param name="NewState"></param>
    public delegate void LayerCheckboxClicked(int Handle, bool NewState);

    /// <summary>
    /// Fired when layer colorbox is clicked
    /// </summary>
    /// <param name="Handle">Layer handle</param>
    public delegate void LayerColorboxClicked(int Handle);

    /// <summary>
    /// Fired when colorbox for one of the shapefiles categories is cliked
    /// </summary>
    /// <param name="Handle">Layer handle</param >
    /// <param name="Category">The index of category</param>
    public delegate void LayerCategoryClicked(int Handle, int Category);

    /// <summary>
    /// Fired when chart icon of the shapefile is clicked
    /// </summary>
    /// <param name="Handle">Layer handle</param>
    public delegate void LayerChartClicked(int Handle);

    /// <summary>
    /// Fired when one of the chart field of the shapefile is clicked
    /// </summary>
    /// <param name="Handle">Layer handle</param>
    /// <param name="FieldIndex">The index of field</param>
    public delegate void LayerChartFieldClicked(int Handle, int FieldIndex);

    /// <summary>
    /// Fired when label preview for layer is clicked
    /// </summary>
    /// <param name="Handle">Layer handle</param>
    public delegate void LayerLabelClicked(int Handle);

    /// <summary>
    /// Fired when label preview for particular label category is clicked
    /// </summary>
    /// <param name="Handle">Layer handle</param>
    /// <param name="Handle">Index of label category</param>
    public delegate void LayerLabelCategoryClicked(int Handle, int Category);

    #endregion "Event Delegate definitions"
    
    /// <summary>
    /// Summary description for Legend.
    /// </summary>
    [ToolboxBitmap(@"C:\Dev\MapWindow4Dev\MapWinInterfaces\MapWinLegend.ico")]
    [CLSCompliant(false)]
    public class Legend : System.Windows.Forms.UserControl
    {
        private LegendEventHandler m_eventHandler = null;
        
        /// <summary>
        /// Gets or Sets the MapWinGIS.Map associated with this legend control
        /// Note: This property must be set before manipulating layers
        /// </summary>
        public MapWinGIS.Map Map
        {
            get
            {
                return m_Map;
            }
            set
            {
                m_Map = value;
            }
        }


        public Layer GetLayer(int layerHandle)
        {
            return Layers.ItemByHandle(layerHandle);
        }

        private static string m_appName = "";
        public static string AppName
        {
            get { return m_appName; }
            set { m_appName = value; }
        }
        
        #region "Member Variables"

        private const int INVALID_GROUP = -1;

        private Groups m_GroupManager;
        private Layers m_LayerManager;
        /// <summary>
        /// This variable is used internally to interact with the MapWinGIS.Map
        /// </summary>
        protected internal MapWinGIS.Map m_Map;

        private Color m_SelectedColor;
        private Color m_BoxLineColor;

        private Image m_BackBuffer;
        private Image m_MidBuffer;
        private Graphics m_FrontBuffer;
        private bool m_showGroupFolders;
        //private ToolTip m_ToolTip;

        /// <summary>
        /// List of all groups
        /// </summary>
        protected internal ArrayList m_AllGroups = new ArrayList();

        /// <summary>
        /// Group Position Lookup table (use Group Handle as index)
        /// </summary>
        protected internal ArrayList m_GroupPositions = new ArrayList();
        private Graphics m_Draw = null;
        private DragInfo m_DragInfo = new DragInfo();
        private uint m_LockCount;
        private int m_SelectedLayerHandle;
        private int m_SelectedGroupHandle;
        private System.Drawing.Font m_Font;
        private System.Drawing.Font m_BoldFont;
        private bool m_painting = false;

        private const int cGridIcon = 0;
        private const int cImageIcon = 1;
        private const int cCheckedBoxIcon = 2;
        private const int cUnCheckedBoxIcon = 3;
        private const int cCheckedBoxGrayIcon = 4;
        private const int cUnCheckedBoxGrayIcon = 5;
        private const int cActiveLabelIcon = 6;
        private const int cDimmedLabelIcon = 7;
        private const int cEditing = 8;
        private ImageList Icons;
        private System.Windows.Forms.VScrollBar vScrollBar;

        private bool m_ShowLabels = false;

        #endregion "Member Variables"

        #region "Events"

        public event EventHandler LayerPropertiesChanged;

        /// <summary>
        /// Layer Double Click Event
        /// </summary>
        public event LayerDoubleClick LayerDoubleClick;
        /// <summary>
        /// Layer Mouse Down Click Event
        /// </summary>
        public event LayerMouseDown LayerMouseDown;

        /// <summary>
        /// Layer Mouse Up Event
        /// </summary>
        public event LayerMouseUp LayerMouseUp;

        /// <summary>
        /// Group Double Click Event
        /// </summary>
        public event GroupDoubleClick GroupDoubleClick;

        /// <summary>
        /// Group Mouse Down Event
        /// </summary>
        public event GroupMouseDown GroupMouseDown;

        /// <summary>
        /// Group Mouse Up Event
        /// </summary>
        public event GroupMouseUp GroupMouseUp;

        /// <summary>
        /// Legend was clicked event (not on Group, nor on Layer)
        /// </summary>
        public event LegendClick LegendClick;

        /// <summary>
        /// Selected Layer changed event
        /// </summary>
        public event LayerSelected LayerSelected;

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Added Layer event
        /// </summary>
        public event LayerAdded LayerAdded;

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Removed Layer event
        /// </summary>
        public event LayerRemoved LayerRemoved;

        /// <summary>
        /// Position of a layer has changed event
        /// </summary>
        public event LayerPositionChanged LayerPositionChanged;

        /// <summary>
        /// Position of a group has changed event
        /// </summary>
        public event GroupPositionChanged GroupPositionChanged;

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// A Group has been added
        /// </summary>
        public event GroupAdded GroupAdded;

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// A Group has been removed
        /// </summary>
        public event GroupRemoved GroupRemoved;

        /// <summary>
        /// The visibility of a layer has changed event
        /// </summary>
        public event LayerVisibleChanged LayerVisibleChanged;

        /// <summary>
        /// Fires when the Group checkbox is clicked
        /// </summary>
        public event GroupCheckboxClicked GroupCheckboxClicked;

        /// <summary>
        /// Fires when the Expanded property of a group changes.
        /// </summary>
        public event GroupExpandedChanged GroupExpandedChanged;

        /// <summary>
        /// Fires when the layer checkbox is clicked
        /// </summary>
        public event LayerCheckboxClicked LayerCheckboxClicked;

        /// <summary>
        /// Fires when layer colorbox is clicked
        /// </summary>
        public event LayerColorboxClicked LayerColorboxClicked;

        /// <summary>
        /// Fires when one of the shapefile categories is clicked
        /// </summary>
        public event LayerCategoryClicked LayerCategoryClicked;

        /// <summary>
        /// Fires when charts icon is clicked
        /// </summary>
        public event LayerChartClicked LayerChartClicked;

        /// <summary>
        /// Fires when one of chart fields is clicked
        /// </summary>
        public event LayerChartFieldClicked LayerChartFieldClicked;

        /// <summary>
        /// Fired when label preview for layer is clicked
        /// </summary>
        public event LayerLabelClicked LayerLabelClicked;

        /// <summary>
        /// Fired when label preview for particular label category is clicked
        /// </summary>
        public event LayerLabelCategoryClicked LayerLabelCategoryClicked;

        /// <summary>
        /// Fired when labels icon for a layer is clicked
        /// </summary>
        /// <param name="Handle">Layer handle</param>
        public delegate void LayerLabelsEventArguments(int Handle);

        /// <summary>
        /// Fired when labels icon for the layer is clicked
        /// </summary>
        public event LayerLabelsEventArguments LayerLabelsClicked;

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        public void FireLayerPropertiesChanged(int handle)
        {
            if (LayerPropertiesChanged != null)
                LayerPropertiesChanged(this, new EventArgs());
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerVisibleChanged(int Handle, bool NewState, ref bool Cancel)
        {
            if (LayerVisibleChanged != null)
                LayerVisibleChanged(Handle, NewState, ref Cancel);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupPositionChanged(int Handle)
        {
            if (GroupPositionChanged != null)
                GroupPositionChanged(Handle);
        }

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupAdded(int Handle)
        {
            if (GroupAdded != null)
                GroupAdded(Handle);
        }

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupRemoved(int Handle)
        {
            if (GroupRemoved != null)
                GroupRemoved(Handle);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerPositionChanged(int Handle)
        {
            if (LayerPositionChanged != null)
                LayerPositionChanged(Handle);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerSelected(int Handle)
        {
            if (LayerSelected != null)
                LayerSelected(Handle);
        }

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerAdded(int Handle)
        {
            if (LayerAdded != null)
                LayerAdded(Handle);
        }

        //Christian Degrassi 2010-02-25: This fixes issue 1622
        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerRemoved(int Handle)
        {
            if (LayerRemoved != null)
                LayerRemoved(Handle);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLegendClick(MouseButtons button, Point Location)
        {
            if (LegendClick != null)
                LegendClick(button, Location);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupMouseUp(int Handle, MouseButtons button)
        {
            if (GroupMouseUp != null)
                GroupMouseUp(Handle, button);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupMouseDown(int Handle, MouseButtons button)
        {
            if (GroupMouseDown != null)
                GroupMouseDown(Handle, button);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireGroupDoubleClick(int Handle)
        {
            if (GroupDoubleClick != null)
                GroupDoubleClick(Handle);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerMouseUp(int Handle, MouseButtons button)
        {
            if (LayerMouseUp != null)
                LayerMouseUp(Handle, button);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerMouseDown(int Handle, MouseButtons button)
        {
            if (LayerMouseDown != null)
                LayerMouseDown(Handle, button);
        }

        /// <summary>
        /// Sends event to any listeners
        /// </summary>
        protected internal void FireLayerDoubleClick(int Handle)
        {
            if (LayerDoubleClick != null)
                LayerDoubleClick(Handle);
        }

        /// <summary>
        /// Raise GroupCheckboxClicked event if any listeners are attached.
        /// </summary>
        protected internal void FireGroupCheckboxClicked(int Handle, VisibleStateEnum State)
        {
            if (null != GroupCheckboxClicked)
                GroupCheckboxClicked(Handle, State);
        }

        /// <summary>
        /// Raise GroupExpandedChanged event if any listeners are attached.
        /// </summary>
        protected internal void FireGroupExpandedChanged(int Handle, bool Expanded)
        {
            if (null != GroupExpandedChanged)
                GroupExpandedChanged(Handle, Expanded);
        }

        /// <summary>
        /// Raise LayerCheckboxClicked event if any listeners are attached.
        /// </summary>
        protected internal void FireLayerCheckboxClicked(int Handle, bool NewState)
        {
            if (null != LayerCheckboxClicked)
                LayerCheckboxClicked(Handle, NewState);
        }

        /// <summary>
        /// Raises LayerColorboxClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerColorboxClicked(int Handle)
        {
            if (LayerColorboxClicked != null)
                LayerColorboxClicked(Handle);
        }

        /// <summary>
        /// Raises LayerCategoryClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerCategoryClicked(int Handle, int Category)
        {
            if (LayerCategoryClicked != null)
                LayerCategoryClicked(Handle, Category);
        }

        /// <summary>
        /// Raises LayerChartClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerChartClicked(int Handle)
        {
            if (LayerChartClicked != null)
                LayerChartClicked(Handle);
        }

        /// <summary>
        /// Raises LayerChartFieldClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerChartFieldClicked(int Handle, int Field)
        {
            if (LayerChartFieldClicked != null)
                LayerChartFieldClicked(Handle, Field);
        }

        /// <summary>
        /// Raises LayerLabelClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerLabelClicked(int Handle)
        {
            if (LayerLabelClicked != null)
                LayerLabelClicked(Handle);
        }

        /// <summary>
        /// Raises LayerLabelClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerLabelCategoryClicked(int Handle, int Category)
        {
            if (LayerLabelCategoryClicked != null)
                LayerLabelCategoryClicked(Handle, Category);
        }

        /// <summary>
        /// Raises LayerLabelsClicked event if any listeners are attached
        /// </summary>
        protected internal void FireLayerLabelsClicked(int Handle)
        {
            if (LayerLabelsClicked != null)
                this.LayerLabelsClicked(Handle);
        }

        #endregion "Events"

        private System.ComponentModel.IContainer components;

        /// <summary>
        /// This is the constructor for the <c>Legend</c> control.
        /// </summary>
        public Legend()
        {
            // This call is required by the Windows.Forms Form Designer.
            InitializeComponent();

            m_GroupManager = new Groups(this);
            m_LayerManager = new Layers(this);

            m_LockCount = 0;
            m_SelectedLayerHandle = -1;
            m_SelectedGroupHandle = -1;
            m_Font = new Font("Arial", 8);
            m_BoldFont = new Font("Arial", 8, System.Drawing.FontStyle.Bold);
            m_SelectedColor = Color.FromArgb(255, 240, 240, 240);
            m_BoxLineColor = Color.Gray;
            m_showGroupFolders = true;

            m_eventHandler = new LegendEventHandler(this);

            //m_ToolTip = new ToolTip(components);
        }

        /// <summary>
        /// Destructor for the legend
        /// </summary>
        /// <param name="disposing"></param>
        protected override void Dispose(bool disposing)
        {
            if (disposing)
            {
                if (components != null)
                {
                    components.Dispose();
                }
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Legend));
            this.vScrollBar = new System.Windows.Forms.VScrollBar();
            this.Icons = new System.Windows.Forms.ImageList(this.components);
            this.SuspendLayout();
            // 
            // vScrollBar
            // 
            resources.ApplyResources(this.vScrollBar, "vScrollBar");
            this.vScrollBar.Name = "vScrollBar";
            this.vScrollBar.Scroll += new System.Windows.Forms.ScrollEventHandler(this.vScrollBar_Scroll);
            // 
            // Icons
            // 
            this.Icons.ImageStream = ((System.Windows.Forms.ImageListStreamer)(resources.GetObject("Icons.ImageStream")));
            this.Icons.TransparentColor = System.Drawing.Color.Transparent;
            this.Icons.Images.SetKeyName(0, "");
            this.Icons.Images.SetKeyName(1, "");
            this.Icons.Images.SetKeyName(2, "");
            this.Icons.Images.SetKeyName(3, "");
            this.Icons.Images.SetKeyName(4, "");
            this.Icons.Images.SetKeyName(5, "");
            this.Icons.Images.SetKeyName(6, "tag_blue.png");
            this.Icons.Images.SetKeyName(7, "tag_gray.png");
            this.Icons.Images.SetKeyName(8, "pen.png");
            // 
            // Legend
            // 
            this.BackColor = System.Drawing.Color.White;
            this.Controls.Add(this.vScrollBar);
            this.Name = "Legend";
            resources.ApplyResources(this, "$this");
            this.DoubleClick += new System.EventHandler(this.Legend_DoubleClick);
            this.MouseDown += new System.Windows.Forms.MouseEventHandler(this.Legend_MouseDown);
            this.MouseMove += new System.Windows.Forms.MouseEventHandler(this.Legend_MouseMove);
            this.MouseUp += new System.Windows.Forms.MouseEventHandler(this.Legend_MouseUp);
            this.ResumeLayout(false);

        }

        #endregion Component Designer generated code

        /// <summary>
        /// Toggles the layer preview visiblity in the legend
        /// </summary>
        /// <returns></returns>
        public bool ShowLabels
        {
            get { return m_ShowLabels; }
            set { m_ShowLabels = value; }
        }

        /// <summary>
        /// Gets the Menu for manipulating Groups
        /// </summary>
        public Groups Groups
        {
            get
            {
                return m_GroupManager;
            }
        }

        /// <summary>
        /// Gets the Menu for manipulating Layers (without respect to groups)
        /// </summary>
        public Layers Layers
        {
            get
            {
                return m_LayerManager;
            }
        }

        /// <summary>
        /// Adds a group to the list of all groups
        /// </summary>
        /// <param name="Name">Caption shown in legend</param>
        /// <returns>Handle to the Group on success, -1 on failure</returns>
        protected internal int AddGroup(string Name)
        {
            return AddGroup(Name, -1);
        }

        /// <summary>
        /// Adds a group to the list of all groups
        /// </summary>
        /// <param name="Name">Caption shown in legend</param>
        /// <param name="Position">0-Based index of the new Group</param>
        /// <returns>Handle to the Group on success, -1 on failure</returns>
        protected internal int AddGroup(string Name, int Position)
        {
            Group grp = CreateGroup(Name, Position);
            if (grp == null)
            {
                globals.LastError = "Failed to create instance of class 'Group'";
                return INVALID_GROUP;
            }

            Redraw();

            //Christian Degrassi 2010-02-25: This fixes issue 1622
            FireGroupAdded(grp.Handle);

            return grp.Handle;
        }

        /// <summary>
        /// Gets or Sets the background color of the selected layer within the legend
        /// </summary>
        public Color SelectedColor
        {
            get
            {
                return m_SelectedColor;
            }
            set
            {
                m_SelectedColor = value;
            }
        }

        /// <summary>
        /// Gets or Sets the background color of the selected layer within the legend
        /// </summary>
        public bool ShowGroupFolders
        {
            get
            {
                return m_showGroupFolders;
            }
            set
            {
                m_showGroupFolders = value;
            }
        }

        /// <summary>
        /// Removes a group from the list of groups
        /// </summary>
        /// <param name="Handle">Handle of the group to remove</param>
        /// <returns>True on success, False otherwise</returns>
        protected internal bool RemoveGroup(int Handle)
        {
            Group grp = null;
            bool LayerInGroupWasSelected = false;

            //if(IS_VALID_INDEX(m_GroupPositions,Handle) && m_GroupPositions[Handle] != INVALID_GROUP)
            if (IsValidGroup(Handle) == true)
            {
                int index = (int)m_GroupPositions[Handle];
                grp = (Group)m_AllGroups[index];

                //remove any layers within the specified group
                while (grp.Layers.Count > 0)
                {
                    int lyr = ((Layer)grp.Layers[0]).Handle;
                    LayerInGroupWasSelected = LayerInGroupWasSelected || (m_SelectedLayerHandle == lyr);
                    RemoveLayer(lyr);
                    GC.Collect();
                }

                m_AllGroups.RemoveAt(index);
                UpdateGroupPositions();

                // Chris M 11/16/2006 don't just select nothing, could be
                // problematic. Instead intelligently select a new layer if possible.
                // FireLayerSelected(-1);
                if (LayerInGroupWasSelected)
                {
                    m_SelectedLayerHandle = (m_Map.NumLayers > 0 ? m_Map.get_LayerHandle(m_Map.NumLayers - 1) : -1);

                    FireLayerSelected(m_SelectedLayerHandle);
                }

                Redraw();

                //Christian Degrassi 2010-02-25: This fixes issue 1622
                FireGroupRemoved(Handle);
            }
            else
            {
                globals.LastError = "Invalid Group Handle";
                return false;
            }
            return true;
        }

        /// <summary>
        /// Removes a layer from the list of layers
        /// </summary>
        /// <param name="LayerHandle">Handle of layer to be removed</param>
        /// <returns>True on success, False otherwise</returns>
        protected internal bool RemoveLayer(int LayerHandle)
        {
            int GroupIndex,
                LayerIndex;
            Layer lyr = FindLayerByHandle(LayerHandle, out GroupIndex, out LayerIndex);
            if (lyr == null)
                return false;

            Group grp = (Group)m_AllGroups[GroupIndex];
            grp.Layers.RemoveAt(LayerIndex);

            m_Map.RemoveLayer(LayerHandle);

            if (LayerHandle == m_SelectedLayerHandle)
            {
                m_SelectedLayerHandle = m_Map.get_LayerHandle(m_Map.NumLayers - 1);

                FireLayerSelected(m_SelectedLayerHandle);
            }

            grp.RecalcHeight();
            Redraw();

            //Christian Degrassi 2010-02-25: This fixes issue 1622
            FireLayerRemoved(LayerHandle);

            return true;
        }

        private void UpdateGroupPositions()
        {
            int grpCount = m_AllGroups.Count;
            int HandleCount = m_GroupPositions.Count;
            int i;

            //reset all positions
            for (i = 0; i < HandleCount; i++)
                m_GroupPositions[i] = INVALID_GROUP;

            //set valid group positions for existing groups
            for (i = 0; i < grpCount; i++)
                m_GroupPositions[((Group)m_AllGroups[i]).Handle] = i;
        }

        private Group CreateGroup(string caption, int Position)
        {
            Group grp = new Group(this);
            if (caption.Length < 1)
                grp.Text = "New Group";
            else
                grp.Text = caption;

            grp.m_Handle = m_GroupPositions.Count;
            m_GroupPositions.Add(INVALID_GROUP);

            if (Position < 0 || Position >= m_AllGroups.Count)
            {
                //put it at the top
                m_GroupPositions[grp.Handle] = m_AllGroups.Count;
                m_AllGroups.Add(grp);
            }
            else
            {
                //put it where they requested
                m_GroupPositions[grp.Handle] = Position;
                m_AllGroups.Insert(Position, grp);

                UpdateGroupPositions();
            }
            return grp;
        }

        private void DrawBox(Graphics DrawTool, Rectangle rect, Color LineColor)
        {
            Pen pen;
            pen = new Pen(LineColor);

            DrawTool.DrawRectangle(pen, rect);
            pen = null;
        }

        private void DrawBox(Graphics DrawTool, Rectangle rect, Color LineColor, Color BackColor)
        {
            Pen pen;
            pen = new Pen(BackColor);
            DrawTool.FillRectangle(pen.Brush, rect);

            pen = new Pen(LineColor);

            DrawTool.DrawRectangle(pen, rect);
            pen = null;
        }

        private void SwapBuffers()
        {
            SwapBuffers(m_BackBuffer, m_FrontBuffer);
        }

        private void SwapBuffers(Image BackBuffer)
        {
            SwapBuffers(BackBuffer, m_FrontBuffer);
        }

        private void SwapBuffers(Image BackBuffer, Graphics FrontBuffer)
        {
            try
            {
                // temporary: checking random property to be sure the object is valid
                float k = FrontBuffer.DpiX;
            }
            catch (Exception ex)
            {
                // We'll log the error.
                System.Diagnostics.Trace.WriteLine(ex.Message);
                return;
            }

            FrontBuffer.DrawImage(BackBuffer, 0, 0);
            FrontBuffer.Flush(FlushIntention.Sync);
        }

        private void SwapBuffers(Image BackBuffer, Image FrontBuffer)
        {
            Graphics draw = Graphics.FromImage(FrontBuffer);
            draw.DrawImage(BackBuffer, 0, 0);
            draw.Flush(System.Drawing.Drawing2D.FlushIntention.Sync);
        }

        /// <summary>
        /// Draws a group onto a give graphics object (surface)
        /// </summary>
        /// <param name="DrawTool">Graphics object with which to draw</param>
        /// <param name="grp">Group to be drawn</param>
        /// <param name="bounds">Clipping boundaries for this group</param>
        /// <param name="IsSnapshot">Drawing is handled in special way if this is a Snapshot</param>
        protected internal void DrawGroup(Graphics DrawTool, Group grp, Rectangle bounds, bool IsSnapshot)
        {
            int CurLeft = Constants.GRP_INDENT,
                CurWidth = bounds.Width - Constants.GRP_INDENT - Constants.ITEM_RIGHT_PAD,
                CurTop = bounds.Top,
                CurHeight = Constants.ITEM_HEIGHT;

            Rectangle rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

            CurLeft = Constants.GRP_INDENT + Constants.EXPAND_BOX_LEFT_PAD;
            CurTop = bounds.Top;
            bool DrawCheck = false;
            //Color BoxBackColor = Color.White;
            bool DrawGrayCheckbox = false;
            int GroupHeight = grp.Height;
            grp.Top = bounds.Top;

            if (grp.VisibleState == VisibleStateEnum.vsALL_VISIBLE || grp.VisibleState == VisibleStateEnum.vsPARTIAL_VISIBLE)
                DrawCheck = true;

            //draw the border if the group is the one that contains the selected layer and
            //the group is collapsed
            if (grp.Handle == m_SelectedGroupHandle && grp.Expanded == false && IsSnapshot == false)
            {
                rect = new Rectangle(Constants.GRP_INDENT, CurTop, bounds.Width - Constants.GRP_INDENT - Constants.ITEM_RIGHT_PAD, Constants.ITEM_HEIGHT);
                DrawBox(DrawTool, rect, m_BoxLineColor, m_SelectedColor);
            }

            //draw the +- box if there are sub items
            if (grp.Layers.Count > 0 && IsSnapshot == false)
                DrawExpansionBox(DrawTool, bounds.Top + Constants.EXPAND_BOX_TOP_PAD, Constants.GRP_INDENT + Constants.EXPAND_BOX_LEFT_PAD, grp.Expanded);

            if (grp.VisibleState == VisibleStateEnum.vsPARTIAL_VISIBLE)
                DrawGrayCheckbox = true;
            //BoxBackColor = Color.LightGray;

            if (IsSnapshot == false && grp.Expanded == true && grp.Layers.Count > 0 && Map.ShapeDrawingMethod != MapWinGIS.tkShapeDrawingMethod.dmNewSymbology)
            {
                int endY = grp.Top + Constants.ITEM_HEIGHT;

                Pen BlackPen = new Pen(m_BoxLineColor);
                DrawTool.DrawLine(BlackPen, Constants.VERT_LINE_INDENT, bounds.Top + Constants.VERT_LINE_GRP_TOP_OFFSET, Constants.VERT_LINE_INDENT, endY);
            }

            CurLeft = Constants.GRP_INDENT;
            if (bounds.Width > 35 && IsSnapshot == false)
            {
                if (!grp.StateLocked)
                {
                    CurLeft = Constants.GRP_INDENT + Constants.CHECK_LEFT_PAD;
                    DrawCheckBox(DrawTool, bounds.Top + Constants.CHECK_TOP_PAD, CurLeft, DrawCheck, DrawGrayCheckbox);
                }
            }

            CurLeft = Constants.GRP_INDENT + Constants.TEXT_LEFT_PAD;

            if (grp.Icon != null && bounds.Width > 55)
            {
                //draw the icon
                DrawPicture(DrawTool, bounds.Right - Constants.ICON_RIGHT_PAD, CurTop + Constants.ICON_TOP_PAD, Constants.ICON_SIZE, Constants.ICON_SIZE, grp.Icon);

                //set the boundaries for text so that the text and icon don't overlap
                if (IsSnapshot == true)
                    CurLeft = Constants.GRP_INDENT;
                else
                    CurLeft = Constants.GRP_INDENT + Constants.TEXT_LEFT_PAD;

                CurTop = bounds.Top + Constants.TEXT_TOP_PAD;
                CurWidth = bounds.Width - CurLeft - Constants.TEXT_RIGHT_PAD;
                CurHeight = Constants.TEXT_HEIGHT;
                rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
            }
            else
            {
                //Bitmap bmp = MapWindow.Legend.Properties.Resources.folder_open;
                //DrawPicture(DrawTool, bounds.Right - Constants.ICON_RIGHT_PAD, CurTop + Constants.ICON_TOP_PAD, Constants.ICON_SIZE, Constants.ICON_SIZE, bmp);

                if (IsSnapshot == true)
                    CurLeft = Constants.GRP_INDENT;
                else
                    CurLeft = Constants.GRP_INDENT + Constants.TEXT_LEFT_PAD;

                CurTop = bounds.Top + Constants.TEXT_TOP_PAD;
                CurWidth = bounds.Width - CurLeft - Constants.TEXT_RIGHT_PAD_NO_ICON;
                CurHeight = Constants.TEXT_HEIGHT;
                rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
            }

            // group icon
            if (Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology && m_showGroupFolders)
            {
                int size = 16;
                Bitmap bmp = grp.Expanded ? MapWindow.Legend.Properties.Resources.folder_open : MapWindow.Legend.Properties.Resources.folder;
                rect.Offset(0, -2);
                DrawPicture(DrawTool, rect.Left, rect.Top, size, size, bmp);

                rect = new Rectangle(rect.X + size + 3, rect.Y + 2, rect.Width - size, rect.Height);
            }

            // group name
            if (grp.Handle == m_SelectedGroupHandle && IsSnapshot == false)
                DrawText(DrawTool, grp.Text, rect, m_BoldFont);
            else
                DrawText(DrawTool, grp.Text, rect, m_Font);

            //set up the boundaries for drawing list items
            CurTop = bounds.Top + Constants.ITEM_HEIGHT;

            if (grp.Expanded || IsSnapshot == true)
            {
                int ItemCount;
                ItemCount = grp.Layers.Count;
                Layer lyr = null;
                int newLeft = bounds.X + Constants.LIST_ITEM_INDENT;
                int newWidth = bounds.Width - newLeft;
                rect = new Rectangle(newLeft, CurTop, newWidth, bounds.Height - CurTop);

                Pen pen = new Pen(this.m_BoxLineColor);

                //now draw each of the subitems
                for (int i = ItemCount - 1; i >= 0; i--)
                {
                    lyr = (Layer)grp.Layers[i];
                    if (!lyr.HideFromLegend)
                    {
                        //clipping
                        if (rect.Top + lyr.Height < this.ClientRectangle.Top && IsSnapshot == false)
                        {
                            //update the rectangle for the next layer
                            rect.Y += lyr.Height;
                            rect.Height -= lyr.Height;

                            //Skip drawing this layer and move onto the next one
                            continue;
                        }

                        DrawLayer(DrawTool, lyr, rect, IsSnapshot);

                        bool drawLines = (m_Map.ShapeDrawingMethod != MapWinGIS.tkShapeDrawingMethod.dmNewSymbology);

                        if (IsSnapshot == false && drawLines == true)
                        {
                            //draw sub-item vertical line
                            if (i != 0 && !((Layer)grp.Layers[i - 1]).HideFromLegend)//not the last visible layer
                                DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, lyr.Top, Constants.VERT_LINE_INDENT, lyr.Top + lyr.Height + Constants.ITEM_PAD);
                            else//only draw down to box, not down to next item in list(since there is no next item)
                                DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, lyr.Top, Constants.VERT_LINE_INDENT, (int)(lyr.Top + .55 * Constants.ITEM_HEIGHT));

                            //draw Horizontal line over to the Vertical Sub-lyr line
                            CurTop = (int)(rect.Top + .5 * Constants.ITEM_HEIGHT);

                            if (lyr.ColorLegend == null || lyr.ColorLegend.Count == 0)

                                // Color or image schemes do not exist with the layer

                                // if the layer is selected
                                if (lyr.Handle == m_SelectedLayerHandle)
                                {
                                    DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, CurTop, rect.Left + Constants.EXPAND_BOX_LEFT_PAD - 3, CurTop);
                                }
                                else
                                {
                                    // if the layer is not selected
                                    DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, CurTop, rect.Left + Constants.CHECK_LEFT_PAD, CurTop);
                                    //DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, CurTop, rect.Left + Constants.EXPAND_BOX_LEFT_PAD, CurTop);
                                }
                            //
                            else
                            {
                                // There is color or image scheme with the layer

                                // if the layer is selected
                                if (lyr.Handle == m_SelectedLayerHandle)
                                    DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, CurTop, rect.Left + Constants.EXPAND_BOX_LEFT_PAD - 3, CurTop);
                                // if the layer is not selected
                                else
                                    DrawTool.DrawLine(pen, Constants.VERT_LINE_INDENT, CurTop, rect.Left + Constants.EXPAND_BOX_LEFT_PAD, CurTop);
                            }

                            //set up the rectangle for the next layer
                            rect.Y += lyr.Height;
                            rect.Height -= lyr.Height;
                        }
                        else if (IsSnapshot == true)
                        {
                            rect.Y += lyr.CalcHeight(true);
                            rect.Height -= lyr.CalcHeight(true);
                        }
                        else
                        {
                            rect.Y += lyr.CalcHeight(lyr.Expanded);
                            rect.Height -= lyr.CalcHeight(lyr.Expanded);
                        }

                        if (rect.Top >= this.ClientRectangle.Bottom && IsSnapshot == false)
                            break;
                    }
                }
            }
        }

        private void DrawText(Graphics DrawTool, string text, Rectangle rect, Font font, Color penColor)
        {
            Pen pen = new Pen(penColor);
            DrawTool.DrawString(text, font, pen.Brush, rect);
        }

        private void DrawText(Graphics DrawTool, string text, Rectangle rect, Font font)
        {
            DrawText(DrawTool, text, rect, font, Color.Black);
        }

        /// <summary>
        /// Gets or Sets the Selected layer within the legend
        /// </summary>
        public int SelectedLayer
        {
            get
            {
                if (m_LayerManager == null) return 0;
                return (m_LayerManager.Count == 0 ? -1 : m_SelectedLayerHandle);
            }

            set
            {
                int GroupIndex, LayerIndex;

                if (m_SelectedLayerHandle != value && FindLayerByHandle(value, out GroupIndex, out LayerIndex) != null)
                {
                    //only redraw if the selected layer is changing and the handle is valid
                    m_SelectedLayerHandle = value;
                    m_SelectedGroupHandle = ((Group)m_AllGroups[GroupIndex]).Handle;

                    FireLayerSelected(value);

                    Redraw();
                }
            }
        }

        #region Load from map
        //public bool ReadFromMap()
        //{
        //    if (Map == null)
        //        return false;

        //    this.Layers.Clear();
        //    for (int i = 0; i < Map.NumLayers; i++)
        //    {
        //        int handle = Map.get_LayerHandle(i);
        //        var sf = Map.get_Shapefile(handle);
        //        var img = Map.get_Image(handle);
        //        if (sf != null || img != null)
        //        {
        //            object obj = sf != null ? (object)sf : (object)img;
        //            this.Layers.Add(obj, Map.get_LayerVisible(handle), 0);      // TODO: add group handle
        //        }
        //    }
        //    return true;
        //}
        #endregion

        #region AddLayer
        /// <summary>
        /// Adds a layer to the topmost Group
        /// </summary>
        /// <param name="newLayer">object to be added as new layer</param>
        /// <param name="Visible">Should this layer to be visible?</param>
        /// <param name="TargetGroupHandle">Handle of the group into which this layer should be added</param>
        /// <returns>Handle to the Layer on success, -1 on failure</returns>
        protected internal int AddLayer(object newLayer, bool Visible, int TargetGroupHandle)
        {
            return AddLayer(newLayer, Visible, TargetGroupHandle, true);
        }

        /// <summary>
        /// Adds a layer to the map, optionally placing it above the currently selected layer (otherwise at top of layer list).
        /// </summary>
        /// <param name="newLayer">The object to add (must be a supported Layer type)</param>
        /// <param name="Visible">Whether or not the layer is visible upon adding it</param>
        /// <param name="PlaceAboveCurrentlySelected">Whether the layer should be placed above currently selected layer, or at top of layer list.</param>
        /// <returns>Handle of the newly added layer, -1 on failure</returns>
        protected internal int AddLayer(object newLayer, bool Visible, bool PlaceAboveCurrentlySelected)
        {
            int MapLayerHandle = AddLayer(newLayer, Visible);

            if (!PlaceAboveCurrentlySelected) return MapLayerHandle;

            if (SelectedLayer != -1 && PlaceAboveCurrentlySelected)
            {
                int addPos = 0;
                int addGrp = 0;
                addPos = Layers.PositionInGroup(SelectedLayer) + 1;
                addGrp = Layers.GroupOf(SelectedLayer);
                Layers.MoveLayer(MapLayerHandle, addGrp, addPos);
                return MapLayerHandle;
            }

            return MapLayerHandle;
        }

        /// <summary>
        /// Adds a layer to the topmost Group
        /// </summary>
        /// <param name="newLayer">object to be added as new layer</param>
        /// <param name="Visible">Should this layer to be visible in the map?</param>
        /// <param name="TargetGroupHandle">Handle of the group into which this layer should be added</param>
        /// <param name="LegendVisible">Should this layer be visible in the legend?</param>
        /// <returns>Handle to the Layer on success, -1 on failure</returns>
        protected internal int AddLayer(object newLayer, bool Visible, int TargetGroupHandle, bool LegendVisible, int afterLayerHandle = -1)
        {
            Group grp = null;

            if (m_Map == null)
                throw new System.Exception("MapWinGIS.Map Object not yet set. Set Map Property before adding layers");

            m_Map.LockWindow(MapWinGIS.tkLockMode.lmLock);
            int MapLayerHandle = m_Map.AddLayer(newLayer, Visible);
            //newLayer = new Object();

            if (MapLayerHandle < 0)
            {
                m_Map.LockWindow(MapWinGIS.tkLockMode.lmUnlock);
                return MapLayerHandle;
            }

            if (m_AllGroups.Count == 0 || IsValidGroup(TargetGroupHandle) == false)
            {
                //we have to create or find a group to put this layer into
                if (m_AllGroups.Count == 0)
                {
                    grp = CreateGroup("Data Layers", -1);
                    m_GroupPositions[grp.Handle] = m_AllGroups.Count - 1;

                    //Christian Degrassi 2010-02-25: This fixes issue 1622
                    FireGroupAdded(grp.Handle);
                }
                else
                {
                    grp = (Group)m_AllGroups[m_AllGroups.Count - 1];
                }
            }
            else
            {
                grp = (Group)m_AllGroups[(int)m_GroupPositions[TargetGroupHandle]];
            }

            Layer lyr = CreateLayer(MapLayerHandle, newLayer);
            lyr.HideFromLegend = !LegendVisible;

            bool inserted = false;
            if (afterLayerHandle != -1)
            {
                for (int i = 0; i < grp.Layers.Count; i++)
                {
                    if (grp.Layers[i].Handle == afterLayerHandle)
                    {
                        grp.Layers.Insert(i, lyr);
                        inserted = true;
                        break;
                    }
                }
            }
            if (!inserted)
                grp.Layers.Add(lyr);
            grp.RecalcHeight();

            grp.UpdateGroupVisibility();

            // don't show preview for the large layer by default
            MapWinGIS.Shapefile sf = (newLayer as MapWinGIS.Shapefile);

            if (LegendVisible)
            {
                m_SelectedGroupHandle = grp.Handle;
                m_SelectedLayerHandle = MapLayerHandle;

                FireLayerSelected(MapLayerHandle);
            }

            m_Map.LockWindow(MapWinGIS.tkLockMode.lmUnlock);
            Redraw();

            //Christian Degrassi 2010-02-25: This fixes issue 1622
            FireLayerAdded(MapLayerHandle);

            return MapLayerHandle;
        }

        /// <summary>
        /// Adds a layer to the topmost Group
        /// </summary>
        /// <param name="newLayer">object to be added as new layer</param>
        /// <param name="Visible">Should this layer to be visible?</param>
        /// <returns>Handle to the Layer on success, -1 on failure</returns>
        protected internal int AddLayer(object newLayer, bool Visible)
        {
            return AddLayer(newLayer, Visible, -1, true);
        }

        /// <summary>
        /// Assigns all layers outside group to a new group. This allows normal functioning of the legend after map deserialization.
        /// </summary>
        public int AssignOrphanLayersToNewGroup(string groupName)
        {
            Group g = this.Groups.GroupByName(groupName);
            if (g == null)
            {
                int groupHandle = this.Groups.Add(groupName);
                g = this.Groups.ItemByHandle(groupHandle);
            }

            for (int i = 0; i < this.Map.NumLayers; i++)
            {
                int handle = this.Map.get_LayerHandle(i);
                Layer lyr = CreateLayer(handle, (object)this.Map.get_GetObject(handle));
                g.Layers.Add(lyr);
            }
            return g.Handle;
        }
        #endregion

        private Layer CreateLayer(int Handle, object newLayer)
        {
            Layer lyr = new Layer(this);
            lyr.m_Handle = Handle;
            lyr.Type = GetLayerType(newLayer);
            if (lyr.Type == eLayerType.Image)
            {
                lyr.HasTransparency = HasTransparency(newLayer);
            }

            return lyr;
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend
        /// </summary>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot()
        {
            return Snapshot(false, 200);
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend
        /// </summary>
        /// <param name="imgWidth">Width in pixels of the desired Snapshot</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(int imgWidth)
        {
            return Snapshot(false, imgWidth);
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend
        /// </summary>
        /// <param name="VisibleLayersOnly">Only visible layers used in Snapshot?</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(bool VisibleLayersOnly)
        {
            return Snapshot(VisibleLayersOnly, 200);
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend with specified font.
        /// </summary>
        /// <param name="VisibleLayersOnly">Only visible layers used in Snapshot?</param>
        /// <param name="useFont">Which font to use for legend text?</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(bool VisibleLayersOnly, Font useFont)
        {
            Font o = m_Font;
            m_Font = useFont;
            Bitmap b = Snapshot(VisibleLayersOnly, 200);
            m_Font = o;
            return b;
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend with specified font and width.
        /// </summary>
        /// <param name="VisibleLayersOnly">Only visible layers used in Snapshot?</param>
        /// <param name="imgWidth">Width of the image.</param>
        /// <param name="useFont">Which font to use for legend text?</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(bool VisibleLayersOnly, int imgWidth, Font useFont)
        {
            Font o = m_Font;
            m_Font = useFont;
            Bitmap b = Snapshot(VisibleLayersOnly, imgWidth);
            m_Font = o;
            return b;
        }

        /// <summary>
        /// Gets a snapshot of all layers within the legend with specified font and width.
        /// </summary>
        /// <param name="VisibleLayersOnly">Only visible layers used in Snapshot?</param>
        /// <param name="imgWidth">Width of the image.</param>
        /// <param name="useFont">Which font to use for legend text?</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(bool VisibleLayersOnly, int imgWidth, Font useFont, Color foreColor)
        {
            Color fore = this.ForeColor;
            this.ForeColor = foreColor;
            
            Font o = m_Font;
            m_Font = useFont;

            Bitmap b = null;
            try
            {
                this.Lock();
                b = Snapshot(VisibleLayersOnly, imgWidth, useFont);
                
            }
            finally
            {
                this.ForeColor = fore;
                m_Font = o;
                this.Unlock();
            }
            return b;
        }


        /// <summary>
        /// Gets a snapshot of all layers within the legend
        /// </summary>
        /// <param name="VisibleLayersOnly">Only visible layers used in Snapshot?</param>
        /// <param name="imgWidth">Width in pixels of the desired Snapshot</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        public System.Drawing.Bitmap Snapshot(bool VisibleLayersOnly, int imgWidth)
        {
            int imgHeight = 0;// = CalcTotalDrawHeight(true);
            Bitmap bmp = null;// = new Bitmap(imgWidth,imgHeight);
            Rectangle rect = new Rectangle(0, 0, 0, 0);
            int GrpCount, i;
            Layer lyr;
            int LyrHeight;

            try
            {
                System.Drawing.Graphics g;

                if (VisibleLayersOnly == true)
                {
                    ArrayList VisibleLayers = new ArrayList();

                    //figure out how big the img needs to be in height
                    for (i = m_LayerManager.Count - 1; i >= 0; i--)
                    {
                        lyr = m_LayerManager[i];
                        if (lyr.Visible && !lyr.HideFromLegend)
                        {
                            imgHeight += lyr.CalcHeight(true) - 1;
                            VisibleLayers.Add(lyr);
                        }
                    }

                    imgHeight += Constants.ITEM_PAD;

                    bmp = new Bitmap(imgWidth, imgHeight, this.CreateGraphics());
                    g = Graphics.FromImage(bmp);
                    g.Clear(this.BackColor);

                    if (VisibleLayers.Count > 0)
                    {	//set up the boundaries for the first layer
                        LyrHeight = ((Layer)VisibleLayers[0]).CalcHeight(true);
                        rect = new Rectangle(2, 2, imgWidth - 4, LyrHeight - 1);
                    }

                    for (i = 0; i < VisibleLayers.Count; i++)
                    {
                        lyr = (Layer)VisibleLayers[i];

                        DrawLayer(g, lyr, rect, true);

                        LyrHeight = lyr.CalcHeight(true);

                        rect.Y += LyrHeight - 1;
                        rect.Height = LyrHeight;
                    }
                }
                else
                {//draw all layers
                    GrpCount = m_GroupManager.Count;
                    Group grp;
                    int LyrCount;

                    imgHeight = 0;

                    //figure out how tall the image is going to need to be
                    for (i = GrpCount - 1; i >= 0; i--)
                    {
                        grp = m_GroupManager[i];
                        LyrCount = grp.Layers.Count;
                        for (int j = LyrCount - 1; j >= 0; j--)
                        {
                            lyr = (Layer)grp.Layers[j];
                            if (!lyr.HideFromLegend)
                            {
                                imgHeight += lyr.CalcHeight(true) - 1;
                            }
                        }
                    }

                    imgHeight += Constants.ITEM_PAD;

                    //create a new bitmap of the right size, then create a graphics object from the bitmap
                    bmp = new Bitmap(imgWidth, imgHeight, this.CreateGraphics());
                    g = Graphics.FromImage(bmp);
                    g.Clear(this.BackColor);

                    rect = new Rectangle(2, 2, imgWidth - 4, imgHeight - 1);

                    //now draw the snapshot
                    for (i = GrpCount - 1; i >= 0; i--)
                    {
                        grp = m_GroupManager[i];
                        LyrCount = grp.Layers.Count;
                        for (int j = LyrCount - 1; j >= 0; j--)
                        {
                            lyr = (Layer)grp.Layers[j];
                            if (!lyr.HideFromLegend)
                            {
                                this.DrawLayer(g, lyr, rect, true);

                                LyrHeight = lyr.CalcHeight(true);

                                rect.Y += LyrHeight - 1;
                                rect.Height = LyrHeight;
                            }
                        }
                    }
                }

                return bmp;
            }
            catch (Exception ex)
            {
                System.Diagnostics.Debug.Print("Error: Legend.Snaphot. " + ex.Message);
                return null;
            }
        }

        /// <summary>
        /// Gets a snapshot of a specific layer
        /// </summary>
        /// <param name="LayerHandle">Handle of the requested layer</param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        protected internal System.Drawing.Bitmap LayerSnapshot(int LayerHandle)
        {
            return LayerSnapshot(LayerHandle, 200);
        }

        /// <summary>
        /// Gets a snapshot of a specific layer
        /// </summary>
        /// <param name="LayerHandle">Handle of the requested layer</param>
        /// <param name="imgWidth"></param>
        /// <returns>Bitmap if successful, null (nothing) otherwise</returns>
        protected internal Bitmap LayerSnapshot(int LayerHandle, int imgWidth)
        {
            if (Layers.IsValidHandle(LayerHandle) == false)
                return null;

            Layer lyr = Layers.ItemByHandle(LayerHandle);

            Bitmap bmp;
            Graphics g;
            int LyrHeight = lyr.CalcHeight(true);
            bmp = new Bitmap(imgWidth, LyrHeight);
            g = Graphics.FromImage(bmp);

            Rectangle rect = new Rectangle(0, 0, imgWidth - 1, LyrHeight - 1);
            DrawLayer(g, lyr, rect, true);

            return bmp;
        }

        internal bool HasTransparency(object newLayer)
        {
            try
            {
                MapWinGIS.Image img = newLayer as MapWinGIS.Image;
                if (img != null)
                {
                    if (img.UseTransparencyColor == true)
                        return true;
                }
            }
            catch (System.Exception ex)
            {
                string str = ex.Message;
            }

            return false;
        }

        private eLayerType GetLayerType(object newLayer)
        {
            //see if this is a shapefile
            MapWinGIS.Shapefile shpfile = null;
            //MapWinGIS.Image img = null;

            //try
            //{
            //    shpfile = (MapWinGIS.Shapefile)newLayer;
            //}
            //catch (System.InvalidCastException)
            //{
            //}

            //try
            //{
            //    img = (MapWinGIS.Image)newLayer;
            //}
            //catch (System.InvalidCastException)
            //{
            //}

            //Christian Degrassi 2010-03-15: This fixes dirty exceptions related to issue 1643
            if (newLayer is MapWinGIS.Image && newLayer != null)
            {
                return eLayerType.Image;
            }
            else if (newLayer is MapWinGIS.Shapefile && (shpfile = (MapWinGIS.Shapefile)newLayer) != null)
            {
                if (shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINT
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINTM
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINTZ
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINT
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINTM
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINTZ)
                    return eLayerType.PointShapefile;
                else if (shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINE
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINEM
                    || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINEZ)
                    return eLayerType.LineShapefile;
                else
                    return eLayerType.PolygonShapefile;
            }

            //if (img != null)
            //{
            //    return eLayerType.Image;
            //}
            //else if (shpfile != null)
            //{
            //    if (shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINT
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINTM
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POINTZ
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINT
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINTM
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_MULTIPOINTZ)
            //        return eLayerType.PointShapefile;
            //    else if (shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINE
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINEM
            //        || shpfile.ShapefileType == MapWinGIS.ShpfileType.SHP_POLYLINEZ)
            //        return eLayerType.LineShapefile;
            //    else
            //        return eLayerType.PolygonShapefile;
            //}

            return eLayerType.Invalid;
        }

       
        /// <summary>
        /// Tells you if a group exists with the specified handle
        /// </summary>
        /// <param name="Handle">Handle of the group to check</param>
        /// <returns>True if the Group extists, False otherwise</returns>
        protected internal bool IsValidGroup(int Handle)
        {
            if (Handle >= 0 && Handle < m_GroupPositions.Count)
            {
                if ((int)m_GroupPositions[Handle] >= 0)
                    return true;
            }
            return false;
        }

        /// <summary>
        /// Finds a layer given the handle
        /// </summary>
        /// <param name="Handle">Handle of the layer to find</param>
        /// <param name="GroupIndex">ByRef parameter that receives the index to the containing group</param>
        /// <param name="LayerIndex">ByRef parameter that receives the index of the Layer within the group</param>
        /// <returns>Layer on success, null (nothing) otherwise</returns>
        protected internal Layer FindLayerByHandle(int Handle, out int GroupIndex, out int LayerIndex)
        {
            GroupIndex = -1;
            LayerIndex = -1;

            int GroupCount, ItemCount;
            GroupCount = m_AllGroups.Count;
            Group grp;
            Layer lyr;

            for (int i = 0; i < GroupCount; i++)
            {
                grp = (Group)m_AllGroups[i];
                ItemCount = grp.Layers.Count;
                for (int j = 0; j < ItemCount; j++)
                {
                    lyr = (Layer)grp.Layers[j];
                    if (lyr.Handle == Handle)
                    {
                        GroupIndex = i;
                        LayerIndex = j;
                        return lyr;
                    }
                }
            }
            return null;
        }

        /// <summary>
        /// Finds a layer by handle
        /// </summary>
        /// <param name="Handle">Handle of the layer to lookup</param>
        /// <returns>Layer if Successful, null (nothing) otherwise</returns>
        protected internal Layer FindLayerByHandle(int Handle)
        {
            int GroupIndex, LayerIndex;
            return FindLayerByHandle(Handle, out GroupIndex, out LayerIndex);
        }

        private void DrawCheckBox(Graphics DrawTool, int ItemTop, int ItemLeft, bool DrawCheck, bool DrawGrayBackground)
        {
            Image icon;
            int index = 0;
            if (DrawCheck == true)
            {
                if (DrawGrayBackground == true)
                    index = cCheckedBoxGrayIcon;
                else
                    index = cCheckedBoxIcon;
            }
            else
            {
                if (DrawGrayBackground == true)
                    index = cUnCheckedBoxGrayIcon;
                else
                    index = cUnCheckedBoxIcon;
            }
            icon = Icons.Images[index];
            DrawPicture(DrawTool, ItemLeft, ItemTop, Constants.CHECK_BOX_SIZE, Constants.CHECK_BOX_SIZE, icon);
        }

        /// <summary>
        /// Draws picture in the legend. Picture can be either an image or an icon
        /// </summary>
        private void DrawPicture(Graphics DrawTool, int PicLeft, int PicTop, int PicWidth, int PicHeight, object picture)
        {
            if (picture == null) return;

            SmoothingMode oldSM = DrawTool.SmoothingMode;
            DrawTool.SmoothingMode = SmoothingMode.HighQuality;

            Rectangle rect = new Rectangle(PicLeft, PicTop, PicWidth, PicHeight);

            Icon icon = null;
            if (picture is Icon)
            {
                icon = (Icon)picture;
            }

            if (icon != null)
            {
                DrawTool.DrawIcon(icon, rect);
            }
            else
            {
                //try casting it to an Image
                Image img = null;
                try { img = (Image)picture; }
                catch (InvalidCastException) { }

                if (img != null)
                {
                    DrawTool.DrawImage(img, rect);
                }
                else
                {
                    MapWinGIS.Image mwImg = null;
                    try { mwImg = (MapWinGIS.Image)picture; }
                    catch (InvalidCastException) { }
                    if (mwImg != null)
                    {
                        try
                        {
                            img = Image.FromHbitmap(new System.IntPtr(mwImg.Picture.Handle));
                            DrawTool.DrawImage(img, rect);
                        }
                        catch (Exception) { }
                    }
                    mwImg = null;
                }
            }

            DrawTool.SmoothingMode = oldSM;
        }

        /// <summary>
        /// Expansion box with plus or minus sign
        /// </summary>
        /// <param name="DrawTool"></param>
        /// <param name="ItemTop"></param>
        /// <param name="ItemLeft"></param>
        /// <param name="Expanded"></param>
        private void DrawExpansionBox(Graphics DrawTool, int ItemTop, int ItemLeft, bool Expanded)
        {
            int MidX, MidY;
            Rectangle rect;

            Pen pen = new Pen(m_BoxLineColor, 1);

            rect = new Rectangle(ItemLeft, ItemTop, Constants.EXPAND_BOX_SIZE, Constants.EXPAND_BOX_SIZE);

            //draw the border
            DrawBox(DrawTool, rect, m_BoxLineColor, Color.White);

            MidX = (int)(rect.Left + .5 * (rect.Width));
            MidY = (int)(rect.Top + .5 * (rect.Height));

            if (Expanded == false)
            {//draw a + sign, indicating that there is more to be seen
                //draw the vertical part
                DrawTool.DrawLine(pen, MidX, ItemTop + 2, MidX, ItemTop + Constants.EXPAND_BOX_SIZE - 2);

                //draw the horizontal part
                DrawTool.DrawLine(pen, ItemLeft + 2, MidY, ItemLeft + Constants.EXPAND_BOX_SIZE - 2, MidY);
            }
            else
            {//draw a - sign
                DrawTool.DrawLine(pen, ItemLeft + 2, MidY, ItemLeft + Constants.EXPAND_BOX_SIZE - 2, MidY);
            }
        }

        /// <summary>
        /// Locks the Legend, stopping it from redrawing until it is unlocked.
        /// Use this as a way of adding multiple layers without redrawing between each layer being added.
        /// Make sure to Unlock the Legend when done.
        /// </summary>
        public void Lock()
        {
            m_LockCount++;
        }

        /// <summary>
        /// Unlocks the legend.  See Lock() function for description
        /// </summary>
        public void Unlock()
        {
            if (m_LockCount > 0)
                m_LockCount--;
            if (m_LockCount == 0)
                Redraw();
        }

        /// <summary>
        /// Provides a high-quality snapshot with adjustable resolution and number of columns.
        /// </summary>
        /// <param name="LayerHandle">Layer handle.</param>
        /// <param name="Width">Desired width of image.</param>
        /// <param name="Columns">Number of desired columns.</param>
        /// <param name="FontFamily">Font family to use, e.g. "Arial"</param>
        /// <param name="MinFontSize">Min font size - will start from here and increae until used all width.</param>
        /// <param name="MaxFontSize">Max font size - will go no higher than this.</param>
        /// <param name="UnderlineLayerTitles">Underline the main layer titles?</param>
        /// <param name="BoldLayerTitles">Make the main layer titles bold?</param>
        /// <returns></returns>
        /// Chris Michaelis, January 2008
        public System.Drawing.Bitmap SnapshotHQ(int LayerHandle, int Width, int Columns, string FontFamily, int MinFontSize, int MaxFontSize, bool UnderlineLayerTitles, bool BoldLayerTitles)
        {
            int CurLeft = 0,
                CurTop = 0;

            Layer lyr = m_LayerManager.ItemByHandle(LayerHandle);
            Bitmap b = new Bitmap(Width, 1000);

            // Establish the size of this image
            string TestString = m_Map.get_LayerName(lyr.Handle); // ...width estimate - start with layer name, move out to largest break caption
            object o = m_Map.GetColorScheme(lyr.Handle);
            if (o != null && o is MapWinGIS.GridColorScheme)
            {
                MapWinGIS.GridColorScheme t = (MapWinGIS.GridColorScheme)o;
                for (int i = 0; i < t.NumBreaks; i++)
                {
                    if (t.get_Break(i).Caption.Length > TestString.Length) TestString = t.get_Break(i).Caption;
                }
            }
            else if (o != null && o is MapWinGIS.ShapefileColorScheme)
            {
                MapWinGIS.ShapefileColorScheme t = (MapWinGIS.ShapefileColorScheme)o;
                for (int i = 0; i < t.NumBreaks(); i++)
                {
                    if (t.get_ColorBreak(i).Caption.Length > TestString.Length) TestString = t.get_ColorBreak(i).Caption;
                }
            }

            Graphics g = Graphics.FromImage(b);
            int currSize = MinFontSize;
            SizeF estSize = g.MeasureString(TestString.ToUpper(), new Font(FontFamily, currSize));
            // Multicolumn sizing mode - font must fit in single column - if Cols > 1
            // If cols == 1, width/1 = width
            while (estSize.Width < ((Width / Columns) * .95)) // slight tolerance
            {
                if (currSize + 1 > MaxFontSize) break;
                currSize += 1;
                estSize = g.MeasureString(TestString, new Font(FontFamily, currSize));
            }

            Font f = new Font(FontFamily, currSize);
            int LineSpacing = (int)(estSize.Height * 0.25); // one quarter line height = spacing
            int boxSize = (int)g.MeasureString("X", f).Width;

            // Recreate image with desired size
            int itemMultiplier = (Math.Max(1, lyr.ColorLegend.Count) / Columns);
            // Add in either the height of the breaks + spacig, or spacing + the one box for the color
            int newHeight = (int)(estSize.Height + (lyr.ColorLegend.Count > 1 ? (estSize.Height * 1.25 * (lyr.ColorLegend.Count)) : estSize.Height * 1.25));
            // Do we have multiple columns and enough legend items for it to matter?
            if (Columns > 1 && lyr.ColorLegend.Count > 1)
            {
                // Subtract off some height to account for multi column mode
                // Done with three temporary ints to make this more readible
                int MaxPerCol = (int)Math.Ceiling((double)lyr.ColorLegend.Count / Columns);
                int SubtractPerExtraItem = (int)(estSize.Height * 1.25);
                int ExcessItemsPerCol = lyr.ColorLegend.Count - MaxPerCol;
                newHeight -= SubtractPerExtraItem * ExcessItemsPerCol;
            }
            estSize = new SizeF(Width, newHeight);
            g.Dispose();
            b.Dispose();
            b = new Bitmap((int)estSize.Width, (int)estSize.Height);
            g = Graphics.FromImage(b);

            // Layer Title
            g.DrawString(m_Map.get_LayerName(lyr.Handle), new Font(f.FontFamily, f.Size, (BoldLayerTitles ? FontStyle.Bold : FontStyle.Regular) | (UnderlineLayerTitles ? FontStyle.Underline : FontStyle.Regular)), Brushes.Black, CurLeft, CurTop);
            CurTop += (int)g.MeasureString(m_Map.get_LayerName(lyr.Handle), f).Height + LineSpacing;

            int ColumnCurTop = CurTop; // Layer name covers all columns of the legend
            int ItemsPerColumn = (lyr.ColorLegend.Count > 0 ? ((int)Math.Floor((lyr.ColorLegend.Count / Columns) + 0.5)) : lyr.ColorLegend.Count);

            int LastColumnUsed = 1;

            // Draw a single symbol if there are no breaks
            if (lyr.ColorLegend.Count == 0)
            {
                DrawHQLayerSymbol(g, lyr, CurLeft + 4, CurTop, boxSize);
            }
            else
            {
                // Otherwise, draw the breaks into the columns
                long NumBreaks = lyr.ColorLegend.Count;
                int CurrentColumn = 1;

                CurLeft = LineSpacing;

                for (int p = 0; p < NumBreaks; p++)
                {
                    ColorInfo ci = (ColorInfo)lyr.ColorLegend[p];
                    if (ci.IsTransparent == false)
                    {
                        if (lyr.Type == eLayerType.Grid || lyr.Type == eLayerType.Image)
                        {
                            DrawColorPatch(g, ci.StartColor, ci.EndColor, CurTop, CurLeft, boxSize, boxSize, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), false);
                        }
                        else
                        {
                            System.Drawing.Color sc = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.StartColor);
                            System.Drawing.Color ec = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.EndColor);
                            DrawColorPatch(g, sc, ec, CurTop, CurLeft, boxSize, boxSize, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), (m_Map.get_ShapeLayerLineWidth(lyr.Handle) != 0));
                        }
                    }
                    else
                    {
                        DrawTransparentPatch(g, CurTop, CurLeft, boxSize, boxSize, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), (m_Map.get_ShapeLayerLineWidth(lyr.Handle) != 0));
                    }

                    // Old - Untrimmed to column width:
                    // g.DrawString(ci.Caption, f, Brushes.Black, CurLeft + boxSize, CurTop);

                    // New - Trimmed to column width:
                    string ActualText = ci.Caption;
                    while (g.MeasureString(ActualText, f).Width >= (Width / Columns) - LineSpacing - boxSize)
                    {
                        ActualText = ActualText.Remove(ActualText.Length - 1, 1);
                    }
                    g.DrawString(ActualText, f, Brushes.Black, CurLeft + boxSize, CurTop);

                    LastColumnUsed = CurrentColumn;

                    CurTop += boxSize + LineSpacing;
                    if (p % ItemsPerColumn == 0 && p != 0) // Last one in this column
                    {
                        CurrentColumn += 1;
                        CurTop = ColumnCurTop;
                        CurLeft = ((Width / Columns) * (CurrentColumn - 1)) + LineSpacing;
                    }
                }
            }

            g.Dispose();

            if (LastColumnUsed < Columns)
            {
                // Image will be bigger than it needs to be. Shrink
                // it by columns - lastcolumnused
                System.Drawing.Bitmap newB = new Bitmap(b.Width - ((Width / Columns) * (Columns - LastColumnUsed)), b.Height);
                Graphics newG = Graphics.FromImage(newB);
                newG.DrawImageUnscaled(b, 1, 1);
                newG.Dispose();
                return newB;
            }

            return b;
        }

        //BM Wrote this stuff to allow getting a single color patch from the reports interface
        //May. 18, 2009
        public void DrawHQLayerSymbolBreaks(Graphics g, int LayerHandle, int category, int CurTop, int CurLeft, int Width, int Height)
        {
            Layer lyr = m_LayerManager.ItemByHandle(LayerHandle);
            ColorInfo ci = new ColorInfo();
            if (lyr.ColorLegend.Count >= 1)
                ci = (ColorInfo)lyr.ColorLegend[category];
            else
            {
                ci.StartColor = Colors.UintToColor(m_Map.get_ShapeLayerFillColor(lyr.Handle));
                ci.EndColor = Colors.UintToColor(m_Map.get_ShapeLayerLineColor(lyr.Handle));
            }
            if (lyr.Type == eLayerType.Grid || lyr.Type == eLayerType.Image)
            {
                DrawColorPatch(g, ci.StartColor, ci.EndColor, CurTop, CurLeft, Width, Height, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), false);
            }
            else
            {
                System.Drawing.Color sc = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.StartColor);
                System.Drawing.Color ec = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.EndColor);
                DrawLayerSymbolHQ(g, lyr, CurLeft, CurTop, Width, sc, ec);
            }
        }

        private void DrawHQLayerSymbol(Graphics g, Layer lyr, int Left, int Top, int Dimension)
        {
            //draw Layer Type symbol
            if (lyr.Icon == null)
            {
                uint Fill = 0,
                    Line = 0;

                if (lyr.Type == eLayerType.LineShapefile)
                    Fill = m_Map.get_ShapeLayerLineColor(lyr.Handle);
                else if (lyr.Type == eLayerType.PointShapefile)
                    Fill = m_Map.get_ShapeLayerPointColor(lyr.Handle);
                else if (lyr.Type == eLayerType.PolygonShapefile)
                {
                    Fill = m_Map.get_ShapeLayerFillColor(lyr.Handle);
                    Line = m_Map.get_ShapeLayerLineColor(lyr.Handle);
                }

                Color FillColor = Colors.UintToColor(Fill);
                Color LineColor = Colors.UintToColor(Line);
                DrawLayerSymbolHQ(g, lyr, Left, Top, Dimension, FillColor, LineColor);
            }
            else
            {
                DrawPicture(g, Left, Top, Dimension, Dimension, lyr.Icon);
            }
        }

        /// <summary>
        /// Drawing procedure for the new symbology
        /// </summary>
        /// <param name="DrawTool"></param>
        /// <param name="lyr"></param>
        /// <param name="bounds"></param>
        /// <param name="IsSnapshot"></param>
        protected internal void DrawLayerExt(Graphics DrawTool, Layer lyr, Rectangle bounds, bool IsSnapshot)
        {
        }

        /// <summary>
        /// Draws a layer onto a given graphics surface
        /// </summary>
        /// <param name="DrawTool">Graphics surface (object) onto which the give layer should be drawn</param>
        /// <param name="lyr">Layer object to be drawn</param>
        /// <param name="bounds">Rectangle oulining the allowable draw area</param>
        /// <param name="IsSnapshot">Drawing is done differently when it is a snapshot we are takeing of this layer</param>
        protected internal void DrawLayer(Graphics DrawTool, Layer lyr, Rectangle bounds, bool IsSnapshot)
        {
            int CurLeft,
                CurTop,
                CurWidth,
                CurHeight;
            Rectangle rect;

            lyr.m_smallIconWasDrawn = false;
            lyr.Top = bounds.Top;
            lyr.Elements.Clear();

            CurLeft = bounds.Left;
            CurTop = bounds.Top;
            CurWidth = bounds.Width - Constants.ITEM_RIGHT_PAD;
            CurHeight = lyr.Height;
            rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

            // ------------------------------------------------------
            //  drawing background (with selection if needed)
            // ------------------------------------------------------
            if (IsSnapshot == false)
            {
                CurLeft = bounds.Left;
                CurTop = bounds.Top;
                CurWidth = bounds.Width - Constants.ITEM_RIGHT_PAD;
                CurHeight = lyr.Height;
                rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                if (lyr.Handle == m_SelectedLayerHandle && bounds.Width > 25)
                {
                    // selects the title only
                    if (m_Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology)
                        rect.Height = Constants.ITEM_HEIGHT;
                    else
                        rect.Height = CurHeight;

                    if (CurTop + rect.Height > 0 || CurTop < this.ClientRectangle.Height)
                    {
                        DrawBox(DrawTool, rect, m_BoxLineColor, m_SelectedColor);
                    }
                }
            }
            else
            {
                CurLeft = bounds.Left;
                CurTop = bounds.Top;
                CurWidth = bounds.Width - 1;
                CurHeight = lyr.CalcHeight(true) - 1;
                rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                DrawBox(DrawTool, rect, m_BoxLineColor, System.Drawing.Color.White);
                // MessageBox.Show("IsSnapshot");
            }

            // -------------------------------------------------------
            //  drawing checkbox
            // -------------------------------------------------------
            if (bounds.Width > 55 && IsSnapshot == false)
            {
                CurTop = bounds.Top + Constants.CHECK_TOP_PAD;
                CurLeft = bounds.Left + Constants.CHECK_LEFT_PAD;

                bool visible = true;
                if (m_Map.ShapeDrawingMethod != MapWinGIS.tkShapeDrawingMethod.dmNewSymbology)
                {
                    visible = m_Map.get_LayerVisible(lyr.Handle);
                }
                else
                {
                    if (lyr.UseDynamicVisibility)
                    {
                        visible = (m_Map.CurrentScale >= lyr.MinVisibleScale) && (m_Map.CurrentScale <= lyr.MaxVisibleScale)
                                   && m_Map.Tiles.CurrentZoom >= lyr.MinVisibleZoom && m_Map.Tiles.CurrentZoom <= lyr.MaxVisibleZoom;
                    }
                    visible = visible && m_Map.get_LayerVisible(lyr.Handle);
                }

                DrawCheckBox(DrawTool, CurTop, CurLeft, visible, lyr.UseDynamicVisibility); // draw a grey background if the layer is in dynamic visibility mode.
            }

            // ----------------------------------------------------------
            //    Drawing text
            // ----------------------------------------------------------
            SizeF textSize = new SizeF(0.0f, 0.0f);
            if (bounds.Width > 60)
            {
                //draw text
                string text = m_Map.get_LayerName(lyr.Handle);
                textSize = DrawTool.MeasureString(text, m_Font);

                if (IsSnapshot == true)
                    CurLeft = bounds.Left + Constants.CHECK_LEFT_PAD;
                else
                    CurLeft = bounds.Left + Constants.TEXT_LEFT_PAD;

                CurTop = bounds.Top + Constants.TEXT_TOP_PAD;
                //CurWidth = bounds.Width - CurLeft - Constants.TEXT_RIGHT_PAD;
                CurWidth = bounds.Width - Constants.TEXT_RIGHT_PAD - 27;
                CurHeight = Constants.TEXT_HEIGHT;

                rect = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
                DrawText(DrawTool, text, rect, m_Font, this.ForeColor);

                LayerElement el = new LayerElement(LayerElementType.Name, rect, text);
                lyr.Elements.Add(el);
            }

            // -------------------------------------------------------------
            //    Drawing layer icon
            // -------------------------------------------------------------
            if (m_Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology)
            {
                if (bounds.Width > 60 && bounds.Right - CurLeft - 41 > textSize.Width)  // -5 (offset)
                {
                    int top = bounds.Top + Constants.ICON_TOP_PAD;
                    int left = bounds.Right - 36;
                    Image icon;

                    if (lyr.Icon != null)
                    {
                        DrawPicture(DrawTool, left, CurTop, Constants.ICON_SIZE, Constants.ICON_SIZE, lyr.Icon);
                    }
                    else if (lyr.Type == eLayerType.Image)
                    {
                        icon = Icons.Images[cImageIcon];
                        DrawPicture(DrawTool, left, top, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                    }
                    else if (lyr.Type == eLayerType.Grid)
                    {
                        icon = Icons.Images[cGridIcon];
                        DrawPicture(DrawTool, left, top, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                    }
                    else
                    {
                        // drawing shapefile symbology preview, but only in case the layer is collapsed
                        if (!lyr.Expanded)
                        {
                            lyr.m_smallIconWasDrawn = true;
                            Rectangle iconBounds = new Rectangle(left, top, Constants.ICON_SIZE, Constants.ICON_SIZE);

                            // drawing category symbol
                            IntPtr hdc = DrawTool.GetHdc();
                            Color clr = (lyr.Handle == m_SelectedLayerHandle && bounds.Width > 25) ? m_SelectedColor : this.BackColor;
                            uint backColor = Convert.ToUInt32(ColorTranslator.ToOle(clr));

                            MapWinGIS.Shapefile sf = m_Map.get_GetObject(lyr.Handle) as MapWinGIS.Shapefile;

                            if (lyr.Type == eLayerType.PointShapefile)
                                sf.DefaultDrawingOptions.DrawPoint(hdc, left, top, Constants.ICON_SIZE, Constants.ICON_SIZE, backColor);
                            else if (lyr.Type == eLayerType.LineShapefile)
                                sf.DefaultDrawingOptions.DrawLine(hdc, left, top, Constants.ICON_SIZE - 1, Constants.ICON_SIZE - 1, false, Constants.ICON_SIZE, Constants.ICON_SIZE, backColor);
                            else if (lyr.Type == eLayerType.PolygonShapefile)
                                sf.DefaultDrawingOptions.DrawRectangle(hdc, left, top, Constants.ICON_SIZE - 1, Constants.ICON_SIZE - 1, false, Constants.ICON_SIZE, Constants.ICON_SIZE, backColor);

                            DrawTool.ReleaseHdc(hdc);
                        }
                    }
                }

                //  labels link
                if (bounds.Width > 60 && bounds.Right - CurLeft - 62 > textSize.Width)   // -62
                {
                    var sf = m_Map.Shapefile[lyr.Handle];
                    if (sf != null)
                    {
                        int top = bounds.Top + Constants.ICON_TOP_PAD;
                        int left = bounds.Right - 56;

                        //Image icon = null;
                        double scale = m_Map.CurrentScale;
                        bool labelsVisible = sf.Labels.Count > 0 && sf.Labels.Visible && sf.Labels.Expression.Trim() != "";
                        labelsVisible &= scale >= sf.Labels.MinVisibleScale && scale <= sf.Labels.MaxVisibleScale;
                        Image icon = labelsVisible ? Icons.Images[cActiveLabelIcon] : Icons.Images[cDimmedLabelIcon];
                        DrawPicture(DrawTool, left, top, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                    }
                }

                //  editing icon
                if (bounds.Width > 60 && bounds.Right - CurLeft - 82 > textSize.Width)
                {
                    var sf = m_Map.Shapefile[lyr.Handle];
                    if (sf != null && sf.InteractiveEditing)
                    {
                        int top = bounds.Top + Constants.ICON_TOP_PAD;
                        int left = bounds.Right - 76;

                        Image icon = Icons.Images[cEditing];
                        DrawPicture(DrawTool, left, top, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                    }
                }
            }
            else
            {
                if (bounds.Width > 60)
                {
                    //draw Layer Type symbol
                    if (lyr.Icon == null)
                    {
                        uint Fill = 0,
                            Line = 0;

                        if (lyr.Type == eLayerType.LineShapefile)
                            Fill = m_Map.get_ShapeLayerLineColor(lyr.Handle);
                        else if (lyr.Type == eLayerType.PointShapefile)
                            Fill = m_Map.get_ShapeLayerPointColor(lyr.Handle);
                        else if (lyr.Type == eLayerType.PolygonShapefile)
                        {
                            Fill = m_Map.get_ShapeLayerFillColor(lyr.Handle);
                            Line = m_Map.get_ShapeLayerLineColor(lyr.Handle);
                        }

                        CurTop = bounds.Top + Constants.ICON_TOP_PAD;
                        CurLeft = bounds.Right - Constants.ICON_RIGHT_PAD;

                        Color FillColor = Colors.UintToColor(Fill);
                        Color LineColor = Colors.UintToColor(Line);
                        DrawLayerSymbol(DrawTool, lyr.Type, CurTop, CurLeft, FillColor, LineColor, lyr.Handle);
                    }
                    else
                    {
                        CurTop = bounds.Top + Constants.ICON_TOP_PAD;
                        CurLeft = bounds.Right - Constants.ICON_RIGHT_PAD;
                        DrawPicture(DrawTool, CurLeft, CurTop, Constants.ICON_SIZE, Constants.ICON_SIZE, lyr.Icon);
                    }
                }
            }

            // -------------------------------------------------------------
            //    Drawing categories and expansion box for shapefiles
            // -------------------------------------------------------------
            if (m_Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology &&
                                                    (lyr.Type == eLayerType.PointShapefile ||
                                                    lyr.Type == eLayerType.LineShapefile ||
                                                    lyr.Type == eLayerType.PolygonShapefile))
            {
                if (bounds.Width > 17 && IsSnapshot == false)
                {
                    rect = new Rectangle(bounds.Left, bounds.Top, bounds.Width - Constants.ITEM_RIGHT_PAD, bounds.Height);
                    DrawExpansionBox(DrawTool, rect.Top + Constants.EXPAND_BOX_TOP_PAD, rect.Left + Constants.EXPAND_BOX_LEFT_PAD, lyr.Expanded);
                }

                // drawing shapefile
                DrawShapefileCategories(DrawTool, lyr, bounds, IsSnapshot);      // drawing of categories for the new symbology

                // drawing image
                //MapWinGIS.Image img = m_Map.get_GetObject(lyr.Handle) as MapWinGIS.Image;
            }
            else
            {
                // ----------------------------------------------------------
                //   Draw the expansion box and sub items (if they exist or if we're being forced)
                // ----------------------------------------------------------
                bool Handled = false;
                if (lyr.Expanded && lyr.ExpansionBoxCustomRenderFunction != null)
                    lyr.ExpansionBoxCustomRenderFunction(lyr.Handle, new Rectangle(bounds.Left + Constants.CHECK_LEFT_PAD, lyr.Top + Constants.ITEM_HEIGHT + Constants.EXPAND_BOX_TOP_PAD, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT - Constants.EXPAND_BOX_LEFT_PAD, bounds.Height - lyr.Top), ref DrawTool, ref Handled);

                // Here, draw the + or - sign according to based on  layer.expanded property
                if (lyr.ExpansionBoxForceAllowed || lyr.ColorLegend.Count > 0)
                {
                    if (bounds.Width > 17 && IsSnapshot == false)
                    {
                        //SetRect(&LocalBounds, bounds.left + LIST_ITEM_INDENT,Top,bounds.right-ITEM_PAD,Top+lyr.Height);
                        rect = new Rectangle(bounds.Left, bounds.Top, bounds.Width - Constants.ITEM_RIGHT_PAD, bounds.Height);
                        DrawExpansionBox(DrawTool, rect.Top + Constants.EXPAND_BOX_TOP_PAD, rect.Left + Constants.EXPAND_BOX_LEFT_PAD, lyr.Expanded);
                    }
                }

                // -------------------------------------------------------------
                //    Drawing old style legend
                // -------------------------------------------------------------
                int p = 0;
                if (!Handled && (lyr.ColorLegend.Count > 0 || lyr.Icon != null))
                {
                    if ((IsSnapshot == true || lyr.Expanded == true) && bounds.Width > 47)
                    {
                        //figure out if we can clip any of the breaks at the top
                        if (bounds.Top + Constants.ITEM_HEIGHT < this.ClientRectangle.Top && IsSnapshot == false)
                        {
                            int Difference = this.ClientRectangle.Top - (bounds.Top + Constants.ITEM_HEIGHT);
                            p = Difference / Constants.CS_ITEM_HEIGHT;
                        }

                        
                        // Header Text
                        if (lyr.ColorSchemeFieldCaption != "")
                        {
                            rect = new Rectangle(bounds.Left + 7, bounds.Top + Constants.ITEM_HEIGHT + p * Constants.CS_ITEM_HEIGHT, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);

                            if (lyr.StippleSchemeFieldCaption != "")
                            {
                                DrawText(DrawTool, "Fill: " + lyr.ColorSchemeFieldCaption, rect, m_Font, Color.Black);
                            }
                            else
                            {
                                DrawText(DrawTool, lyr.ColorSchemeFieldCaption, rect, m_Font, Color.Black);
                            }
                        }
                        rect = new Rectangle(Constants.CS_PATCH_LEFT_INDENT, bounds.Top + Constants.ITEM_HEIGHT, rect.Width - Constants.CS_PATCH_LEFT_INDENT, lyr.CalcHeight(IsSnapshot));

                        long NumBreaks = lyr.ColorLegend.Count;

                        for (; p < NumBreaks; p++)
                        {
                            int offset = p + (lyr.ColorSchemeFieldCaption != "" ? 1 : 0);
                            ColorInfo ci = (ColorInfo)lyr.ColorLegend[p];
                            if (ci.IsTransparent == false)
                            {
                                if (lyr.Type == eLayerType.Grid || lyr.Type == eLayerType.Image)
                                {
                                    DrawColorPatch(DrawTool, ci.StartColor, ci.EndColor, bounds.Top + Constants.ITEM_HEIGHT + offset * Constants.CS_ITEM_HEIGHT, bounds.Left + Constants.CS_PATCH_LEFT_INDENT, Constants.CS_PATCH_HEIGHT, Constants.CS_PATCH_WIDTH, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), true);
                                }
                                else
                                {
                                    System.Drawing.Color sc = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.StartColor);
                                    System.Drawing.Color ec = System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), ci.EndColor);
                                    DrawColorPatch(DrawTool, sc, ec, bounds.Top + Constants.ITEM_HEIGHT + offset * Constants.CS_ITEM_HEIGHT, bounds.Left + Constants.CS_PATCH_LEFT_INDENT, Constants.CS_PATCH_HEIGHT, Constants.CS_PATCH_WIDTH, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), (m_Map.get_ShapeLayerLineWidth(lyr.Handle) != 0), lyr.Type);
                                }
                            }
                            else
                            {
                                DrawTransparentPatch(DrawTool, bounds.Top + Constants.ITEM_HEIGHT + offset * Constants.CS_ITEM_HEIGHT, bounds.Left + Constants.CS_PATCH_LEFT_INDENT, Constants.CS_PATCH_HEIGHT, Constants.CS_PATCH_WIDTH, System.Drawing.ColorTranslator.FromOle(Convert.ToInt32(m_Map.get_ShapeLayerLineColor(lyr.Handle))), (m_Map.get_ShapeLayerLineWidth(lyr.Handle) != 0));
                            }

                            rect = new Rectangle(bounds.Left + Constants.CS_TEXT_LEFT_INDENT, bounds.Top + Constants.ITEM_HEIGHT + offset * Constants.CS_ITEM_HEIGHT, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
                            DrawText(DrawTool, ci.Caption, rect, m_Font, Color.Black);

                            if (rect.Top >= this.ClientRectangle.Bottom && IsSnapshot == false)
                                break;
                        }
                    }
                }
            }
        }

        /// <summary>
        /// Draws color scheme (categories) for the shapefile layer
        /// </summary>
        private void DrawShapefileCategories(Graphics DrawTool, Layer layer, Rectangle bounds, bool IsSnapshot)
        {
            if (layer.Type != eLayerType.PointShapefile &&
                layer.Type != eLayerType.LineShapefile &&
                layer.Type != eLayerType.PolygonShapefile) return;

            if ((!IsSnapshot && !layer.Expanded) || bounds.Width <= 47)
                return;

            var sf = m_Map.get_Shapefile(layer.Handle);
            if (sf == null) return;

            int maxWidth = Constants.ICON_WIDTH; ;
            if (layer.Type == eLayerType.PointShapefile)
                maxWidth = layer.get_MaxIconWidth(sf);

            int top = bounds.Top + Constants.ITEM_HEIGHT + 2;

            // temp
            int val1;
            int height;
            val1 = (layer.GetCategoryHeight(sf.DefaultDrawingOptions) + 2);  // default symbology

            //if (sf.Labels.Count == 0 || (!sf.Labels.Visible) || !this.ShowLabels)
            //{
            height = val1;
            //}
            //else
            //{
            //    string text = "Arizona";
            //    LabelStyle style = new LabelStyle(sf.Labels.Options);
            //    System.Drawing.Bitmap img = new System.Drawing.Bitmap(500, 200);
            //    Graphics g = Graphics.FromImage(img);
            //    Size size = style.MeasureString(g, text, 30);
            //    val2 = size.Height + 2;
            //    height = val1 > val2 ? val1 : val2;

            //    SmoothingMode oldMode = DrawTool.SmoothingMode;
            //    DrawTool.SmoothingMode = SmoothingMode.AntiAlias;

            //    int leftTemp = bounds.Left + Constants.TEXT_LEFT_PAD + maxWidth + 5;
            //    Rectangle textBounds = style.Draw(DrawTool, new Point(leftTemp, top), text, false, 30);

            //    DrawTool.SmoothingMode = oldMode;

            //    // storing rectangle
            //    LayerElement el = new LayerElement(LayerElementType.Label, textBounds, text);
            //    layer.Elements.Add(el);
            //}

            if (top + height > this.ClientRectangle.Top)
            {
                DrawShapefileCategory(DrawTool, sf.DefaultDrawingOptions, layer, bounds, top, "", maxWidth, -1);
            }

            top += height;
            // end temp

            Rectangle rect = new Rectangle();
            if (sf.Categories.Count > 0)
            {
                // categories caption
                string caption = sf.Categories.Caption;
                if (caption == string.Empty) caption = "Categories";
                int left = bounds.Left + Constants.TEXT_LEFT_PAD;
                if (!(top + Constants.TEXT_HEIGHT < 0))
                {
                    rect = new Rectangle(left, top, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
                    DrawText(DrawTool, caption, rect, m_Font, this.ForeColor);
                }
                top += Constants.CS_ITEM_HEIGHT + 2;

                //figure out if we can clip any of the categories at the top
                int i = 0;
                MapWinGIS.ShapefileCategories categories = sf.Categories;
                int numCategories = sf.Categories.Count;
                if (top < this.ClientRectangle.Top && IsSnapshot == false)
                {
                    while (i < numCategories)
                    {
                        // for point categories height can be different
                        top += layer.GetCategoryHeight(categories.get_Item(i).DrawingOptions);

                        if (top < this.ClientRectangle.Top)
                        {
                            i++;
                        }
                        else
                        {
                            top -= layer.GetCategoryHeight(categories.get_Item(i).DrawingOptions);  // this category should be drawn
                            break;
                        }
                    }
                }

                // we shall draw symbology first and text second
                // symbology is drawn from ocx, so it's better to draw all categories at once
                // avoiding additional GetHDC calls
                IntPtr hdc = DrawTool.GetHdc();
                int topTemp = top;
                int startIndex = i;
                for (; i < categories.Count; i++)
                {
                    MapWinGIS.ShapefileCategory cat = categories.get_Item(i);
                    MapWinGIS.ShapeDrawingOptions options = cat.DrawingOptions;

                    DrawShapefileCategorySymbology(DrawTool, options, layer, bounds, topTemp, maxWidth, i, hdc);

                    topTemp += layer.GetCategoryHeight(options);
                    if (topTemp >= this.ClientRectangle.Bottom && IsSnapshot == false)     // stop drawing in case there are not visible
                        break;
                }
                DrawTool.ReleaseHdc(hdc);

                // now when hdc is released, GDI+ can be used for the text
                i = startIndex;
                for (; i < categories.Count; i++)
                {
                    MapWinGIS.ShapefileCategory cat = categories.get_Item(i);
                    MapWinGIS.ShapeDrawingOptions options = cat.DrawingOptions;

                    DrawShapefileCategoryText(DrawTool, options, layer, bounds, top, cat.Name, maxWidth, i);

                    top += layer.GetCategoryHeight(options);
                    if (top >= this.ClientRectangle.Bottom && IsSnapshot == false)     // stop drawing in case there are not visible
                        break;
                }
            }

            // charts
            if (sf.Charts.Count > 0 && sf.Charts.NumFields > 0 && sf.Charts.Visible)
            {
                // charts caption
                string caption = sf.Charts.Caption;
                if (caption == string.Empty)
                    caption = "Charts";

                int left = bounds.Left + Constants.TEXT_LEFT_PAD;
                rect = new Rectangle(left, top, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
                DrawText(DrawTool, caption, rect, m_Font, this.ForeColor);
                top += Constants.CS_ITEM_HEIGHT + 2;

                // storing bounds
                LayerElement el = new LayerElement(LayerElementType.Charts, rect);
                layer.Elements.Add(el);

                // preview
                IntPtr hdc = DrawTool.GetHdc();
                uint backColor = Convert.ToUInt32(ColorTranslator.ToOle(this.BackColor));

                left = bounds.Left + Constants.TEXT_LEFT_PAD;
                sf.Charts.DrawChart(hdc, left, top, true, backColor);
                top += sf.Charts.IconHeight + 2;
                DrawTool.ReleaseHdc(hdc);

                // storing bounds
                el = new LayerElement(LayerElementType.ChartField, rect);
                layer.Elements.Add(el);

                // fields
                Color color = ColorTranslator.FromOle(Convert.ToInt32(sf.Charts.LineColor));
                Pen pen = new Pen(color);

                for (int i = 0; i < sf.Charts.NumFields; i++)
                {
                    rect = new Rectangle(left, top, Constants.ICON_WIDTH, Constants.ICON_HEIGHT);
                    color = ColorTranslator.FromOle(Convert.ToInt32(sf.Charts.get_Field(i).Color));
                    SolidBrush brush = new SolidBrush(color);
                    DrawTool.FillRectangle(brush, rect);
                    DrawTool.DrawRectangle(pen, rect);

                    // storing bounds
                    el = new LayerElement(LayerElementType.ChartField, rect, i);
                    layer.Elements.Add(el);

                    rect = new Rectangle(left + Constants.ICON_WIDTH + 5, top, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
                    string name = sf.Charts.get_Field(i).Name;
                    DrawText(DrawTool, name, rect, m_Font, Color.Black);

                    // storing bounds
                    el = new LayerElement(LayerElementType.ChartFieldName, rect, name, i);
                    layer.Elements.Add(el);

                    top += (Constants.CS_ITEM_HEIGHT + 2);
                }
            }
        }

        /// <summary>
        /// Draws shapefile category in specified location
        /// </summary>
        /// <param name="options">Options to use for drawing</param>
        /// <param name="type">Shapefile type</param>
        private void DrawShapefileCategory(Graphics DrawTool, MapWinGIS.ShapeDrawingOptions options, Layer layer,
                                           Rectangle bounds, int top, string name, int maxWidth, int index)
        {
            int categoryHeight = layer.GetCategoryHeight(options);
            int categoryWidth = layer.GetCategoryWidth(options);

            // drawing category symbol
            IntPtr hdc = DrawTool.GetHdc();
            uint backColor = Convert.ToUInt32(ColorTranslator.ToOle(this.BackColor));

            int left = bounds.Left + Constants.TEXT_LEFT_PAD;
            if (categoryWidth != Constants.ICON_WIDTH)
            {
                left -= ((categoryWidth - Constants.ICON_WIDTH) / 2);
            }

            if (layer.Type == eLayerType.PointShapefile)
                options.DrawPoint(hdc, left, top, categoryWidth + 1, categoryHeight + 1, backColor);
            else if (layer.Type == eLayerType.LineShapefile)
                options.DrawLine(hdc, left, top, categoryWidth - 1, Constants.ICON_HEIGHT - 1, false, categoryWidth, categoryHeight, backColor);
            else if (layer.Type == eLayerType.PolygonShapefile)
                options.DrawRectangle(hdc, left, top, categoryWidth - 1, Constants.ICON_HEIGHT - 1, false, categoryWidth, categoryHeight, backColor);

            DrawTool.ReleaseHdc(hdc);

            if (categoryHeight > Constants.CS_ITEM_HEIGHT)
                top += (categoryHeight - Constants.CS_ITEM_HEIGHT) / 2;

            // drawing category name
            left = (bounds.Left + Constants.TEXT_LEFT_PAD + Constants.ICON_WIDTH / 2 + maxWidth / 2 + 5);

            Rectangle rect = new Rectangle(left, top, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
            DrawText(DrawTool, name, rect, m_Font, Color.Black);
        }

        /// <summary>
        /// Draws shapefile category. It's asumed here that GetHDC and ReleaseHDC calls are made by caller
        /// </summary>
        private void DrawShapefileCategorySymbology(Graphics DrawTool, MapWinGIS.ShapeDrawingOptions options, Layer layer,
                                           Rectangle bounds, int top, int maxWidth, int index, IntPtr hdc)
        {
            int categoryHeight = layer.GetCategoryHeight(options);
            int categoryWidth = layer.GetCategoryWidth(options);

            uint backColor = Convert.ToUInt32(ColorTranslator.ToOle(this.BackColor));

            int left = bounds.Left + Constants.TEXT_LEFT_PAD;
            if (categoryWidth != Constants.ICON_WIDTH)
            {
                left -= ((categoryWidth - Constants.ICON_WIDTH) / 2);
            }

            if (layer.Type == eLayerType.PointShapefile)
                options.DrawPoint(hdc, left, top, categoryWidth + 1, categoryHeight + 1, backColor);
            else if (layer.Type == eLayerType.LineShapefile)
                options.DrawLine(hdc, left, top, categoryWidth - 1, Constants.ICON_HEIGHT - 1, false, categoryWidth, categoryHeight, backColor);
            else if (layer.Type == eLayerType.PolygonShapefile)
                options.DrawRectangle(hdc, left, top, categoryWidth - 1, Constants.ICON_HEIGHT - 1, false, categoryWidth, categoryHeight, backColor);

            if (categoryHeight > Constants.CS_ITEM_HEIGHT)
                top += (categoryHeight - Constants.CS_ITEM_HEIGHT) / 2;
        }

        /// <summary>
        /// Draw the text for the shapefile category
        /// </summary>
        private void DrawShapefileCategoryText(Graphics DrawTool, MapWinGIS.ShapeDrawingOptions options, Layer layer,
                                           Rectangle bounds, int top, string name, int maxWidth, int index)
        {
            int categoryHeight = layer.GetCategoryHeight(options);
            if (categoryHeight > Constants.CS_ITEM_HEIGHT)
                top += (categoryHeight - Constants.CS_ITEM_HEIGHT) / 2;

            // drawing category name
            int left = (bounds.Left + Constants.TEXT_LEFT_PAD + Constants.ICON_WIDTH / 2 + maxWidth / 2 + 5);

            Rectangle rect = new Rectangle(left, top, bounds.Width - Constants.TEXT_RIGHT_PAD_NO_ICON - Constants.CS_TEXT_LEFT_INDENT, Constants.TEXT_HEIGHT);
            DrawText(DrawTool, name, rect, m_Font, this.ForeColor);
        }

        private System.Drawing.Drawing2D.HatchStyle GetHatchStyle(MapWinGIS.tkFillStipple stip)
        {
            switch (stip)
            {
                case MapWinGIS.tkFillStipple.fsDiagonalDownLeft:
                    return System.Drawing.Drawing2D.HatchStyle.DarkDownwardDiagonal;
                case MapWinGIS.tkFillStipple.fsDiagonalDownRight:
                    return System.Drawing.Drawing2D.HatchStyle.DarkUpwardDiagonal;
                case MapWinGIS.tkFillStipple.fsHorizontalBars:
                    return System.Drawing.Drawing2D.HatchStyle.DarkHorizontal;
                case MapWinGIS.tkFillStipple.fsPolkaDot:
                    return System.Drawing.Drawing2D.HatchStyle.OutlinedDiamond;
                case MapWinGIS.tkFillStipple.fsVerticalBars:
                    return System.Drawing.Drawing2D.HatchStyle.DarkVertical;
                default:
                    return System.Drawing.Drawing2D.HatchStyle.Max;
            }
        }

        /// <summary>
        /// Drawing icon for the new symbology
        /// </summary>
        /// <param name="DrawTool"></param>
        /// <param name="LayerType"></param>
        /// <param name="TopPos"></param>
        /// <param name="LeftPos"></param>
        /// <param name="FillColor"></param>
        /// <param name="OutlineColor"></param>
        /// <param name="LayerHandle"></param>
        private void DrawLayerSymbolNew(Graphics DrawTool, Layer layer, int TopPos, int LeftPos)
        {
            System.Drawing.Drawing2D.SmoothingMode OldSmoothingMode;
            OldSmoothingMode = DrawTool.SmoothingMode;

            try
            {
                DrawTool.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                Image icon;

                switch (layer.Type)
                {
                    case eLayerType.Grid:
                        icon = Icons.Images[cGridIcon];
                        DrawPicture(DrawTool, LeftPos, TopPos, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                        break;
                    case eLayerType.Image:
                        icon = Icons.Images[cImageIcon];
                        DrawPicture(DrawTool, LeftPos, TopPos, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                        break;
                    default:
                        MapWinGIS.Shapefile sf = m_Map.get_GetObject(layer.Handle) as MapWinGIS.Shapefile;
                        if (sf == null)
                        {
                            MessageBox.Show("Error: shapefile not set");
                            return;
                        }

                        IntPtr hdc = DrawTool.GetHdc();

                        uint backColor = Convert.ToUInt32(ColorTranslator.ToOle(this.BackColor));

                        if (layer.Type == eLayerType.PointShapefile)
                            sf.DefaultDrawingOptions.DrawPoint(hdc, LeftPos, TopPos, Constants.ICON_WIDTH, Constants.ICON_HEIGHT, backColor);
                        else if (layer.Type == eLayerType.LineShapefile)
                            sf.DefaultDrawingOptions.DrawLine(hdc, LeftPos, TopPos, Constants.ICON_WIDTH - 1, Constants.ICON_SIZE - 1, false, Constants.ICON_WIDTH, Constants.ICON_HEIGHT, backColor);
                        else if (layer.Type == eLayerType.PolygonShapefile)
                            sf.DefaultDrawingOptions.DrawRectangle(hdc, LeftPos, TopPos, Constants.ICON_WIDTH - 1, Constants.ICON_SIZE - 1, false, Constants.ICON_WIDTH, Constants.ICON_HEIGHT, backColor);

                        DrawTool.ReleaseHdc(hdc);
                        break;
                }
            }
            catch (System.Exception ex)
            {
                string temp = ex.Message;
            }

            DrawTool.SmoothingMode = OldSmoothingMode;
        }

        /// <summary>
        /// Drawing icon for the old symbology
        /// </summary>
        /// <param name="DrawTool"></param>
        /// <param name="LayerType"></param>
        /// <param name="TopPos"></param>
        /// <param name="LeftPos"></param>
        /// <param name="FillColor"></param>
        /// <param name="OutlineColor"></param>
        /// <param name="LayerHandle"></param>
        private void DrawLayerSymbol(Graphics DrawTool, eLayerType LayerType, int TopPos, int LeftPos, Color FillColor, Color OutlineColor, int LayerHandle)
        {
            System.Drawing.Drawing2D.SmoothingMode OldSmoothingMode;
            OldSmoothingMode = DrawTool.SmoothingMode;

            try
            {
                DrawTool.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                Image icon;
                Pen pen;

                switch (LayerType)
                {
                    case eLayerType.Grid:
                        icon = Icons.Images[cGridIcon];
                        DrawPicture(DrawTool, LeftPos, TopPos, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                        break;
                    case eLayerType.Image:
                        icon = Icons.Images[cImageIcon];
                        DrawPicture(DrawTool, LeftPos, TopPos, Constants.ICON_SIZE, Constants.ICON_SIZE, icon);
                        break;
                    case eLayerType.LineShapefile:
                        pen = new Pen(FillColor, 2);

                        OldSmoothingMode = DrawTool.SmoothingMode;
                        DrawTool.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                        DrawTool.DrawLine(pen, LeftPos, TopPos + 8, LeftPos + 4, TopPos + 3);
                        DrawTool.DrawLine(pen, LeftPos + 4, TopPos + 3, LeftPos + 9, TopPos + 10);
                        DrawTool.DrawLine(pen, LeftPos + 9, TopPos + 10, LeftPos + 13, TopPos + 4);

                        DrawTool.SmoothingMode = OldSmoothingMode;

                        break;
                    case eLayerType.PointShapefile:
                        pen = new Pen(FillColor);

                        float offset;
                        MapWinGIS.tkPointType pntType;
                        pntType = m_Map.get_ShapeLayerPointType(LayerHandle);
                        Point[] pnts;

                        switch (pntType)
                        {
                            case MapWinGIS.tkPointType.ptCircle:
                                offset = (float)(.2 * Constants.ICON_SIZE);
                                DrawTool.FillEllipse(pen.Brush, (float)LeftPos + offset, TopPos + offset, 7, 7);
                                break;
                            case MapWinGIS.tkPointType.ptDiamond:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[4];

                                //top
                                pnts[0] = new Point((int)(LeftPos + offset), TopPos + 2);
                                //left
                                pnts[1] = new Point(LeftPos + 2, (int)(TopPos + offset));
                                //bottom
                                pnts[2] = new Point((int)(LeftPos + offset), TopPos + Constants.ICON_SIZE - 3);
                                //right
                                pnts[3] = new Point(LeftPos + Constants.ICON_SIZE - 3, (int)(TopPos + offset));

                                DrawTool.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptSquare:
                                offset = (float)(.25 * Constants.ICON_SIZE);
                                DrawTool.FillRectangle(pen.Brush, (int)(LeftPos + offset), (int)(TopPos + offset), 6, 6);
                                break;
                            case MapWinGIS.tkPointType.ptTriangleDown:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[3];

                                //bottom middle
                                pnts[0] = new Point((int)(LeftPos + offset), TopPos + Constants.ICON_SIZE - 3);
                                //top right
                                pnts[1] = new Point(LeftPos + Constants.ICON_SIZE - 3, TopPos + 3);
                                //top left
                                pnts[2] = new Point((int)(LeftPos + 3), TopPos + 3);

                                DrawTool.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleLeft:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[3];

                                //left middle
                                pnts[0] = new Point(LeftPos + 3, (int)(TopPos + offset));
                                //top right
                                pnts[1] = new Point(LeftPos + Constants.ICON_SIZE - 3, TopPos + 3);
                                //bottom right
                                pnts[2] = new Point(LeftPos + Constants.ICON_SIZE - 3, TopPos + Constants.ICON_SIZE - 3);

                                DrawTool.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleRight:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[3];

                                //right middle
                                pnts[0] = new Point(LeftPos + Constants.ICON_SIZE - 3, (int)(TopPos + offset));
                                //top left
                                pnts[1] = new Point(LeftPos + 3, TopPos + 3);
                                //bottom left
                                pnts[2] = new Point(LeftPos + 3, TopPos + Constants.ICON_SIZE - 3);

                                DrawTool.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleUp:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[3];

                                //top middle
                                pnts[0] = new Point((int)(LeftPos + offset), TopPos + 3);
                                //bottom right
                                pnts[1] = new Point(LeftPos + Constants.ICON_SIZE - 3, TopPos + Constants.ICON_SIZE - 3);
                                //bottom left
                                pnts[2] = new Point((int)(LeftPos + 3), TopPos + Constants.ICON_SIZE - 3);

                                DrawTool.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptUserDefined:
                                object img = m_Map.get_UDPointType(LayerHandle);
                                DrawPicture(DrawTool, LeftPos, TopPos, Constants.ICON_SIZE, Constants.ICON_SIZE, img);
                                img = null;
                                break;
                        }
                        break;
                    case eLayerType.PolygonShapefile:
                        bool DrawFill = m_Map.get_ShapeLayerDrawFill(LayerHandle);
                        MapWinGIS.tkFillStipple fillStyle = m_Map.get_ShapeLayerFillStipple(LayerHandle);

                        Pen FillPen;
                        Pen OutlinePen = new Pen(OutlineColor, 2);

                        Rectangle rect = new Rectangle(LeftPos + 1, TopPos + 1, Constants.ICON_SIZE - 2, Constants.ICON_SIZE - 2);

                        if (DrawFill == true)
                        {
                            switch (fillStyle)
                            {
                                case MapWinGIS.tkFillStipple.fsNone:
                                    // Chris Michaelis 7/5/2007 - Changed this coloring to take into account transparency "paleness"
                                    FillPen = new Pen(System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(LayerHandle) * 255), FillColor));
                                    break;
                                case MapWinGIS.tkFillStipple.fsDiagonalDownLeft:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightUpwardDiagonal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsDiagonalDownRight:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightDownwardDiagonal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsHorizontalBars:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightHorizontal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsPolkaDot:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.SmallCheckerBoard, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsVerticalBars:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightVertical, Color.Black, Color.White));
                                    break;
                                default:
                                    // Chris Michaelis 7/5/2007 - Changed this coloring to take into account transparency "paleness"
                                    FillPen = new Pen(System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(LayerHandle) * 255), FillColor));
                                    break;
                            }

                            DrawTool.FillRectangle(FillPen.Brush, rect);
                        }
                        else
                        {
                            //							Color HatchColor;
                            //							globals.GetHSL(FillColor,out hue, out sat, out lum);
                            //
                            //							if(lum >= .75)
                            //								HatchColor = globals.HSLtoColor(0f,0f,lum -.5f);
                            //							else if (lum >= .5)
                            //								HatchColor = globals.HSLtoColor(0f,0f,lum -.25f);
                            //							else if (lum >= .25)
                            //								HatchColor = globals.HSLtoColor(0f,0f,lum +.25f);
                            //							else
                            //								HatchColor = globals.HSLtoColor(0f,0f,lum +.5f);
                            //
                            //                          FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightUpwardDiagonal,HatchColor,FillColor));
                            //							DrawTool.FillRectangle(FillPen.Brush,rect);
                        }

                        if (m_Map.get_ShapeLayerLineWidth(LayerHandle) > 0) DrawTool.DrawRectangle(OutlinePen, rect);

                        break;
                }
            }
            catch (System.Exception ex)
            {
                string temp = ex.Message;
            }

            DrawTool.SmoothingMode = OldSmoothingMode;
        }

        private void DrawLayerSymbolHQ(Graphics g, Layer lyr, int Left, int Top, int Dimension, Color FillColor, Color OutlineColor)
        {
            System.Drawing.Drawing2D.SmoothingMode OldSmoothingMode;
            OldSmoothingMode = g.SmoothingMode;

            try
            {
                g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;
                Image icon;
                Pen pen;

                switch (lyr.Type)
                {
                    case eLayerType.Grid:
                        icon = Icons.Images[cGridIcon];
                        DrawPicture(g, Left, Top, Dimension, Dimension, icon);
                        break;
                    case eLayerType.Image:
                        icon = Icons.Images[cImageIcon];
                        DrawPicture(g, Left, Top, Dimension, Dimension, icon);
                        break;
                    case eLayerType.LineShapefile:
                        pen = new Pen(FillColor, 2);

                        OldSmoothingMode = g.SmoothingMode;
                        g.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                        g.DrawLine(pen, Left, Top + (Dimension / 2), Left + (Dimension / 3), Top);
                        g.DrawLine(pen, Left + (Dimension / 3), Top, (int)(Left + (Dimension * (2.0 / 3.0))), Top + Dimension);
                        g.DrawLine(pen, (int)(Left + (Dimension * (2.0 / 3.0))), Top + Dimension, Left + Dimension, Top + (Dimension / 2));

                        g.SmoothingMode = OldSmoothingMode;

                        break;
                    case eLayerType.PointShapefile:
                        pen = new Pen(FillColor);

                        float offset;
                        MapWinGIS.tkPointType pntType;
                        pntType = m_Map.get_ShapeLayerPointType(lyr.Handle);
                        Point[] pnts;

                        switch (pntType)
                        {
                            case MapWinGIS.tkPointType.ptCircle:
                                g.FillEllipse(pen.Brush, Left, Top, Dimension, Dimension);
                                break;

                            case MapWinGIS.tkPointType.ptDiamond:
                                pnts = new Point[4];

                                //top
                                pnts[0] = new Point(Left, Top + (Dimension / 2));
                                //left
                                pnts[1] = new Point(Left + (Dimension / 2), Top + Dimension);
                                //bottom
                                pnts[2] = new Point(Left + Dimension, Top + (Dimension / 2));
                                //right
                                pnts[3] = new Point(Left + (Dimension / 2), Top);

                                g.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptSquare:
                                g.FillRectangle(pen.Brush, Left, Top, Left + Dimension, Top + Dimension);
                                break;
                            case MapWinGIS.tkPointType.ptTriangleDown:
                                offset = (float)(.5 * Constants.ICON_SIZE);
                                pnts = new Point[3];

                                pnts[0] = new Point(Left, Top);
                                pnts[1] = new Point(Left + (Dimension / 2), Top + Dimension);
                                pnts[2] = new Point(Left + Dimension, Top);

                                g.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleLeft:
                                pnts = new Point[3];

                                pnts[0] = new Point(Left + Dimension, Top);
                                pnts[1] = new Point(Left, Top + (Dimension / 2));
                                pnts[2] = new Point(Left + Dimension, Top + Dimension);

                                g.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleRight:
                                pnts = new Point[3];

                                pnts[0] = new Point(Left, Top);
                                pnts[1] = new Point(Left + Dimension, Top + (Dimension / 2));
                                pnts[2] = new Point(Left, Top + Dimension);

                                g.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptTriangleUp:
                                pnts = new Point[3];

                                pnts[0] = new Point(Left, Top + Dimension);
                                pnts[1] = new Point(Left + (Dimension / 2), Top);
                                pnts[2] = new Point(Left + Dimension, Top + Dimension);

                                g.FillPolygon(pen.Brush, pnts);
                                pnts = null;
                                break;
                            case MapWinGIS.tkPointType.ptUserDefined:
                                object img = m_Map.get_UDPointType(lyr.Handle);
                                DrawPicture(g, Left, Top, Dimension, Dimension, img);
                                img = null;
                                break;
                        }
                        break;
                    case eLayerType.PolygonShapefile:
                        bool DrawFill = m_Map.get_ShapeLayerDrawFill(lyr.Handle);
                        MapWinGIS.tkFillStipple fillStyle = m_Map.get_ShapeLayerFillStipple(lyr.Handle);

                        Pen FillPen;
                        Pen OutlinePen = new Pen(OutlineColor, 2);
                        Rectangle rect = new Rectangle(Left, Top, Dimension, Dimension);

                        if (DrawFill == true)
                        {
                            switch (fillStyle)
                            {
                                case MapWinGIS.tkFillStipple.fsNone:
                                    FillPen = new Pen(System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), FillColor));
                                    break;
                                case MapWinGIS.tkFillStipple.fsDiagonalDownLeft:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightUpwardDiagonal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsDiagonalDownRight:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightDownwardDiagonal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsHorizontalBars:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightHorizontal, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsPolkaDot:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.SmallCheckerBoard, Color.Black, Color.White));
                                    break;
                                case MapWinGIS.tkFillStipple.fsVerticalBars:
                                    FillPen = new Pen(new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightVertical, Color.Black, Color.White));
                                    break;
                                default:
                                    // Chris Michaelis 7/5/2007 - Changed this coloring to take into account transparency "paleness"
                                    FillPen = new Pen(System.Drawing.Color.FromArgb((int)(m_Map.get_ShapeLayerFillTransparency(lyr.Handle) * 255), FillColor));
                                    break;
                            }

                            g.FillRectangle(FillPen.Brush, rect);
                        }

                        if (m_Map.get_ShapeLayerLineWidth(lyr.Handle) > 0) g.DrawRectangle(OutlinePen, rect);

                        break;
                }
            }
            catch (System.Exception ex)
            {
                string temp = ex.Message;
            }

            g.SmoothingMode = OldSmoothingMode;
        }

        /// <summary>
        /// Control is being resized
        /// </summary>
        protected override void OnResize(System.EventArgs e)
        {
            if (this.Width > 0 && this.Height > 0)
            {
                m_BackBuffer = new Bitmap(this.Width, this.Height);
                m_Draw = Graphics.FromImage(m_BackBuffer);
                vScrollBar.Top = 0;
                vScrollBar.Height = this.Height;
                vScrollBar.Left = this.Width - vScrollBar.Width;
            }
            this.Invalidate();
        }

        /// <summary>
        /// Locates the group that was clicked on in the legend, based on the coordinate within the legend (0,0) being top left of legend)
        /// </summary>
        /// <param name="point">The point inside of the legend that was clicked.</param>
        /// <param name="InCheckbox">(by reference/out) Indicates whether a group visibilty check box was clicked.</param>
        /// <param name="InExpandbox">(by reference/out) Indicates whether the expand box next to a group was clicked.</param>
        /// <returns>Returns the group that was clicked on or null/nothing.</returns>
        public Group FindClickedGroup(Point point, out bool InCheckbox, out bool InExpandbox)
        {
            //finds the group that was clicked, i.e. heading of group, not subitems
            InExpandbox = false;
            InCheckbox = false;

            int GroupCount = m_AllGroups.Count;
            Group grp = null;

            int CurLeft = 0,
                CurTop = 0,
                CurWidth = 0,
                CurHeight = 0;
            Rectangle bounds;

            GroupCount = m_AllGroups.Count;

            for (int i = 0; i < GroupCount; i++)
            {
                grp = (Group)m_AllGroups[i];

                //set group header bounds
                CurLeft = Constants.GRP_INDENT;
                CurWidth = this.Width - Constants.GRP_INDENT - Constants.ITEM_RIGHT_PAD;
                CurTop = grp.Top;
                CurHeight = Constants.ITEM_HEIGHT;
                bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                if (bounds.Contains(point) == true)
                {//we are in the group heading
                    //now check to see if the click was in the expansion box
                    //+- a little to make the hot spot a little more precise
                    CurLeft = Constants.GRP_INDENT + Constants.EXPAND_BOX_LEFT_PAD + 1;
                    CurWidth = Constants.EXPAND_BOX_SIZE - 1;
                    CurTop = grp.Top + Constants.EXPAND_BOX_TOP_PAD + 1;
                    CurHeight = Constants.EXPAND_BOX_SIZE - 1;
                    bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                    if (bounds.Contains(point) == true)
                    {
                        //we are in the bounds for the expansion box
                        //but only if there is an expansion box visible
                        if ((int)(grp.Layers.Count) > 0)
                            InExpandbox = true;
                    }
                    else
                    {
                        //now check to see if in the check box
                        CurLeft = Constants.GRP_INDENT + Constants.CHECK_LEFT_PAD + 1;
                        CurWidth = Constants.CHECK_BOX_SIZE - 1;
                        CurTop = grp.Top + Constants.CHECK_TOP_PAD + 1;
                        CurHeight = Constants.CHECK_BOX_SIZE - 1;
                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
                        if (bounds.Contains(point) == true)
                            InCheckbox = true;
                    }
                    return grp;
                }
            }
            return null;//if we get to this point, no group item found
        }

        /// <summary>
        /// Locates the layer that was clicked on in the legend, based on the coordinate within the legend (0,0) being top left of legend)
        /// </summary>
        /// <param name="point">The point inside of the legend that was clicked.</param>
        /// <param name="InCheckBox">(by reference/out) Indicates whether a layer visibilty check box was clicked.</param>
        /// <param name="InExpansionBox">(by reference/out) Indicates whether the expand box next to a layer was clicked.</param>
        /// <returns>Returns the group that was clicked on or null/nothing.</returns>
        public Layer FindClickedLayer(Point point, out bool InCheckBox, out bool InExpansionBox)
        {
            //int ContainingGroupIndex = 0;
            //int CategoryIndex = -1;
            //int ChartFieldIndex = -1;
            //bool inColorBox = false;
            //bool inCharts = false;
            //return FindClickedLayer(point,out InCheckBox,out InExpansionBox,out inColorBox, out inCharts, out ContainingGroupIndex, out CategoryIndex, out ChartFieldIndex );
            ClickedElement element = new ClickedElement();
            Layer lyr = FindClickedLayer(point, ref element);
            InCheckBox = element.CheckBox;
            InExpansionBox = element.ExpansionBox;
            return lyr;
        }

        /// <summary>
        ///
        /// </summary>
        /// <param name="element"></param>
        /// <returns></returns>
        public Layer FindClickedLayer(Point point, ref ClickedElement Element)
        {
            int GroupCount = m_AllGroups.Count;
            int LayerCount;

            Element.Nullify();

            Layer lyr = null;
            Group grp = null;

            int CurLeft, CurTop, CurWidth, CurHeight;
            CurLeft = CurTop = CurWidth = CurHeight = 0;
            Rectangle bounds;

            for (int i = 0; i < GroupCount; i++)
            {
                grp = (Group)m_AllGroups[i];

                if (grp.Expanded == false)
                    continue;

                LayerCount = grp.Layers.Count;

                for (int j = 0; j < LayerCount; j++)
                {
                    lyr = (Layer)grp.Layers[j];

                    //see if we are inside the current Layer
                    CurLeft = Constants.LIST_ITEM_INDENT;
                    CurTop = lyr.Top;
                    CurWidth = this.Width - CurLeft - Constants.ITEM_RIGHT_PAD;
                    CurHeight = lyr.Height;
                    bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                    if (bounds.Contains(point))
                    {
                        //we are inside the Layer boundaries,
                        //but we need to narrow down the search
                        Element.GroupIndex = i;

                        //check to see if in the check box
                        CurLeft = Constants.LIST_ITEM_INDENT + Constants.CHECK_LEFT_PAD + 1;
                        CurTop = lyr.Top + Constants.CHECK_TOP_PAD + 1;
                        CurWidth = Constants.CHECK_BOX_SIZE - 1;
                        CurHeight = Constants.CHECK_BOX_SIZE - 1;
                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                        if (bounds.Contains(point))
                        {
                            //we are in the check box
                            Element.CheckBox = true;
                            return lyr;
                        }
                        else
                        {
                            //check to see if we are in the expansion box for this item
                            CurLeft = Constants.LIST_ITEM_INDENT + Constants.EXPAND_BOX_LEFT_PAD + 1;
                            CurTop = lyr.Top + Constants.EXPAND_BOX_TOP_PAD + 1;
                            CurWidth = Constants.EXPAND_BOX_SIZE;
                            CurHeight = Constants.EXPAND_BOX_SIZE;
                            bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                            if (m_Map.ShapeDrawingMethod != MapWinGIS.tkShapeDrawingMethod.dmNewSymbology ||
                               (lyr.Type == eLayerType.Image || lyr.Type == eLayerType.Grid))
                            {
                                if (bounds.Contains(point) == true && (lyr.ColorLegend.Count > 0 || lyr.ExpansionBoxForceAllowed ))
                                {
                                    //We are in the Expansion box
                                    Element.ExpansionBox = true;
                                    return lyr;
                                }
                                else
                                {
                                    //we aren't in the checkbox or the expansion box
                                    return lyr;
                                }
                            }
                            else if (m_Map.ShapeDrawingMethod == MapWinGIS.tkShapeDrawingMethod.dmNewSymbology)
                            {
                                
                                if (bounds.Contains(point))
                                {
                                    //We are in the Expansion box
                                    Element.ExpansionBox = true;
                                    return lyr;
                                }
                                else
                                {
                                    if (!lyr.Expanded && 
                                         (lyr.Type == eLayerType.LineShapefile || 
                                         lyr.Type == eLayerType.PointShapefile || 
                                         lyr.Type == eLayerType.PolygonShapefile) && lyr.m_smallIconWasDrawn)
                                    {
                                        CurHeight = Constants.ICON_SIZE;
                                        CurWidth = Constants.ICON_SIZE;
                                        CurTop = lyr.Top + Constants.ICON_TOP_PAD;
                                        CurLeft = this.Width - 36;
                                        if (this.vScrollBar.Visible) 
                                            CurLeft -= this.vScrollBar.Width;
                                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
                                        if (bounds.Contains(point))
                                        {
                                            Element.ColorBox = true;
                                            return lyr;
                                        }
                                    }

                                    // layer icon (to the right from the caption)
                                    if (lyr.Type == eLayerType.LineShapefile ||
                                        lyr.Type == eLayerType.PointShapefile ||
                                        lyr.Type == eLayerType.PolygonShapefile)
                                    {
                                        CurHeight = Constants.ICON_SIZE;
                                        CurWidth = Constants.ICON_SIZE;
                                        CurTop = lyr.Top + Constants.ICON_TOP_PAD;
                                        CurLeft = this.Width - 56;
                                        if (this.vScrollBar.Visible)
                                            CurLeft -= this.vScrollBar.Width;

                                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);
                                        if (bounds.Contains(point))
                                        {
                                            Element.LabelsIcon = true;
                                            return lyr;
                                        }
                                    }

                                    // check to see if we are in the default color box
                                    MapWinGIS.Shapefile sf = m_Map.get_GetObject(lyr.Handle) as MapWinGIS.Shapefile;

                                    CurHeight = lyr.GetCategoryHeight(sf.DefaultDrawingOptions);
                                    CurWidth = lyr.GetCategoryWidth(sf.DefaultDrawingOptions);
                                    CurTop = lyr.Top + Constants.ITEM_HEIGHT + 2;
                                    CurLeft = Constants.LIST_ITEM_INDENT + Constants.TEXT_LEFT_PAD;
                                    if (CurWidth != Constants.ICON_WIDTH)
                                    {
                                        CurLeft -= ((CurWidth - Constants.ICON_WIDTH) / 2);
                                    }
                                    bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                                    if (bounds.Contains(point))
                                    {
                                        Element.ColorBox = true;
                                        return lyr;
                                    }
                                    else
                                    {
                                        // check to sse if we are in the label
                                        CurHeight = lyr.GetCategoryHeight(sf.DefaultDrawingOptions);
                                        CurWidth = lyr.GetCategoryWidth(sf.DefaultDrawingOptions);
                                        CurTop = lyr.Top + Constants.ITEM_HEIGHT + 2;
                                        int maxWidth = lyr.get_MaxIconWidth(sf);
                                        CurLeft = bounds.Left + Constants.TEXT_LEFT_PAD + maxWidth + 5;
                                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                                        if (bounds.Contains(point))
                                        {
                                            Element.Label = true;
                                            return lyr;
                                        }
                                        else
                                        {
                                            // categories
                                            CurLeft = Constants.LIST_ITEM_INDENT + Constants.TEXT_LEFT_PAD;
                                            CurTop = lyr.Top + Constants.ITEM_HEIGHT + 2;   // name
                                            CurTop += CurHeight + 2;                        // default symbology

                                            if (sf.Categories.Count > 0)
                                            {
                                                CurTop += Constants.CS_ITEM_HEIGHT + 2;         // categories caption

                                                for (int cat = 0; cat < sf.Categories.Count; cat++)
                                                {
                                                    MapWinGIS.ShapeDrawingOptions options = sf.Categories.get_Item(cat).DrawingOptions;
                                                    CurWidth = lyr.GetCategoryWidth(options);
                                                    CurHeight = lyr.GetCategoryHeight(options);
                                                    bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                                                    CurTop += CurHeight;

                                                    if (bounds.Contains(point))
                                                    {
                                                        Element.ColorBox = true;
                                                        Element.CategoryIndex = cat;
                                                        return lyr;
                                                    }
                                                }
                                            }

                                            if (sf.Charts.NumFields > 0 && sf.Charts.Count > 0)
                                            {
                                                CurTop += Constants.CS_ITEM_HEIGHT + 2;         // charts caption
                                                CurWidth = sf.Charts.IconWidth;
                                                CurHeight = sf.Charts.IconHeight;
                                                bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                                                if (bounds.Contains(point))
                                                {
                                                    Element.Charts = true;
                                                    return lyr;
                                                }
                                                else
                                                {
                                                    CurTop += (CurHeight + 2);
                                                    CurHeight = Constants.ICON_HEIGHT;
                                                    CurWidth = Constants.ICON_WIDTH;

                                                    for (int fld = 0; fld < sf.Charts.NumFields; fld++)
                                                    {
                                                        bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

                                                        if (bounds.Contains(point))
                                                        {
                                                            Element.Charts = true;
                                                            Element.ChartFieldIndex = fld;
                                                            //MessageBox.Show("Field selected: " + fld.ToString());
                                                            return lyr;
                                                        }

                                                        CurTop += (Constants.CS_ITEM_HEIGHT + 2);
                                                    }
                                                }
                                            }

                                            // nothing was hit
                                            return lyr;
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
            return null;
        }

        /// <summary>
        /// Locates the layer that was clicked on in the legend, based on the coordinate within the legend (0,0) being top left of legend)
        /// </summary>
        /// <param name="point">The point inside of the legend that was clicked.</param>
        /// <param name="InCheckBox">(by reference/out) Indicates whether a layer visibilty check box was clicked.</param>
        /// <param name="InExpansionBox">(by reference/out) Indicates whether the expand box next to a layer was clicked.</param>
        /// <param name="ContainingGroupIndex">(by reference/out) Indicates the group ID of the layer that was clicked, if any.</param>
        /// <param name="CategoryIndex">(by reference/out) The index of category that was clicked (-1 by default)</param>
        /// <returns>Returns the group that was clicked on or null/nothing.</returns>
        //public Layer FindClickedLayer(Point point,out bool InCheckBox,out bool InExpansionBox, out bool InColorBox, out bool InCharts,
        //                              out int ContainingGroupIndex, out int CategoryIndex, out int ChartFieldIndex)
        //{
        //    int GroupCount = m_AllGroups.Count;
        //    int LayerCount;

        //    InCheckBox = InExpansionBox = InColorBox = InCharts = false;
        //    ContainingGroupIndex = -1;
        //    CategoryIndex = -1;
        //    ChartFieldIndex = - 1;

        //    Layer lyr = null;
        //    Group grp = null;

        //    int CurLeft =0,
        //        CurTop =0,
        //        CurWidth =0,
        //        CurHeight =0;
        //    Rectangle bounds;

        //    for (int i = 0; i < GroupCount; i++)
        //    {
        //        grp = (Group)m_AllGroups[i];

        //        if(grp.Expanded == false)
        //            continue;

        //        LayerCount = grp.Layers.Count;

        //        for(int j = 0; j < LayerCount; j++)
        //        {
        //            lyr = (Layer)grp.Layers[j];

        //            //see if we are inside the current Layer
        //            CurLeft = Constants.LIST_ITEM_INDENT;
        //            CurTop = lyr.Top;
        //            CurWidth = this.Width - CurLeft - Constants.ITEM_RIGHT_PAD;
        //            CurHeight = lyr.Height;
        //            bounds = new Rectangle(CurLeft,CurTop,CurWidth,CurHeight);

        //            if( bounds.Contains(point))
        //            {
        //                //we are inside the Layer boundaries,
        //                //but we need to narrow down the search
        //                ContainingGroupIndex = i;

        //                //check to see if in the check box
        //                CurLeft = Constants.LIST_ITEM_INDENT + Constants.CHECK_LEFT_PAD + 1;
        //                CurTop = lyr.Top + Constants.CHECK_TOP_PAD + 1;
        //                CurWidth = Constants.CHECK_BOX_SIZE-1;
        //                CurHeight = Constants.CHECK_BOX_SIZE-1;
        //                bounds = new Rectangle(CurLeft,CurTop,CurWidth,CurHeight);

        //                if(bounds.Contains(point))
        //                {
        //                    //we are in the check box
        //                    InCheckBox = true;
        //                    return lyr;
        //                }
        //                else
        //                {
        //                    //check to see if we are in the expansion box for this item
        //                    CurLeft = Constants.LIST_ITEM_INDENT + Constants.EXPAND_BOX_LEFT_PAD + 1;
        //                    CurTop = lyr.Top + Constants.EXPAND_BOX_TOP_PAD + 1;
        //                    CurWidth = Constants.EXPAND_BOX_SIZE;
        //                    CurHeight = Constants.EXPAND_BOX_SIZE;
        //                    bounds = new Rectangle(CurLeft,CurTop,CurWidth,CurHeight);

        //                    if (m_Map.VersionNumber == 0 || (lyr.Type == eLayerType.Image || lyr.Type == eLayerType.Grid))
        //                    {
        //                        if (bounds.Contains(point) == true && (lyr.ColorLegend.Count > 0 || lyr.ExpansionBoxForceAllowed ||
        //                                                               (lyr.HatchingScheme != null && lyr.HatchingScheme.NumHatches() > 0) ||
        //                                                               (lyr.PointImageScheme != null && lyr.PointImageScheme.NumberItems > 0)))
        //                        {
        //                            //We are in the Expansion box
        //                            InExpansionBox = true;
        //                            return lyr;
        //                        }
        //                        else
        //                        {
        //                            //we aren't in the checkbox or the expansion box
        //                            return lyr;
        //                        }
        //                    }
        //                    else if (m_Map.VersionNumber == 1)
        //                    {
        //                        MapWinGIS.Shapefile sf = m_Map.get_GetObject(lyr.Handle) as MapWinGIS.Shapefile;
        //                        if (bounds.Contains(point))
        //                        {
        //                            //We are in the Expansion box
        //                            InExpansionBox = true;
        //                            return lyr;
        //                        }
        //                        else
        //                        {
        //                            // check to see if we are in the default color box
        //                            CurHeight = lyr.get_CategoryHeight(sf.DefaultDrawingOptions);
        //                            CurWidth = lyr.get_CategoryWidth(sf.DefaultDrawingOptions);
        //                            CurTop = lyr.Top + Constants.ITEM_HEIGHT + 2;
        //                            CurLeft = Constants.LIST_ITEM_INDENT + Constants.TEXT_LEFT_PAD;
        //                            if (CurWidth != Constants.ICON_WIDTH)
        //                            {
        //                                CurLeft -= ((CurWidth - Constants.ICON_WIDTH) / 2);
        //                            }
        //                            bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

        //                            if (bounds.Contains(point))
        //                            {
        //                                InColorBox = true;
        //                                return lyr;
        //                            }
        //                            else
        //                            {
        //                                CurLeft = Constants.LIST_ITEM_INDENT + Constants.TEXT_LEFT_PAD;
        //                                CurTop = lyr.Top + Constants.ITEM_HEIGHT + 2;   // name
        //                                CurTop += CurHeight + 2;                        // default symbology
        //                                CurTop += Constants.CS_ITEM_HEIGHT + 2;         // categories caption

        //                                for (int cat = 0; cat < sf.Categories.NumCategories; cat++)
        //                                {
        //                                    MapWinGIS.ShapeDrawingOptions options = sf.Categories.get_Item(cat).DrawingOptions;
        //                                    CurWidth = lyr.get_CategoryWidth(options);
        //                                    CurHeight = lyr.get_CategoryHeight(options);
        //                                    bounds = new Rectangle(CurLeft, CurTop, CurWidth, CurHeight);

        //                                    CurTop += CurHeight;

        //                                    if (bounds.Contains(point))
        //                                    {
        //                                        InColorBox = true;
        //                                        CategoryIndex = cat;
        //                                        return lyr;
        //                                    }
        //                                }

        //                                //we aren't in the checkbox, expansion box or colorbox
        //                                return lyr;
        //                            }
        //                        }
        //                    }
        //                }
        //            }
        //        }
        //    }
        //    return null;
        //}

        /// <summary>
        /// Gets whether or not the legend is locked.  See Lock() function for description
        /// </summary>
        public bool Locked
        {
            get
            {
                if (m_LockCount > 0)
                    return true;
                else
                    return false;
            }
        }

        /// <summary>
        /// Gets the number of groups that exist in the legend
        /// </summary>
        protected internal int NumGroups
        {
            get
            {
                return this.m_AllGroups.Count;
            }
        }

        /// <summary>
        /// Redraw the Legend if not locked - See 'Locked' Property for more details
        /// </summary>
        protected internal void Redraw()
        {
            if (Locked == false)
            {
                //Application.DoEvents();
                this.Invalidate();
            }
        }

        public void FullRedraw()
        {
            if (Locked == false)
            {
                //Application.DoEvents();
                this.Invalidate();
            }
        }

        public void RedrawLegendAndMap()
        {
            if (!Locked)
            {
                this.Map.Redraw();
                this.Invalidate();
            }
        }

        /// <summary>
        /// Clears all layers
        /// </summary>
        protected internal void ClearLayers()
        {
            Group grp = null;
            int GrpCount = m_AllGroups.Count;

            for (int i = 0; i < GrpCount; i++)
            {
                grp = (Group)m_AllGroups[i];
                grp.Layers.Clear();
            }
            m_Map.RemoveAllLayers();

            Redraw();

            FireLayerSelected(-1);
        }

        /// <summary>
        /// Clears all groups
        /// </summary>
        protected internal void ClearGroups()
        {
            ClearLayers();
            m_Map.RemoveAllLayers();
            m_AllGroups.Clear();

            //int count = m_GroupPositions.Count;
            //for(int i = 0; i < count; i++)
            //    m_GroupPositions[i] = INVALID_GROUP;

            //Christian Degrassi 2010-02-18: Fixes issue 0001572
            m_GroupPositions.Clear();

            Redraw();
        }

        private int CalcTotalDrawHeight(bool UseExpandedHeight)
        {
            int i = 0,
                count = m_AllGroups.Count,
                retval = 0;

            if (UseExpandedHeight == true)
            {
                for (i = 0; i < count; i++)
                {
                    Groups[i].RecalcHeight();
                    retval += Groups[i].ExpandedHeight;
                }
            }
            else
            {
                for (i = 0; i < count; i++)
                {
                    Groups[i].RecalcHeight();
                    retval += Groups[i].Height + Constants.ITEM_PAD;
                }
            }

            return retval;
        }

        private void RecalcItemPositions()
        {
            //this function calculates the top of each group and layer.
            //this is important because the click events use the stored top as
            //the way of figuring out if the item was clicked
            //and if the checkbox or expansion box was clicked

            int TotalHeight = CalcTotalDrawHeight(false);
            Group grp;
            Layer lyr;
            int CurTop = 0;

            if (vScrollBar.Visible == true)
                CurTop = -vScrollBar.Value;

            for (int i = m_AllGroups.Count - 1; i >= 0; i--)
            {
                grp = (Group)m_AllGroups[i];
                grp.Top = CurTop;
                if (grp.Expanded)
                {
                    CurTop += Constants.ITEM_HEIGHT;
                    for (int j = grp.Layers.Count - 1; j >= 0; j--)
                    {
                        lyr = (Layer)grp.Layers[j];
                        if (!lyr.HideFromLegend)
                        {
                            lyr.Top = CurTop;

                            CurTop += lyr.Height;
                        }
                    }
                    CurTop += Constants.ITEM_PAD;
                }
                else
                    CurTop += grp.Height + Constants.ITEM_PAD;
            }
        }

        private void DrawNextFrame()
        {
            // bool scrollBarChanged = false;
            //System.Diagnostics.Stopwatch watch = new System.Diagnostics.Stopwatch();
            //watch.Start();

            if (Locked == false)
            {
                int TotalHeight = CalcTotalDrawHeight(false);
                Rectangle rect;
                if (TotalHeight > this.Height)
                {
                    // scrollBarChanged = true;
                    vScrollBar.Minimum = 0;
                    vScrollBar.SmallChange = Constants.ITEM_HEIGHT;
                    vScrollBar.LargeChange = this.Height;
                    vScrollBar.Maximum = TotalHeight;

                    if (vScrollBar.Visible == false)
                    {
                        vScrollBar.Value = 0;
                        vScrollBar.Visible = true;
                        //m_painting = true;
                        //Application.DoEvents();
                        //m_painting = false;
                    }

                    RecalcItemPositions();
                    rect = new Rectangle(0, -vScrollBar.Value, this.Width - vScrollBar.Width, TotalHeight);
                }
                else
                {
                    vScrollBar.Visible = false;
                    //m_painting = true;
                    //Application.DoEvents();
                    //m_painting = false;
                    rect = new Rectangle(0, 0, this.Width, this.Height);
                }

                m_Draw.Clear(Color.White);

                int NumGroups = m_AllGroups.Count;
                Group grp = null;

                for (int i = NumGroups - 1; i >= 0; i--)
                {
                    grp = (Group)m_AllGroups[i];
                    if (rect.Top + grp.Height < this.ClientRectangle.Top)
                    {
                        //update the drawing rectangle
                        rect.Y += grp.Height + Constants.ITEM_PAD;

                        //move on to the next group
                        continue;
                    }
                    DrawGroup(m_Draw, grp, rect, false);
                    rect.Y += grp.Height + Constants.ITEM_PAD;
                    if (rect.Top >= this.ClientRectangle.Bottom)
                        break;//
                    //rect.Height -= grp.Height + Constants.ITEM_PAD;
                }
            }

            //watch.Stop();
            //MessageBox.Show(watch.Elapsed.ToString());
            SwapBuffers();
        }

        /// <summary>
        /// The Control is being redrawn
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaint(System.Windows.Forms.PaintEventArgs e)
        {
            // we don't want to paint when when statusbar visibility changed
            if (m_painting)
                return;

            m_FrontBuffer = e.Graphics;

            //			m_FrontBuffer.CompositingMode = System.Drawing.Drawing2D.CompositingMode.SourceCopy;
            //			m_FrontBuffer.CompositingQuality = System.Drawing.Drawing2D.CompositingQuality.HighSpeed;
            DrawNextFrame();
        }

        /// <summary>
        /// the background of the control is to be redrawn
        /// </summary>
        /// <param name="e"></param>
        protected override void OnPaintBackground(System.Windows.Forms.PaintEventArgs e)
        {
            //do nothing, this helps us avoid flicker

            //if we don't override this function, then
            //the system will clear that background before
            //we draw, causing a flicker when resizing
        }

        private void HandleLeftMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (m_DragInfo.Dragging == true || m_DragInfo.MouseDown == true)
            {//someting went wrong and the legend got locked but never got unlocked
                if (m_DragInfo.LegendLocked)
                    this.Unlock();
                m_DragInfo.Reset();
            }

            Group grp = null;
            Layer lyr = null;
            Point pnt = new Point(e.X, e.Y);

            m_DragInfo.Reset();

            //pnt = this.PointToClient(pnt);
            bool InCheckBox = false,
                InExpandBox = false;


            grp = FindClickedGroup(pnt, out InCheckBox, out InExpandBox);
            if (grp != null)
            {
                if (InCheckBox == true)
                {
                    if (!grp.StateLocked)
                    {
                        if (grp.VisibleState == VisibleStateEnum.vsALL_VISIBLE)
                            grp.VisibleState = VisibleStateEnum.vsALL_HIDDEN;
                        else
                            grp.VisibleState = VisibleStateEnum.vsALL_VISIBLE;

                        try
                        {
                            FireGroupCheckboxClicked(grp.Handle, grp.VisibleState);
                        }
                        catch
                        {
                            // We don't care about plug-in exceptions here
                        }

                        Redraw();

                        return;
                    }
                }
                else if (InExpandBox == true)
                {
                    grp.Expanded = !grp.Expanded;
                    FireGroupExpandedChanged(grp.Handle, grp.Expanded);
                    Redraw();
                    return;
                }
                else
                {
                    //set up group dragging
                    if (m_AllGroups.Count > 1)
                    {
                        m_DragInfo.StartGroupDrag(pnt.Y, (int)m_GroupPositions[grp.Handle]);
                        //m_DragInfo.StartDrag(pnt.Y,(int)m_GroupPositions[grp.Handle],Constants.INVALID_INDEX);
                    }
                }
                FireGroupMouseDown(grp.Handle, MouseButtons.Left);
                return;
            }

            // -------------------------------------------------------
            //      Selecting a layer
            // -------------------------------------------------------
            ClickedElement element = new ClickedElement();

            lyr = FindClickedLayer(pnt, ref element);
            if (lyr != null)
            {
                grp = (Group)m_AllGroups[element.GroupIndex];
                if (element.CheckBox)
                {
                    bool NewState = !m_Map.get_LayerVisible(lyr.Handle);

                    bool cancel = false;
                    FireLayerVisibleChanged(lyr.Handle, NewState, ref cancel);
                    if (cancel == true)
                        return;

                    m_Map.set_LayerVisible(lyr.Handle, NewState);

                    grp = (Group)m_AllGroups[element.GroupIndex];
                    grp.UpdateGroupVisibility();

                    FireLayerCheckboxClicked(lyr.Handle, NewState);
                    Redraw();
                    return;
                }
                else if (element.ExpansionBox)
                {
                    lyr.Expanded = !lyr.Expanded;
                    FireLayerPropertiesChanged(lyr.Handle);
                    Redraw();
                    return;
                }
                else if (element.ColorBox && element.CategoryIndex == -1)
                {
                    // default symbology
                    FireLayerColorboxClicked(lyr.Handle);
                    Redraw();
                    return;
                }
                else if (element.LabelsIcon)
                {
                    this.FireLayerLabelsClicked(lyr.Handle);
                    return;
                    //this.Redraw();
                }
                else if (element.ColorBox == true && element.CategoryIndex != -1)
                {
                    // category symbology
                    FireLayerCategoryClicked(lyr.Handle, element.CategoryIndex);
                    Redraw();
                    return;
                }
                else if (element.Charts && element.ChartFieldIndex == -1)
                {
                    // default symbology
                    FireLayerChartClicked(lyr.Handle);
                    Redraw();
                    return;
                }
                else if (element.Charts == true && element.ChartFieldIndex != -1)
                {
                    // category symbology
                    FireLayerChartFieldClicked(lyr.Handle, element.ChartFieldIndex);
                    Redraw();
                    return;
                }
                else
                {
                    //Christian Degrassi 2010-02-25: Removing this if() fixes issue 1580
                    ////just select the Layer
                    //if (lyr.Handle != m_SelectedLayerHandle)
                    //    FireLayerSelected(lyr.Handle);

                    SelectedLayer = lyr.Handle;

                    if (m_AllGroups.Count > 1 || grp.Layers.Count > 1)
                    {
                        m_DragInfo.StartLayerDrag(pnt.Y, (int)m_GroupPositions[grp.Handle], grp.LayerPositionInGroup(lyr.Handle));
                        //m_DragInfo.StartDrag(pnt.Y,(int)m_GroupPositions[grp.Handle],grp.LayerPositionInGroup(lyr.Handle));
                    }

                    FireLayerMouseDown(lyr.Handle, MouseButtons.Left);
                    return;
                }
            }

            FireLegendClick(MouseButtons.Left, pnt);

            Redraw();
        }

        private void HandleRightMouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            Group grp = null;
            Layer lyr = null;

            Point pnt = new Point(e.X, e.Y);

            bool InCheckBox = false,
                InExpandBox = false;
            grp = FindClickedGroup(pnt, out InCheckBox, out InExpandBox);
            if (grp != null)
            {
                if (InCheckBox == false && InExpandBox == false)
                {
                    FireGroupMouseDown(grp.Handle, MouseButtons.Right);
                }
                return;
            }

            ClickedElement element = new ClickedElement();
            lyr = FindClickedLayer(pnt, ref element);
            if (lyr != null)
            {
                if (element.CheckBox == false && element.ExpansionBox == false)
                {
                    FireLayerMouseDown(lyr.Handle, MouseButtons.Right);
                }
                else if (element.LabelsIcon)
                {
                    this.FireLayerLabelsClicked(lyr.Handle);
                    return;
                    //this.Redraw();
                }
                return;
            }
            FireLegendClick(MouseButtons.Right, pnt);
        }

        private void HandleRightMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            Group grp = null;
            Layer lyr = null;

            Point pnt = new Point(e.X, e.Y);

            bool InCheckBox = false, InExpandBox = false;
            grp = FindClickedGroup(pnt, out InCheckBox, out InExpandBox);
            if (grp != null)
            {
                if (InCheckBox == false && InExpandBox == false)
                {
                    FireGroupMouseUp(grp.Handle, MouseButtons.Right);
                }
                return;
            }

            ClickedElement element = new ClickedElement();

            lyr = FindClickedLayer(pnt, ref element);
            if (lyr != null)
            {
                if (element.CheckBox == false && element.ExpansionBox == false)
                {
                    FireLayerMouseUp(lyr.Handle, MouseButtons.Right);
                }
                return;
            }
        }

        private void Legend_MouseDown(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            switch (e.Button)
            {
                case System.Windows.Forms.MouseButtons.Left:
                    HandleLeftMouseDown(sender, e);
                    break;
                case System.Windows.Forms.MouseButtons.Right:
                    HandleRightMouseDown(sender, e);
                    break;
            }
        }

        private void HandleLeftMouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            this.Capture = false;
            Point pnt = new Point(e.X, e.Y);

            Layer lyr = null;
            Group grp = null;
            Group TargetGroup = null;

            m_DragInfo.MouseDown = false;

            if (m_DragInfo.Dragging == true)
            {
                if (m_DragInfo.LegendLocked == true)
                {
                    m_DragInfo.LegendLocked = false;
                    Unlock();//unlock the legend
                }

                m_MidBuffer = null;

                if (m_DragInfo.DraggingLayer)
                {
                    if (m_DragInfo.TargetGroupIndex != Constants.INVALID_INDEX)
                    {
                        TargetGroup = m_GroupManager[m_DragInfo.TargetGroupIndex];
                        grp = (Group)m_AllGroups[m_DragInfo.DragGroupIndex];

                        int OldPos = 0,
                            NewPos = 0,
                            LayerHandle = -1,
                            temp = 0;

                        LayerHandle = grp.LayerHandle(m_DragInfo.DragLayerIndex);

                        if (TargetGroup.Handle == grp.Handle)
                        {
                            //movement within the same group

                            FindLayerByHandle(LayerHandle, out temp, out OldPos);

                            //we may have to adjust the new position if moving up in the group
                            //because the way we are using TargetLayerIndex is marking things differently
                            //than the moveLayer function expects it
                            if (OldPos < m_DragInfo.TargetLayerIndex)
                                NewPos = m_DragInfo.TargetLayerIndex - 1;
                            else
                                NewPos = m_DragInfo.TargetLayerIndex;
                        }
                        else
                        {
                            //movement from one group to another group
                            NewPos = m_DragInfo.TargetLayerIndex;
                        }

                        MoveLayer(TargetGroup.Handle, LayerHandle, NewPos);
                    }
                }
                else
                {//we are dragging a group
                    if (IsValidIndex(m_AllGroups, m_DragInfo.DragGroupIndex) == false)
                    {
                        m_DragInfo.Reset();
                        return;
                    }

                    int grpHandle = ((Group)m_AllGroups[m_DragInfo.DragGroupIndex]).Handle;

                    //adjust the target group index because we are setting TargetGroupIndex
                    //differently than the MoveGroup Function expects it
                    if (m_DragInfo.DragGroupIndex < m_DragInfo.TargetGroupIndex)
                        m_DragInfo.TargetGroupIndex -= 1;

                    MoveGroup(grpHandle, m_DragInfo.TargetGroupIndex);
                }

                m_DragInfo.Reset();
                Redraw();
            }

            //are we completing a mouseup on a group?
            bool InCheck = false;
            bool InExpansion = false;
            grp = FindClickedGroup(pnt, out InCheck, out InExpansion);
            if (grp != null && InCheck == false && (InExpansion == false || grp.Layers.Count == 0))
            {
                FireGroupMouseUp(grp.Handle, MouseButtons.Left);
                return;
            }

            InCheck = false;
            InExpansion = false;
            //now figure out if we are completing a mouseup on a layer
            lyr = FindClickedLayer(pnt, out InCheck, out InExpansion);
            if (lyr != null && InCheck == false)
            {
                if (InExpansion == false)
                {
                    FireLayerMouseUp(lyr.Handle, MouseButtons.Left);
                    return;
                }
                else
                {
                    if (lyr.ColorLegend.Count == 0)
                    {
                        //if the expansion box is hidden, then fire a mouse up
                        FireLayerMouseUp(lyr.Handle, MouseButtons.Left);
                        return;
                    }
                }
            }

            //if no other mouseup event is send, then send the LegendMouseUp
            FireLegendClick(MouseButtons.Left, pnt);
        }

        private bool IsValidIndex(ArrayList list, int index)
        {
            if (index >= list.Count)
                return false;
            if (index < 0)
                return false;

            return true;
        }

        private void UpdateMapLayerPositions()
        {
            int GrpCount = m_AllGroups.Count;
            int LyrCount;
            Layer lyr = null;
            Group grp = null;
            int lyrPosition;

            m_Map.LockWindow(MapWinGIS.tkLockMode.lmLock);
            for (int i = GrpCount - 1; i >= 0; i--)
            {
                grp = (Group)m_AllGroups[i];
                LyrCount = grp.Layers.Count;
                for (int j = LyrCount - 1; j >= 0; j--)
                {
                    lyr = (Layer)grp.Layers[j];
                    lyrPosition = m_Map.get_LayerPosition(lyr.Handle);
                    m_Map.MoveLayerBottom(lyrPosition);
                }
            }
            m_Map.LockWindow(MapWinGIS.tkLockMode.lmUnlock);
        }

        private void Legend_MouseUp(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (e.Button == System.Windows.Forms.MouseButtons.Left)
                HandleLeftMouseUp(sender, e);
            if (e.Button == MouseButtons.Right)
                HandleRightMouseUp(sender, e);

            if (m_DragInfo.Dragging == true)
            {
                if (m_DragInfo.LegendLocked)
                    this.Unlock();
                m_DragInfo.Reset();
            }
        }

        private void Legend_MouseMove(object sender, System.Windows.Forms.MouseEventArgs e)
        {
            if (m_DragInfo.MouseDown == true && Math.Abs(m_DragInfo.StartY - e.Y) > 10)
            {
                m_DragInfo.Dragging = true;
                if (m_DragInfo.LegendLocked == false)
                {
                    Lock();//lock the Legend
                    m_DragInfo.LegendLocked = true;
                }
            }

            if (m_DragInfo.Dragging == true)
            {
                FindDropLocation(e.Y);
                DrawDragLine(m_DragInfo.TargetGroupIndex, m_DragInfo.TargetLayerIndex);
            }
            //			else
            //			{
            //				bool InCheck, InExpand;
            //				//show a tooltip if the mouse is over a layer
            //				Layer lyr = FindClickedLayer(new Point(e.X,e.Y),out InCheck, out InExpand);
            //				if(lyr != null)
            //				{
            //					m_ToolTip.AutoPopDelay = 5000;
            //					m_ToolTip.InitialDelay = 1000;
            //					m_ToolTip.ReshowDelay = 500;
            //					m_ToolTip.ShowAlways = false;
            //
            //					string caption = m_Map.get_LayerName(lyr.Handle);
            //
            //					m_ToolTip.SetToolTip(this,caption);
            //
            //				}
            //			}
        }

        private void DrawDragLine(int GrpIndex, int LyrIndex)
        {
            int DrawY = 0;
            Group grp = null;

            if (m_DragInfo.Dragging)
            {
                if (IsValidIndex(m_AllGroups, GrpIndex) == true)
                    grp = (Group)m_AllGroups[GrpIndex];

                if (m_DragInfo.DraggingLayer)
                {
                    if (grp == null)
                        return; //don't draw anything

                    int LayerCount = grp.Layers.Count;

                    if (LyrIndex < 0 && LayerCount > 0)
                    {//the item goes at the bottom of the list
                        DrawY = ((Layer)grp.Layers[0]).Top + ((Layer)grp.Layers[0]).Height;
                    }
                    if (LayerCount > LyrIndex && LyrIndex >= 0)
                    {
                        int ItemTop = ((Layer)grp.Layers[LyrIndex]).Top;
                        DrawY = ItemTop + ((Layer)grp.Layers[LyrIndex]).Height;
                    }
                    else
                    {//the layer is to be placed at the top of the list
                        DrawY = grp.Top + Constants.ITEM_HEIGHT;
                    }
                }
                else
                {//we are dragging a group
                    if (GrpIndex < 0 || GrpIndex >= (int)m_AllGroups.Count)
                    {//the mouse is either above the top layer or below the bottom layer
                        if (GrpIndex < 0)
                            DrawY = ((Group)m_AllGroups[0]).Top + ((Group)m_AllGroups[0]).Height;
                        else
                            DrawY = ((Group)m_AllGroups[m_AllGroups.Count - 1]).Top;
                    }
                    else
                    {
                        //if(grp.Expanded == true)
                        DrawY = grp.Top + grp.Height;//CalcGroupHeight(grp);
                        //else
                        //	DrawY = grp.Top + grp.Height;//CalcGroupHeight(grp);
                    }
                }

                m_FrontBuffer = this.CreateGraphics();
                if (m_MidBuffer == null)
                    m_MidBuffer = new Bitmap(m_BackBuffer.Width, m_BackBuffer.Height, m_Draw);

                Graphics LocalDraw = Graphics.FromImage(m_MidBuffer);
                SwapBuffers(m_BackBuffer, LocalDraw);

                Pen pen = (Pen)Pens.Gray.Clone();
                pen.Width = 3;

                //draw a horizontal line
                LocalDraw.DrawLine(pen, Constants.ITEM_PAD, DrawY, this.Width - Constants.ITEM_RIGHT_PAD, DrawY);

                //draw the left vertical line
                LocalDraw.DrawLine(pen, Constants.ITEM_PAD, DrawY - 3, Constants.ITEM_PAD, DrawY + 3);

                //draw the right vertical line
                LocalDraw.DrawLine(pen, this.Width - Constants.ITEM_RIGHT_PAD, DrawY - 3, this.Width - Constants.ITEM_RIGHT_PAD, DrawY + 3);

                SwapBuffers(m_MidBuffer, m_FrontBuffer);
            }
        }

        private void FindDropLocation(int YPosition)
        {
            m_DragInfo.TargetGroupIndex = Constants.INVALID_INDEX;
            m_DragInfo.TargetLayerIndex = Constants.INVALID_INDEX;

            int grpCount, itemCount;
            Group grp = null;
            Group TopGroup = null,
                BottomGroup = null,
                TempGroup = null;
            Layer lyr = null;
            int grpHeight;

            grpCount = m_AllGroups.Count;

            if (grpCount < 1)
                return;

            TopGroup = (Group)m_AllGroups[grpCount - 1];
            BottomGroup = (Group)m_AllGroups[0];

            if (m_DragInfo.DraggingLayer == true)
            {
                if (YPosition >= (BottomGroup.Top + BottomGroup.Height))
                {//the mouse is below the bottom layer, mark for drop at bottom
                    m_DragInfo.TargetGroupIndex = 0;
                    m_DragInfo.TargetLayerIndex = 0;

                    return;
                }
                else if (YPosition <= TopGroup.Top)
                {//the mouse is above the top layer, mark for drop at top
                    m_DragInfo.TargetGroupIndex = grpCount - 1;
                    m_DragInfo.TargetLayerIndex = TopGroup.Layers.Count;

                    return;
                }

                //not the bottom or the top, so we must search for the correct one
                for (int i = grpCount - 1; i >= 0; i--)
                {
                    grp = (Group)m_AllGroups[i];

                    grpHeight = grp.Height;

                    //can we drop it at the top of the group?
                    //if(YPosition <= grp.Top && YPosition < grp.Top+Constants.ITEM_HEIGHT)
                    if (YPosition < grp.Top + Constants.ITEM_HEIGHT)
                    {
                        m_DragInfo.TargetLayerIndex = grp.Layers.Count;
                        m_DragInfo.TargetGroupIndex = i;
                        return;
                    }
                    else
                    {
                        itemCount = grp.Layers.Count;

                        if (itemCount == 0)
                        {
                            //if(YPosition > grp.Top && YPosition <= grp.Top + grpHeight)
                            if (YPosition > grp.Top && YPosition <= grp.Top + Constants.ITEM_HEIGHT)
                            {
                                m_DragInfo.TargetGroupIndex = i;
                                m_DragInfo.TargetLayerIndex = Constants.INVALID_INDEX;
                                return;
                            }
                        }
                        else if (grp.Expanded == true)
                        {
                            for (int j = itemCount - 1; j >= 0; j--)
                            {
                                lyr = (Layer)grp.Layers[j];
                                if (YPosition <= (lyr.Top + lyr.Height))
                                {
                                    //drop before this item
                                    m_DragInfo.TargetGroupIndex = i;
                                    m_DragInfo.TargetLayerIndex = j;
                                    return;
                                }
                                if (j == 0)
                                {
                                    //if this item is the bottom one, check to see if the item can be
                                    //dropped after this item
                                    if (i > 0)//if the group is not the bottom group
                                    {
                                        TempGroup = (Group)m_AllGroups[i - 1];
                                        if (YPosition <= TempGroup.Top && YPosition > lyr.Top + lyr.Height)
                                        {
                                            m_DragInfo.TargetGroupIndex = i;
                                            m_DragInfo.TargetLayerIndex = 0;
                                            return;
                                        }
                                    }
                                    else
                                    {
                                        if (YPosition > lyr.Top + lyr.Height)
                                        {
                                            m_DragInfo.TargetGroupIndex = 0;
                                            m_DragInfo.TargetLayerIndex = 0;
                                        }
                                    }
                                }
                            }
                        }
                        else
                        {//the group is not expanded
                            if (YPosition > grp.Top && YPosition < grp.Top + grpHeight)
                            {
                                m_DragInfo.TargetGroupIndex = i;
                                m_DragInfo.TargetLayerIndex = grp.Layers.Count;//put the item at the top
                            }
                        }
                    }
                }
            }
            else
            {//we are dragging a group
                if (YPosition > (BottomGroup.Top + BottomGroup.Height))
                {//the mouse is below the bottom layer, mark for drop at bottom
                    m_DragInfo.TargetGroupIndex = Constants.INVALID_INDEX;
                    m_DragInfo.TargetLayerIndex = Constants.INVALID_INDEX;

                    return;
                }
                else if (YPosition <= TopGroup.Top)
                {//the mouse is above the top Group, mark for drop at top
                    m_DragInfo.TargetGroupIndex = grpCount;
                    m_DragInfo.TargetLayerIndex = Constants.INVALID_INDEX;
                    return;
                }

                //we have to compare against all groups because we aren't at the top or bottom
                for (int i = grpCount - 1; i >= 0; i--)
                {
                    grp = (Group)m_AllGroups[i];

                    if (YPosition < grp.Top + grp.Height)
                    {
                        m_DragInfo.TargetGroupIndex = i;
                        return;
                    }
                }
            }
        }

        //Christian Degrassi 2010-03-12: Method refactored to fix issues 1642
        /// <summary>
        /// Move a layer to a new location and/or group
        /// </summary>
        /// <param name="TargetGroupHandle">Handle of group into which to move the layer</param>
        /// <param name="LayerHandle">Handle of layer to move</param>
        /// <param name="NewPos">0-based index into list of layers within target group</param>
        /// <returns>True on success, False otherwise</returns>
        //protected internal bool MoveLayer( int TargetGroupHandle, int LayerHandle, int NewPos)
        //{
        //    Group SrcGrp = null,
        //        TargetGrp = null;
        //    int SrcGroupIndex = 0, LayerIndex = 0 ;

        //    int OldMapPos = m_Map.get_LayerPosition(LayerHandle);

        //    Layer lyr = null;
        //    lyr = FindLayerByHandle(LayerHandle,out SrcGroupIndex,out LayerIndex);

        //    if(lyr == null)
        //    {
        //        globals.LastError = "Invalid Layer Handle";
        //        return false;
        //    }

        //    SrcGrp = m_GroupManager[SrcGroupIndex];
        //    TargetGrp = m_GroupManager.ItemByHandle(TargetGroupHandle);

        //    if(TargetGrp == null)
        //    {
        //        globals.LastError = "Invalid Handle (TargetGroupHandle)";
        //        return false;
        //    }

        //    if(SrcGrp.Handle != TargetGrp.Handle)
        //    {
        //        SrcGrp.Layers.RemoveAt(LayerIndex);

        //        if(NewPos >= TargetGrp.Layers.Count)
        //            TargetGrp.Layers.Add(lyr);
        //        else if (NewPos <= 0)
        //            TargetGrp.Layers.Insert(0,lyr);
        //        else
        //            TargetGrp.Layers.Insert(NewPos,lyr);

        //        SrcGrp.RecalcHeight();
        //        TargetGrp.RecalcHeight();
        //        m_SelectedGroupHandle = TargetGrp.Handle;

        //        TargetGrp.UpdateGroupVisibility();
        //        SrcGrp.UpdateGroupVisibility();
        //    }
        //    else
        //    {
        //        if(LayerIndex == NewPos)
        //            return true;

        //        //				if(NewPos > LayerIndex)
        //        //				{
        //        //					if(NewPos >= SrcGrp.Layers.Count)
        //        //						SrcGrp.Layers.Add(lyr);
        //        //					else if (NewPos <= 0)
        //        //						SrcGrp.Layers.Insert(0,lyr);
        //        //					else
        //        //						SrcGrp.Layers.Insert(NewPos,lyr);
        //        //					SrcGrp.Layers.RemoveAt(LayerIndex);
        //        //				}
        //        //				else
        //        //{
        //        SrcGrp.Layers.RemoveAt(LayerIndex);
        //        if(NewPos >= SrcGrp.Layers.Count)
        //            SrcGrp.Layers.Add(lyr);
        //        else if (NewPos <= 0)
        //            SrcGrp.Layers.Insert(0,lyr);
        //        else
        //            SrcGrp.Layers.Insert(NewPos,lyr);
        //        //}

        //        SrcGrp.RecalcHeight();
        //        SrcGrp.UpdateGroupVisibility();

        //    }

        //    UpdateMapLayerPositions();

        //    int NewMapPos = m_Map.get_LayerPosition(LayerHandle);

        //    if(OldMapPos != NewMapPos)
        //    {
        //        int CurHandle, CurPos, EndPos;
        //        CurPos = Math.Min(OldMapPos,NewMapPos);
        //        EndPos = Math.Max(OldMapPos,NewMapPos);

        //        while(CurPos <= EndPos)
        //        {
        //            CurHandle = m_Map.get_LayerHandle(CurPos);
        //            FireLayerPositionChanged(CurHandle);
        //            CurPos += 1;
        //        }
        //    }

        //    Redraw();
        //    return true;

        //}

        /// <summary>
        /// Handles Layer position changes within groups
        /// </summary>
        /// <param name="Lyr">The Layer being moved</param>
        /// <param name="SourceGroup">The Source group</param>
        /// <param name="DestinationGroup">The Destination group. Can be the same as the Source</param>
        /// <param name="TargetPosition">The target position within a group</param>
        /// <returns></returns>
        private void ChangeLayerPosition(int CurrentPositionInGroup, Group SourceGroup, int TargetPositionInGroup, Group DestinationGroup)
        {
            //Christian Degrassi 2010-03-12: Support method to fix issues 1642
            Layer Lyr = null;

            if (CurrentPositionInGroup < 0 || CurrentPositionInGroup >= SourceGroup.Layers.Count)
                throw new Exception("Invalid Layer Index");

            Lyr = (Layer)SourceGroup.Layers[CurrentPositionInGroup];
            SourceGroup.Layers.Remove(Lyr);

            if (TargetPositionInGroup >= DestinationGroup.Layers.Count)
                DestinationGroup.Layers.Add(Lyr);
            else if (TargetPositionInGroup <= 0)
                DestinationGroup.Layers.Insert(0, Lyr);
            else
                DestinationGroup.Layers.Insert(TargetPositionInGroup, Lyr);

            SourceGroup.RecalcHeight();
            SourceGroup.UpdateGroupVisibility();

            if (SourceGroup.Handle != DestinationGroup.Handle)
            {
                DestinationGroup.RecalcHeight();
                DestinationGroup.UpdateGroupVisibility();

                m_SelectedGroupHandle = DestinationGroup.Handle;
            }
        }

        //Christian Degrassi 2010-03-12: Refactored method to fix issues 1642
        /// <summary>
        /// Move a layer to a new location and/or group
        /// </summary>
        /// <param name="TargetGroupHandle">Handle of group into which to move the layer</param>
        /// <param name="LayerHandle">Handle of layer to move</param>
        /// <param name="NewPos">0-based index into list of layers within target group</param>
        /// <returns>True if Layer position has changed, False otherwise</returns>
        protected internal bool MoveLayer(int TargetGroupHandle, int LayerHandle, int TargetPositionInGroup)
        {
            Group SourceGroup = null;
            Group DestinationGroup = null;
            //Layer Lyr = null;

            int SourceGroupIndex = 0;
            int CurrentPositionInGroup = 0;
            int OldMapPos;
            int NewMapPos;

            bool result = false;

            try
            {
                if (!m_LayerManager.IsValidHandle(LayerHandle))
                    throw new Exception("Invalid Handle (LayerHandle)");

                if (!IsValidGroup(TargetGroupHandle))
                    throw new Exception("Invalid Handle (TargetGroupHandle)");

                FindLayerByHandle(LayerHandle, out SourceGroupIndex, out CurrentPositionInGroup);

                SourceGroup = Groups[SourceGroupIndex];
                DestinationGroup = m_GroupManager.ItemByHandle(TargetGroupHandle);

                if (CurrentPositionInGroup != TargetPositionInGroup || SourceGroup.Handle != DestinationGroup.Handle)
                {
                    OldMapPos = m_Map.get_LayerPosition(LayerHandle);

                    ChangeLayerPosition(CurrentPositionInGroup, SourceGroup, TargetPositionInGroup, DestinationGroup);
                    UpdateMapLayerPositions();

                    NewMapPos = m_Map.get_LayerPosition(LayerHandle);

                    int CurHandle;
                    int CurPos;
                    int EndPos;

                    CurPos = Math.Min(OldMapPos, NewMapPos);
                    EndPos = Math.Max(OldMapPos, NewMapPos);

                    while (CurPos <= EndPos)
                    {
                        CurHandle = m_Map.get_LayerHandle(CurPos);
                        FireLayerPositionChanged(CurHandle);
                        CurPos += 1;
                    }

                    Redraw();
                }

                result = true;
            }
            catch (Exception ex)
            {
                globals.LastError = ex.Message;
                result = false;
            }

            return result;
        }

        /// <summary>
        /// Moves a group to a new location
        /// </summary>
        /// <param name="GroupHandle">Handle of group to move</param>
        /// <param name="NewPos">0-Based index of new location</param>
        /// <returns>True on success, False otherwise</returns>
        protected internal bool MoveGroup(int GroupHandle, int NewPos)
        {
            if (IsValidGroup(GroupHandle))
            {
                int OldPos = (int)m_GroupPositions[GroupHandle];

                if (OldPos == NewPos)
                    return true;

                Group grp = m_GroupManager.ItemByHandle(GroupHandle);

                if (NewPos < 0)
                {
                    NewPos = 0;
                }

                if (NewPos >= NumGroups)
                {
                    m_AllGroups.RemoveAt(OldPos);
                    m_AllGroups.Add(grp);
                }
                else
                {
                    m_AllGroups.RemoveAt(OldPos);
                    m_AllGroups.Insert(NewPos, grp);
                }

                if (grp.Layers.Count > 0)
                {//now we have to move the layers around
                    UpdateMapLayerPositions();
                }

                UpdateGroupPositions();
                Redraw();

                FireGroupPositionChanged(grp.Handle);
                return true;
            }
            else
            {
                globals.LastError = "Invalid Group Handle";
                return false;
            }
        }

        private void Legend_DoubleClick(object sender, System.EventArgs e)
        {
            Group grp = null;
            Layer lyr = null;

            Point pnt = globals.GetCursorLocation();
            pnt = this.PointToClient(pnt);

            bool InCheckBox = false, InExpandBox = false;

            grp = FindClickedGroup(pnt, out InCheckBox, out InExpandBox);
            if (grp != null)
            {
                if (InCheckBox == false && InExpandBox == false)
                {
                    FireGroupDoubleClick(grp.Handle);
                }
                return;
            }

            ClickedElement element = new ClickedElement();

            lyr = FindClickedLayer(pnt, ref element);
            if (lyr != null)
            {
                if (element.CheckBox == false && element.ExpansionBox == false)
                {
                    FireLayerDoubleClick(lyr.Handle);
                }
                return;
            }
        }

        private void DrawTransparentPatch(Graphics DrawTool, int TopPos, int LeftPos, int BoxHeight, int BoxWidth, Color OutlineColor, bool DrawOutline)
        {
            Rectangle rect = new Rectangle(LeftPos, TopPos, BoxWidth, BoxHeight);
            Pen pen = new Pen(OutlineColor);

            //fill the rectangle with a diagonal hatch
            System.Drawing.Brush brush = new System.Drawing.Drawing2D.HatchBrush(System.Drawing.Drawing2D.HatchStyle.LightUpwardDiagonal, m_BoxLineColor, Color.White);
            DrawTool.FillRectangle(brush, rect);

            if (DrawOutline) DrawTool.DrawRectangle(pen, LeftPos, TopPos, BoxWidth, BoxHeight);
            //			//draw the Top border
            //			DrawTool.DrawLine(pen,LeftPos,TopPos,LeftPos+BoxWidth,TopPos);
            //
            //			//draw the Left border
            //			DrawTool.DrawLine(pen,LeftPos,TopPos,LeftPos,TopPos+BoxHeight);
            //
            //			//draw the Bottom border
            //			DrawTool.DrawLine(pen,LeftPos,TopPos+BoxHeight,LeftPos+BoxWidth,TopPos+BoxHeight);
            //
            //			//draw the Right border
            //			DrawTool.DrawLine(pen,LeftPos+BoxWidth,TopPos,LeftPos+BoxWidth,TopPos+BoxHeight);
        }

        private void DrawColorPatch(Graphics DrawTool, Color StartColor, Color EndColor, int TopPos, int LeftPos, int BoxHeight, int BoxWidth, Color OutlineColor, bool DrawOutline)
        {
            DrawColorPatch(DrawTool, StartColor, EndColor, TopPos, LeftPos, BoxHeight, BoxWidth, OutlineColor, DrawOutline, eLayerType.Invalid);
        }

        private void DrawColorPatch(Graphics DrawTool, Color StartColor, Color EndColor, int TopPos, int LeftPos, int BoxHeight, int BoxWidth, Color OutlineColor, bool DrawOutline, eLayerType LayerType)
        {
            // Note - LayerType == invalid when we don't care :)

            if (LayerType == eLayerType.LineShapefile)
            {
                if (StartColor.A == 0) StartColor = Color.FromArgb(255, StartColor);
                Pen pen = new Pen(StartColor, 2);

                System.Drawing.Drawing2D.SmoothingMode OldSmoothingMode = DrawTool.SmoothingMode;
                DrawTool.SmoothingMode = System.Drawing.Drawing2D.SmoothingMode.AntiAlias;

                DrawTool.DrawLine(pen, LeftPos, TopPos + 8, LeftPos + 4, TopPos + 3);
                DrawTool.DrawLine(pen, LeftPos + 4, TopPos + 3, LeftPos + 9, TopPos + 10);
                DrawTool.DrawLine(pen, LeftPos + 9, TopPos + 10, LeftPos + 13, TopPos + 4);

                DrawTool.SmoothingMode = OldSmoothingMode;
            }
            else
            {
                Rectangle rect = new Rectangle(LeftPos, TopPos, BoxWidth, BoxHeight);
                Pen pen = new Pen(OutlineColor);

                //fill the rectangle with a gradient fill
                System.Drawing.Brush brush = new System.Drawing.Drawing2D.LinearGradientBrush(rect, StartColor, EndColor, System.Drawing.Drawing2D.LinearGradientMode.Horizontal);
                DrawTool.FillRectangle(brush, rect);

                if (DrawOutline) DrawTool.DrawRectangle(pen, LeftPos, TopPos, BoxWidth, BoxHeight);
            }
        }

        private void vScrollBar_Scroll(object sender, System.Windows.Forms.ScrollEventArgs e)
        {
            VScrollBar sbar = (VScrollBar)sender;
            sbar.Value = e.NewValue;
            Redraw();
        }

        /// <summary>
        /// handles mouse wheel event
        /// </summary>
        /// <param name="e"></param>
        protected override void OnMouseWheel(System.Windows.Forms.MouseEventArgs e)
        {
            if (vScrollBar.Visible == true)
            {
                int StepSize;
                int MaxSize = vScrollBar.Maximum - this.Height;

                StepSize = vScrollBar.SmallChange;
                if (e.Delta >= 0)
                    StepSize *= -1;

                if (vScrollBar.Value + StepSize < 0)
                {
                    vScrollBar.Value = 0;
                }
                else if (vScrollBar.Value + StepSize > MaxSize)
                {
                    vScrollBar.Value = MaxSize + 1;
                }
                else
                {
                    vScrollBar.Value += StepSize;
                }
                Redraw();
            }
        }
    }

    /// <summary>
    /// The element of legend that was clicked
    /// </summary>
    public class ClickedElement
    {
        public bool LabelsIcon = false;
        public bool ColorBox = false;
        public bool CheckBox = false;
        public bool ExpansionBox = false;
        public bool Charts = false;
        public bool Label = false;
        public int ChartFieldIndex = -1;
        public int CategoryIndex = -1;
        public int GroupIndex = -1;

        public void Nullify()
        {
            ColorBox = false;
            CheckBox = false;
            ExpansionBox = false;
            Charts = false;
            Label = false;
            ChartFieldIndex = -1;
            CategoryIndex = -1;
            GroupIndex = -1;
        }
    }

    /// <summary>
    /// Elements of the layer representation in the legend
    /// </summary>
    internal enum LayerElementType
    {
        None = 0,
        Name = 1,
        Symbol = 2,
        Label = 3,
        CategoriesCaption = 4,
        CategoryName = 5,
        ChartsCaption = 7,
        Charts = 8,
        ChartField = 9,
        ChartFieldName = 10,
    }

    /// <summary>
    /// The layer element, holding position and size of elements
    /// </summary>
    internal class LayerElement
    {
        internal LayerElementType ElementType = LayerElementType.None;
        internal int Index = -1;                // of category or field
        internal string Text = string.Empty;    // associated text (name or caption)
        // dimensions for easy click text
        internal int Top = 0;
        internal int Left = 0;
        internal int Width = 0;
        internal int Height = 0;

        internal LayerElement(LayerElementType type, int top, int left, int width, int height)
        {
            ElementType = type;
            Top = top;
            Left = left;
            Width = width;
            Height = height;
        }

        internal LayerElement(LayerElementType type, Rectangle rect, string text, int index)
        {
            ElementType = type;
            Top = rect.Top;
            Left = rect.Left;
            Width = rect.Width;
            Height = rect.Height;
            Index = index;
        }

        internal LayerElement(LayerElementType type, Rectangle rect) : this(type, rect, string.Empty, -1) { }

        internal LayerElement(LayerElementType type, Rectangle rect, int index) : this(type, rect, string.Empty, index) { }

        internal LayerElement(LayerElementType type, Rectangle rect, string text) : this(type, rect, text, -1) { }
    }
}