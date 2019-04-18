namespace interopCreator
{
    partial class Form1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
            this.button1 = new System.Windows.Forms.Button();
            this.button2 = new System.Windows.Forms.Button();
            this.button3 = new System.Windows.Forms.Button();
            this.btn_ogrinfo = new System.Windows.Forms.Button();
            this.txtResults = new System.Windows.Forms.TextBox();
            this.btnOgr2Ogr = new System.Windows.Forms.Button();
            this.btnGdalInfo = new System.Windows.Forms.Button();
            this.btnGdalTranslate = new System.Windows.Forms.Button();
            this.button4 = new System.Windows.Forms.Button();
            this.button5 = new System.Windows.Forms.Button();
            this.button6 = new System.Windows.Forms.Button();
            this.button7 = new System.Windows.Forms.Button();
            this.button8 = new System.Windows.Forms.Button();
            this.button9 = new System.Windows.Forms.Button();
            this.button10 = new System.Windows.Forms.Button();
            this.button11 = new System.Windows.Forms.Button();
            this.axMap1 = new AxMapWinGIS.AxMap();
            this.button12 = new System.Windows.Forms.Button();
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
            this.SuspendLayout();
            // 
            // button1
            // 
            this.button1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button1.Location = new System.Drawing.Point(686, 12);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(136, 23);
            this.button1.TabIndex = 1;
            this.button1.Text = "ClipGridWithPolygon";
            this.button1.UseVisualStyleBackColor = true;
            this.button1.Click += new System.EventHandler(this.button1_Click);
            // 
            // button2
            // 
            this.button2.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button2.Location = new System.Drawing.Point(686, 38);
            this.button2.Name = "button2";
            this.button2.Size = new System.Drawing.Size(136, 23);
            this.button2.TabIndex = 2;
            this.button2.Text = "Grid Statistics";
            this.button2.UseVisualStyleBackColor = true;
            this.button2.Click += new System.EventHandler(this.button2_Click);
            // 
            // button3
            // 
            this.button3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button3.Location = new System.Drawing.Point(686, 64);
            this.button3.Name = "button3";
            this.button3.Size = new System.Drawing.Size(136, 23);
            this.button3.TabIndex = 3;
            this.button3.Text = "Label.Serialize()";
            this.button3.UseVisualStyleBackColor = true;
            this.button3.Click += new System.EventHandler(this.button3_Click);
            // 
            // btn_ogrinfo
            // 
            this.btn_ogrinfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btn_ogrinfo.Location = new System.Drawing.Point(686, 90);
            this.btn_ogrinfo.Name = "btn_ogrinfo";
            this.btn_ogrinfo.Size = new System.Drawing.Size(136, 23);
            this.btn_ogrinfo.TabIndex = 4;
            this.btn_ogrinfo.Text = "OgrInfo";
            this.btn_ogrinfo.UseVisualStyleBackColor = true;
            this.btn_ogrinfo.Click += new System.EventHandler(this.btn_ogrinfo_Click);
            // 
            // txtResults
            // 
            this.txtResults.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Right)));
            this.txtResults.Location = new System.Drawing.Point(686, 431);
            this.txtResults.Multiline = true;
            this.txtResults.Name = "txtResults";
            this.txtResults.Size = new System.Drawing.Size(136, 264);
            this.txtResults.TabIndex = 5;
            this.txtResults.TextChanged += new System.EventHandler(this.txtResults_TextChanged);
            // 
            // btnOgr2Ogr
            // 
            this.btnOgr2Ogr.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnOgr2Ogr.Location = new System.Drawing.Point(686, 116);
            this.btnOgr2Ogr.Name = "btnOgr2Ogr";
            this.btnOgr2Ogr.Size = new System.Drawing.Size(136, 23);
            this.btnOgr2Ogr.TabIndex = 6;
            this.btnOgr2Ogr.Text = "OGR2OGR";
            this.btnOgr2Ogr.UseVisualStyleBackColor = true;
            this.btnOgr2Ogr.Click += new System.EventHandler(this.btnOgr2Ogr_Click);
            // 
            // btnGdalInfo
            // 
            this.btnGdalInfo.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnGdalInfo.Location = new System.Drawing.Point(686, 142);
            this.btnGdalInfo.Name = "btnGdalInfo";
            this.btnGdalInfo.Size = new System.Drawing.Size(136, 23);
            this.btnGdalInfo.TabIndex = 7;
            this.btnGdalInfo.Text = "GdalInfo";
            this.btnGdalInfo.UseVisualStyleBackColor = true;
            this.btnGdalInfo.Click += new System.EventHandler(this.btnGdalInfo_Click);
            // 
            // btnGdalTranslate
            // 
            this.btnGdalTranslate.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnGdalTranslate.Location = new System.Drawing.Point(686, 168);
            this.btnGdalTranslate.Name = "btnGdalTranslate";
            this.btnGdalTranslate.Size = new System.Drawing.Size(136, 23);
            this.btnGdalTranslate.TabIndex = 8;
            this.btnGdalTranslate.Text = "GdalTranslate";
            this.btnGdalTranslate.UseVisualStyleBackColor = true;
            // 
            // button4
            // 
            this.button4.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button4.Location = new System.Drawing.Point(686, 194);
            this.button4.Name = "button4";
            this.button4.Size = new System.Drawing.Size(136, 23);
            this.button4.TabIndex = 9;
            this.button4.Text = "Encoding";
            this.button4.UseVisualStyleBackColor = true;
            this.button4.Click += new System.EventHandler(this.button4_Click);
            // 
            // button5
            // 
            this.button5.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button5.Location = new System.Drawing.Point(686, 220);
            this.button5.Name = "button5";
            this.button5.Size = new System.Drawing.Size(136, 23);
            this.button5.TabIndex = 10;
            this.button5.Text = "HDF5";
            this.button5.UseVisualStyleBackColor = true;
            this.button5.Click += new System.EventHandler(this.button5_Click);
            // 
            // button6
            // 
            this.button6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button6.Location = new System.Drawing.Point(686, 246);
            this.button6.Name = "button6";
            this.button6.Size = new System.Drawing.Size(136, 23);
            this.button6.TabIndex = 11;
            this.button6.Text = "Open multiple files";
            this.button6.UseVisualStyleBackColor = true;
            this.button6.Click += new System.EventHandler(this.button6_Click);
            // 
            // button7
            // 
            this.button7.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button7.Location = new System.Drawing.Point(686, 272);
            this.button7.Name = "button7";
            this.button7.Size = new System.Drawing.Size(136, 23);
            this.button7.TabIndex = 12;
            this.button7.Text = "Open grayscale tiff";
            this.button7.UseVisualStyleBackColor = true;
            this.button7.Click += new System.EventHandler(this.button7_Click);
            // 
            // button8
            // 
            this.button8.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button8.Location = new System.Drawing.Point(686, 298);
            this.button8.Name = "button8";
            this.button8.Size = new System.Drawing.Size(136, 23);
            this.button8.TabIndex = 13;
            this.button8.Text = "Zonal statistics";
            this.button8.UseVisualStyleBackColor = true;
            this.button8.Click += new System.EventHandler(this.button8_Click);
            // 
            // button9
            // 
            this.button9.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button9.Location = new System.Drawing.Point(686, 324);
            this.button9.Name = "button9";
            this.button9.Size = new System.Drawing.Size(136, 23);
            this.button9.TabIndex = 14;
            this.button9.Text = "DrawLabel";
            this.button9.UseVisualStyleBackColor = true;
            this.button9.Click += new System.EventHandler(this.button9_Click);
            // 
            // button10
            // 
            this.button10.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button10.Location = new System.Drawing.Point(686, 350);
            this.button10.Name = "button10";
            this.button10.Size = new System.Drawing.Size(136, 23);
            this.button10.TabIndex = 15;
            this.button10.Text = "Spatial index";
            this.button10.UseVisualStyleBackColor = true;
            this.button10.Click += new System.EventHandler(this.button10_Click);
            // 
            // button11
            // 
            this.button11.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button11.Location = new System.Drawing.Point(686, 376);
            this.button11.Name = "button11";
            this.button11.Size = new System.Drawing.Size(136, 23);
            this.button11.TabIndex = 16;
            this.button11.Text = "Prefetch Tiles";
            this.button11.UseVisualStyleBackColor = true;
            this.button11.Click += new System.EventHandler(this.button11_Click);
            // 
            // axMap1
            // 
            this.axMap1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.axMap1.Enabled = true;
            this.axMap1.Location = new System.Drawing.Point(2, 2);
            this.axMap1.Name = "axMap1";
            this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
            this.axMap1.Size = new System.Drawing.Size(677, 694);
            this.axMap1.TabIndex = 0;
            this.axMap1.FileDropped += new AxMapWinGIS._DMapEvents_FileDroppedEventHandler(this.axMap1_FileDropped);
            this.axMap1.ShapeIdentified += new AxMapWinGIS._DMapEvents_ShapeIdentifiedEventHandler(this.axMap1_ShapeIdentified);
            // 
            // button12
            // 
            this.button12.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.button12.Location = new System.Drawing.Point(686, 402);
            this.button12.Name = "button12";
            this.button12.Size = new System.Drawing.Size(136, 23);
            this.button12.TabIndex = 17;
            this.button12.Text = "Zoom in";
            this.button12.UseVisualStyleBackColor = true;
            this.button12.Click += new System.EventHandler(this.button12_Click);
            // 
            // Form1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(831, 699);
            this.Controls.Add(this.button12);
            this.Controls.Add(this.button11);
            this.Controls.Add(this.button10);
            this.Controls.Add(this.button9);
            this.Controls.Add(this.button8);
            this.Controls.Add(this.button7);
            this.Controls.Add(this.button6);
            this.Controls.Add(this.button5);
            this.Controls.Add(this.button4);
            this.Controls.Add(this.btnGdalTranslate);
            this.Controls.Add(this.btnGdalInfo);
            this.Controls.Add(this.btnOgr2Ogr);
            this.Controls.Add(this.txtResults);
            this.Controls.Add(this.btn_ogrinfo);
            this.Controls.Add(this.button3);
            this.Controls.Add(this.button2);
            this.Controls.Add(this.button1);
            this.Controls.Add(this.axMap1);
            this.Name = "Form1";
            this.Text = "Form1";
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private AxMapWinGIS.AxMap axMap1;
        private System.Windows.Forms.Button button1;
        private System.Windows.Forms.Button button2;
        private System.Windows.Forms.Button button3;
        private System.Windows.Forms.Button btn_ogrinfo;
        private System.Windows.Forms.TextBox txtResults;
        private System.Windows.Forms.Button btnOgr2Ogr;
        private System.Windows.Forms.Button btnGdalInfo;
        private System.Windows.Forms.Button btnGdalTranslate;
        private System.Windows.Forms.Button button4;
        private System.Windows.Forms.Button button5;
        private System.Windows.Forms.Button button6;
        private System.Windows.Forms.Button button7;
        private System.Windows.Forms.Button button8;
        private System.Windows.Forms.Button button9;
        private System.Windows.Forms.Button button10;
        private System.Windows.Forms.Button button11;
        private System.Windows.Forms.Button button12;
    }
}

