using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.GUI.Forms;

namespace MWLite.GUI.Helpers
{
    internal static class TilesHelper
    {
        public static void Init(ToolStripMenuItem root)
        {
            root.DropDownItems.Clear();
            var except = new[]
            {
                tkTileProvider.Rosreestr, tkTileProvider.ProviderNone, tkTileProvider.ProviderCustom,
                tkTileProvider.Topomapper
            };
            var list = Enum.GetValues(typeof(tkTileProvider)).Cast<tkTileProvider>().Except(except);
            foreach (var p in list)
            {
                var item = root.DropDownItems.Add(p.ToString());
                item.Click += item_Click;
                item.Tag = (int)p;
            }
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
                App.Map.TileProvider = (tkTileProvider)item.Tag;
            }
        }
    }
}
