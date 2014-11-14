// --------------------------------------------------------------------------------------------------------------------
// <copyright file="PostGisTests.cs" company="MapWindow Open Source GIS">
//   MapWindow developers community - 2014
// </copyright>
// <summary>
//   The PostGIS tests.
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace TestApplication
{
    #region

    using System;
    using System.Collections.Generic;
    using System.IO;
    using System.Windows.Forms;

    using AxMapWinGIS;

    using MapWinGIS;

    #endregion

    /// <summary>
    /// The post gis tests.
    /// </summary>
    public static class PostGisTests
    {
        #region Properties

        /// <summary>
        ///     Gets or sets Map.
        /// </summary>
        internal static AxMap Map { get; set; }

        #endregion

        #region Methods

        /// <summary>
        /// Create PostGIS database.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// Tru on success.
        /// </returns>
        internal static bool RunPostGisCreateDatabase(string textfileLocation, Form1 theForm)
        {
            theForm.Progress(
                "-----------------------The creation of a PostGIS database has started." + Environment.NewLine);

            var numErrors = RunNonExecuteQuery(textfileLocation, theForm);

            theForm.Progress(string.Format("The postGIS create database test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Drop PostGIS database.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// Tru on success.
        /// </returns>
        internal static bool RunPostGisDropDatabase(string textfileLocation, Form1 theForm)
        {
            theForm.Progress(
                "-----------------------The drop of a PostGIS database has started." + Environment.NewLine);

            var numErrors = RunNonExecuteQuery(textfileLocation, theForm);

            theForm.Progress(string.Format("The postGIS drop database test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Run the open postGIS layers test.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// True on success
        /// </returns>
        internal static bool RunOpenPostGisLayers(string textfileLocation, Form1 theForm)
        {
            var numErrors = 0;
            Map = Fileformats.Map;
            Map.RemoveAllLayers();
            Map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;
            
            // TODO: How to switch between these two:
            //Map.Projection = tkMapProjection.PROJECTION_WGS84;
            Map.GrabProjectionFromData = true;

            var gs = new GlobalSettings();
            theForm.Progress("OgrLayerMaxFeatureCount: " + gs.OgrLayerMaxFeatureCount);

            Application.DoEvents();

            theForm.Progress("----------------------- Opening PostGIS layers has started." + Environment.NewLine);

            // Read testfile:
            string connectionString;
            List<string> layersList;
            if (!ReadTextfile(textfileLocation, out connectionString, out layersList))
            {
                throw new Exception("Cannot read text file");
            }

            // Connect to data source:
            var ds = new OgrDatasource { GlobalCallback = theForm };
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get queries:
            foreach (var layerName in layersList)
            {
                var layer = ds.GetLayerByName(layerName, true);

                if (layer == null)
                {
                    continue;
                }

                layer.MaxFeatureCount = 10000;
                layer.GlobalCallback = theForm;

                theForm.Progress("Opening " + layerName);
                var handle = Map.AddLayer(layer, true);
                if (handle == -1)
                {
                    theForm.WriteError("Failed to add database layer " + layerName + " to the map.");
                    numErrors++;
                }

                Application.DoEvents();
            }

            // Close database connection:
            ds.Close();

            theForm.Progress(string.Format("Opening PostGIS layers test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Import shapefiles into the PostGIS database
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunPostGisImportSf(string textfileLocation, Form1 theForm)
        {
            var numErrors = 0;
            Map = Fileformats.Map;
            Map.RemoveAllLayers();
            Map.Projection = tkMapProjection.PROJECTION_WGS84;
            Map.ProjectionMismatchBehavior = tkMismatchBehavior.mbCheckLooseAndReproject;

            // Add the symbology of the shapefile to the database during import:
            var settings = new GlobalSettings { OgrUseStyles = true };

            Application.DoEvents();

            theForm.Progress("----------------------- Importing of shapefiles has started." + Environment.NewLine);

            // Read testfile:
            string connectionString;
            List<string> shapefileList;
            if (!ReadTextfile(textfileLocation, out connectionString, out shapefileList))
            {
                throw new Exception("Cannot read text file");
            }

            // Connect to data source:
            var ds = new OgrDatasource { GlobalCallback = theForm };
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get queries:
            foreach (var shapefileLocation in shapefileList)
            {
                var layerName = Path.GetFileNameWithoutExtension(shapefileLocation);
                if (!File.Exists(shapefileLocation))
                {
                    theForm.WriteError(shapefileLocation + " does not exists. Skipping");
                    continue;
                }

                // Open shapefile:
                theForm.Progress(string.Format("Reading {0} shapefile", layerName));
                var fm = new FileManager();
                var sf = fm.OpenShapefile(shapefileLocation, theForm);
                theForm.Progress(string.Format("Importing {0} shapefile", layerName));
                if (!ds.ImportShapefile(sf, layerName, "OVERWRITE=YES", tkShapeValidationMode.NoValidation))
                {
                    var errorMsg = fm.ErrorMsg[fm.LastErrorCode];

                    // let's check GDAL error as well
                    var gs = new GlobalSettings();
                    errorMsg += " GDAL error message: " + gs.GdalLastErrorMsg;

                    theForm.WriteError(
                        string.Format("Error importing shapefile [{0}]: {1}", shapefileLocation, errorMsg));
                    numErrors++;
                }
                else
                {
                    // Read layer and add to map:
                    theForm.Progress(string.Format("Reading {0} layer from db", layerName));
                    var handle = Map.AddLayerFromDatabase(connectionString, layerName, true);
                    if (handle == -1)
                    {
                        theForm.WriteError("Failed to open database layer: " + layerName);
                        numErrors++;
                    }

                    Application.DoEvents();
                }

                // Close shapefile:
                sf.Close();
            }

            // Close database connection:
            ds.Close();

            theForm.Progress(string.Format("Importing of shapefiles test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Test setting PostGIS privileges.
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="bool"/>.
        /// </returns>
        internal static bool RunPostGisPostGisPrivileges(string textfileLocation, Form1 theForm)
        {
            var numErrors = 0;

            theForm.Progress(
                "-----------------------The setting of the grants and privileges has started." + Environment.NewLine);

            // Read testfile:
            string connectionString;
            List<string> queryList;
            if (!ReadTextfile(textfileLocation, out connectionString, out queryList))
            {
                throw new Exception("Cannot read text file");
            }

            // Connect to data source:
            var ds = new OgrDatasource { GlobalCallback = theForm };
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get every first and second line:
            foreach (var query in queryList)
            {
                string errorMsg;
                if (!ds.ExecuteSQL(query, out errorMsg))
                {
                    theForm.WriteError(string.Format("Error executing query [{0}]: {1}", query, errorMsg));
                    numErrors++;
                }
            }

            // Close database connection:
            ds.Close();

            theForm.Progress(
                string.Format("The setting of the grants and privileges test has finished, with {0} errors", numErrors));

            return numErrors == 0;
        }

        /// <summary>
        /// Read the textfile
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="connectionString">
        /// The connection string
        /// </param>
        /// <param name="queryList">
        /// The query list.
        /// </param>
        /// <returns>
        /// True on success
        /// </returns>
        private static bool ReadTextfile(string textfileLocation, out string connectionString, out List<string> queryList)
        {
            // Open text file:
            if (!File.Exists(textfileLocation))
            {
                throw new FileNotFoundException("Cannot find text file.", textfileLocation);
            }

            // Read text file:
            var lines = Helper.ReadTextfile(textfileLocation);

            // First line is connection string:
            connectionString = lines[0];
            if (!connectionString.StartsWith("PG"))
            {
                throw new Exception(
                    "Input file is not correct. The first line should be the connection string, starting with PG");
            }

            queryList = new List<string>();

            // Get every first and second line:
            for (var i = 1; i < lines.Count; i++)
            {
                queryList.Add(lines[i]);
            }

            return true;
        }

        /// <summary>
        /// Run non execute query, like create database and drop database
        /// </summary>
        /// <param name="textfileLocation">
        /// The textfile location.
        /// </param>
        /// <param name="theForm">
        /// The  form.
        /// </param>
        /// <returns>
        /// The number of errors
        /// </returns>
        private static int RunNonExecuteQuery(string textfileLocation, Form1 theForm)
        {
            var numErrors = 0;

            // Read testfile:
            string connectionString;
            List<string> queryList;
            if (!ReadTextfile(textfileLocation, out connectionString, out queryList))
            {
                throw new Exception("Cannot read text file");
            }

            // Connect to data source:
            var ds = new OgrDatasource { GlobalCallback = theForm };
            if (!ds.Open(connectionString))
            {
                throw new Exception("Failed to open datasource: " + ds.GdalLastErrorMsg);
            }

            // Get queries:
            foreach (var query in queryList)
            {
                string errorMsg;
                if (ds.ExecuteSQL(query, out errorMsg))
                {
                    theForm.Progress("Executed query: " + query);
                    continue;
                }

                if (ds.GdalLastErrorMsg.Contains("cannot run inside a transaction block"))
                {
                    errorMsg += " You're probably using a too old version of GDAL v2, please update.";
                }

                theForm.WriteError(string.Format("Error executing query [{0}]: {1}", query, errorMsg));
                numErrors++;
            }

            // Close database connection:
            ds.Close();

            return numErrors;
        }

        #endregion
    }
}