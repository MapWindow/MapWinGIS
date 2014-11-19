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
        Copy = 14,
        Paste = 15,
        Cut = 16,
        SplitByPolyline = 17,
        EraseByPolygon = 18,
        ClipByPolygon = 19,
        SplitByPolygon = 20,
        VertexEditor = 21,
        PartEditor = 22,
        ClearEditor = 23,
        SaveShape = 24,
        SelectByRectangle = 25,
        ClearSelection = 26,
    }
}
