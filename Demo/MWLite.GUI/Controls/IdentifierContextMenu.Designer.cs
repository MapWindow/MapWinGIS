namespace MWLite.GUI.Controls
{
    partial class IdentifierContextMenu
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
            this.ctxAllLayers = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxCurrentLayer = new System.Windows.Forms.ToolStripMenuItem();
            this.toolStripSeparator1 = new System.Windows.Forms.ToolStripSeparator();
            this.ctxHotTracking = new System.Windows.Forms.ToolStripMenuItem();
            this.ctxShowTooltip = new System.Windows.Forms.ToolStripMenuItem();
            this.contextMenuStrip1.SuspendLayout();
            this.SuspendLayout();
            // 
            // contextMenuStrip1
            // 
            this.contextMenuStrip1.Items.AddRange(new System.Windows.Forms.ToolStripItem[] {
            this.ctxAllLayers,
            this.ctxCurrentLayer,
            this.toolStripSeparator1,
            this.ctxHotTracking,
            this.ctxShowTooltip});
            this.contextMenuStrip1.Name = "contextMenuStrip1";
            this.contextMenuStrip1.Size = new System.Drawing.Size(153, 120);
            // 
            // ctxAllLayers
            // 
            this.ctxAllLayers.Name = "ctxAllLayers";
            this.ctxAllLayers.Size = new System.Drawing.Size(152, 22);
            this.ctxAllLayers.Text = "All Layers";
            this.ctxAllLayers.Click += new System.EventHandler(this.ctxAllLayers_Click);
            // 
            // ctxCurrentLayer
            // 
            this.ctxCurrentLayer.Name = "ctxCurrentLayer";
            this.ctxCurrentLayer.Size = new System.Drawing.Size(152, 22);
            this.ctxCurrentLayer.Text = "Current Layer";
            this.ctxCurrentLayer.Click += new System.EventHandler(this.ctxCurrentLayer_Click);
            // 
            // toolStripSeparator1
            // 
            this.toolStripSeparator1.Name = "toolStripSeparator1";
            this.toolStripSeparator1.Size = new System.Drawing.Size(149, 6);
            // 
            // ctxHotTracking
            // 
            this.ctxHotTracking.Name = "ctxHotTracking";
            this.ctxHotTracking.Size = new System.Drawing.Size(152, 22);
            this.ctxHotTracking.Text = "Hot Tracking";
            this.ctxHotTracking.Click += new System.EventHandler(this.ctxHotTracking_Click);
            // 
            // ctxShowTooltip
            // 
            this.ctxShowTooltip.Name = "ctxShowTooltip";
            this.ctxShowTooltip.Size = new System.Drawing.Size(152, 22);
            this.ctxShowTooltip.Text = "Show Tooltip";
            this.ctxShowTooltip.Click += new System.EventHandler(this.ctxShowTooltip_Click);
            // 
            // IdentifierContextMenu
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Name = "IdentifierContextMenu";
            this.contextMenuStrip1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.ContextMenuStrip contextMenuStrip1;
        private System.Windows.Forms.ToolStripMenuItem ctxAllLayers;
        private System.Windows.Forms.ToolStripMenuItem ctxCurrentLayer;
        private System.Windows.Forms.ToolStripSeparator toolStripSeparator1;
        private System.Windows.Forms.ToolStripMenuItem ctxHotTracking;
        private System.Windows.Forms.ToolStripMenuItem ctxShowTooltip;
    }
}
