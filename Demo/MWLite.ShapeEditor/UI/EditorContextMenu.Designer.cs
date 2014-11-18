namespace MWLite.ShapeEditor.UI
{
    partial class EditorContextMenu
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
            this.components = new System.ComponentModel.Container();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.ctxSplitShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxMergeShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxRotateShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxMoveShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxCopy = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxCut = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxPaste = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxRemoveShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.toolStripMenuItem4 = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxSplitShapes,
            this.ctxMergeShapes,
            this.ctxRotateShapes,
            this.ctxMoveShapes,
            this.toolStripSeparator3,
            this.ctxCopy,
            this.ctxCut,
            this.ctxPaste,
            this.toolStripSeparator1,
            this.ctxRemoveShapes,
            this.toolStripSeparator4,
            this.toolStripMenuItem4});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(163, 314);
            // 
            // ctxSplitShapes
            // 
            this.ctxSplitShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.split;
            this.ctxSplitShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxSplitShapes.Name = "ctxSplitShapes";
            this.ctxSplitShapes.Size = new System.Drawing.Size(162, 30);
            this.ctxSplitShapes.Text = "Split multi-part";
            // 
            // ctxMergeShapes
            // 
            this.ctxMergeShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.merge;
            this.ctxMergeShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxMergeShapes.Name = "ctxMergeShapes";
            this.ctxMergeShapes.Size = new System.Drawing.Size(162, 30);
            this.ctxMergeShapes.Text = "Merge";
            // 
            // ctxRotateShapes
            // 
            this.ctxRotateShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.shape_rotate;
            this.ctxRotateShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxRotateShapes.Name = "ctxRotateShapes";
            this.ctxRotateShapes.Size = new System.Drawing.Size(162, 30);
            this.ctxRotateShapes.Text = "Rotate";
            // 
            // ctxMoveShapes
            // 
            this.ctxMoveShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.move;
            this.ctxMoveShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxMoveShapes.Name = "ctxMoveShapes";
            this.ctxMoveShapes.Size = new System.Drawing.Size(162, 30);
            this.ctxMoveShapes.Text = "Move";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(159, 6);
            // 
            // ctxCopy
            // 
            this.ctxCopy.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_copy;
            this.ctxCopy.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxCopy.Name = "ctxCopy";
            this.ctxCopy.Size = new System.Drawing.Size(162, 30);
            this.ctxCopy.Text = "Copy";
            // 
            // ctxCut
            // 
            this.ctxCut.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_cut;
            this.ctxCut.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxCut.Name = "ctxCut";
            this.ctxCut.Size = new System.Drawing.Size(162, 30);
            this.ctxCut.Text = "Cut";
            // 
            // ctxPaste
            // 
            this.ctxPaste.Image = global::MWLite.ShapeEditor.Properties.Resources.edit_paste;
            this.ctxPaste.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxPaste.Name = "ctxPaste";
            this.ctxPaste.Size = new System.Drawing.Size(162, 30);
            this.ctxPaste.Text = "Paste";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(159, 6);
            // 
            // ctxRemoveShapes
            // 
            this.ctxRemoveShapes.Image = global::MWLite.ShapeEditor.Properties.Resources.element_delete;
            this.ctxRemoveShapes.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.ctxRemoveShapes.Name = "ctxRemoveShapes";
            this.ctxRemoveShapes.Size = new System.Drawing.Size(162, 30);
            this.ctxRemoveShapes.Text = "Delete";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(159, 6);
            // 
            // toolStripMenuItem4
            // 
            this.toolStripMenuItem4.Image = global::MWLite.ShapeEditor.Properties.Resources.table;
            this.toolStripMenuItem4.ImageScaling = System.Windows.Forms.ToolStripItemImageScaling.None;
            this.toolStripMenuItem4.Name = "toolStripMenuItem4";
            this.toolStripMenuItem4.Size = new System.Drawing.Size(162, 30);
            this.toolStripMenuItem4.Text = "Attributes";
            // 
            // EditorContextMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "EditorContextMenu";
            this.Size = new System.Drawing.Size(150, 182);
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem ctxSplitShapes;
        private System.Windows.Forms.ToolStripMenuItem ctxMergeShapes;
        private System.Windows.Forms.ToolStripMenuItem ctxRotateShapes;
        private System.Windows.Forms.ToolStripMenuItem ctxMoveShapes;
        private System.Windows.Forms.ToolStripMenuItem ctxRemoveShapes;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripMenuItem ctxCopy;
        private System.Windows.Forms.ToolStripMenuItem ctxPaste;
        private System.Windows.Forms.ToolStripMenuItem ctxCut;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem toolStripMenuItem4;
    }
}
