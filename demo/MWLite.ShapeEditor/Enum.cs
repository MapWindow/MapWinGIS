using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MWLite.ShapeEditor
{
    public enum MergeResult
    {
        Ok = 0,
        TooManyShapes = 1,
        Failed = 2,
        NoInput = 3,
    }

    public enum ExplodeResult
    {
        Ok = 0,
        Failed = 1,
        NoMultiPart = 2,
        NoInput = 3,
    }

    public enum PasteResult
    {
        Ok = 0,
        NoInput = 1,
        ShapeTypeMismatch = 2
    }
}
