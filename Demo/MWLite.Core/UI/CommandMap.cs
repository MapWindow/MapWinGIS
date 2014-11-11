using System;
using System.Diagnostics;
using System.Linq;
using System.Windows.Forms;

namespace MWLite.Core.UI
{
    public abstract class CommandMap<T> where T : struct, IConvertible
    {
        protected bool CommandFromName(ToolStripItem item, ref T command)
        {
            string itemName = item.Name;
            itemName = itemName.ToLower();
            var prefixes = new[] {"tool", "mnu", "ctx"};
            foreach (var prefix in prefixes)
            {
                if (itemName.StartsWith(prefix) && itemName.Length > prefix.Length)
                    itemName = itemName.Substring(prefix.Length);
            }

            var dict = Enum.GetValues(typeof(T)).Cast<T>().ToDictionary(v => v.ToString().ToLower(), v => v);
            if (dict.ContainsKey(itemName))
            {
                command = dict[itemName];
                return true;
            }

            Debug.Print("Command not found: " + itemName);

            var menu = item as ToolStripMenuItem;
            if (menu != null && menu.DropDownItems.Count > 0)
                return false;

            if (item is ToolStripSeparator) return false;

            CommandNotFound(item);
            return false;
        }

        public abstract void Run(T command);

        protected abstract void CommandNotFound(ToolStripItem item);

        /// <summary>
        /// Sets event handlers for menu items
        /// </summary>
        public void InitMenu(ToolStripItemCollection items)
        {
            if (items == null)
                return;

            foreach (ToolStripItem item in items)
            {
                if (item.Tag == null)
                    item.Click += ItemClick;
                var menuItem = item as ToolStripMenuItem;
                if (menuItem != null)
                {
                    InitMenu(menuItem.DropDownItems);
                }
            }
        }

        /// <summary>
        /// Runs menu commands
        /// </summary>
        private void ItemClick(object sender, EventArgs e)
        {
            var item = sender as ToolStripItem;
            if (item == null)
                return;
            var command = Activator.CreateInstance<T>();
            if (CommandFromName(item, ref command))
                Run(command);
        }
    }
}
