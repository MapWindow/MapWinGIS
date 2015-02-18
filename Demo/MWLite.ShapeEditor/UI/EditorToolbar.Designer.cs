namespace MWLite.ShapeEditor.UI
{
    partial class EditorToolbar
    {
        /// <summary> 
        /// Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary> 
        /// Clean up any resources being used.
        /// </summary>
        /// <param name="disposing">true if managed resources should be disposed; otherwise, false.</param>
        protected override void Dispose(bool disposing)
        {
            if (disposing && (components != null))
            {
                components.Dispose();
            }
            base.Dispose(disposing);
        }

        #region Component Designer generated code

        /// <summary> 
        /// Required method for Designer support - do not modify 
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.toolStripSeparator21 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator24 = new System.Windows.Forms.ToolStripSeparator();
            this.toolUndoCount = new System.Windows.Forms.ToolStripLabel();
            this._editToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolEditLayer = new System.Windows.Forms.ToolStripButton();
            this.toolAddShape = new System.Windows.Forms.ToolStripButton();
            this.toolEditShape = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator25 = new System.Windows.Forms.ToolStripSeparator();
            this.toolMergeShapes = new System.Windows.Forms.ToolStripButton();
            this.toolSplitShapes = new System.Windows.Forms.ToolStripButton();
            this.toolMoveShapes = new System.Windows.Forms.ToolStripButton();
            this.toolRotateShapes = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator28 = new System.Windows.Forms.ToolStripSeparator();
            this.toolPolygonOperation = new System.Windows.Forms.ToolStripDropDownButton();
            this.toolEraseByPolygon = new System.Windows.Forms.ToolStripMenuItem();
            this.toolClipByPolygon = new System.Windows.Forms.ToolStripMenuItem();
            this.toolSplitByPolygon = new System.Windows.Forms.ToolStripMenuItem();
            this.toolSplitByPolyline = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator23 = new System.Windows.Forms.ToolStripSeparator();
            this.toolCopy = new System.Windows.Forms.ToolStripButton();
            this.toolCut = new System.Windows.Forms.ToolStripButton();
            this.toolPaste = new System.Windows.Forms.ToolStripButton();
            this.toolUndo = new System.Windows.Forms.ToolStripButton();
            this.toolRedo = new System.Windows.Forms.ToolStripButton();
            this._editToolStrip.SuspendLayout();
            this.SuspendLayout();
            // 
            // toolStripSeparator21
            // 
            this.toolStripSeparator21.Name = "toolStripSeparator21";
            this.toolStripSeparator21.Size = new System.Drawing.Size(6, 41);
            // 
            // toolStripSeparator24
            // 
            this.toolStripSeparator24.Name = "toolStripSeparator24";
            this.toolStripSeparator24.Size = new System.Drawing.Size(6, 41);
            // 
            // toolUndoCount
            // 
            this.toolUndoCount.Name = "toolUndoCount";
            this.toolUndoCount.Padding = new System.Windows.Forms.Padding(5);
            this.toolUndoCount.Size = new System.Drawing.Size(34, 38);
            this.toolUndoCount.Text = "0/0";
            // 
            // _editToolStrip
            // 
            this._editToolStrip.Dock = System.Windows.Forms.DockStyle.None;
            this._editToolStrip.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolEditLayer,
            this.toolStripSeparator21,
            this.toolAddShape,
            this.toolEditShape,
            this.toolStripSeparator25,
            this.toolMergeShapes,
            this.toolSplitShapes,
            this.toolMoveShapes,
            this.toolRotateShapes,
            this.toolStripSeparator28,
            this.toolPolygonOperation,
            this.toolSplitByPolyline,
            this.toolStripSeparator23,
            this.toolCopy,
            this.toolCut,
            this.toolPaste,
            this.toolStripSeparator24,
            this.toolUndo,
            this.toolUndoCount,
            this.toolRedo});
            this._editToolStrip.Location = new System.Drawing.Point(0, 4);
            this._editToolStrip.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this._editToolStrip.Name = "_editToolStrip";
            this._editToolStrip.Padding = new System.Windows.Forms.Padding(0);
            this._editToolStrip.Size = new System.Drawing.Size(641, 41);
            this._editToolStrip.TabIndex = 2;
            // 
            // toolEditLayer
            // 
            this.toolEditLayer.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolEditLayer.Image = global::MWLite.ShapeEditor.Properties.Resources.edit;
            this.toolEditLayer.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolEditLayer.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolEditLayer.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolEditLayer.Name = "toolEditLayer";
            this.toolEditLayer.Padding = new System.Windows.Forms.Padding(5);
            this.toolEditLayer.Size = new System.Drawing.Size(38, 39);
            this.toolEditLayer.Text = "Edit Layer";
            // 
            // toolAddShape
            // 
            this.toolAddShape.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolAddShape.Image = global::MWLite.ShapeEditor.Properties.Resources.vector_create;
            this.toolAddShape.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolAddShape.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAddShape.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolAddShape.Name = "toolAddShape";
            this.toolAddShape.Padding = new System.Windows.Forms.Padding(5);
            this.toolAddShape.Size = new System.Drawing.Size(38, 39);
            this.toolAddShape.Text = "Add Shape";
            // 
            // toolEditShape
            // 
            this.toolEditShape.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolEditShape.Image = global::MWLite.ShapeEditor.Properties.Resources.vertex_tools;
            this.toolEditShape.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolEditShape.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolEditShape.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolEditShape.Name = "toolEditShape";
            this.toolEditShape.Padding = new System.Windows.Forms.Padding(5);
            this.toolEditShape.Size = new System.Drawing.Size(38, 39);
            this.toolEditShape.Text = "Vertex Editor";
            // 
            // toolStripSeparator25
            // 
            this.toolStripSeparator25.Name = "toolStripSeparator25";
            this.toolStripSeparator25.Size = new System.Drawing.Size(6, 41);
            // 
            // toolMergeShapes
            // 
            this.toolMergeShapes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMergeShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.merge;
            this.toolMergeShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolMergeShapes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMergeShapes.Name = "toolMergeShapes";
            this.toolMergeShapes.Padding = new System.Windows.Forms.Padding(5);
            this.toolMergeShapes.Size = new System.Drawing.Size(38, 38);
            this.toolMergeShapes.Text = "Merge Shapes";
            // 
            // toolSplitShapes
            // 
            this.toolSplitShapes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSplitShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.split;
            this.toolSplitShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSplitShapes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSplitShapes.Name = "toolSplitShapes";
            this.toolSplitShapes.Padding = new System.Windows.Forms.Padding(5);
            this.toolSplitShapes.Size = new System.Drawing.Size(38, 38);
            this.toolSplitShapes.Text = "Split Shapes";
            // 
            // toolMoveShapes
            // 
            this.toolMoveShapes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolMoveShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.move;
            this.toolMoveShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolMoveShapes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolMoveShapes.Name = "toolMoveShapes";
            this.toolMoveShapes.Padding = new System.Windows.Forms.Padding(5);
            this.toolMoveShapes.Size = new System.Drawing.Size(38, 38);
            this.toolMoveShapes.Text = "Move Shapes";
            // 
            // toolRotateShapes
            // 
            this.toolRotateShapes.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRotateShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.shape_rotate;
            this.toolRotateShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolRotateShapes.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRotateShapes.Name = "toolRotateShapes";
            this.toolRotateShapes.Padding = new System.Windows.Forms.Padding(5);
            this.toolRotateShapes.Size = new System.Drawing.Size(32, 38);
            this.toolRotateShapes.Text = "Rotate Shapes";
            // 
            // toolStripSeparator28
            // 
            this.toolStripSeparator28.Name = "toolStripSeparator28";
            this.toolStripSeparator28.Size = new System.Drawing.Size(6, 41);
            // 
            // toolPolygonOperation
            // 
            this.toolPolygonOperation.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolPolygonOperation.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.toolEraseByPolygon,
            this.toolClipByPolygon,
            this.toolSplitByPolygon});
            this.toolPolygonOperation.Image = global::MWLite.ShapeEditor.Properties.Resources.polygon_erase;
            this.toolPolygonOperation.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolPolygonOperation.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPolygonOperation.Name = "toolPolygonOperation";
            this.toolPolygonOperation.Padding = new System.Windows.Forms.Padding(5);
            this.toolPolygonOperation.Size = new System.Drawing.Size(47, 38);
            this.toolPolygonOperation.Text = "Polygon Overlay";
            // 
            // toolEraseByPolygon
            // 
            this.toolEraseByPolygon.Name = "toolEraseByPolygon";
            this.toolEraseByPolygon.Size = new System.Drawing.Size(164, 22);
            this.toolEraseByPolygon.Text = "Erase By Polygon";
            // 
            // toolClipByPolygon
            // 
            this.toolClipByPolygon.Name = "toolClipByPolygon";
            this.toolClipByPolygon.Size = new System.Drawing.Size(164, 22);
            this.toolClipByPolygon.Text = "Clip By Polygon";
            // 
            // toolSplitByPolygon
            // 
            this.toolSplitByPolygon.Name = "toolSplitByPolygon";
            this.toolSplitByPolygon.Size = new System.Drawing.Size(164, 22);
            this.toolSplitByPolygon.Text = "Split By Polygon";
            // 
            // toolSplitByPolyline
            // 
            this.toolSplitByPolyline.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolSplitByPolyline.Image = global::MWLite.ShapeEditor.Properties.Resources.line_polygon_edit;
            this.toolSplitByPolyline.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolSplitByPolyline.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolSplitByPolyline.Name = "toolSplitByPolyline";
            this.toolSplitByPolyline.Padding = new System.Windows.Forms.Padding(5);
            this.toolSplitByPolyline.Size = new System.Drawing.Size(38, 38);
            this.toolSplitByPolyline.Text = "Split By Polyline";
            // 
            // toolStripSeparator23
            // 
            this.toolStripSeparator23.Name = "toolStripSeparator23";
            this.toolStripSeparator23.Size = new System.Drawing.Size(6, 41);
            // 
            // toolCopy
            // 
            this.toolCopy.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolCopy.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_copy;
            this.toolCopy.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolCopy.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolCopy.Name = "toolCopy";
            this.toolCopy.Padding = new System.Windows.Forms.Padding(5);
            this.toolCopy.Size = new System.Drawing.Size(38, 38);
            this.toolCopy.Text = "Copy";
            // 
            // toolCut
            // 
            this.toolCut.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolCut.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_cut;
            this.toolCut.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolCut.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolCut.Name = "toolCut";
            this.toolCut.Padding = new System.Windows.Forms.Padding(5);
            this.toolCut.Size = new System.Drawing.Size(38, 38);
            this.toolCut.Text = "Cut";
            // 
            // toolPaste
            // 
            this.toolPaste.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolPaste.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_paste;
            this.toolPaste.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolPaste.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolPaste.Name = "toolPaste";
            this.toolPaste.Padding = new System.Windows.Forms.Padding(5);
            this.toolPaste.Size = new System.Drawing.Size(38, 38);
            this.toolPaste.Text = "Paste";
            // 
            // toolUndo
            // 
            this.toolUndo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolUndo.Image = global::MWLite.ShapeEditor.Properties.Resources.undo;
            this.toolUndo.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolUndo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolUndo.Name = "toolUndo";
            this.toolUndo.Padding = new System.Windows.Forms.Padding(5);
            this.toolUndo.Size = new System.Drawing.Size(38, 38);
            this.toolUndo.Text = "Undo";
            // 
            // toolRedo
            // 
            this.toolRedo.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRedo.Image = global::MWLite.ShapeEditor.Properties.Resources.redo;
            this.toolRedo.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolRedo.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRedo.Name = "toolRedo";
            this.toolRedo.Padding = new System.Windows.Forms.Padding(5);
            this.toolRedo.Size = new System.Drawing.Size(38, 38);
            this.toolRedo.Text = "Redo";
            // 
            // EditorToolbar
            // 
            this.Controls.Add(this._editToolStrip);
            this.Name = "EditorToolbar";
            this.Size = new System.Drawing.Size(708, 52);
            this._editToolStrip.ResumeLayout(false);
            this._editToolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStripButton toolEditLayer;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator21;
        private System.Windows.Forms.ToolStripButton toolAddShape;
        private System.Windows.Forms.ToolStripButton toolMergeShapes;
        private System.Windows.Forms.ToolStripButton toolSplitShapes;
        private System.Windows.Forms.ToolStripButton toolRotateShapes;
        private System.Windows.Forms.ToolStripButton toolEditShape;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator24;
        private System.Windows.Forms.ToolStripButton toolUndo;
        private System.Windows.Forms.ToolStripLabel toolUndoCount;
        private System.Windows.Forms.ToolStripButton toolRedo;
        private System.Windows.Forms.ToolStrip _editToolStrip;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator23;
        private System.Windows.Forms.ToolStripButton toolMoveShapes;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator25;
        private System.Windows.Forms.ToolStripButton toolSplitByPolyline;
        private System.Windows.Forms.ToolStripDropDownButton toolPolygonOperation;
        private System.Windows.Forms.ToolStripMenuItem toolEraseByPolygon;
        private System.Windows.Forms.ToolStripMenuItem toolClipByPolygon;
        private System.Windows.Forms.ToolStripMenuItem toolSplitByPolygon;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator28;
        private System.Windows.Forms.ToolStripButton toolCopy;
        private System.Windows.Forms.ToolStripButton toolCut;
        private System.Windows.Forms.ToolStripButton toolPaste;

    }
}
