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

using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Controls.Legend
{
	/// <summary>
	/// Summary description for Group.
	/// </summary>
	public class Group
	{
		#region "Member Variables"

		private string m_Caption;

		/// <summary>
		/// A string that a developer can use to hold misc. information about this group
		/// </summary>
		public string Tag;
		private object m_Icon;
		private bool m_Expanded;
		private int m_Height;
		private Legend m_Legend;

		/// <summary>
		/// The Handle for this Group
		/// </summary>
		protected internal int m_Handle;

		/// <summary>
		/// The top position of this group
		/// </summary>
		protected internal int Top;

		/// <summary>
		/// List of All Layers contained within this group
		/// </summary>
		public List<Layer> Layers;
		
		private VisibleStateEnum m_VisibleState;

        protected internal bool m_StateLocked;

        /// <summary>
        /// Returns list of guids associated with layers
        /// </summary>
        public string LayersGuidList()
        {
            return string.Join(";", Layers.Select(item => item.GuidKey + (item.Expanded ? "1" : "0")).ToArray());
        }
		#endregion

		/// <summary>
		/// Constructor
		/// </summary>
		public Group(Legend leg)
		{
			//The next line MUST GO FIRST in the constructor
			m_Legend = leg;
			//The previous line MUST GO FIRST in the constructor

			Layers = new List<Layer>();
			Expanded = true;
			VisibleState = VisibleStateEnum.vsALL_VISIBLE;
			m_Handle = -1;
			Icon = null;
            m_StateLocked = false;
		}
		/// <summary>
		/// Destructor
		/// </summary>
		~Group()
		{
			m_Legend = null;
			Layers.Clear();
			Layers = null;
			m_Icon = null;
		}

		/// <summary>
		/// Gets or sets the Text that appears in the legend for this group
		/// </summary>
		public string Text
		{
			get
			{
				return m_Caption;
			}
			set 
			{
				m_Caption = value;
				m_Legend.Redraw();
			}
		}

		/// <summary>
		/// Gets or sets the icon that appears next to this group in the legend.
		/// Setting this value to null(nothing) removes the icon from the legend
		/// </summary>
		public object Icon
		{
			get
			{
				return m_Icon;
			}
			set 
			{
				if(globals.IsSupportedPicture(value))
				{
					m_Icon = value;
					m_Legend.Redraw();
				}
				else
				{
					throw new System.Exception("Legend Error: Invalid Group Icon type");
				}
			}
		}

		/// <summary>
		/// Gets the number of layers within this group
		/// </summary>
		public int LayerCount
		{
			get
			{
				return Layers.Count;
			}
		}
		
		/// <summary>
		/// 
		/// </summary>
		public Layer this[int LayerPosition]
		{
			get
			{
				if(LayerPosition >=0 && LayerPosition < this.Layers.Count)
					return (Layer)Layers[LayerPosition];

				globals.LastError = "Invalid Layer Position within Group";
					return null;
			}	
		}

		/// <summary>
		/// Gets the Handle (a unique identifier) to this group
		/// </summary>
		public int Handle
		{
			get
			{
				return m_Handle;
			}
		}

		/// <summary>
		/// Looks up a Layer by Handle within this group
		/// </summary>
		/// <param name="Handle">Handle of the Layer to lookup</param>
		/// <returns>Layer item if successful, null (nothing) on failure</returns>
		protected internal Layer LayerByHandle(int Handle)
		{
			int count = Layers.Count;
			Layer lyr = null;
			for(int i = 0; i < count; i++)
			{
				lyr = (Layer)Layers[i];
				if (lyr.Handle == Handle)
					return lyr;
			}
			return null;
		}

		/// <summary>
		/// Gets the Layer's position (index) within a group
		/// </summary>
		/// <param name="Handle">Layer Handle</param>
		/// <returns>0-Based index of the Layer on success, -1 on failure</returns>
		protected internal int LayerPositionInGroup(int Handle)
		{
			int count = Layers.Count;
			Layer lyr = null;
			for(int i = 0; i < count; i++)
			{
				lyr = (Layer)Layers[i];
				if (lyr.Handle == Handle)
					return i;
			}
			return -1;
		}

		/// <summary>
		/// Gets the layer handle of the specified layer
		/// </summary>
		/// <param name="PositionInGroup">0 based index into list of layers</param>
		/// <returns>Layer's handle on success, -1 on failure</returns>
		public int LayerHandle(int PositionInGroup)
		{
			if(PositionInGroup >=0 && PositionInGroup < Layers.Count)
				return ((Layer)Layers[PositionInGroup]).Handle;

			globals.LastError = "Invalid layer position within group";
			return -1;
		}

		/// <summary>
		/// Gets or sets whether or not the group is expanded.  This shows or hides the 
		/// layers within this group
		/// </summary>
		public bool Expanded
		{
			get
			{
				return m_Expanded;
			}
			set
			{
				if(value != m_Expanded)
				{
					m_Expanded = value;
					RecalcHeight();
					m_Legend.Redraw();
				}
			}
		}

		/// <summary>
		/// Gets the drawing height of the group
		/// </summary>
		protected internal int Height
		{
			get
			{
                RecalcHeight();
				return m_Height;
			}
		}

		/// <summary>
		/// Calculates the expanded height of the group
		/// </summary>
		protected internal int ExpandedHeight
		{
			get
			{
				int NumLayers = Layers.Count;
				//initialize the height to just the height of the group item
				int Retval = Constants.ITEM_HEIGHT;
				Layer lyr;

				//now add all the heights of the Layers
				for(int i = 0; i < NumLayers; i++)
				{
					lyr = (Layer)Layers[i];
					Retval += lyr.CalcHeight(true);
				}
				

				return Retval;
			}
		}


		/// <summary>
		/// Recalculates the Height of the Group
		/// </summary>
		protected internal void RecalcHeight()
		{
			int NumLayers = Layers.Count;
			//initialize the height to just the height of the group item
			m_Height = Constants.ITEM_HEIGHT;
			Layer lyr;

			if(m_Expanded == true)
			{
				//now add all the heights of the Layers
				for(int i = 0; i < NumLayers; i++)
				{
					lyr = (Layer)Layers[i];
					if (!lyr.HideFromLegend)
						m_Height += lyr.Height;
				}
			}
			else
			{
				m_Height = Constants.ITEM_HEIGHT;
			}
		}

		/// <summary>
		/// Gets or sets the visibility of the layers within this group.
		/// Note: When reading this property, it returns true if any layer is visible within
		/// this group
		/// </summary>
		public bool LayersVisible
		{
			get
			{
				if (VisibleState == VisibleStateEnum.vsALL_HIDDEN)
					return false;
				else
					return true;
			}
			set
			{
				if(value == true)
					VisibleState = VisibleStateEnum.vsALL_VISIBLE;
				else
					VisibleState = VisibleStateEnum.vsALL_HIDDEN;
			}
		}

		/// <summary>
		/// Gets or Sets the Visibility State for this group
		/// Note: Set cannot be vsPARTIAL_VISIBLE
		/// </summary>
		protected internal VisibleStateEnum VisibleState
		{
			get
			{
				return m_VisibleState;
			}
			set
			{
				if(value == VisibleStateEnum.vsPARTIAL_VISIBLE)
				{
					//not allowed
					throw new System.Exception("Invalid [Property set] value: vsPARTIAL_VISIBLE");					
				}
				
				m_VisibleState = value;
				UpdateLayerVisibility();
			}
		}

        /// <summary>
        /// gets or sets the locked property, which prevents the user from changing the visual state 
        /// except layer by layer
        /// </summary>
        public bool StateLocked
        {
            get
            {
                return m_StateLocked;
            }
            set
            {
                m_StateLocked = value;
            }
        }


		private void UpdateLayerVisibility()
		{
			int NumLayers = Layers.Count;
			Layer lyr = null;
			bool visible = false;
			if (m_VisibleState == VisibleStateEnum.vsALL_VISIBLE)
				visible = true;

			for(int i = 0; i < NumLayers; i++)
			{
				lyr = (Layer)Layers[i];
				bool oldState = m_Legend.m_Map.get_LayerVisible(lyr.Handle);

				m_Legend.m_Map.set_LayerVisible(lyr.Handle,visible);				

				if (oldState != visible)
				{
					bool cancel = false;
					m_Legend.FireLayerVisibleChanged(lyr.Handle,visible, ref cancel);
					if (cancel == true)
						lyr.Visible = !(visible);
				}
			}
		}

		/// <summary>
		/// Updates the Visibility State for this group depending on the visibility of each layer within the group.
		/// </summary>
		protected internal void UpdateGroupVisibility()
		{
			int NumVisible = 0;
			int NumLayers = Layers.Count;
			Layer lyr = null;
			for(int i = 0; i < NumLayers; i++)
			{
				lyr = (Layer)Layers[i];
				if(m_Legend.m_Map.get_LayerVisible(lyr.Handle) == true)
					NumVisible++;
			}

			if (NumVisible == NumLayers)
				m_VisibleState = VisibleStateEnum.vsALL_VISIBLE;
			else if (NumVisible == 0)
				m_VisibleState = VisibleStateEnum.vsALL_HIDDEN;
			else
				m_VisibleState = VisibleStateEnum.vsPARTIAL_VISIBLE;
		}

		/// <summary>
		/// Returns a snapshot image of this group
		/// </summary>
		/// <param name="imgWidth">Width in pixels of the returned image (height is determined by the number of layers in the group)</param>
		/// <returns>Bitmap of the group and sublayers (expanded)</returns>
		public System.Drawing.Bitmap Snapshot(int imgWidth)
		{
			Bitmap bmp = null;// = new Bitmap(imgWidth,imgHeight);
			Rectangle rect;

			System.Drawing.Graphics g;
					
			bmp = new Bitmap(imgWidth,this.ExpandedHeight);
			g = Graphics.FromImage(bmp);
			g.Clear(System.Drawing.Color.White);

			rect = new Rectangle(0,0,imgWidth,this.ExpandedHeight);

			m_Legend.DrawGroup(g,this,rect,true);

			return bmp;
		}

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font, int maxWidth)
        {
            return g.MeasureString(this.Text, font, maxWidth);
        }

        /// <summary>
        /// Measures the size of the layer's name string
        /// </summary>
        public SizeF MeasureCaption(Graphics g, Font font)
        {
            return g.MeasureString(this.Text, font);
        }
	}
}
