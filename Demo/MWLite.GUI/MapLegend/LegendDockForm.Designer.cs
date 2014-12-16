using MWLite.Symbology.LegendControl;
namespace MWLite.GUI.MapLegend
{
    partial class LegendDockForm
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

        #region Windows Form Designer generated code

        /// <summary>
        /// Required method for Designer support - do not modify
        /// the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            this.components = new System.ComponentModel.Container();
            this.ctxZoomToLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxLabels = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxProperties = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1 = new System.Windows.Forms.ContextMenuStrip(this.components);
            this.toolStripSeparator4 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxSaveLayerStyle = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxLoadLayerStyle = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator2 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxPostGis = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxImportOgrLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxRemoveLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator3 = new System.Windows.Forms.ToolStripSeparator();
            this.legend1 = new Legend();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // ctxZoomToLayer
            // 
            this.ctxZoomToLayer.Name = "ctxZoomToLayer";
            this.ctxZoomToLayer.Size = new System.Drawing.Size(155, 22);
            this.ctxZoomToLayer.Text = "Zoom to layer";
            // 
            // ctxLabels
            // 
            this.ctxLabels.ImageAlign = System.Drawing.ContentAlignment.MiddleLeft;
            this.ctxLabels.Name = "ctxLabels";
            this.ctxLabels.Size = new System.Drawing.Size(155, 22);
            this.ctxLabels.Text = "Labels";
            // 
            // ctxProperties
            // 
            this.ctxProperties.Name = "ctxProperties";
            this.ctxProperties.Size = new System.Drawing.Size(155, 22);
            this.ctxProperties.Text = "Properties";
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxLabels,
            this.ctxZoomToLayer,
            this.toolStripSeparator4,
            this.ctxSaveLayerStyle,
            this.ctxLoadLayerStyle,
            this.toolStripSeparator2,
            this.ctxPostGis,
            this.toolStripSeparator1,
            this.ctxRemoveLayer,
            this.toolStripSeparator3,
            this.ctxProperties});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(156, 204);
            this.contextMenuStrip1.Opening += new System.ComponentModel.CancelEventHandler(this.contextMenuStrip1_Opening);
            // 
            // toolStripSeparator4
            // 
            this.toolStripSeparator4.Name = "toolStripSeparator4";
            this.toolStripSeparator4.Size = new System.Drawing.Size(152, 6);
            // 
            // ctxSaveLayerStyle
            // 
            this.ctxSaveLayerStyle.Name = "ctxSaveLayerStyle";
            this.ctxSaveLayerStyle.Size = new System.Drawing.Size(155, 22);
            this.ctxSaveLayerStyle.Text = "Save layer style";
            // 
            // ctxLoadLayerStyle
            // 
            this.ctxLoadLayerStyle.Name = "ctxLoadLayerStyle";
            this.ctxLoadLayerStyle.Size = new System.Drawing.Size(155, 22);
            this.ctxLoadLayerStyle.Text = "Load layer style";
            // 
            // toolStripSeparator2
            // 
            this.toolStripSeparator2.Name = "toolStripSeparator2";
            this.toolStripSeparator2.Size = new System.Drawing.Size(152, 6);
            // 
            // ctxPostGis
            // 
            this.ctxPostGis.DropDownItems.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxImportOgrLayer});
            this.ctxPostGis.Name = "ctxPostGis";
            this.ctxPostGis.Size = new System.Drawing.Size(155, 22);
            this.ctxPostGis.Text = "PostGIS";
            // 
            // ctxImportOgrLayer
            // 
            this.ctxImportOgrLayer.Name = "ctxImportOgrLayer";
            this.ctxImportOgrLayer.Size = new System.Drawing.Size(119, 22);
            this.ctxImportOgrLayer.Text = "Import...";
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(152, 6);
            // 
            // ctxRemoveLayer
            // 
            this.ctxRemoveLayer.Name = "ctxRemoveLayer";
            this.ctxRemoveLayer.Size = new System.Drawing.Size(155, 22);
            this.ctxRemoveLayer.Text = "Remove layer";
            // 
            // toolStripSeparator3
            // 
            this.toolStripSeparator3.Name = "toolStripSeparator3";
            this.toolStripSeparator3.Size = new System.Drawing.Size(152, 6);
            // 
            // legend1
            // 
            this.legend1.BackColor = System.Drawing.Color.White;
            this.legend1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.legend1.Location = new System.Drawing.Point(0, 0);
            this.legend1.Map = null;
            this.legend1.Name = "legend1";
            this.legend1.SelectedColor = System.Drawing.Color.FromArgb(((int)(((byte)(240)))), ((int)(((byte)(240)))), ((int)(((byte)(240)))));
            this.legend1.SelectedLayer = -1;
            this.legend1.ShowGroupFolders = true;
            this.legend1.ShowLabels = false;
            this.legend1.Size = new System.Drawing.Size(331, 358);
            this.legend1.TabIndex = 2;
            // 
            // LegendDockForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.BackColor = System.Drawing.Color.White;
            this.ClientSize = new System.Drawing.Size(331, 358);
            this.Controls.Add(this.legend1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.Name = "LegendDockForm";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.Text = "Legend";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ToolStripMenuItem ctxZoomToLayer;
        private System.Windows.Forms.ToolStripMenuItem ctxLabels;
        private System.Windows.Forms.ToolStripMenuItem ctxProperties;
        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private Legend legend1;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator2;
        private System.Windows.Forms.ToolStripMenuItem ctxPostGis;
        private System.Windows.Forms.ToolStripMenuItem ctxImportOgrLayer;
        private System.Windows.Forms.ToolStripMenuItem ctxSaveLayerStyle;
        private System.Windows.Forms.ToolStripMenuItem ctxRemoveLayer;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator3;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator4;
        private System.Windows.Forms.ToolStripMenuItem ctxLoadLayerStyle;

    }
}