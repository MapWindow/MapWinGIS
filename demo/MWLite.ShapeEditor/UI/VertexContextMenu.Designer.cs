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
            this.ctxUndo = new System.Windows.Forms.ToolStripMenuItem();
            this.digitizerSeparator = new System.Windows.Forms.ToolStripSeparator();
            this.ctxVertexEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxPartEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.editorSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxAddPart = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxRemovePart = new System.Windows.Forms.ToolStripMenuItem();
            this.editorSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxSnapping = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSnappingNone = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSnappingCurrent = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxSnappingAll = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlighting = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlightingNone = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlightingCurrent = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxHighlightingAll = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxSaveShape = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxClearEditor = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxUndo,
            this.digitizerSeparator,
            this.ctxVertexEditor,
            this.ctxPartEditor,
            this.editorSeparator1,
            this.ctxAddPart,
            this.ctxRemovePart,
            this.editorSeparator2,
            this.ctxSnapping,
            this.ctxHighlighting,
            this.toolStripSeparator4,
            this.ctxSaveShape,
            this.ctxClearEditor});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(181, 248);
            // 
            // ctxUndo
            // 
            this.ctxUndo.Name = "ctxUndo";
            this.ctxUndo.ShortcutKeys = ((System.Windows.Forms.Keys)((System.Windows.Forms.Keys.Control | System.Windows.Forms.Keys.Z)));
            this.ctxUndo.Size = new System.Drawing.Size(180, 22);
            this.ctxUndo.Text = "Undo point";
            // 
            // digitizerSeparator
            // 
            this.digitizerSeparator.Name = "digitizerSeparator";
            this.digitizerSeparator.Size = new System.Drawing.Size(177, 6);
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
            // editorSeparator1
            // 
            this.editorSeparator1.Name = "editorSeparator1";
            this.editorSeparator1.Size = new System.Drawing.Size(177, 6);
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
            // editorSeparator2
            // 
            this.editorSeparator2.Name = "editorSeparator2";
            this.editorSeparator2.Size = new System.Drawing.Size(177, 6);
            // 
            // ctxSnapping
            // 
            this.ctxSnapping.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxSnappingNone,
            this.ctxSnappingCurrent,
            this.ctxSnappingAll});
            this.ctxSnapping.Name = "ctxSnapping";
            this.ctxSnapping.Size = new System.Drawing.Size(180, 22);
            this.ctxSnapping.Text = "Snapping";
            // 
            // ctxSnappingNone
            // 
            this.ctxSnappingNone.Name = "ctxSnappingNone";
            this.ctxSnappingNone.Size = new System.Drawing.Size(142, 22);
            this.ctxSnappingNone.Text = "No layers";
            // 
            // ctxSnappingCurrent
            // 
            this.ctxSnappingCurrent.Name = "ctxSnappingCurrent";
            this.ctxSnappingCurrent.Size = new System.Drawing.Size(142, 22);
            this.ctxSnappingCurrent.Text = "Current layer";
            // 
            // ctxSnappingAll
            // 
            this.ctxSnappingAll.Name = "ctxSnappingAll";
            this.ctxSnappingAll.Size = new System.Drawing.Size(142, 22);
            this.ctxSnappingAll.Text = "All layers";
            // 
            // ctxHighlighting
            // 
            this.ctxHighlighting.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxHighlightingNone,
            this.ctxHighlightingCurrent,
            this.ctxHighlightingAll});
            this.ctxHighlighting.Name = "ctxHighlighting";
            this.ctxHighlighting.Size = new System.Drawing.Size(180, 22);
            this.ctxHighlighting.Text = "Highlighting";
            // 
            // ctxHighlightingNone
            // 
            this.ctxHighlightingNone.Name = "ctxHighlightingNone";
            this.ctxHighlightingNone.Size = new System.Drawing.Size(142, 22);
            this.ctxHighlightingNone.Text = "No layers";
            // 
            // ctxHighlightingCurrent
            // 
            this.ctxHighlightingCurrent.Name = "ctxHighlightingCurrent";
            this.ctxHighlightingCurrent.Size = new System.Drawing.Size(142, 22);
            this.ctxHighlightingCurrent.Text = "Current layer";
            // 
            // ctxHighlightingAll
            // 
            this.ctxHighlightingAll.Name = "ctxHighlightingAll";
            this.ctxHighlightingAll.Size = new System.Drawing.Size(142, 22);
            this.ctxHighlightingAll.Text = "All layers";
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(177, 6);
            // 
            // ctxSaveShape
            // 
            this.ctxSaveShape.Name = "ctxSaveShape";
            this.ctxSaveShape.Size = new System.Drawing.Size(180, 22);
            this.ctxSaveShape.Text = "Save changes";
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
        private System.Windows.Forms.ToolStripSeparator editorSeparator1;
        private System.Windows.Forms.ToolStripMenuItem ctxAddPart;
        private System.Windows.Forms.ToolStripMenuItem ctxRemovePart;
        private System.Windows.Forms.ToolStripMenuItem ctxPartEditor;
        private System.Windows.Forms.ToolStripSeparator editorSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxSaveShape;
        private System.Windows.Forms.ToolStripMenuItem ctxClearEditor;
        private System.Windows.Forms.ToolStripMenuItem ctxUndo;
        private System.Windows.Forms.ToolStripSeparator digitizerSeparator;
        private System.Windows.Forms.ToolStripMenuItem ctxSnapping;
        private System.Windows.Forms.ToolStripMenuItem ctxSnappingNone;
        private System.Windows.Forms.ToolStripMenuItem ctxSnappingCurrent;
        private System.Windows.Forms.ToolStripMenuItem ctxSnappingAll;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlighting;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlightingNone;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlightingCurrent;
        private System.Windows.Forms.ToolStripMenuItem ctxHighlightingAll;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
    }
}
