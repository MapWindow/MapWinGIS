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

using System.Drawing;
using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Controls.Legend
{
	/// <summary>
	/// Visibility State of a Group
	/// </summary>
	public enum VisibleStateEnum
	{
		/// <summary>
		/// All Layers are Visible
		/// </summary>
		vsALL_VISIBLE = 0,

		/// <summary>
		/// All Layers are Hidden
		/// </summary>
		vsALL_HIDDEN = 1,

		/// <summary>
		/// Mixed Layer Visibility
		/// </summary>
		vsPARTIAL_VISIBLE = 2,
	}

	internal struct ColorInfo
	{
		#region "Member Variables"

		public Color StartColor;
		public Color EndColor;
		public string Caption;
		public bool IsTransparent;

		#endregion

		public ColorInfo(Color  start,Color end, string pCaption, bool transparent)
		{
			StartColor = start;
			EndColor = end;
			Caption = pCaption;
			IsTransparent = transparent;
		}
		public ColorInfo(Color  start,Color end, string pCaption)
		{
			StartColor = start;
			EndColor = end;
			Caption = pCaption;
			IsTransparent = false;
			
		}
	}

	internal class DragInfo
	{
		#region "Member Variables"
		public bool Dragging;
		public bool MouseDown;
		public bool LegendLocked;
		public int DragLayerIndex;
		public int DragGroupIndex;
		public int TargetGroupIndex;
		public int TargetLayerIndex;
		public int StartY;
		//public int StopY;
		#endregion

		public DragInfo()
		{
			Reset();
		}
		public void Reset()
		{
			Dragging = false;
			MouseDown = false;
			StartY = 0;
			LegendLocked = false;
			DragLayerIndex = -1;
			DragGroupIndex = -1;
			TargetGroupIndex = -1;
			TargetLayerIndex = -1;
		}

		public bool DraggingLayer
		{
			get
			{
				if(DragLayerIndex != -1)
					return true;
				else
					return false;
			}
		}
		public void StartGroupDrag(int MouseY, int GroupIndex)
		{
			MouseDown = true;
			DragGroupIndex = GroupIndex;
			DragLayerIndex = Constants.INVALID_INDEX;
			StartY = MouseY;
		}

		public void StartLayerDrag(int MouseY, int GroupIndex, int LayerIndex)
		{
			MouseDown = true;
			DragGroupIndex = GroupIndex;
			DragLayerIndex = LayerIndex;
			StartY = MouseY;

		}

//		private void StartDrag(int MouseY,int groupIndex, int LayerIndex)
//		{
//			MouseDown = true;
//			DragGroupIndex = groupIndex;
//			DragLayerIndex = LayerIndex;
//			StartY = MouseY;
//		}
	}
}
