// --------------------------------------------------------------------------------------------------------------------
// <copyright file="Fileformats.cs" company="MapWindow Open Source GIS">
//   MapWindow developers community - 2014
// </copyright>
// <summary>
//   Static class to hold the file formats methods
// </summary>
// --------------------------------------------------------------------------------------------------------------------

namespace TestApplication
{
    #region

    using System;
    using System.Diagnostics;
    using System.IO;
    using System.Runtime.InteropServices;
    using System.Windows.Forms;

    using AxMapWinGIS;

    using MapWinGIS;

    #endregion

    /// <summary>Static class to hold the file formats methods</summary>
    internal static class Fileformats
    {
        #region Properties

        /// <summary>
        ///     Gets or sets Map.
        /// </summary>
        internal static AxMap Map { get; set; }

        #endregion

        #region Methods

        /// <summary>
        /// The add layer.
        /// </summary>
        /// <param name="filename">
        /// The filename.
        /// </param>
        /// <param name="theForm">
        /// The the form.
        /// </param>
        /// <returns>
        /// The <see cref="int"/>.
        /// </returns>
        internal static int AddLayer(string filename, Form1 theForm)
        {
            return Map.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, true);
        }

        /// <summary>
        /// Open the grid and load it as an layer.
        /// </summary>
        /// <param name="filename">
        /// The filename.
        /// </param>
        /// <param name="theForm">
        /// The form with the callback implementation.
        /// </param>
        /// <param name="clearLayers">
        /// Clear the layers.
        /// </param>
        /// <returns>
        /// The layer handle
        /// </returns>
        internal static int OpenGridAsLayer(string filename, Form1 theForm, bool clearLayers)
        {
            if (!File.Exists(filename))
            {
                theForm.Error(string.Empty, "Cannot find the file: " + filename);
                return -1;
            }

            var hndl = -1;

            try
            {
                var settings = new GlobalSettings();
                var grd = new Grid { GlobalCallback = theForm };

                settings.ResetGdalError();
                theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
                if (!grd.Open(filename, GridDataType.UnknownDataType, false, GridFileType.UseExtension, theForm))
                {
                    var msg = string.Format(
                        "Error opening grid: {0}{1}", 
                        grd.ErrorMsg[grd.LastErrorCode], 
                        Environment.NewLine);
                    if (settings.GdalLastErrorMsg != string.Empty)
                    {
                        msg += "GDAL Error: " + settings.GdalLastErrorMsg;
                    }

                    Debug.WriteLine(msg);
                    theForm.Error(string.Empty, msg);
                }
                else
                {
                    if (clearLayers)
                    {
                        Map.RemoveAllLayers();
                        Application.DoEvents();

                        // Set projection of map using the grid projection:
                        if (!grd.Header.GeoProjection.IsEmpty)
                        {
                            Map.GeoProjection.CopyFrom(grd.Header.GeoProjection);
                        }
                    }

                    // Log projection:
                    theForm.Progress(string.Empty, 100, "Projection: " + grd.Header.GeoProjection.ExportToProj4());
                    theForm.Progress(string.Empty, 100, "Number of bands: " + grd.NumBands);
                    theForm.Progress(string.Empty, 100, "NoData value: " + grd.Header.NodataValue);

                    hndl = Map.AddLayer(grd, true);

                    // The Grid objects are added to the Map control as Image layers. Once the Grid has been added, 
                    // the user is free to Close the Grid because that object is not actually referenced by the map:
                    grd.Close();

                    theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
                }
            }
            catch (SEHException sehException)
            {
                theForm.Error(string.Empty, "SEHException in OpenGridAsLayer: " + sehException.Message);
            }

            return hndl;
        }

        /// <summary>
        /// Open the image and load it as an layer.
        /// </summary>
        /// <param name="filename">
        /// The filename.
        /// </param>
        /// <param name="theForm">
        /// The form with the callback implementation.
        /// </param>
        /// <param name="clearLayers">
        /// Clear the layers.
        /// </param>
        /// <returns>
        /// The layer handle
        /// </returns>
        internal static int OpenImageAsLayer(string filename, Form1 theForm, bool clearLayers)
        {
            if (!File.Exists(filename))
            {
                theForm.Error(string.Empty, "Cannot find the file: " + filename);
                return -1;
            }

            var hndl = -1;

            try
            {
                // Track GDAL Errors:
                var settings = new GlobalSettings();
                var img = new Image { GlobalCallback = theForm };

                settings.ResetGdalError();
                theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(filename));
                if (!img.Open(filename, ImageType.USE_FILE_EXTENSION, false, null))
                {
                    var msg = string.Format(
                        "Error opening image: {0}{1}", 
                        img.ErrorMsg[img.LastErrorCode], 
                        Environment.NewLine);
                    if (settings.GdalLastErrorMsg != string.Empty)
                    {
                        msg += "GDAL Error: " + settings.GdalLastErrorMsg;
                    }

                    Debug.WriteLine(msg);
                    theForm.Error(string.Empty, msg);
                }
                else
                {
                    // Without overviews this decreases the performance too much:
                    if (img.NumOverviews > 1)
                    {
                        theForm.Progress(string.Empty, 100, "Opening the image with high quality interpolation mode.");
                        img.UpsamplingMode = tkInterpolationMode.imHighQualityBilinear;
                        img.DownsamplingMode = tkInterpolationMode.imBilinear;
                    }

                    if (clearLayers)
                    {
                        Map.RemoveAllLayers();
                        Application.DoEvents();
                    }

                    // Log characteristics:
                    theForm.Progress(string.Empty, 100, "projection: " + img.GetProjection());
                    theForm.Progress(string.Empty, 100, "Number bands: " + img.NoBands);
                    theForm.Progress(string.Empty, 100, "Number overviews: " + img.NumOverviews);
                    theForm.Progress(string.Empty, 100, "Use transparency?: " + img.UseTransparencyColor);
                    theForm.Progress(string.Empty, 100, "Transparency Color: " + img.TransparencyColor);

                    hndl = Map.AddLayer(img, true);

                    theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
                }
            }
            catch (SEHException sehException)
            {
                theForm.Error(string.Empty, "SEHException in OpenImageAsLayer: " + sehException.Message);
            }

