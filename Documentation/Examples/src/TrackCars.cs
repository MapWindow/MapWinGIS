using System.Collections.Generic;
using System.Diagnostics;
using AxMapWinGIS;
using Examples.Classes;
using MapWinGIS;
using System;
using System.IO;
using System.Windows.Forms;

namespace Examples
{
    public partial class MapExamples
    {
        private Shapefile _carShapefile;

        public void TrackCars(AxMap axMap1, string dataPath)
        {
            if (!InitMap(axMap1, dataPath))
                return;

            string filename = dataPath + "path.shp";
            if (!File.Exists(filename))
            {
                MessageBox.Show("Path.shp wasn't found: " + dataPath);
                return;
            }

            int handle = axMap1.AddLayerFromFilename(filename, tkFileOpenStrategy.fosAutoDetect, false);

            var sf = axMap1.get_Shapefile(handle);
            var service = new CarService(axMap1.Extents as Extents, sf, 20);

            _carShapefile = CreateCarShapefile(service);
            _carShapefile.Volatile = true;
            _carShapefile.CollisionMode = tkCollisionMode.AllowCollisions;
            axMap1.AddLayer(_carShapefile, true);

            axMap1.ZoomToLayer(handle);

            service.StateChanged += ServiceStateChanged;
        }

        private bool InitMap(AxMap axMap1, string dataPath)
        {
            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.GrabProjectionFromData = true;
            axMap1.ZoomBehavior = tkZoomBehavior.zbUseTileLevels;

            axMap1.DisableWaitCursor = true;

            string filename1 = dataPath + "buildings.shp";
            string filename2 = dataPath + "roads.shp";
            

            if (!File.Exists(filename1) || !File.Exists(filename2))
            {
                MessageBox.Show("Couldn't find the files (buildings.shp, roads.shp): " + dataPath);
                return false;
            }
            
            Shapefile sf = new Shapefile();
            sf.Open(filename1, null);
            axMap1.AddLayer(sf, true);

            Debug.Print(axMap1.GeoProjection.ExportToWKT());

            sf = new Shapefile();
            sf.Open(filename2, null);
            sf.Labels.Generate("[Name]", tkLabelPositioning.lpLongestSegement, false);

            axMap1.ZoomToMaxExtents();
            return true;
        }

        private Shapefile CreateCarShapefile(CarService service)
        {
            var sf = new Shapefile();
            sf.CreateNew("", ShpfileType.SHP_POINT);
            sf.DefaultDrawingOptions.AlignPictureByBottom = false;

            var ct = sf.Categories.Add("Police");
            var opt = ct.DrawingOptions;
            opt.PointType = tkPointSymbolType.ptSymbolPicture;
            opt.Picture = IconManager.GetIcon(CarType.Police);

            ct = sf.Categories.Add("Taxi");
            opt = ct.DrawingOptions;
            opt.PointType = tkPointSymbolType.ptSymbolPicture;
            opt.Picture = IconManager.GetIcon(CarType.Taxi);

            ct = sf.Categories.Add("Ambulance");
            opt = ct.DrawingOptions;
            opt.PointType = tkPointSymbolType.ptSymbolPicture;
            opt.Picture = IconManager.GetIcon(CarType.Ambulance);

            // general settings for labels should be applied before creating categories
            sf.Labels.FrameVisible = true;
            sf.Labels.TextRenderingHint = tkTextRenderingHint.ClearTypeGridFit;

            var utils = new Utils();
            var lb = sf.Labels.AddCategory("Busy");
            lb.FrameBackColor = utils.ColorByName(tkMapColor.Yellow);

            lb = sf.Labels.AddCategory("Available");
            lb.FrameBackColor = utils.ColorByName(tkMapColor.Green);

            lb = sf.Labels.AddCategory("OutOfService");
            lb.FrameBackColor = utils.ColorByName(tkMapColor.Gray);

            Debug.Print("Num categories: " + sf.Labels.NumCategories);

            for (int i = 0; i < service.Cars.Count; i++)
            {
                var car = service.Cars[i];
                var shp = new Shape();
                shp.Create(ShpfileType.SHP_POINT);
                shp.AddPoint(car.X, car.Y);
                sf.EditAddShape(shp);
                sf.ShapeCategory[i] = (int) car.CarType;
                sf.Labels.AddLabel(car.ToString(), car.X, car.Y);
                sf.Labels.Label[i, 0].Category = (int)(car.State);
            }
            return sf;
        }

        private void ServiceStateChanged(object sender, EventArgs e)
        {
            var service = sender as CarService;
            if (service == null) return;
            for (int i = 0; i < service.Cars.Count; i++)
            {
                var car = service.Cars[i];
                _carShapefile.Shape[i].put_XY(0, car.X, car.Y);
                _carShapefile.ShapeRotation[i] = car.Direction;
                var lbl = _carShapefile.Labels.Label[i, 0];
                lbl.x = car.X + 20;
                lbl.y = car.Y;
            }
            axMap1.Redraw2(tkRedrawType.RedrawSkipDataLayers);
        }
    }

    public enum CarState
    {
        Busy = 0,
        Avaiable = 1,
        OutOfService = 2
    }

    public enum CarType
    {
        Police = 0,
        Taxi = 1,
        Ambulance = 2
    }

