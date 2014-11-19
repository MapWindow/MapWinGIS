using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;

namespace MWLite.ShapeEditor.UI
{
    public partial class VertexContextMenu : UserControl
    {
        public VertexContextMenu()
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

            bool hasShape = !App.Map.ShapeEditor.IsEmpty;
            ctxAddPart.Enabled = hasShape;
            ctxRemovePart.Enabled = hasShape;
            ctxSaveShape.Enabled = hasShape;
            ctxClearEditor.Enabled = hasShape;
            ctxVertexEditor.Enabled = true;
            ctxPartEditor.Enabled = true;
            ctxVertexEditor.Checked = App.Map.ShapeEditor.EditorBehavior == tkEditorBehavior.ebVertexEditor;
            ctxPartEditor.Checked = App.Map.ShapeEditor.EditorBehavior == tkEditorBehavior.ebPartEditor;

            bool hasChanges = App.Map.ShapeEditor.HasChanges;
            if (hasChanges)
            {
                ctxSaveShape.Enabled = true;
                ctxClearEditor.Text = "Discard changes";
            }
            else
            {
                ctxSaveShape.Enabled = false;
                ctxClearEditor.Text = "Stop editing";
            }
        }

        public ContextMenuStrip Menu
        {
            get { return contextMenuStrip1;  }
        }
    }
}