            return hndl;
        }

        /// <summary>
        /// Open the filename as a shapefile
        /// </summary>
        /// <param name="shapefilename">
        /// The gridFilename.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The shapefile object
        /// </returns>
        internal static Shapefile OpenShapefile(string shapefilename, Form1 theForm)
        {
            if (!File.Exists(shapefilename))
            {
                theForm.Error(string.Empty, "Cannot find the file: " + shapefilename);
                return null;
            }

            var sf = new Shapefile { GlobalCallback = theForm };
            theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(shapefilename));
            if (!sf.Open(shapefilename, theForm))
            {
                var msg = string.Format("Error opening shapefile: {0}", sf.ErrorMsg[sf.LastErrorCode]);
                Debug.WriteLine(msg);
                theForm.Error(string.Empty, msg);
                return null;
            }

            // Log some characteristics:
            theForm.Progress(string.Empty, 100, "Number of shapes: " + sf.NumShapes);
            theForm.Progress(string.Empty, 100, "Number of fields: " + sf.NumFields);
            theForm.Progress(string.Empty, 100, "Type: " + sf.ShapefileType);
            theForm.Progress(string.Empty, 100, "Projection: " + sf.GeoProjection.ExportToProj4());
            theForm.Progress(string.Empty, 100, "Has spatial index: " + sf.HasSpatialIndex);

            return sf;
        }

        /// <summary>
        /// Open the grid.
        /// </summary>
        /// <param name="gridFilename">
        /// The grid filename.
        /// </param>
        /// <param name="theForm">
        /// The form.
        /// </param>
        /// <returns>
        /// The <see cref="Grid"/>.
        /// </returns>
        internal static Grid OpenGrid(string gridFilename, Form1 theForm)
        {
            if (!File.Exists(gridFilename))
            {
                theForm.Error(string.Empty, "Cannot find the file: " + gridFilename);
                return null;
            }

            var grid = new Grid { GlobalCallback = theForm };
            theForm.Progress(string.Empty, 0, "Start opening " + Path.GetFileName(gridFilename));
            if (!grid.Open(gridFilename, GridDataType.UnknownDataType, false))
            {
                var msg = string.Format("Error opening grid: {0}", grid.ErrorMsg[grid.LastErrorCode]);
                Debug.WriteLine(msg);
                theForm.Error(string.Empty, msg);
                return null;
            }

            // Log some characteristics:
            theForm.Progress(string.Empty, 100, "Number of bands: " + grid.NumBands);

            return grid;
        }

        /// <summary>
        /// Open the shapefile and load it as an layer.
        /// </summary>
        /// <param name="filename">
        /// The filename.
        /// </param>
        /// <param name="theForm">
        /// The form with the callback implementation.
        /// </param>
        /// <param name="clearLayers">
        /// Clear the layers.
        /// </param>
        /// <returns>
        /// The layer handle
        /// </returns>
        internal static int OpenShapefileAsLayer(string filename, Form1 theForm, bool clearLayers)
        {
            var hndl = -1;
            var sf = OpenShapefile(filename, theForm);

            if (sf != null)
            {
                if (clearLayers)
                {
                    Map.RemoveAllLayers();
                    Application.DoEvents();

                    // Set projection of map using the grid projection:
                    if (!sf.GeoProjection.IsEmpty)
                    {
                        // Set the projection of the shapefile to the map:
                        Map.GeoProjection = sf.GeoProjection.Clone();
                    }
                }

                hndl = Map.AddLayer(sf, true);

                theForm.Progress(string.Empty, 100, "The shapefile is of type " + sf.ShapefileType);
                var msg = string.Format(
                    "The shapefile its projection: {0} ({1})",
                    sf.GeoProjection.Name,
                    sf.GeoProjection.GeogCSName);
                theForm.Progress(msg);
                theForm.Progress(string.Empty, 100, "Done opening " + Path.GetFileName(filename));
            }
            else
            {
                theForm.Error(string.Empty, "Cannot load shapefile");
            }

            return hndl;
        }

        #endregion
    }
}