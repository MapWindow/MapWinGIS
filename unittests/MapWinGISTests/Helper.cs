using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using MapWinGIS;

namespace MapWinGISTests
{
    public static class Helper
    {
        public static void PrintExtents(Extents extents)
        {
            Debug.Write(extents.xMin + "; ");
            Debug.Write(extents.xMax + "; ");
            Debug.Write(extents.yMin + "; ");
            Debug.WriteLine(extents.yMax);
        }

        public static void DeleteShapefile(string filename)
        {
            var folder = Path.GetDirectoryName(filename);
            if (folder == null) return;
            var filenameBody = Path.GetFileNameWithoutExtension(filename);
            foreach (var f in Directory.EnumerateFiles(folder, filenameBody + ".*"))
            {
                Debug.WriteLine("deleting " + f);
                File.Delete(f);
            }
        }
    }
}
