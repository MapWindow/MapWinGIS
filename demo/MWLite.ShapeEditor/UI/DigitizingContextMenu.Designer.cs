namespace MWLite.ShapeEditor.UI
{
    partial class DigitizingContextMenu
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
            this.ctxNoSnapping = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSnapCurrentLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSnapAllLayers = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxNoHighlighting = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlightCurrentLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlightAllLayers = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxFinishShape = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxCancelShape = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxUndoPoint = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxUndoPoint,
            this.toolStripSeparator2,
            this.ctxNoSnapping,
            this.ctxSnapCurrentLayer,
            this.ctxSnapAllLayers,
            this.toolStripSeparator1,
            this.ctxNoHighlighting,
            this.ctxHighlightCurrentLayer,
            this.ctxHighlightAllLayers,
            this.toolStripSeparator3,
            this.ctxFinishShape,
            this.ctxCancelShape});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(176, 220);
            // 
            // ctxNoSnapping
            // 
            this.ctxNoSnapping.Name = "ctxNoSnapping";
            this.ctxNoSnapping.Size = new System.Drawing.Size(158, 22);
            this.ctxNoSnapping.Text = "No snapping";
            // 
            // ctxSnapCurrentLayer
            // 
            this.ctxSnapCurrentLayer.Name = "ctxSnapCurrentLayer";
            this.ctxSnapCurrentLayer.Size = new System.Drawing.Size(158, 22);
            this.ctxSnapCurrentLayer.Text = "Current layer";
            // 
            // ctxSnapAllLayers
            // 
            this.ctxSnapAllLayers.Name = "ctxSnapAllLayers";
            this.ctxSnapAllLayers.Size = new System.Drawing.Size(158, 22);
            this.ctxSnapAllLayers.Text = "All layers";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(155, 6);
            // 
            // ctxNoHighlighting
            // 
            this.ctxNoHighlighting.Name = "ctxNoHighlighting";
            this.ctxNoHighlighting.Size = new System.Drawing.Size(158, 22);
            this.ctxNoHighlighting.Text = "No highlighting";
            // 
            // ctxHighlightCurrentLayer
            // 
            this.ctxHighlightCurrentLayer.Name = "ctxHighlightCurrentLayer";
            this.ctxHighlightCurrentLayer.Size = new System.Drawing.Size(158, 22);
            this.ctxHighlightCurrentLayer.Text = "Current layer";
            // 
            // ctxHighlightAllLayers
            // 
            this.ctxHighlightAllLayers.Name = "ctxHighlightAllLayers";
            this.ctxHighlightAllLayers.Size = new System.Drawing.Size(158, 22);
            this.ctxHighlightAllLayers.Text = "All layers";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(155, 6);
            // 
            // ctxFinishShape
            // 
            this.ctxFinishShape.Name = "ctxFinishShape";
            this.ctxFinishShape.Size = new System.Drawing.Size(158, 22);
            this.ctxFinishShape.Text = "Finish shape";
            // 
            // ctxCancelShape
            // 
            this.ctxCancelShape.Name = "ctxCancelShape";
            this.ctxCancelShape.Size = new System.Drawing.Size(158, 22);
            this.ctxCancelShape.Text = "Cancel shape";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(155, 6);
            // 
            // ctxUndoPoint
            // 
            this.ctxUndoPoint.Name = "ctxUndoPoint";
            this.ctxUndoPoint.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.ctxUndoPoint.Size = new System.Drawing.Size(175, 22);
            this.ctxUndoPoint.Text = "Undo point";
            // 
            // DigitizingContextMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "DigitizingContextMenu";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem ctxNoSnapping;
        private System.Windows.Forms.ToolStripMenuItem ctxSnapCurrentLayer;
        private System.Windows.Forms.ToolStripMenuItem ctxSnapAllLayers;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem ctxNoHighlighting;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlightCurrentLayer;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlightAllLayers;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxFinishShape;
        private System.Windows.Forms.ToolStripMenuItem ctxCancelShape;
        private System.Windows.Forms.ToolStripMenuItem ctxUndoPoint;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
    }
}
