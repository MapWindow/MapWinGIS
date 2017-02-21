using System;
using System.Collections.Generic;
using System.Diagnostics;
using System.IO;
using System.Linq;
using System.Reflection;
using System.Text;
using MapWinGISTests;

namespace ConsoleApplication1
{
    /// <summary>
    /// This program is used as Debug target for MapWinGIS project. 
    /// </summary>
    public class Program
    {
        public static void Main(string[] args)
        {
            var path = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + @"\TestData\";
            Directory.SetCurrentDirectory(path);
            Debug.WriteLine("Current path: " + Directory.GetCurrentDirectory());

            // var t = new GdalTests();
            // t.CreateLayerSQLiteTest();
            // t.GdalInfoHdf5();

            var t = new OgrDatasourceTests();
            t.ImportShapefileTest();

            Console.ReadLine();
        }
    }
}
