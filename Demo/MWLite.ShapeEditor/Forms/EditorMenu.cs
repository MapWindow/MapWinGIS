using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Drawing;
using System.Data;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core;

namespace MWLite.ShapeEditor.Forms
{
    public partial class EditorMenu : UserControl
    {
        public EditorMenu()
        {
            InitializeComponent();
            InitShapeEditorOptions();
        }

        private AxMapWinGIS.AxMap Map
        {
            get { return App.Instance.Map;  }
        }

        public ToolStripMenuItem Menu
        {
            get { return mnuShapeEditor; }
        }

        private void InitShapeEditorOptions()
        {
            mnuEditorVertices.Click += (s, args) => { Map.ShapeEditor.VerticesVisible = !Map.ShapeEditor.VerticesVisible; };
            mnuEditorHighlightShapes.Click += (s, args) => { Map.ShapeEditor.HighlightShapes = !Map.ShapeEditor.HighlightShapes; };
            mnuEditorAutoSnapping.Click += (s, args) =>
            {
                Map.ShapeEditor.SnapBehavior =
                    mnuEditorAutoSnapping.Checked ? tkSnapBehavior.sbSnapWithShift : tkSnapBehavior.sbSnapByDefault;
            };
            mnuShapeEditor.DropDownOpening += mnuShapeEditor_DropDownOpening;
        }

        void mnuShapeEditor_DropDownOpening(object sender, EventArgs e)
        {
            mnuEditorVertices.Checked = Map.ShapeEditor.VerticesVisible;
            mnuEditorAutoSnapping.Checked = Map.ShapeEditor.SnapBehavior == tkSnapBehavior.sbSnapByDefault;
            mnuEditorHighlightShapes.Checked = Map.ShapeEditor.HighlightShapes;
        }
    }
}
