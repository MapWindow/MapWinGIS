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
    public partial class DigitizingContextMenu : UserControl
    {
        public DigitizingContextMenu()
        {
            InitializeComponent();
            contextMenuStrip1.Opening += contextMenuStrip1_Opening;
            var editor = App.Map.ShapeEditor;

            ctxNoHighlighting.Click += (s, e) => editor.HighlightVertices = tkLayerSelection.lsNoLayer;
            ctxHighlightAllLayers.Click += (s, e) => editor.HighlightVertices = tkLayerSelection.lsAllLayers;
            ctxHighlightCurrentLayer.Click += (s, e) => editor.HighlightVertices = tkLayerSelection.lsActiveLayer;

            ctxNoSnapping.Click += (s, e) => editor.SnapBehavior = tkLayerSelection.lsNoLayer;
            ctxSnapCurrentLayer.Click += (s, e) => editor.SnapBehavior = tkLayerSelection.lsActiveLayer;
            ctxSnapAllLayers.Click += (s, e) => editor.SnapBehavior = tkLayerSelection.lsAllLayers;

            ctxFinishShape.Click += (s, e) => editor.SaveChanges();
            ctxCancelShape.Click += (s, e) => editor.Clear();

            ctxUndoPoint.Click += (s, e) => editor.UndoPoint();
        }

        void contextMenuStrip1_Opening(object sender, CancelEventArgs e)
        {
            bool notEmpty = !App.Map.ShapeEditor.IsEmpty;
            ctxCancelShape.Enabled = notEmpty;
            ctxFinishShape.Enabled = notEmpty;
            ctxUndoPoint.Enabled = notEmpty;

            ctxSnapAllLayers.Checked = App.Map.ShapeEditor.SnapBehavior == tkLayerSelection.lsAllLayers;
            ctxNoSnapping.Checked = App.Map.ShapeEditor.SnapBehavior == tkLayerSelection.lsNoLayer;
            ctxSnapCurrentLayer.Checked = App.Map.ShapeEditor.SnapBehavior == tkLayerSelection.lsActiveLayer;

            ctxHighlightCurrentLayer.Checked = App.Map.ShapeEditor.HighlightVertices == tkLayerSelection.lsActiveLayer;
            ctxHighlightAllLayers.Checked = App.Map.ShapeEditor.HighlightVertices == tkLayerSelection.lsAllLayers;
            ctxNoHighlighting.Checked = App.Map.ShapeEditor.HighlightVertices == tkLayerSelection.lsNoLayer;
        }

        public ContextMenuStrip Menu
        {
            get { return contextMenuStrip1; }
        }
    }
}
