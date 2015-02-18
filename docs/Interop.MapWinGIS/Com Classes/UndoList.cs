using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

#if nsp
namespace MapWinGIS
{
#endif
    /// <summary>
    /// Holds list of actions performed by user with interactive editing tools to enable undo functionality.
    /// </summary>
    /// \new493 Added in version 4.9.3
    #if nsp
        #if upd
            public class UndoList : MapWinGIS.IUndoList
        #else        
            public class IUndoList
        #endif
    #else
        public class UndoList
    #endif
    {
        /// <summary>
        /// Registers editing operation in undo list.
        /// </summary>
        /// <param name="operationType">Type of undo operation (uoMoveShapes and uoRotateShapes aren't accepted here).</param>
        /// <param name="LayerHandle">Layer handle the subject shape belongs to.</param>
        /// <param name="ShapeIndex">Index if subject shape.</param>
        /// <returns>True on success.</returns>
        public bool Add(tkUndoOperation operationType, int LayerHandle, int ShapeIndex)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Starts batch which can hold one or more operation. UndoList.Undo and UndoList.Redo
        /// process all operations within batch as a single entity.
        /// </summary>
        /// <returns>True on success, and false if there is a batch was already started.</returns>
        public bool BeginBatch()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Remove all operations from undo list.
        /// </summary>
        public void Clear()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Clears all operations for particular layer from undo list.
        /// </summary>
        /// <param name="LayerHandle">Layer handle.</param>
        /// <remarks>Should be called when certain layer is remove from map or editing session for it is finished.</remarks>
        public void ClearForLayer(int LayerHandle)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Ends batch started by UndoList.BeginBatch command.
        /// </summary>
        /// <returns>Returns number of operation in the batch.</returns>
        public int EndBatch()
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// A text string associated with object. Any value can be stored by developer in this property.
        /// </summary>
        public string Key {get; set;}

        /// <summary>
        /// Gets the code of last error which took place inside this object.
        /// </summary>
        public int LastErrorCode
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Number operations performed by users that were undone but can be applied once again.
        /// </summary>
        public int RedoCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Returns total number of entries in undo list.
        /// </summary>
        /// <remarks>All opeations within batch are considered a single entries.</remarks>
        public int TotalLength
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Number of operations performed by user that can be reverted with UndoList.Undo method.
        /// </summary>
        public int UndoCount
        {
            get { throw new NotImplementedException(); }
        }

        /// <summary>
        /// Gets the description of the specific error code.
        /// </summary>
        /// <param name="ErrorCode">The error code returned by LastErrorCode property.</param>
        /// <returns>String with the description.</returns>
        public string get_ErrorMsg(int ErrorCode)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reapplies a single operation in the list that was previously undone.
        /// </summary>
        /// <param name="ZoomToShape">If true, zooms to the subject shape in case it's outside the current map view.</param>
        /// <returns>True on success, i.e. there was at least one operation to redo.</returns>
        public bool Redo(bool ZoomToShape = true)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// Reverts a single operation performed by user.
        /// </summary>
        /// <param name="ZoomToShape">If true, zooms to the subject shape in case it's outside the current map view.</param>
        /// <returns>True on success, i.e. there was at least one operation to undo</returns>
        public bool Undo(bool ZoomToShape = true)
        {
            throw new NotImplementedException();
        }
    }
#if nsp
}
#endif
