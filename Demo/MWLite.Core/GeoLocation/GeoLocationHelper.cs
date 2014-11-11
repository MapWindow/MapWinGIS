using System;
using System.Collections.Generic;
using System.IO;
using System.Net;
using System.Text;
using System.Web.Script.Serialization;
using MapWinGIS;

namespace MWLite.Core.GeoLocation
{
    public static class GeoLocationHelper
    {
        public static Extents FindLocation(string query)
        {
            string json = ResquestLocation(query);
            return ParseResult(json);
        }

        private static string ResquestLocation(string query)
        {
            var url = "https://maps.googleapis.com/maps/api/geocode/json?address=" + query;
            var request = (HttpWebRequest)WebRequest.Create(url);
            const string UserAgent = "Mozilla/5.0 (Windows; U; Windows NT 6.0; en-US; rv:1.9.1.7) Gecko/20091221 Firefox/3.5.7";
            const string requestAccept = "*/*";

            request.UserAgent = UserAgent;
            request.Timeout = 5000;
            request.ReadWriteTimeout = request.Timeout * 6;
            request.Accept = requestAccept;
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
                var list = new List<string>();
                numbers.Add((double)obj.results[0].geometry.bounds.northeast.lat);
                numbers.Add((double)obj.results[0].geometry.bounds.northeast.lng);
                numbers.Add((double)obj.results[0].geometry.bounds.southwest.lat);
                numbers.Add((double)obj.results[0].geometry.bounds.southwest.lng);

                if (numbers.Count == 4)
                {
                    double lat1 = numbers[0];
                    double lng1 = numbers[1];
                    double lat2 = numbers[2];
                    double lng2 = numbers[3];
                    var box = new Extents();
                    box.SetBounds(lng1, lat2, 0.0, lng2, lat1, 0.0);
                    return box;
                }
            }
            catch
            {
                throw new ApplicationException("Failed to find coordinates of location.");
            }
            return null;
        }
    }
}
