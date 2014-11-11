using System.Linq;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.ShapeEditor.Properties;

namespace MWLite.ShapeEditor.Forms
{
    public partial class EditorToolbar : UserControl
    {
        public EditorToolbar()
        {
            InitializeComponent();
        }

        public ToolStrip Toolbar
        {
            get { return _editToolStrip; }
        }

        public void RefreshUI()
        {
            int layerHandle = App.Instance.Legend.SelectedLayer;
            bool hasLayer = layerHandle != -1;
            var sf = App.Instance.Map.get_Shapefile(layerHandle);
            bool hasShapefile = sf != null;

            toolEditLayer.Checked = false;

            toolSplitShapes.Enabled = false;
            toolMergeShapes.Enabled = false;
            toolRotateShape.Enabled = false;

            bool editing = false;
            if (hasShapefile)
            {
                editing = sf.InteractiveEditing;
                int numSelected = sf.NumSelected;
                toolSplitShapes.Enabled = numSelected > 0;
                toolMergeShapes.Enabled = numSelected > 1;
                toolRotateShape.Enabled = numSelected > 0;
            }

            foreach (var item in _editToolStrip.Items.OfType<ToolStripItem>().
                    Where(item => item != toolEditLayer))
            
            item.Enabled = editing;
            toolEditLayer.Enabled = hasShapefile;
            toolEditLayer.Image = editing ? Resources.save1 : Resources.edit;  // save
            toolEditLayer.Text = editing ? "Save Changes" : "Edit Layer";

            var map = App.Instance.Map;
            toolAddPart.Checked = map.CursorMode == tkCursorMode.cmAddPart;
            toolAddShape.Checked = map.CursorMode == tkCursorMode.cmAddShape;
            toolEditShape.Checked = map.CursorMode == tkCursorMode.cmEditShape;
            toolRemovePart.Checked = map.CursorMode == tkCursorMode.cmRemovePart;
            toolMoveShapes.Checked = map.CursorMode == tkCursorMode.cmMoveShapes;
           
            toolUndo.Enabled = map.UndoList.UndoCount > 0;
            toolRedo.Enabled = map.UndoList.RedoCount > 0;
            toolUndoCount.Text = string.Format("{0}\\{1}", map.UndoList.UndoCount, map.UndoList.TotalLength);
        }
    }
}
