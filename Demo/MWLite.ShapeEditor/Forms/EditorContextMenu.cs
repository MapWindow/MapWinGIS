using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MWLite.ShapeEditor.Helpers;

namespace MWLite.ShapeEditor.Forms
{
    public partial class EditorContextMenu : UserControl
    {
        public EditorContextMenu()
        {
            InitializeComponent();
            contextMenuStrip1.Opening += contextMenuStrip1_Opening;
        }

        private void InitMenu()
        {
            foreach(var item in contextMenuStrip1.Items.OfType<ToolStripMenuItem>())
                item.Enabled = false;
        }

        void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            InitMenu();

            var sf = App.SelectedShapefile;

            if (sf != null && sf.InteractiveEditing)
            {
                int selectedCount = sf.NumSelected;
                ctxMergeShapes.Enabled = selectedCount > 1;
                ctxSplitShapes.Enabled = selectedCount > 0;
                ctxMoveShapes.Enabled = selectedCount > 0;
                ctxRemoveShapes.Enabled = selectedCount > 0;
                ctxRotateShapes.Enabled = selectedCount > 0;
                ctxCopyShapes.Enabled = selectedCount > 0;
                ctxCutShapes.Enabled = selectedCount > 0;
                ctxPasteShapes.Enabled = !OperationHelper.BufferIsEmpty;
            }
        }

        public ContextMenuStrip Menu
        {
            get { return contextMenuStrip1;  }
        }
    }
}
