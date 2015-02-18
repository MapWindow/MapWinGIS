namespace TestGrids
{
    partial class TestForm
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TestForm));
            this.axMap1 = new AxMapWinGIS.AxMap();
            this.button1 = new System.Windows.Forms.Button();
            this.btnAnalyzeFiles = new System.Windows.Forms.Button();
            this.btnTestOpen = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
            this.SuspendLayout();
            // 
            // axMap1
            // 
            this.axMap1.Enabled = true;
            this.axMap1.Location = new System.Drawing.Point(12, 12);
            this.axMap1.Name = "axMap1";
            this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
            this.axMap1.Size = new System.Drawing.Size(361, 267);
            this.axMap1.TabIndex = 0;
            // 
            // button1
            // 
            this.button1.Location = new System.Drawing.Point(386, 21);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(97, 30);
            this.button1.TabIndex = 1;
            this.button1.Text = "Test .img";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // btnAnalyzeFiles
            // 
            this.btnAnalyzeFiles.Location = new System.Drawing.Point(386, 57);
            this.btnAnalyzeFiles.Name = "btnAnalyzeFiles";
            this.btnAnalyzeFiles.Size = new System.Drawing.Size(97, 30);
            this.btnAnalyzeFiles.TabIndex = 2;
            this.btnAnalyzeFiles.Text = "File manager";
            this.btnAnalyzeFiles.UseVisualStyleBackColor = true;
            // 
            // btnTestOpen
            // 
            this.btnTestOpen.Location = new System.Drawing.Point(386, 93);
            this.btnTestOpen.Name = "btnTestOpen";
            this.btnTestOpen.Size = new System.Drawing.Size(97, 30);
            this.btnTestOpen.TabIndex = 3;
            this.btnTestOpen.Text = "Test grid open";
            this.btnTestOpen.UseVisualStyleBackColor = true;
            // 
            // TestForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(488, 294);
            this.Controls.Add(this.btnTestOpen);
            this.Controls.Add(this.btnAnalyzeFiles);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.axMap1);
            this.Name = "TestForm";
            this.Text = "TestForm";
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
            this.ResumeLayout(false);

        }

        #endregion

        private AxMapWinGIS.AxMap axMap1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button btnAnalyzeFiles;
        private System.Windows.Forms.Button btnTestOpen;
    }
}