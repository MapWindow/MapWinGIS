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
using MWLite.Symbology.Classes;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.LegendControl
{
	/// <summary>
	/// A Layer management class for interacting with the legend in code
	/// </summary>
	public class Layers: IEnumerable<Layer>
	{
		private Legend m_Legend;

        #region Constructor
        public Layers(Legend leg)
		{
			//The next line MUST GO FIRST in the constructor
			m_Legend = leg;
			//The previous line MUST GO FIRST in the constructor
		}
        #endregion

        #region IEnumerable interface
        System.Collections.IEnumerator System.Collections.IEnumerable.GetEnumerator()
        {
            return this.GetEnumerator();
        }

        public IEnumerator<Layer> GetEnumerator()
        {
            for (int i = 0; i < Count; i++ )
            {
                var l = this[i];
                if (l == null)
                    break;
                yield return l;
            }
        }
        #endregion

        /// <summary>
		/// Iterates through the list of all layers by position
		/// </summary>
		/// <param name="Position">The 0-Based index into the list of all layers</param>
		public Layer this[int Position]
		{
			get
			{
				if(Position >=0 && Position < Count)	
				{
					int Handle = m_Legend.m_Map.get_LayerHandle(Position);
					return m_Legend.FindLayerByHandle(Handle);
				}

				globals.LastError = "Invalid Layer Position";
				return null;
			}
		}

        /// <summary>
		/// Gets the count of all layers
		/// </summary>
		public int Count
		{
			get
			{
				if (m_Legend == null) return 0;
				if (m_Legend.m_Map == null) return 0;
				return m_Legend.m_Map.NumLayers;
			}
		}

		/// <summary>
		/// Get a Layer by the handle to the layer (without knowing what group the layer is in)
		/// </summary>
		/// <param name="Handle">The handle to the layer</param>
		/// <returns>Layer class item, null (nothing) on success</returns>
		public Layer ItemByHandle(int Handle)
		{
			return m_Legend.FindLayerByHandle(Handle);
		}


		/// <summary>
		/// Move a layer to a specified location within a specified group
		/// </summary>
		/// <param name="LayerHandle">Handle to the layer to move</param>
		/// <param name="TargetGroupHandle">Handle of the group into which to move the layer</param>
		/// <param name="PositionInGroup">0-Based index into the list of layers within the Target Group</param>
		/// <returns>True on success, False otherwise</returns>
		public bool MoveLayer(int LayerHandle, int TargetGroupHandle, int PositionInGroup)
		{
			return m_Legend.MoveLayer(TargetGroupHandle,LayerHandle,PositionInGroup);
		}

		/// <summary>
		/// Clears all layers from the legend (from all groups)
		/// </summary>
		public void Clear()
		{
			m_Legend.ClearLayers();
		}

		/// <summary>
		/// Move a layer to a new position within a group
		/// </summary>
		/// <param name="LyrHandle">Handle to the layer to move</param>
		/// <param name="NewPosition">0-based index of the desired position</param>
		/// <returns>True on Success, False otherwise</returns>
		public bool MoveLayerWithinGroup(int LyrHandle, int NewPosition)
		{
			int GroupIndex,
				LayerIndex;
          
			if (m_Legend.FindLayerByHandle(LyrHandle, out GroupIndex, out LayerIndex) != null)
			{
				Group grp = (Group)m_Legend.m_AllGroups[GroupIndex];
				return m_Legend.MoveLayer(grp.Handle,LyrHandle,NewPosition);
			}

			globals.LastError = "Invalid Layer Handle";
			return false;			
		}

		/// <summary>
		/// Adds a layer to the topmost group
		/// </summary>
		/// <param name="newLayer">The object to add (must be a supported Layer type)</param>
		/// <param name="Visible">Whether or not the layer is visible upon adding it</param>
		/// <returns>Handle of the newly added layer, -1 on failure</returns>
		public int Add(object newLayer, bool Visible)
		{
			return m_Legend.AddLayer(newLayer,Visible);
		}

		/// <summary>
		/// Adds a layer to the map, optionally placing it above the currently selected layer (otherwise at top of layer list).
		/// </summary>
		/// <param name="newLayer">The object to add (must be a supported Layer type)</param>
		/// <param name="Visible">Whether or not the layer is visible upon adding it</param>
		/// <param name="PlaceAboveCurrentlySelected">Whether the layer should be placed above currently selected layer, or at top of layer list.</param>
		/// <returns>Handle of the newly added layer, -1 on failure</returns>
		public int Add(object newLayer, bool Visible, bool PlaceAboveCurrentlySelected)
		{
			return m_Legend.AddLayer(newLayer,Visible,PlaceAboveCurrentlySelected);
		}

		/// <summary>
		/// Adds a layer to the topmost group
		/// </summary>
		/// <param name="newLayer">The object to add (must be a supported Layer type)</param>
		/// <param name="Visible">Whether or not the layer is visible upon adding it</param>
		/// <param name="TargetGroupHandle">Handle of the group into which the layer should be added</param>
		/// <returns>Handle of the newly added layer, -1 on failure</returns>
		public int Add(object newLayer, bool Visible, int TargetGroupHandle)
		{
			return m_Legend.AddLayer(newLayer,Visible,TargetGroupHandle);
		}

        /// <summary>
        /// Adds a layer to a certain group in a certain position
        /// </summary>
        public int Add(object newLayer, bool Visible, int TargetGroupHandle, int afterLayerHandle)
        {
            return m_Legend.AddLayer(newLayer, Visible, TargetGroupHandle, true, afterLayerHandle);
        }

        /// <summary>
        /// Adds a layer to the topmost group
        /// </summary>
        /// <param name="newLayer">The object to add (must be a supported Layer type)</param>
        /// <param name="MapVisible">Whether or not the layer is visible in the map upon adding it.</param>
        /// <param name="LegendVisible">Whether or not the layer is visible in the legend.</param>
        /// <returns>Handle of the newly added layer, -1 on failure</returns>
        public int Add(bool LegendVisible, object newLayer, bool MapVisible)
        {
            return m_Legend.AddLayer(newLayer, MapVisible, -1, LegendVisible);
        }

		/// <summary>
		/// Removes a layer
		/// </summary>
		/// <param name="LayerHandle">Handle of the layer to be removed</param>
		/// <returns>True on success, False otherwise</returns>
		public bool Remove(int LayerHandle)
		{
			return m_Legend.RemoveLayer(LayerHandle);
		}

		/// <summary>
		/// Gets the position (index) of the specified layer within the group
		/// </summary>
		/// <param name="LayerHandle">Handle of the layer</param>
		/// <returns>0-Based Index into list of layers within group, -1 on failure</returns>
		public int PositionInGroup(int LayerHandle)
		{
			int LayerIndex ,
				GroupIndex ;
			Layer lyr = m_Legend.FindLayerByHandle(LayerHandle,out GroupIndex, out LayerIndex);

			if(lyr != null)
				return LayerIndex;
			
			globals.LastError = "Invalid Layer Handle";
			return -1;
		}

		/// <summary>
		/// Gets the handle of the group containing the specified layer
		/// </summary>
		/// <param name="LayerHandle">Handle of the layer</param>
		/// <returns>Group Handle of the group that contains the layer, -1 on failure</returns>
		public int GroupOf(int LayerHandle)
		{

			int LayerIndex,
				GroupIndex;
			Layer lyr = m_Legend.FindLayerByHandle(LayerHandle,out GroupIndex, out LayerIndex);

			if(lyr != null)
			{
                Group grp = (Group)m_Legend.m_AllGroups[GroupIndex];
				return grp.Handle;
			}
			
			globals.LastError = "Invalid Layer Handle";
			return -1;
		}

		/// <summary>
		/// Tells you if the group represented by the specified handle still exists in the list of groups
		/// </summary>
		/// <param name="Handle">Group Handle</param>
		/// <returns>True if the group exists, False otherwise</returns>
		public bool IsValidHandle(int Handle)
		{
			if(m_Legend.m_Map.get_LayerPosition(Handle) >=0)
				return true;
			else
				return false;
		}


		/// <summary>
		/// Collapse all layers
		/// </summary>
		public void CollapseAll()
		{
			m_Legend.Lock();
			int i, count;

			count = Count;
			for( i = 0; i < count; i++)
				this[i].Expanded = false;
			m_Legend.Unlock();
		}

		/// <summary>
		/// Expand all layers
		/// </summary>
		public void ExpandAll()
		{
			m_Legend.Lock();
			int i, count;

			count = Count;
			for( i = 0; i < count; i++)
				this[i].Expanded = true;
			m_Legend.Unlock();
		}

        
    }
}
