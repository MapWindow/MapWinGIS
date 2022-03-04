namespace WinFormsApp1
{
    sealed partial class Form1
    {
        /// <summary>
        ///  Required designer variable.
        /// </summary>
        private System.ComponentModel.IContainer components = null;

        /// <summary>
        ///  Clean up any resources being used.
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
        ///  Required method for Designer support - do not modify
        ///  the contents of this method with the code editor.
        /// </summary>
        private void InitializeComponent()
        {
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.ClearMapButton = new System.Windows.Forms.Button();
            this.RunTestButton = new System.Windows.Forms.Button();
            this.TxtProgress = new System.Windows.Forms.TextBox();
            this.axMap1 = new AxMapWinGIS.AxMap();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Cursor = System.Windows.Forms.Cursors.VSplit;
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.ClearMapButton);
            this.splitContainer1.Panel1.Controls.Add(this.RunTestButton);
            this.splitContainer1.Panel1.Controls.Add(this.TxtProgress);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.axMap1);
            this.splitContainer1.Panel2MinSize = 250;
            this.splitContainer1.Size = new System.Drawing.Size(1048, 725);
            this.splitContainer1.SplitterDistance = 233;
            this.splitContainer1.TabIndex = 0;
            // 
            // button1
            // 
            this.ClearMapButton.Location = new System.Drawing.Point(12, 12);
            this.ClearMapButton.Name = "BtnClearMap";
            this.ClearMapButton.Size = new System.Drawing.Size(75, 23);
            this.ClearMapButton.TabIndex = 0;
            this.ClearMapButton.Text = "Clear map";
            this.ClearMapButton.UseVisualStyleBackColor = true;
            this.ClearMapButton.Click += new System.EventHandler(this.ClearMapButtonClick);
            // 
            // button1
            // 
            this.RunTestButton.Location = new System.Drawing.Point(12, 36);
            this.RunTestButton.Name = "BtnRunTest";
            this.RunTestButton.Size = new System.Drawing.Size(75, 23);
            this.RunTestButton.TabIndex = 0;
            this.RunTestButton.Text = "Run test";
            this.RunTestButton.UseVisualStyleBackColor = true;
            this.RunTestButton.Click += new System.EventHandler(this.RunTestButtonClick);
            // 
            // TxtProgress
            // 
            this.TxtProgress.Location = new System.Drawing.Point(3, 185);
            this.TxtProgress.Multiline = true;
            this.TxtProgress.Name = "TxtProgress";
            this.TxtProgress.Size = new System.Drawing.Size(227, 510);
            this.TxtProgress.TabIndex = 0;
            // 
            // axMap1
            // 
            this.axMap1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.axMap1.Location = new System.Drawing.Point(0, 0);
            this.axMap1.Name = "axMap1";
            //this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
            this.axMap1.Padding = new System.Windows.Forms.Padding(3);
            this.axMap1.Size = new System.Drawing.Size(811, 725);
            this.axMap1.FileDropped += new AxMapWinGIS._DMapEvents_FileDroppedEventHandler(this.AxMap1_FileDropped);
            this.axMap1.TabIndex = 0;
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(7F, 15F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(1048, 725);
            this.Controls.Add(this.splitContainer1);
            this.MinimumSize = new System.Drawing.Size(823, 737);
            this.Name = "Form1";
            this.Text = "MapWindow using MapWinGIS";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel1.PerformLayout();
            this.splitContainer1.Panel2.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private SplitContainer splitContainer1;
        private TextBox TxtProgress;
        private AxMapWinGIS.AxMap axMap1;
        private Button ClearMapButton;
        private Button RunTestButton;
    }
}