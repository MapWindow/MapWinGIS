using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core;
using MWLite.Core.UI;
using MWLite.ShapeEditor.Helpers;
using MWLite.ShapeEditor.Operations;

namespace MWLite.ShapeEditor
{
    internal class EditorDispatcher: CommandMap<EditorCommand>
    {
        private void SetMapCursor(tkCursorMode mode)
        {
            App.Instance.Map.CursorMode = mode;
            App.Instance.RefreshUI();
        }
        
        public override void Run(EditorCommand command)
        {
            if (HandleGroupOperation(command)) return;

            if (HandleChangeTool(command)) return;

            var map = App.Instance.Map;
            switch (command)
            {
                case EditorCommand.Undo:
                    map.UndoList.Undo();
                    map.Redraw2(tkRedrawType.RedrawSkipDataLayers);
                    break;
                case EditorCommand.Redo:
                    map.UndoList.Redo();
                    map.Redraw2(tkRedrawType.RedrawSkipDataLayers);
                    break;
                case EditorCommand.EditLayer:
                    EditorHelper.ToggleEditLayer();
                    break;
                case EditorCommand.CreateLayer:
                    EditorHelper.CreateLayer();
                    break;
               
            }
            App.Instance.RefreshUI();
        }

        public bool HandleChangeTool(EditorCommand command)
        {
            switch (command)
            {
                case EditorCommand.MoveShapes:
                    SetMapCursor(tkCursorMode.cmMoveShapes);
                    return true;
                case EditorCommand.AddPart:
                    SetMapCursor(tkCursorMode.cmAddPart);
                    return true;
                case EditorCommand.RemovePart:
                    SetMapCursor(tkCursorMode.cmRemovePart);
                    return true;
                case EditorCommand.AddShape:
                    SetMapCursor(tkCursorMode.cmAddShape);
                    return true;
                case EditorCommand.EditShape:
                    SetMapCursor(tkCursorMode.cmEditShape);
                    return true;
            }
            return false;
        }

        public bool HandleGroupOperation(EditorCommand command)
        {
            switch (command)
            {
                case EditorCommand.CopyShapes:
                    OperationHelper.CopyShapes();
                    return true;
                case EditorCommand.PasteShapes:
                    OperationHelper.PasteShapes();
                    return true;
                case EditorCommand.CutShapes:
                    OperationHelper.CutShapes();
                    return true;
                case EditorCommand.SplitShapes:
                    OperationHelper.ExplodeShapes();
                    return true;
                case EditorCommand.MergeShapes:
                    OperationHelper.MergeShapes();
                    return true;
                case EditorCommand.RemoveShapes:
                    OperationHelper.RemoveShapes();
                    return true;
            }
            return false;
        }

        protected override void CommandNotFound(ToolStripItem item)
        {
            MessageHelper.Info("No handle is found: " + item.Name);
        }
    }
}
