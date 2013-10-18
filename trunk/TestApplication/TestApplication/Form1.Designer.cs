namespace TestApplication
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
      this.components = new System.ComponentModel.Container();
      System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(Form1));
      this.splitContainer1 = new System.Windows.Forms.SplitContainer();
      this.splitContainer2 = new System.Windows.Forms.SplitContainer();
      this.groupBox1 = new System.Windows.Forms.GroupBox();
      this.groupBox4 = new System.Windows.Forms.GroupBox();
      this.RunImageTest = new System.Windows.Forms.Button();
      this.SelectImageInputfile = new System.Windows.Forms.Button();
      this.ImageInputfile = new System.Windows.Forms.TextBox();
      this.label4 = new System.Windows.Forms.Label();
      this.groupBox3 = new System.Windows.Forms.GroupBox();
      this.RunGridTest = new System.Windows.Forms.Button();
      this.SelectGridInputfile = new System.Windows.Forms.Button();
      this.GridInputfile = new System.Windows.Forms.TextBox();
      this.label3 = new System.Windows.Forms.Label();
      this.groupBox2 = new System.Windows.Forms.GroupBox();
      this.RunShapefileTest = new System.Windows.Forms.Button();
      this.SelectShapefileInputfile = new System.Windows.Forms.Button();
      this.ShapefileInputfile = new System.Windows.Forms.TextBox();
      this.label2 = new System.Windows.Forms.Label();
      this.label1 = new System.Windows.Forms.Label();
      this.axMap1 = new AxMapWinGIS.AxMap();
      this.Progressbox = new System.Windows.Forms.TextBox();
      this.toolTip1 = new System.Windows.Forms.ToolTip(this.components);
      this.splitContainer1.Panel1.SuspendLayout();
      this.splitContainer1.Panel2.SuspendLayout();
      this.splitContainer1.SuspendLayout();
      this.splitContainer2.Panel1.SuspendLayout();
      this.splitContainer2.Panel2.SuspendLayout();
      this.splitContainer2.SuspendLayout();
      this.groupBox1.SuspendLayout();
      this.groupBox4.SuspendLayout();
      this.groupBox3.SuspendLayout();
      this.groupBox2.SuspendLayout();
      ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
      this.SuspendLayout();
      // 
      // splitContainer1
      // 
      this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer1.Location = new System.Drawing.Point(0, 0);
      this.splitContainer1.Name = "splitContainer1";
      this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
      // 
      // splitContainer1.Panel1
      // 
      this.splitContainer1.Panel1.Controls.Add(this.splitContainer2);
      // 
      // splitContainer1.Panel2
      // 
      this.splitContainer1.Panel2.Controls.Add(this.Progressbox);
      this.splitContainer1.Size = new System.Drawing.Size(667, 482);
      this.splitContainer1.SplitterDistance = 394;
      this.splitContainer1.TabIndex = 0;
      // 
      // splitContainer2
      // 
      this.splitContainer2.Dock = System.Windows.Forms.DockStyle.Fill;
      this.splitContainer2.FixedPanel = System.Windows.Forms.FixedPanel.Panel1;
      this.splitContainer2.Location = new System.Drawing.Point(0, 0);
      this.splitContainer2.Name = "splitContainer2";
      // 
      // splitContainer2.Panel1
      // 
      this.splitContainer2.Panel1.Controls.Add(this.groupBox1);
      this.splitContainer2.Panel1.Controls.Add(this.label1);
      // 
      // splitContainer2.Panel2
      // 
      this.splitContainer2.Panel2.Controls.Add(this.axMap1);
      this.splitContainer2.Size = new System.Drawing.Size(667, 394);
      this.splitContainer2.SplitterDistance = 211;
      this.splitContainer2.TabIndex = 0;
      // 
      // groupBox1
      // 
      this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left)
                  | System.Windows.Forms.AnchorStyles.Right)));
      this.groupBox1.Controls.Add(this.groupBox4);
      this.groupBox1.Controls.Add(this.groupBox3);
      this.groupBox1.Controls.Add(this.groupBox2);
      this.groupBox1.Location = new System.Drawing.Point(3, 28);
      this.groupBox1.Name = "groupBox1";
      this.groupBox1.Size = new System.Drawing.Size(205, 306);
      this.groupBox1.TabIndex = 1;
      this.groupBox1.TabStop = false;
      this.groupBox1.Text = "File formats";
      // 
      // groupBox4
      // 
      this.groupBox4.Controls.Add(this.RunImageTest);
      this.groupBox4.Controls.Add(this.SelectImageInputfile);
      this.groupBox4.Controls.Add(this.ImageInputfile);
      this.groupBox4.Controls.Add(this.label4);
      this.groupBox4.Location = new System.Drawing.Point(6, 209);
      this.groupBox4.Name = "groupBox4";
      this.groupBox4.Size = new System.Drawing.Size(188, 89);
      this.groupBox4.TabIndex = 2;
      this.groupBox4.TabStop = false;
      this.groupBox4.Text = "Image files";
      // 
      // RunImageTest
      // 
      this.RunImageTest.Location = new System.Drawing.Point(9, 58);
      this.RunImageTest.Name = "RunImageTest";
      this.RunImageTest.Size = new System.Drawing.Size(173, 23);
      this.RunImageTest.TabIndex = 3;
      this.RunImageTest.Text = "Run test";
      this.RunImageTest.UseVisualStyleBackColor = true;
      this.RunImageTest.Click += new System.EventHandler(this.RunImageTestClick);
      // 
      // SelectImageInputfile
      // 
      this.SelectImageInputfile.Location = new System.Drawing.Point(155, 32);
      this.SelectImageInputfile.Name = "SelectImageInputfile";
      this.SelectImageInputfile.Size = new System.Drawing.Size(27, 20);
      this.SelectImageInputfile.TabIndex = 2;
      this.SelectImageInputfile.Text = "...";
      this.SelectImageInputfile.UseVisualStyleBackColor = true;
      this.SelectImageInputfile.Click += new System.EventHandler(this.SelectImageInputfileClick);
      // 
      // ImageInputfile
      // 
      this.ImageInputfile.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::TestApplication.Properties.Settings.Default, "ImageInputText", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
      this.ImageInputfile.Location = new System.Drawing.Point(9, 32);
      this.ImageInputfile.Name = "ImageInputfile";
      this.ImageInputfile.Size = new System.Drawing.Size(140, 20);
      this.ImageInputfile.TabIndex = 1;
      this.ImageInputfile.Text = global::TestApplication.Properties.Settings.Default.ImageInputText;
      // 
      // label4
      // 
      this.label4.AutoSize = true;
      this.label4.Location = new System.Drawing.Point(6, 16);
      this.label4.Name = "label4";
      this.label4.Size = new System.Drawing.Size(179, 13);
      this.label4.TabIndex = 0;
      this.label4.Text = "Select a txt file with the files to open:";
      // 
      // groupBox3
      // 
      this.groupBox3.Controls.Add(this.RunGridTest);
      this.groupBox3.Controls.Add(this.SelectGridInputfile);
      this.groupBox3.Controls.Add(this.GridInputfile);
      this.groupBox3.Controls.Add(this.label3);
      this.groupBox3.Location = new System.Drawing.Point(6, 114);
      this.groupBox3.Name = "groupBox3";
      this.groupBox3.Size = new System.Drawing.Size(188, 89);
      this.groupBox3.TabIndex = 1;
      this.groupBox3.TabStop = false;
      this.groupBox3.Text = "Grid files";
      // 
      // RunGridTest
      // 
      this.RunGridTest.Location = new System.Drawing.Point(9, 58);
      this.RunGridTest.Name = "RunGridTest";
      this.RunGridTest.Size = new System.Drawing.Size(173, 23);
      this.RunGridTest.TabIndex = 3;
      this.RunGridTest.Text = "Run test";
      this.RunGridTest.UseVisualStyleBackColor = true;
      this.RunGridTest.Click += new System.EventHandler(this.RunGridTestClick);
      // 
      // SelectGridInputfile
      // 
      this.SelectGridInputfile.Location = new System.Drawing.Point(155, 32);
      this.SelectGridInputfile.Name = "SelectGridInputfile";
      this.SelectGridInputfile.Size = new System.Drawing.Size(27, 20);
      this.SelectGridInputfile.TabIndex = 2;
      this.SelectGridInputfile.Text = "...";
      this.SelectGridInputfile.UseVisualStyleBackColor = true;
      this.SelectGridInputfile.Click += new System.EventHandler(this.SelectGridInputfileClick);
      // 
      // GridInputfile
      // 
      this.GridInputfile.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::TestApplication.Properties.Settings.Default, "GridInputText", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
      this.GridInputfile.Location = new System.Drawing.Point(9, 32);
      this.GridInputfile.Name = "GridInputfile";
      this.GridInputfile.Size = new System.Drawing.Size(140, 20);
      this.GridInputfile.TabIndex = 1;
      this.GridInputfile.Text = global::TestApplication.Properties.Settings.Default.GridInputText;
      // 
      // label3
      // 
      this.label3.AutoSize = true;
      this.label3.Location = new System.Drawing.Point(6, 16);
      this.label3.Name = "label3";
      this.label3.Size = new System.Drawing.Size(179, 13);
      this.label3.TabIndex = 0;
      this.label3.Text = "Select a txt file with the files to open:";
      // 
      // groupBox2
      // 
      this.groupBox2.Controls.Add(this.RunShapefileTest);
      this.groupBox2.Controls.Add(this.SelectShapefileInputfile);
      this.groupBox2.Controls.Add(this.ShapefileInputfile);
      this.groupBox2.Controls.Add(this.label2);
      this.groupBox2.Location = new System.Drawing.Point(6, 19);
      this.groupBox2.Name = "groupBox2";
      this.groupBox2.Size = new System.Drawing.Size(188, 89);
      this.groupBox2.TabIndex = 0;
      this.groupBox2.TabStop = false;
      this.groupBox2.Text = "Shapefiles";
      // 
      // RunShapefileTest
      // 
      this.RunShapefileTest.Location = new System.Drawing.Point(9, 58);
      this.RunShapefileTest.Name = "RunShapefileTest";
      this.RunShapefileTest.Size = new System.Drawing.Size(173, 23);
      this.RunShapefileTest.TabIndex = 3;
      this.RunShapefileTest.Text = "Run test";
      this.RunShapefileTest.UseVisualStyleBackColor = true;
      this.RunShapefileTest.Click += new System.EventHandler(this.RunShapefileTestClick);
      // 
      // SelectShapefileInputfile
      // 
      this.SelectShapefileInputfile.Location = new System.Drawing.Point(155, 32);
      this.SelectShapefileInputfile.Name = "SelectShapefileInputfile";
      this.SelectShapefileInputfile.Size = new System.Drawing.Size(27, 20);
      this.SelectShapefileInputfile.TabIndex = 2;
      this.SelectShapefileInputfile.Text = "...";
      this.toolTip1.SetToolTip(this.SelectShapefileInputfile, "Select text file with on every line the location of a shapefile");
      this.SelectShapefileInputfile.UseVisualStyleBackColor = true;
      this.SelectShapefileInputfile.Click += new System.EventHandler(this.SelectShapefileInputfileClick);
      // 
      // ShapefileInputfile
      // 
      this.ShapefileInputfile.AcceptsReturn = true;
      this.ShapefileInputfile.DataBindings.Add(new System.Windows.Forms.Binding("Text", global::TestApplication.Properties.Settings.Default, "ShapefileInputText", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
      this.ShapefileInputfile.Location = new System.Drawing.Point(9, 32);
      this.ShapefileInputfile.Name = "ShapefileInputfile";
      this.ShapefileInputfile.Size = new System.Drawing.Size(140, 20);
      this.ShapefileInputfile.TabIndex = 1;
      this.ShapefileInputfile.Text = global::TestApplication.Properties.Settings.Default.ShapefileInputText;
      // 
      // label2
      // 
      this.label2.AutoSize = true;
      this.label2.Location = new System.Drawing.Point(6, 16);
      this.label2.Name = "label2";
      this.label2.Size = new System.Drawing.Size(179, 13);
      this.label2.TabIndex = 0;
      this.label2.Text = "Select a txt file with the files to open:";
      // 
      // label1
      // 
      this.label1.AutoSize = true;
      this.label1.Font = new System.Drawing.Font("Microsoft Sans Serif", 9.75F, System.Drawing.FontStyle.Bold, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
      this.label1.Location = new System.Drawing.Point(12, 9);
      this.label1.Name = "label1";
      this.label1.Size = new System.Drawing.Size(47, 16);
      this.label1.TabIndex = 0;
      this.label1.Text = "Tests";
      // 
      // axMap1
      // 
      this.axMap1.Dock = System.Windows.Forms.DockStyle.Fill;
      this.axMap1.Enabled = true;
      this.axMap1.Location = new System.Drawing.Point(0, 0);
      this.axMap1.Name = "axMap1";
      this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
      this.axMap1.Size = new System.Drawing.Size(452, 394);
      this.axMap1.TabIndex = 0;
      this.axMap1.FileDropped += new AxMapWinGIS._DMapEvents_FileDroppedEventHandler(this.AxMap1FileDropped);
      // 
      // Progressbox
      // 
      this.Progressbox.BackColor = System.Drawing.Color.White;
      this.Progressbox.Dock = System.Windows.Forms.DockStyle.Fill;
      this.Progressbox.Location = new System.Drawing.Point(0, 0);
      this.Progressbox.Multiline = true;
      this.Progressbox.Name = "Progressbox";
      this.Progressbox.ReadOnly = true;
      this.Progressbox.ScrollBars = System.Windows.Forms.ScrollBars.Vertical;
      this.Progressbox.Size = new System.Drawing.Size(667, 84);
      this.Progressbox.TabIndex = 0;
      // 
      // Form1
      // 
      this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
      this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
      this.ClientSize = new System.Drawing.Size(667, 482);
      this.Controls.Add(this.splitContainer1);
      this.DataBindings.Add(new System.Windows.Forms.Binding("Location", global::TestApplication.Properties.Settings.Default, "FormLocation", true, System.Windows.Forms.DataSourceUpdateMode.OnPropertyChanged));
      this.Location = global::TestApplication.Properties.Settings.Default.FormLocation;
      this.MinimumSize = new System.Drawing.Size(680, 520);
      this.Name = "Form1";
      this.Text = "Test application for MapWinGIS";
      this.Load += new System.EventHandler(this.Form1Load);
      this.FormClosing += new System.Windows.Forms.FormClosingEventHandler(this.Form1FormClosing);
      this.splitContainer1.Panel1.ResumeLayout(false);
      this.splitContainer1.Panel2.ResumeLayout(false);
      this.splitContainer1.Panel2.PerformLayout();
      this.splitContainer1.ResumeLayout(false);
      this.splitContainer2.Panel1.ResumeLayout(false);
      this.splitContainer2.Panel1.PerformLayout();
      this.splitContainer2.Panel2.ResumeLayout(false);
      this.splitContainer2.ResumeLayout(false);
      this.groupBox1.ResumeLayout(false);
      this.groupBox4.ResumeLayout(false);
      this.groupBox4.PerformLayout();
      this.groupBox3.ResumeLayout(false);
      this.groupBox3.PerformLayout();
      this.groupBox2.ResumeLayout(false);
      this.groupBox2.PerformLayout();
      ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
      this.ResumeLayout(false);

    }

    #endregion

    private System.Windows.Forms.SplitContainer splitContainer1;
    private System.Windows.Forms.SplitContainer splitContainer2;
    private System.Windows.Forms.Label label1;
    private System.Windows.Forms.GroupBox groupBox1;
    private System.Windows.Forms.GroupBox groupBox2;
    private System.Windows.Forms.Label label2;
    private System.Windows.Forms.Button RunShapefileTest;
    private System.Windows.Forms.Button SelectShapefileInputfile;
    private System.Windows.Forms.TextBox ShapefileInputfile;
    private System.Windows.Forms.GroupBox groupBox4;
    private System.Windows.Forms.Button RunImageTest;
    private System.Windows.Forms.Button SelectImageInputfile;
    private System.Windows.Forms.TextBox ImageInputfile;
    private System.Windows.Forms.Label label4;
    private System.Windows.Forms.GroupBox groupBox3;
    private System.Windows.Forms.Button RunGridTest;
    private System.Windows.Forms.Button SelectGridInputfile;
    private System.Windows.Forms.TextBox GridInputfile;
    private System.Windows.Forms.Label label3;
    private System.Windows.Forms.ToolTip toolTip1;
    private AxMapWinGIS.AxMap axMap1;
    private System.Windows.Forms.TextBox Progressbox;
  }
}

