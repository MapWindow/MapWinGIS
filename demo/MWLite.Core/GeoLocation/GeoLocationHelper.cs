using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Web.Script.Serialization;
using MapWinGIS;
using System.Globalization;

namespace MWLite.Core.GeoLocation
{
    public static class GeoLocationHelper
    {
        private static string _license = "";

        public static string License
        {
            get { return _license; }
        }

        public static Extents FindLocation(string query)
        {
            string json = ResquestLocation(query);
            return ParseResult(json);
        }

        private static string ResquestLocation(string query)
        {
            //var url = "https://maps.googleapis.com/maps/api/geocode/json?address=" + query;
            var url = string.Format("https://nominatim.openstreetmap.org/search/{0}?format=json&limit=1", query);
            var request = (HttpWebRequest)WebRequest.Create(url);
            
            request.Referer = "mapwingis.codeplex.com";
            request.UserAgent = "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.1.7) Gecko/20091221 Firefox/3.5.7";
            request.Timeout = 5000;
            request.ReadWriteTimeout = request.Timeout * 6;
            request.Accept = "*/*";
            string result = "";

            try
            {
                using (var response = request.GetResponse() as HttpWebResponse)
                {
                    if (response != null)
                    {
                        if (response.StatusCode == HttpStatusCode.OK)
                        {
                            using (Stream responseStream = response.GetResponseStream())
                            {
                                if (responseStream != null)
                                {
                                    using (var read = new StreamReader(responseStream, Encoding.UTF8))
                                    {
                                        result = read.ReadToEnd();
                                    }
                                }
                            }
                        }
                        response.Close();
                    }
                }
            }
            catch
            {
                throw new ApplicationException("Failed to run HTTP request.");
            }
            return result;
        }

        private static Extents ParseResult(string result)
        {
            try
            {
                var serializer = new JavaScriptSerializer();
                serializer.RegisterConverters(new[] { new DynamicJsonConverter() });
                dynamic obj = serializer.Deserialize(result, typeof(object));

                var numbers = new List<double>();
                
                for (int i = 0; i < 4; i++)
			    {
                    numbers.Add(double.Parse(obj[0].boundingbox[i], CultureInfo.InvariantCulture));
			    }

                Extents box = null;
                if (numbers.Count == 4)
                {
                    double lat1 = numbers[0];
                    double lat2 = numbers[1];
                    double lng1 = numbers[2];
                    double lng2 = numbers[3];
                    box = new Extents();
                    box.SetBounds(lng1, lat1, 0.0, lng2, lat2, 0.0);
                }

                try { 
                    _license = obj[0].licence;
                }
                catch {}

                return box;
            }
            catch
            {
                throw new ApplicationException("Failed to find coordinates of location.");
            }
        }
    }
}
