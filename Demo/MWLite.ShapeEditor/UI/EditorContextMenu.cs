using System.ComponentModel;
using System.Linq;
using System.Windows.Forms;
using MWLite.ShapeEditor.Helpers;

namespace MWLite.ShapeEditor.UI
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
            }

            var list = new[] { ctxCopy, ctxCut, ctxPaste };
            foreach (var item in list)
            {
                item.Enabled = StateHelper.GetEnabled(item);
            }
        }

        public ContextMenuStrip Menu
        {
            get { return contextMenuStrip1;  }
        }
    }
}
