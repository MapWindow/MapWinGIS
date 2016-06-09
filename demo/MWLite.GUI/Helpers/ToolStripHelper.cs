using System;
using System.Collections.Generic;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;

namespace MapWinGIS.Demo.Helpers
{
    public static class ToolStripHelper
    {
        private static ToolStrip _draggedToolStrip = null;
        private static bool _dragging = false;

        public static void Init(Form form, ToolStripContainer ct)
        {
            //ToolStripManager.LoadSettings(form);
            //var panels = new[] {ct.TopToolStripPanel, ct.BottomToolStripPanel, ct.LeftToolStripPanel, ct.RightToolStripPanel};
            //foreach (var panel in panels)
            //{
            //    panel.MinimumSize = new Size(0, 0);
            //    panel.MaximumSize = new Size(0, 0);
            //}

            //var strips = panels.SelectMany(p => p.Controls.OfType<ToolStrip>());
            //foreach (var strip in strips)
            //{
            //    strip.BeginDrag += toolStrip1_BeginDrag;
            //    strip.EndDrag += toolStrip1_EndDrag;
            //    strip.LocationChanged += toolStrip1_LocationChanged;
            //}
        }

        private static void toolStrip1_LocationChanged(object sender, EventArgs e)
        {
            var ts = sender as ToolStrip;
            if (ts == null) return;
            if (ts == _draggedToolStrip) return;
            if (_draggedToolStrip != null)
            {
                if (ts.Top > 0 && ts.Orientation == Orientation.Horizontal)
                    ts.Top = 0;
                if (ts.Left > 0 && ts.Orientation == Orientation.Vertical)
                    ts.Left = 0;
            }
        }

        private static void toolStrip1_EndDrag(object sender, EventArgs e)
        {
            _dragging = false;
            _draggedToolStrip = null;
            MainForm.Instance.Map.LockWindow(tkLockMode.lmUnlock);
        }

        private static void toolStrip1_BeginDrag(object sender, EventArgs e)
        {
            _dragging = true;
            _draggedToolStrip = sender as ToolStrip;
            MainForm.Instance.Map.LockWindow(tkLockMode.lmLock);
        }
    }
}
