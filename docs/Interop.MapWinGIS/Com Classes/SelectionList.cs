using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Holds list of shapes and pixels which where identified by user with Identify tool.
    /// </summary>
    /// \new494 Added in version 4.9.4
#if nsp
    #if upd
        public class SelectionList : MapWinGIS.ISelectionList
    #else        
        public class ISelectionList        
    #endif
#else
    public class SelectionList
#endif
    {
        /// <summary>
        /// Adds the shape from the specified layer to the list.
        /// </summary>
        /// <param name="LayerHandle">The layer handle.</param>
        /// <param name="ShapeIndex">Index of the shape.</param>
        public void AddShape(int LayerHandle, int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all the shapes and pixels from the list.
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Removes all items from the list which belong to specified layer.
        /// </summary>
        /// <param name="LayerHandle">The layer handle.</param>
        public void RemoveByLayerHandle(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Adds a pixel to the list.
        /// Updated in v4.9.5
        /// </summary>
        /// <param name="LayerHandle">The layer handle.</param>
        /// <param name="Column">The column.</param>
        /// <param name="Row">The row.</param>
        public void AddPixel(int LayerHandle, int Column, int Row)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Toggles the state of the specified pixel (add it to the list if it is not present there, and removes otherwise).
        /// Updated in v4.9.5
        /// </summary>
        /// <param name="LayerHandle">The layer handle.</param>
        /// <param name="Column">The column.</param>
        /// <param name="Row">The row.</param>
        public void TogglePixel(int LayerHandle, int Column, int Row)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the count.
        /// </summary>
        public int Count { get; private set; }

        /// <summary>
        /// Gets the layer handle for the item with specified pixel.
        /// </summary>
        /// <param name="Index">The index of the item (either shape or pixels).</param>
        /// <returns>The layer handle.</returns>
        public int get_LayerHandle(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the index of the shape for the specified item.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>The index of the shape or -1, if this item is a pixel.</returns>
        /// <exception cref="System.NotImplementedException"></exception>
        public int get_ShapeIndex(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the type of the layer for the specified pixel.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>The type of the layer.</returns>
        public tkLayerType get_LayerType(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets X coordinate pixel.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>X coordinate or -1 if the specified item is a shape.</returns>
        [Obsolete("get_RasterX is deprecated, please use get_Column instead.")]
        public int get_RasterX(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets Y coordinate pixel.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>Y coordinate or -1 if the specified item is a shape.</returns>
        [Obsolete("get_RasterY is deprecated, please use get_Row instead.")]
        public int get_RasterY(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the row.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>Row index or -1 if the specified item is a shape.</returns>
        /// \new495 Added in version 4.9.5
        public int get_Row(int Index)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Gets the column.
        /// </summary>
        /// <param name="Index">The index of the item.</param>
        /// <returns>Column index or -1 if the specified item is a shape.</returns>
        /// \new495 Added in version 4.9.5
        public int get_Column(int Index)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif



