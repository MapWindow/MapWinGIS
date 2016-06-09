
namespace Examples
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;

    /// <summary>
    /// The groups of the examples for GUI
    /// </summary>
    public enum ExampleGroup
    {
        None = -1,
        Editing = 0,
        Geoprocessing = 1,
        General = 2,
        Image = 3,
        Interaction = 4,
        Selection = 5,
        Symbology = 6,
        Version49 = 7,
    }

    /// <summary>
    /// Holds description of the example to be displayed in the documentation and GUI of this project.
    /// </summary>
    class Example
    {
        private string guiName;           // in the interface
        public string function;           // to call with reflection
        public string description;        // to generate documentation page and show in the GUI
        public bool image;                // whether an image should be attached to the page
        public ExampleGroup group = ExampleGroup.None;

        public Example()
        {
            image = true;
        }

        public string GuiName
        {
            get { return guiName; }
            set { guiName = value; }
        }
    }

    class Description
    {
        // list of the examples
        public List<Example> examples = new List<Example>();

        // event declaration
        delegate void ExampleAddedEventHandler(Example ex, int index);
        event ExampleAddedEventHandler ExampleAdded;
        void FireExampleAdded(Example ex, int index)
        {
            if (this.ExampleAdded != null)
                ExampleAdded(ex, index);
        }

        /// <summary>
        /// Initializes examples
        /// </summary>
        public Description()
        {
            this.ExampleAdded += Description_ExampleAdded;
            for (int i = 0; i < 30; i++)
            {
                Example ex = new Example();
                FireExampleAdded(ex, i);
                examples.Add(ex);
            }
        }

        /// <summary>
        /// Adds description of the examples
        /// </summary>
        void Description_ExampleAdded(Example ex, int index)
        {
            switch (index)
            {
                case 0:
                    ex.GuiName = "Multiple buffer";
                    ex.description = "create several buffers around the rivers. Four buffers are build sequentially, each of " +
                                     "them as the separate shapefile." +
                                     "The overlapping shapes of each buffer are merged together. Then smaller buffers are " +
                                     "subtracted from the larger ones, making \"holes\" in them. After it all 4 buffers are copied to " +
                                     "a single shapefile with buffer distance field. Finally a color scheme ranging from blue to yellow " +
                                     "is applied. An implementation of ICallback interface is used for reporting progress information.";
                    ex.function = "CreateBuffer";
                    ex.group = ExampleGroup.Geoprocessing;
                    break;
                case 1:
                    ex.GuiName = "Selection box";
                    ex.description = "manually select shapes and to show information about the their relative area. The shapes area selected " +
                                     "by mouse dragging. Chart is update automatically.";
                    ex.function = "SelectBox";
                    ex.group = ExampleGroup.Selection;
                    ex.image = true;
                    break;
                case 2:
                    ex.GuiName = "Edit attributes";
                    ex.description = "implement a simple GUI for editing attributes of the individual shapes. A shapefile with buildings will be shown." +
                                     "The labels which display the name and type of building are added. After clicking on a building a dialog will appear " +
                                     "with text boxes to display attributes of the object. The new values will be saved after the clicking on OK button. " +
                                     "The label on the map will be updated showing the new values of the attributes.";
                    ex.function = "EditAttributes";
                    ex.group = ExampleGroup.Editing;
                    break;
                case 3:
                    ex.GuiName = "Length of intersection";
                    ex.description = "calculate the total length of rivers for each land parcel. Two shapefiles with rivers and land parcels act " +
                                     "as an input. The code performs \"intersects\" test for each pair of river and parcel and computes " +
                                     "intersection if such takes place. The positions of intersections are added as new polyline shapefile. " +
                                     "The [Length] field is added to the parcels shapefile which will hold the total length of rivers. " +
                                     " Finally the values of the new field are visualized using bar charts.";
                    ex.function = "IntersectionLength";
                    
                    ex.group = ExampleGroup.Geoprocessing;
                    break;
                case 4:
                    ex.GuiName = "Mark points";
                    ex.description = "how to add markers for the points of interest. The marker represented by image loaded from the file will be " +
                                     "added on the mouse down event. The markers will be stored in the temporary in-memory shapefile.";
                    ex.function = "MarkPoints";
                    
                    ex.group = ExampleGroup.Interaction;
                    break;
                case 5:
                    ex.GuiName = "Tracking";
                    ex.description = "visualize a moving vehicle on the map. The examples loads the layers with roads and buildings. A path for a vehicle is " +
                                      "opened a separate shapefile with a single closed polyline shape. It is not added to the map. " +
                                      "On the events generated by timer a new position of the vehicle is calculated assuming the constant speed. Then " +
                                      "a red dot is displayed on the drawing layer on every even occurrence of the event (0, 2, 4, etc). The drawing " +
                                      "layer is cleared on every new occurrence of the event. The usage of the drawing layer helps to avoid complete " +
                                      " redraws of the map which can be slow.";
                    ex.function = "Tracking";
                    
                    ex.group = ExampleGroup.Interaction;
                    break;
                case 6:
                    ex.GuiName = "Zoom to values";
                    ex.description = "build a list of unique values of the given field and implement zooming to them from the context menu. After " +
                                     "the right click on the map a context menu will be displayed with names of the buildings under submenu for each " +
                                     "letter of the alphabet. Only the buildings with unique names will be included in the list. " +
                                     "After choosing a particular name the extents of the map will be changed to display the " +
                                     "corresponding building.";
                    ex.function = "ZoomToValues";
                    
                    ex.group = ExampleGroup.Interaction;
                    break;
                case 7:
                    ex.GuiName = "Minimal distance";
                    ex.description = "calculate the minimal distance from each building to the river and visualize it by charts.";
                    ex.function = "MinimalDistance";
                    
                    ex.group = ExampleGroup.Geoprocessing;
                    break;
                case 8:
                    ex.GuiName = "Show tooltip";
                    ex.description = "show tootip with attributes of a shape in mousemove event. The tooltip is drawn as a label on the " +
                                     "spatially referenced drawing layer. The smooth redraw of the tooltip is possible only for version higher than 4.8 " +
                                     " where the redraw of the spatially referenced drawing layers can be done independently of the data layers. ";
                    ex.function = "ToolTip";
                    
                    ex.group = ExampleGroup.Interaction;
                    break;
                case 9:
                    ex.GuiName = "Split by attribute";
                    ex.description = "split a shapefile into several shapefiles according the values of the given attribute. For each unique value of the " +
                                     "specified field a new shapefile will be created. Then each shape of the input shapefile will be copied to the " +
                                     "one of the newly created shapefiles. To determine the list of the unique value generation of shapefile categories " +
                                     "is made. Finally all new shapefiles are added to the map. A common color scheme is applied to them using ColorScheme " +
                                     "class. Note: generally it is not needed split a shapefile into parts for applying color scheme. More likely use of " +
                                     "such approach is splitting large shapefiles into several parts for faster rendering.";
                    ex.function = "SplitByAttribute";
                    
                    ex.group = ExampleGroup.Editing;
                    break;
                case 10:
                    ex.GuiName = "Segmentation";
                    ex.description = "do a segmentation of layers by regular grid, to calculate the percentage of land usage for each cell " +
                                     "and to show it as pie charts. " +
                                     "The combined extents of 2 polygon shapefiles are split by regular grid with 4 by 4 cells, "+ 
                                     "which is represented by polygon shapefile. " +
                                     "An intersection for each cell of grid and each input shapefile is calculated." +
                                     "To visualize the relative area of polygons obtained by intersection a temporary shapefile " +
                                     "with charts is created. Each chart denote the percentage of area covered by one of the input " +
                                     "shapefiles in the given cell. Note: the grid mentioned in this example has nothing to do with Grid class " +
                                     "provided by MapWinGIS. The displayed areas are represented in hectares.";
                    ex.function = "Segmentation";
                    
                    ex.group = ExampleGroup.Geoprocessing;
                    break;
                case 11:
                    ex.GuiName = "Remove shape";
                    ex.description = "remove shapes by mouse click and update their labels.";
                    ex.function = "RemoveShape";
                    ex.image = true;
                    ex.group = ExampleGroup.Editing;
                    break;
                case 12:
                    ex.GuiName = "Label selection";
                    ex.description = "select labels on the map by handling SelectBoxFinal event of the map control.";
                    ex.function = "LabelSelection";
                    
                    ex.group = ExampleGroup.Selection;
                    break;
                case 13:
                    ex.GuiName = "Point icons";
                    ex.description = "generate unique value classification for point shapefile and to assign an icon for each category. The " +
                    "icons will be chosen from the folder on disk automatically provided that the name of the icon matches the value of the " + 
                    "chosen field.";
                    ex.function = "PointIcons";
                    
                    ex.group = ExampleGroup.Symbology;
                    break;
                case 14:
                    ex.GuiName = "Line pattern";
                    ex.description = "create custom line patterns.";
                    ex.function = "LinePattern";
                    
                    ex.group = ExampleGroup.Symbology;
                    break;
                case 15:
                    ex.GuiName = "Image labels";
                    ex.description = "to add randomly positioned labels to the image layer.";
                    ex.function = "ImageLabels";
                    ex.image = true;
                    ex.group = ExampleGroup.Image;
                    break;
                case 16:
                    ex.GuiName = "GeoProjection";
                    ex.description = "initialize GeoProjection object and retrieve an information from it.";
                    ex.function = "GeoProjection";
                    ex.image = true;
                    ex.group = ExampleGroup.General;
                    break;
                case 17:
                    ex.GuiName = "Calculate area";
                    ex.description = "calculate the area of polygons, to write it to the attribute table, and to display as labels.";
                    ex.function = "CalculateArea";
                    ex.image = true;
                    ex.group = ExampleGroup.Editing;
                    break;
                case 18:
                    ex.GuiName = "Create point shapefile";
                    ex.description = "create a point shapefile by placing 1000 points randomly.";
                    ex.function = "CreatePointShapefile";
                    ex.image = true;
                    ex.group = ExampleGroup.Editing;
                    break;
                case 19:
                    ex.GuiName = "Create polygons shapefile";
                    ex.description = "create a polygon shapefile by placing 100 circles randomly.";
                    ex.function = "CreatePolygonShapefile";
                    
                    ex.group = ExampleGroup.Editing;
                    break;
                case 20:
                    ex.GuiName = "Add layers from path";
                    ex.description = "add all the shapefiles and images with .tif and .png extensions from the specified folder to the map.";
                    ex.function = "AddLayers";
                    ex.image = true;
                    ex.group = ExampleGroup.General;
                    break;
                case 21:
                    ex.GuiName = "Convert shapefile to drawing";
                    ex.description = "create a drawing layer from the specified shapefile.";
                    ex.function = "ShapefileToDrawingLayer";
                    ex.image = true;
                    ex.group = ExampleGroup.Editing;
                    break;
                case 22:
                    ex.GuiName = "Select by query";
                    ex.description = "select shapes with certain attributes using expression. The query string is: [type] = \"residential\" AND [osm_id]" +
                                     " > 40000000.";
                    ex.function = "SelectByQuery";
                    ex.image = true;
                    ex.group = ExampleGroup.Selection;
                    break;
                case 23:
                    ex.GuiName = "Polygons with holes";
                    ex.description = "create a shapefile holding circular polygons with holes.";
                    ex.function = "PolygonsWithHoles";
                    
                    ex.group = ExampleGroup.Editing;
                    break;
                case 24:
                    ex.GuiName = "Select by distance";
                    ex.description = "select buildings which lie within specified distance from the parks.";
                    ex.function = "SelectByDistance";
                    
                    ex.group = ExampleGroup.Selection;
                    break;
                case 25:
                    ex.GuiName = "Sort layers";
                    ex.description = "sorts layers by the type of shapefile (first polygons, the polylines and finally points) and " +
                                      "alphabetically.";
                    ex.function = "SortLayers";
                    ex.image = true;
                    ex.group = ExampleGroup.General;
                    break;
                case 26:
                    ex.GuiName = "Show attributes";
                    ex.description = "highlight shapes when mouse cursor is over them and to show the attribute values in the status bar.";
                    ex.function = "ShowAttributes";
                    ex.image = false;
                    ex.group = ExampleGroup.Version49;
                    break;
                case 27:
                    ex.GuiName = "Add range of categories";
                    ex.description = "calculate area of polygons and to set 3 ranges of categories with different symbology. This " +
                                     "example demonstrates quite elaborate specification of categories. In most cases much easier " +
                                     "approaches can be used (see ShapefileCategories.Generate).";
                    ex.function = "AddCategoryRange";
                    
                    ex.group = ExampleGroup.Symbology;
                    break;
                case 28:
                    ex.GuiName = "Generate labels manually";
                    ex.description = "Adds labels to the layer without using automated procedures like Labels.Generate.";
                    ex.function = "GenerateLabelsManually";
                    ex.image = true;
                    ex.group = ExampleGroup.Symbology;
                    break;
                case 29:
                    ex.GuiName = "Track vehicles";
                    ex.description = "simulate motion of vehicles based on GPS data.";
                    ex.function = "TrackCars";
                    ex.group = ExampleGroup.Interaction;
                    break;
                    
            }
        }
    }
}
