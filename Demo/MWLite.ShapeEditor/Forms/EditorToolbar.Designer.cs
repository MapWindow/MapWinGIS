namespace MWLite.ShapeEditor.Forms
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
            this.toolStripSeparator22 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripSeparator24 = new System.Windows.Forms.ToolStripSeparator();
            this.toolUndoCount = new System.Windows.Forms.ToolStripLabel();
            this._editToolStrip = new System.Windows.Forms.ToolStrip();
            this.toolEditLayer = new System.Windows.Forms.ToolStripButton();
            this.toolAddShape = new System.Windows.Forms.ToolStripButton();
            this.toolAddPart = new System.Windows.Forms.ToolStripButton();
            this.toolRemovePart = new System.Windows.Forms.ToolStripButton();
            this.toolMergeShapes = new System.Windows.Forms.ToolStripButton();
            this.toolSplitShapes = new System.Windows.Forms.ToolStripButton();
            this.toolRotateShape = new System.Windows.Forms.ToolStripButton();
            this.toolMoveShapes = new System.Windows.Forms.ToolStripButton();
            this.toolStripSeparator23 = new System.Windows.Forms.ToolStripSeparator();
            this.toolEditShape = new System.Windows.Forms.ToolStripButton();
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
            // toolStripSeparator22
            // 
            this.toolStripSeparator22.Name = "toolStripSeparator22";
            this.toolStripSeparator22.Size = new System.Drawing.Size(6, 41);
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
            this.toolAddPart,
            this.toolRemovePart,
            this.toolStripSeparator22,
            this.toolMergeShapes,
            this.toolSplitShapes,
            this.toolRotateShape,
            this.toolMoveShapes,
            this.toolStripSeparator23,
            this.toolEditShape,
            this.toolStripSeparator24,
            this.toolUndo,
            this.toolUndoCount,
            this.toolRedo});
            this._editToolStrip.Location = new System.Drawing.Point(0, 4);
            this._editToolStrip.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this._editToolStrip.Name = "_editToolStrip";
            this._editToolStrip.Padding = new System.Windows.Forms.Padding(0);
            this._editToolStrip.Size = new System.Drawing.Size(512, 41);
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
            // toolAddPart
            // 
            this.toolAddPart.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolAddPart.Image = global::MWLite.ShapeEditor.Properties.Resources.ring_add;
            this.toolAddPart.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolAddPart.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolAddPart.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolAddPart.Name = "toolAddPart";
            this.toolAddPart.Padding = new System.Windows.Forms.Padding(5);
            this.toolAddPart.Size = new System.Drawing.Size(38, 39);
            this.toolAddPart.Text = "Add Part";
            // 
            // toolRemovePart
            // 
            this.toolRemovePart.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRemovePart.Image = global::MWLite.ShapeEditor.Properties.Resources.ring_delete;
            this.toolRemovePart.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolRemovePart.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRemovePart.Margin = new System.Windows.Forms.Padding(0, 0, 0, 2);
            this.toolRemovePart.Name = "toolRemovePart";
            this.toolRemovePart.Padding = new System.Windows.Forms.Padding(5);
            this.toolRemovePart.Size = new System.Drawing.Size(38, 39);
            this.toolRemovePart.Text = "Remove Part by Polygon";
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
            // toolRotateShape
            // 
            this.toolRotateShape.DisplayStyle = System.Windows.Forms.ToolStripItemDisplayStyle.Image;
            this.toolRotateShape.Image = global::MWLite.ShapeEditor.Properties.Resources.shape_rotate;
            this.toolRotateShape.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolRotateShape.ImageTransparentColor = System.Drawing.Color.Magenta;
            this.toolRotateShape.Name = "toolRotateShape";
            this.toolRotateShape.Padding = new System.Windows.Forms.Padding(5);
            this.toolRotateShape.Size = new System.Drawing.Size(32, 38);
            this.toolRotateShape.Text = "Rotate Shapes";
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
            // toolStripSeparator23
            // 
            this.toolStripSeparator23.Name = "toolStripSeparator23";
            this.toolStripSeparator23.Size = new System.Drawing.Size(6, 41);
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
            this.Size = new System.Drawing.Size(625, 52);
            this._editToolStrip.ResumeLayout(false);
            this._editToolStrip.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.ToolStripButton toolEditLayer;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator21;
        private System.Windows.Forms.ToolStripButton toolAddShape;
        private System.Windows.Forms.ToolStripButton toolAddPart;
        private System.Windows.Forms.ToolStripButton toolRemovePart;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator22;
        private System.Windows.Forms.ToolStripButton toolMergeShapes;
        private System.Windows.Forms.ToolStripButton toolSplitShapes;
        private System.Windows.Forms.ToolStripButton toolRotateShape;
        private System.Windows.Forms.ToolStripButton toolEditShape;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator24;
        private System.Windows.Forms.ToolStripButton toolUndo;
        private System.Windows.Forms.ToolStripLabel toolUndoCount;
        private System.Windows.Forms.ToolStripButton toolRedo;
        private System.Windows.Forms.ToolStrip _editToolStrip;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator23;
        private System.Windows.Forms.ToolStripButton toolMoveShapes;

    }
}