    public class GeometryHelper
    {
        /// <summary>
        /// Returns direction (angle in radians) at which a point is located relative to the coordinate origin
        /// </summary>
        public static double GetDirection(double x, double y)
        {
            if (Math.Abs(y) > 1e-10)
            {
                double angle = Math.Atan(x / y);
                if (y < 0)
                {
                    return Math.PI + angle;
                }
                if (x >= 0)
                {
                    return angle;
                }
                return 2.0 * Math.PI + angle;
            }
            if (x > 0)
            {
                return Math.PI / 2.0;
            }
            if (x < 0)
            {
                return 1.5 * Math.PI;
            }
            return 0.0;
        }
    }

    public class Car
    {
        private double _distance;

        public Car(Shape path, Random rnd)
        {
            Path = path;
            _distance = path.Length * rnd.NextDouble();
        }

        #region Properties

        public int Id { get; set; }
        public double X { get; set; }
        public double Y { get; set; }
        public double Direction { get; set; }
        public double Speed { get; set; }       // km per hour
        public CarState State { get; set; }
        public CarType CarType { get; set; }
        public Shape Path { get; set; }

        public double MetersPerSecond
        {
            get { return Speed * 1000 / 3600; }
        }

        #endregion

        #region Methods

        public override string ToString()
        {
            return CarType + " " + Id;
        }

        public void Move(int interval)
        {
            double step = MetersPerSecond * interval / 1000;      // meters

            _distance += step;
            if (_distance > Path.Length) _distance = 0;

            //calculating the current position (x2, y2)
            double dist = 0.0;
            double x1, x2, y1, y2;
            x1 = x2 = y1 = y2 = 0.0;

            for (int i = 1; i < Path.NumPoints; i++)
            {
                Path.get_XY(i, ref x2, ref y2);
                Path.get_XY(i - 1, ref x1, ref y1);

                double val = Math.Sqrt(Math.Pow(x2 - x1, 2.0) + Math.Pow(y2 - y1, 2.0));
                if (dist + val > _distance)
                {
                    double ratio = (_distance - dist) / val;
                    x2 = x1 + (x2 - x1) * ratio;
                    y2 = y1 + (y2 - y1) * ratio;
                    break;
                }
                if (dist + val < _distance)
                {
                    dist += val;
                }
                else
                {
                    break;
                }
            }

            Direction = GeometryHelper.GetDirection(x2 - X, y2 - Y) / Math.PI * 180.0;
            X = x2;
            Y = y2;
        }

        #endregion
    }

    public class IconManager
    {
        private static Dictionary<CarType, Image> _dict = new Dictionary<CarType, Image>();
        private static string _iconPath = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\icons\";

        static IconManager()
        {
            foreach (CarType val in Enum.GetValues(typeof(CarType)))
            {
                string path = IconPathForCarType(val);
                if (File.Exists(path))
                {
                    var img = new Image();
                    if (img.Open(path))
                    {
                        _dict.Add(val, img);
                    }
                }
            }
        }

        private static string IconPathForCarType(CarType type)
        {
            string path = _iconPath;
            switch (type)
            {
                case CarType.Police:
                    return path + "police_car.png";
                case CarType.Taxi:
                    return path + "taxi_car.png";
                case CarType.Ambulance:
                    return path + "ambulance_car.png";
            }
            return string.Empty;
        }

        public static Image GetIcon(CarType type)
        {
            return _dict.ContainsKey(type) ? _dict[type] : null;
        }
    }

    public class CarService
    {
        private const int UPDATE_INTERVAL = 200;   // milliseconds

        public List<Car> Cars = new List<Car>();
        public event EventHandler StateChanged;
        private Timer _timer = new Timer();
        private Random _random = new Random();

        private void FireStateChanged()
        {
            var handler = StateChanged;
            if (handler != null)
                handler(this, new EventArgs());
        }

        public CarService(Extents ext, Shapefile paths, int numCars)
        {
            if (ext == null)
                throw new NullReferenceException();

            CreateCars(ext, paths, numCars);

            UpdatePositions();

            InitTimer();
        }

        private void CreateCars(Extents ext, Shapefile paths, int numCars)
        {
            Cars.Clear();
            for (int i = 0; i < numCars; i++)
            {
                var path = paths.Shape[0]; //paths.Shape[_random.Next(paths.NumShapes - 1)];
                double x = ext.xMin + (ext.xMax - ext.xMin) * _random.NextDouble();
                double y = ext.yMin + (ext.yMax - ext.yMin) * _random.NextDouble();
                var car = new Car(path, _random)
                {
                    Id = i,
                    X = x,
                    Y = y,
                    Speed = 30 + 50 * _random.NextDouble(),
                    CarType = (CarType)_random.Next(3),
                    State = (CarState)_random.Next(3),
                };
                Cars.Add(car);
            }
        }

        private void InitTimer()
        {
            _timer.Interval = UPDATE_INTERVAL;
            _timer.Start();
            _timer.Tick += (s, e) => UpdatePositions();
        }

        private void UpdatePositions()
        {
            foreach (var car in Cars)
            {
                car.Move(UPDATE_INTERVAL);
            }
            FireStateChanged();
        }
    }
}

