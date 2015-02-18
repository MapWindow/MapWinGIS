using System;
using MapWinGIS;

namespace MWLite.Core.Events
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
