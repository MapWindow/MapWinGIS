using MapWinGIS;
using MWLite.Core.Events;
using MWLite.Core.UI;
using MWLite.Databases.Forms;

namespace MWLite.Databases
{
    public static class OgrHelper
    {
        public static bool OpenDatasource(OgrDatasource ds, ConnectionParams connection)
        {
            if (ds == null) return false;
            if (!ds.Open(connection.GetPostGisConnection()))
            {
                MessageHelper.Warn("Failed to open datasource: " + ds.GdalLastErrorMsg);
                return false;
            }
            return true;
        }
    }
}
