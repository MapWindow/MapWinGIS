namespace MWLite.ShapeEditor.Forms
{
    partial class EditorMenu
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
            this.menuStrip1 = new System.Windows.Forms.MenuStrip();
            this.mnuShapeEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEditorVertices = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEditorAutoSnapping = new System.Windows.Forms.ToolStripMenuItem();
            this.mnuEditorHighlightShapes = new System.Windows.Forms.ToolStripMenuItem();
            this.menuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // menuStrip1
            // 
            this.menuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuShapeEditor});
            this.menuStrip1.Location = new System.Drawing.Point(0, 0);
            this.menuStrip1.Name = "menuStrip1";
            this.menuStrip1.Size = new System.Drawing.Size(228, 24);
            this.menuStrip1.TabIndex = 0;
            this.menuStrip1.Text = "menuStrip1";
            // 
            // mnuShapeEditor
            // 
            this.mnuShapeEditor.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.mnuEditorVertices,
            this.mnuEditorAutoSnapping,
            this.mnuEditorHighlightShapes});
            this.mnuShapeEditor.Name = "mnuShapeEditor";
            this.mnuShapeEditor.Size = new System.Drawing.Size(85, 20);
            this.mnuShapeEditor.Text = "Shape Editor";
            // 
            // mnuEditorVertices
            // 
            this.mnuEditorVertices.Name = "mnuEditorVertices";
            this.mnuEditorVertices.Size = new System.Drawing.Size(164, 22);
            this.mnuEditorVertices.Text = "Vertices Visible";
            // 
            // mnuEditorAutoSnapping
            // 
            this.mnuEditorAutoSnapping.Name = "mnuEditorAutoSnapping";
            this.mnuEditorAutoSnapping.Size = new System.Drawing.Size(164, 22);
            this.mnuEditorAutoSnapping.Text = "Auto Snapping";
            // 
            // mnuEditorHighlightShapes
            // 
            this.mnuEditorHighlightShapes.Name = "mnuEditorHighlightShapes";
            this.mnuEditorHighlightShapes.Size = new System.Drawing.Size(164, 22);
            this.mnuEditorHighlightShapes.Text = "Highlight Shapes";
            // 
            // EditorMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.menuStrip1);
            this.Name = "EditorMenu";
            this.Size = new System.Drawing.Size(228, 150);
            this.menuStrip1.ResumeLayout(false);
            this.menuStrip1.PerformLayout();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.MenuStrip menuStrip1;
        private System.Windows.Forms.ToolStripMenuItem mnuShapeEditor;
        private System.Windows.Forms.ToolStripMenuItem mnuEditorVertices;
        private System.Windows.Forms.ToolStripMenuItem mnuEditorAutoSnapping;
        private System.Windows.Forms.ToolStripMenuItem mnuEditorHighlightShapes;
    }
}
