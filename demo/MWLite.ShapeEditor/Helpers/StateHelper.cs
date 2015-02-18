using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MWLite.ShapeEditor.Helpers;

namespace MWLite.ShapeEditor
{
    internal static class StateHelper
    {
        internal static bool GetEnabled(ToolStripItem item)
        {
            var cmd = EditorCommand.None;
            if (App.Dispatcher.CommandFromName(item, ref cmd))
            {
                return GetEnabled(cmd);
            }
            return false;
        }
        
        internal static bool GetEnabled(EditorCommand command)
        {
            var sf = App.SelectedShapefile;
            if (sf == null) return false;
            
            switch (command)
            {
                case EditorCommand.Undo:
                    return App.Map.UndoList.UndoCount > 0;
                case EditorCommand.Redo:
                    return  App.Map.UndoList.RedoCount > 0;
                case EditorCommand.Copy:
                    return sf.NumSelected > 0;
                case EditorCommand.Cut:
                    return sf.NumSelected > 0 && sf.InteractiveEditing;
                case EditorCommand.Paste:
                    return !OperationHelper.BufferIsEmpty && sf.InteractiveEditing;
            }
            return false;
        }
    }
}
