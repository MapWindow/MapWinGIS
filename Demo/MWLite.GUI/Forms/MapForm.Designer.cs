namespace MWLite.GUI.Forms
{
    partial class MapForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(MapForm));
            this.axMap1 = new AxMapWinGIS.AxMap();
            this.panel1 = new System.Windows.Forms.Panel();
            this.lblProjection = new System.Windows.Forms.Label();
            this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
            this.panel1.SuspendLayout();
            this.SuspendLayout();
            // 
            // axMap1
            // 
            this.axMap1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axMap1.Enabled = true;
            this.axMap1.Location = new System.Drawing.Point(0, 0);
            this.axMap1.Name = "axMap1";
            this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
            this.axMap1.Size = new System.Drawing.Size(402, 264);
            this.axMap1.TabIndex = 3;
            // 
            // panel1
            // 
            this.panel1.Controls.Add(this.lblProjection);
            this.panel1.Dock = System.Windows.Forms.DockStyle.Bottom;
            this.panel1.Location = new System.Drawing.Point(0, 264);
            this.panel1.Name = "panel1";
            this.panel1.Size = new System.Drawing.Size(402, 22);
            this.panel1.TabIndex = 4;
            // 
            // lblProjection
            // 
            this.lblProjection.Dock = System.Windows.Forms.DockStyle.Fill;
            this.lblProjection.Location = new System.Drawing.Point(0, 0);
            this.lblProjection.Name = "lblProjection";
            this.lblProjection.Size = new System.Drawing.Size(402, 22);
            this.lblProjection.TabIndex = 0;
            this.lblProjection.Text = "No projection";
            this.lblProjection.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
            // 
            // toolTip1
            // 
            this.toolTip1.AutomaticDelay = 300;
            this.toolTip1.ShowAlways = true;
            // 
            // MapForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(402, 286);
            this.Controls.Add(this.axMap1);
            this.Controls.Add(this.panel1);
            this.Font = new System.Drawing.Font("Microsoft Sans Serif", 8.25F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.None;
            this.Name = "MapForm";
            this.ShowIcon = false;
            this.Text = "Map";
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
            this.panel1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private AxMapWinGIS.AxMap axMap1;
        private System.Windows.Forms.Panel panel1;
        private System.Windows.Forms.Label lblProjection;
        private System.Windows.Forms.ToolTip toolTip1;


    }
}