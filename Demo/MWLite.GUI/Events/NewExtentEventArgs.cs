using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MapWinGIS;
using MWLite.Core;

namespace MWLite.GUI.Events
{
    public class NewExtentsEventArgs : EventArgs
    {
        public Extents GeographicExtents;
        public Extents ProjExtents;
        public tkKnownExtents KnownExtents;
        public ExtentType ExtentsType;

        public bool Validate()
        {
            if (ExtentsType == ExtentType.Geogrpahic && GeographicExtents == null) return false;
            if (ExtentsType == ExtentType.Projected && ProjExtents == null) return false;
            return true;
        }
    }
}
