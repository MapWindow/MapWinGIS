using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MWLite.ShapeEditor
{
    public enum EditorCommand
    {
        None = 0,
        AddShape = 1,
        EditShape = 2,
        AddPart = 3,
        RemovePart = 4,
        EditLayer = 5,
        CreateLayer = 6,
        Undo = 7,
        Redo = 8,
        SplitShapes = 9,
        MergeShapes = 10,
        RotateShapes = 11,
        MoveShapes = 12,
        RemoveShapes = 13,
        CopyShapes = 14,
        PasteShapes = 15,
        CutShapes = 16,
        SplitByPolyline = 17,
        EraseByPolygon = 18,
        ClipByPolygon = 19,
        SplitByPolygon = 20,
    }
}
