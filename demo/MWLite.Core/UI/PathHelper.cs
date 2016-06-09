using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;

namespace MWLite.GUI.Helpers
{
    public static class PathHelper
    {
        public static string GetSettingsPath()
        {
            string folder = Environment.GetFolderPath(Environment.SpecialFolder.ApplicationData);

            string path = Path.Combine(folder, "MWLite");

            if (!Directory.Exists(path))
                Directory.CreateDirectory(path);

            return path + "\\settings.xml";
        }
    }
}
