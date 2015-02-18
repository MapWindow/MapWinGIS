using System;
using System.Collections.Generic;
using System.IO;
using System.Linq;
using System.Text;
using System.Xml.Serialization;
using MWLite.Core.UI;
using MWLite.GUI.Helpers;

namespace MWLite.GUI.Classes
{
    [Serializable]
    public class AppSettings
    {
        public AppSettings()
        {
            BingApiKey = "";
            LastProject = "";
            ShowTooltip = true;
        }

        public bool ShowTooltip { get; set; }
        public string LastProject { get; set;}
        public string BingApiKey { get; set; }

        private static AppSettings _settings;

        public static AppSettings Instance
        {
            get
            {
                if (_settings == null)
                {
                    Deserialize();
                    if (_settings == null) _settings = new AppSettings();
                }
                return _settings;
            }
        }

        private static void Deserialize()
        {
            try
            {
                if (File.Exists(PathHelper.GetSettingsPath()))
                {
                    var ser = new XmlSerializer(typeof(AppSettings));
                    using (var stream = new FileStream(PathHelper.GetSettingsPath(), FileMode.Open))
                    {
                        _settings = ser.Deserialize(stream) as AppSettings;
                        stream.Close();
                    }
                }
            }
            catch { }
        }

        public static void Save()
        {
            try
            {
                var sett = Instance;
                if (_settings != null)
                {
                    var ser = new XmlSerializer(typeof(AppSettings));
                    using (var stream = new FileStream(PathHelper.GetSettingsPath(), FileMode.Create))
                    {
                        ser.Serialize(stream, _settings);
                        stream.Flush();
                        stream.Close();
                    }
                }
            }
            catch (Exception ex)
            {
                MessageHelper.Info("Failed to save app settings: " + ex.Message);
            }
        }
    }
}
