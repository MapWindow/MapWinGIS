using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.GUI.Forms;
using MWLite.GUI.Classes;
using MWLite.Symbology.Classes;
using MWLite.Core.UI;

namespace MWLite.GUI.Helpers
{
    internal static class TilesHelper
    {
        private enum Commands
        {
            SetBingApiKey = -2,
        }
        
        public static void Init(ToolStripMenuItem root)
        {
            root.DropDownItems.Clear();
            var item = root.DropDownItems.Add("No tiles");
            item.Click += item_Click;
            item.Tag = -1;

            root.DropDownItems.Add(new ToolStripSeparator());

            var list = new tkTileProvider[]
            {
                tkTileProvider.OpenStreetMap, tkTileProvider.OpenTransportMap,
                tkTileProvider.OpenHumanitarianMap, tkTileProvider.OpenCycleMap,
                tkTileProvider.MapQuestAerial, tkTileProvider.BingMaps,
                tkTileProvider.BingHybrid, tkTileProvider.BingSatellite
            };
            foreach (var p in list)
            {
                item = root.DropDownItems.Add(p.ToString());
                item.Click += item_Click;
                item.Tag = (int)p;
            }

            root.DropDownItems.Add(new ToolStripSeparator());

            item = root.DropDownItems.Add("Set Bing Maps API key");
            item.Click += item_Click;
            item.Tag = Commands.SetBingApiKey;

            root.DropDownOpening += root_DropDownOpening;
            App.Map.Tiles.DoCaching[tkCacheType.Disk] = true;
            App.Map.Tiles.UseCache[tkCacheType.Disk] = true;
        }

        static void root_DropDownOpening(object sender, EventArgs e)
        {
            var menu = sender as ToolStripMenuItem;
            if (menu == null) return;
            Func<ToolStripMenuItem, bool> predicate =
                item => item.Tag != null && ((tkTileProvider)item.Tag == App.Map.TileProvider);
            var items = menu.DropDownItems.OfType<ToolStripMenuItem>().ToList();
            foreach (var item in items)
                item.Checked = false;
            var selectedItem = items.FirstOrDefault(predicate);
            if (selectedItem != null)
                selectedItem.Checked = true;
        }

        private static void item_Click(object sender, EventArgs e)
        {
            var item = sender as ToolStripItem;
            if (item != null && item.Tag != null)
            {
                if ((int)item.Tag == (int)Commands.SetBingApiKey)
                {
                    SetBingApiKey();
                    return;
                }
                
                tkTileProvider provider = (tkTileProvider)item.Tag;
                switch(provider)
                {
                    case tkTileProvider.BingSatellite:
                    case tkTileProvider.BingMaps:
                    case tkTileProvider.BingHybrid:
                        var gs = new GlobalSettings();
                        if (string.IsNullOrWhiteSpace(gs.BingApiKey))
                        {
                            if (!string.IsNullOrWhiteSpace(AppSettings.Instance.BingApiKey))
                            {
                                gs.BingApiKey = AppSettings.Instance.BingApiKey;
                            }
                            else
                            {
                                if (!SetBingApiKey()) return;
                            }
                        }
                        break;
                }
                App.Map.TileProvider = (tkTileProvider)item.Tag;
                App.Map.Redraw();
            }
        }

        private static bool SetBingApiKey()
        {
            using (var form = new BingApiKeyForm())
            {
                if (form.ShowDialog(MainForm.Instance) != DialogResult.OK)
                    return false;
            }
            return true;
        }
    }
}
