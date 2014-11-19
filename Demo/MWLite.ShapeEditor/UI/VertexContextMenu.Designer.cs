namespace MWLite.ShapeEditor.UI
{
    partial class VertexContextMenu
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
            this.ctxVertexEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxPartEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxAddPart = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxRemovePart = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSaveShape = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxClearEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxVertexEditor,
            this.ctxPartEditor,
            this.toolStripSeparator1,
            this.ctxAddPart,
            this.ctxRemovePart,
            this.toolStripSeparator2,
            this.ctxSaveShape,
            this.ctxClearEditor});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(181, 170);
            // 
            // ctxVertexEditor
            // 
            this.ctxVertexEditor.Name = "ctxVertexEditor";
            this.ctxVertexEditor.Size = new System.Drawing.Size(180, 22);
            this.ctxVertexEditor.Text = "Vertex editor";
            // 
            // ctxPartEditor
            // 
            this.ctxPartEditor.Name = "ctxPartEditor";
            this.ctxPartEditor.Size = new System.Drawing.Size(180, 22);
            this.ctxPartEditor.Text = "Part editor";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(177, 6);
            // 
            // ctxAddPart
            // 
            this.ctxAddPart.Name = "ctxAddPart";
            this.ctxAddPart.Size = new System.Drawing.Size(180, 22);
            this.ctxAddPart.Text = "Add part";
            // 
            // ctxRemovePart
            // 
            this.ctxRemovePart.Name = "ctxRemovePart";
            this.ctxRemovePart.Size = new System.Drawing.Size(180, 22);
            this.ctxRemovePart.Text = "Remove by polygon";
            // 
            // ctxSaveShape
            // 
            this.ctxSaveShape.Name = "ctxSaveShape";
            this.ctxSaveShape.Size = new System.Drawing.Size(180, 22);
            this.ctxSaveShape.Text = "Save changes";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(177, 6);
            // 
            // ctxClearEditor
            // 
            this.ctxClearEditor.Name = "ctxClearEditor";
            this.ctxClearEditor.Size = new System.Drawing.Size(180, 22);
            this.ctxClearEditor.Text = "Discard changes";
            // 
            // VertexContextMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "VertexContextMenu";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem ctxVertexEditor;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem ctxAddPart;
        private System.Windows.Forms.ToolStripMenuItem ctxRemovePart;
        private System.Windows.Forms.ToolStripMenuItem ctxPartEditor;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxSaveShape;
        private System.Windows.Forms.ToolStripMenuItem ctxClearEditor;
    }
}
