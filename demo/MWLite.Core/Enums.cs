using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MWLite.Core
{
    public enum ExtentType
    {
        Geogrpahic = 0,
        Projected = 1,
        Known = 2,
    }

    public enum ProjectState
    {
        NotSaved = 0,
        HasChanges = 1,
        NoChanges = 2,
        Empty = 3,
    }

    public enum LayerType
    {
        Vector = 0,
        Raster = 1,
        All = 2,
    }

    public enum FileType
    {
        Project = 0,
    }
}
