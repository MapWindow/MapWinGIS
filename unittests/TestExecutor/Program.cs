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
    class Program
    {
        static void Main(string[] args)
        {
            string path = Path.GetDirectoryName(Assembly.GetExecutingAssembly().Location) + @"\TestData\";
            Directory.SetCurrentDirectory(path);
            Debug.WriteLine("Current path: " + Directory.GetCurrentDirectory());

            // var t = new GdalTests();
            // t.CreateLayerSQLiteTest();
            
            var t = new GdalTests();
            t.GdalInfoHdf5();

            Console.ReadLine();
        }
    }
}
