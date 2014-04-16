namespace TestGrids
{
    partial class TestGridForm1
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
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TestGridForm1));
            this.btnOpen = new System.Windows.Forms.Button();
            this.progressBar1 = new System.Windows.Forms.ProgressBar();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.chkUseFileManager = new System.Windows.Forms.CheckBox();
            this.lstFilenames = new System.Windows.Forms.ListBox();
            this.btnReloadMapState = new System.Windows.Forms.Button();
            this.btnOpenProxy = new System.Windows.Forms.Button();
            this.btnClearMap = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.groupBox6 = new System.Windows.Forms.GroupBox();
            this.label7 = new System.Windows.Forms.Label();
            this.udMaxSizeWoProxy = new System.Windows.Forms.NumericUpDown();
            this.cboProxyMode = new System.Windows.Forms.ComboBox();
            this.label6 = new System.Windows.Forms.Label();
            this.cboProxyFormat = new System.Windows.Forms.ComboBox();
            this.label3 = new System.Windows.Forms.Label();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.chkLayerVisible = new System.Windows.Forms.CheckBox();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.btnOpenFolder = new System.Windows.Forms.Button();
            this.btnRemoveProxy = new System.Windows.Forms.Button();
            this.lblHasProxy = new System.Windows.Forms.Label();
            this.btnUpdateProxyMode = new System.Windows.Forms.Button();
            this.txtImageName = new System.Windows.Forms.TextBox();
            this.label8 = new System.Windows.Forms.Label();
            this.label10 = new System.Windows.Forms.Label();
            this.cboActiveProxyMode = new System.Windows.Forms.ComboBox();
            this.lblExtents = new System.Windows.Forms.Label();
            this.label9 = new System.Windows.Forms.Label();
            this.txtSourceName = new System.Windows.Forms.TextBox();
            this.lblCurrentProxyMode = new System.Windows.Forms.Label();
            this.groupBox3 = new System.Windows.Forms.GroupBox();
            this.cboActiveBand = new System.Windows.Forms.ComboBox();
            this.label4 = new System.Windows.Forms.Label();
            this.chkAllowExternal = new System.Windows.Forms.CheckBox();
            this.btnRebuild = new System.Windows.Forms.Button();
            this.label2 = new System.Windows.Forms.Label();
            this.cboColoringScheme = new System.Windows.Forms.ComboBox();
            this.cboColoring = new System.Windows.Forms.ComboBox();
            this.label1 = new System.Windows.Forms.Label();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox8 = new System.Windows.Forms.GroupBox();
            this.gdalInfoProxy = new System.Windows.Forms.RichTextBox();
            this.groupBox7 = new System.Windows.Forms.GroupBox();
            this.gdalInfoSource = new System.Windows.Forms.RichTextBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.groupBox4 = new System.Windows.Forms.GroupBox();
            this.label14 = new System.Windows.Forms.Label();
            this.cboZoomBehavior = new System.Windows.Forms.ComboBox();
            this.label13 = new System.Windows.Forms.Label();
            this.cboCoordinates = new System.Windows.Forms.ComboBox();
            this.label11 = new System.Windows.Forms.Label();
            this.btnSetKnownExtents = new System.Windows.Forms.Button();
            this.cboCountry = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.cboScalebarUnits = new System.Windows.Forms.ComboBox();
            this.label12 = new System.Windows.Forms.Label();
            this.chkDisplayAngles = new System.Windows.Forms.CheckBox();
            this.optArea = new System.Windows.Forms.RadioButton();
            this.optDistance = new System.Windows.Forms.RadioButton();
            this.btnStopMeasuring = new System.Windows.Forms.Button();
            this.btnMeasure = new System.Windows.Forms.Button();
            this.lblProgress = new System.Windows.Forms.Label();
            this.axMap1 = new AxMapWinGIS.AxMap();
            this.groupBox1.SuspendLayout();
            this.tabControl1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            this.groupBox6.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.udMaxSizeWoProxy)).BeginInit();
            this.tabPage1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.groupBox3.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox8.SuspendLayout();
            this.groupBox7.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.groupBox4.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
            this.SuspendLayout();
            // 
            // btnOpen
            // 
            this.btnOpen.Location = new System.Drawing.Point(222, 19);
            this.btnOpen.Name = "btnOpen";
            this.btnOpen.Size = new System.Drawing.Size(86, 32);
            this.btnOpen.TabIndex = 1;
            this.btnOpen.Text = "Open grid";
            this.btnOpen.UseVisualStyleBackColor = true;
            this.btnOpen.Click += new System.EventHandler(this.BtnOpenClick);
            // 
            // progressBar1
            // 
            this.progressBar1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Bottom | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.progressBar1.Location = new System.Drawing.Point(10, 597);
            this.progressBar1.Name = "progressBar1";
            this.progressBar1.Size = new System.Drawing.Size(588, 19);
            this.progressBar1.TabIndex = 5;
            // 
            // groupBox1
            // 
            this.groupBox1.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox1.Controls.Add(this.chkUseFileManager);
            this.groupBox1.Controls.Add(this.lstFilenames);
            this.groupBox1.Controls.Add(this.btnReloadMapState);
            this.groupBox1.Controls.Add(this.btnOpenProxy);
            this.groupBox1.Controls.Add(this.btnClearMap);
            this.groupBox1.Controls.Add(this.btnOpen);
            this.groupBox1.Location = new System.Drawing.Point(6, 172);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(314, 224);
            this.groupBox1.TabIndex = 10;
            this.groupBox1.TabStop = false;
            this.groupBox1.Text = "Commands";
            // 
            // chkUseFileManager
            // 
            this.chkUseFileManager.AutoSize = true;
            this.chkUseFileManager.Location = new System.Drawing.Point(16, 197);
            this.chkUseFileManager.Name = "chkUseFileManager";
            this.chkUseFileManager.Size = new System.Drawing.Size(105, 17);
            this.chkUseFileManager.TabIndex = 18;
            this.chkUseFileManager.Text = "Use file manager";
            this.chkUseFileManager.UseVisualStyleBackColor = true;
            // 
            // lstFilenames
            // 
            this.lstFilenames.Font = new System.Drawing.Font("Microsoft Sans Serif", 9F, System.Drawing.FontStyle.Regular, System.Drawing.GraphicsUnit.Point, ((byte)(0)));
            this.lstFilenames.FormattingEnabled = true;
            this.lstFilenames.ItemHeight = 15;
            this.lstFilenames.Location = new System.Drawing.Point(8, 16);
            this.lstFilenames.Name = "lstFilenames";
            this.lstFilenames.Size = new System.Drawing.Size(208, 169);
            this.lstFilenames.TabIndex = 17;
            // 
            // btnReloadMapState
            // 
            this.btnReloadMapState.Location = new System.Drawing.Point(222, 117);
            this.btnReloadMapState.Name = "btnReloadMapState";
            this.btnReloadMapState.Size = new System.Drawing.Size(86, 32);
            this.btnReloadMapState.TabIndex = 15;
            this.btnReloadMapState.Text = "Reload state";
            this.btnReloadMapState.UseVisualStyleBackColor = true;
            this.btnReloadMapState.Click += new System.EventHandler(this.BtnReloadMapStateClick);
            // 
            // btnOpenProxy
            // 
            this.btnOpenProxy.Location = new System.Drawing.Point(222, 155);
            this.btnOpenProxy.Name = "btnOpenProxy";
            this.btnOpenProxy.Size = new System.Drawing.Size(86, 32);
            this.btnOpenProxy.TabIndex = 14;
            this.btnOpenProxy.Text = "Open proxy";
            this.btnOpenProxy.UseVisualStyleBackColor = true;
            this.btnOpenProxy.Click += new System.EventHandler(this.BtnOpenProxyClick);
            // 
            // btnClearMap
            // 
            this.btnClearMap.Location = new System.Drawing.Point(222, 57);
            this.btnClearMap.Name = "btnClearMap";
            this.btnClearMap.Size = new System.Drawing.Size(86, 32);
            this.btnClearMap.TabIndex = 13;
            this.btnClearMap.Text = "Clear map";
            this.btnClearMap.UseVisualStyleBackColor = true;
            this.btnClearMap.Click += new System.EventHandler(this.BtnClearMapClick);
            // 
            // tabControl1
            // 
            this.tabControl1.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Location = new System.Drawing.Point(605, 12);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(334, 624);
            this.tabControl1.TabIndex = 20;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.groupBox1);
            this.tabPage3.Controls.Add(this.groupBox6);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(326, 598);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Open";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // groupBox6
            // 
            this.groupBox6.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox6.Controls.Add(this.label7);
            this.groupBox6.Controls.Add(this.udMaxSizeWoProxy);
            this.groupBox6.Controls.Add(this.cboProxyMode);
            this.groupBox6.Controls.Add(this.label6);
            this.groupBox6.Controls.Add(this.cboProxyFormat);
            this.groupBox6.Controls.Add(this.label3);
            this.groupBox6.Location = new System.Drawing.Point(6, 13);
            this.groupBox6.Name = "groupBox6";
            this.groupBox6.Size = new System.Drawing.Size(314, 153);
            this.groupBox6.TabIndex = 21;
            this.groupBox6.TabStop = false;
            this.groupBox6.Text = "Opening options (from GlobalSettings)";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(17, 72);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(186, 13);
            this.label7.TabIndex = 6;
            this.label7.Text = "Max size to be opened w/o proxy, MB";
            // 
            // udMaxSizeWoProxy
            // 
            this.udMaxSizeWoProxy.Location = new System.Drawing.Point(209, 70);
            this.udMaxSizeWoProxy.Name = "udMaxSizeWoProxy";
            this.udMaxSizeWoProxy.Size = new System.Drawing.Size(49, 20);
            this.udMaxSizeWoProxy.TabIndex = 5;
            // 
            // cboProxyMode
            // 
            this.cboProxyMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboProxyMode.FormattingEnabled = true;
            this.cboProxyMode.Location = new System.Drawing.Point(123, 29);
            this.cboProxyMode.Name = "cboProxyMode";
            this.cboProxyMode.Size = new System.Drawing.Size(135, 21);
            this.cboProxyMode.TabIndex = 4;
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(17, 32);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(62, 13);
            this.label6.TabIndex = 3;
            this.label6.Text = "Proxy mode";
            // 
            // cboProxyFormat
            // 
            this.cboProxyFormat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboProxyFormat.FormattingEnabled = true;
            this.cboProxyFormat.Location = new System.Drawing.Point(123, 113);
            this.cboProxyFormat.Name = "cboProxyFormat";
            this.cboProxyFormat.Size = new System.Drawing.Size(135, 21);
            this.cboProxyFormat.TabIndex = 1;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(17, 116);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(65, 13);
            this.label3.TabIndex = 0;
            this.label3.Text = "Proxy format";
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.chkLayerVisible);
            this.tabPage1.Controls.Add(this.groupBox2);
            this.tabPage1.Controls.Add(this.groupBox3);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(326, 598);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Update";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // chkLayerVisible
            // 
            this.chkLayerVisible.AutoSize = true;
            this.chkLayerVisible.Location = new System.Drawing.Point(15, 15);
            this.chkLayerVisible.Name = "chkLayerVisible";
            this.chkLayerVisible.Size = new System.Drawing.Size(84, 17);
            this.chkLayerVisible.TabIndex = 38;
            this.chkLayerVisible.Text = "Layer visible";
            this.chkLayerVisible.UseVisualStyleBackColor = true;
            this.chkLayerVisible.CheckedChanged += new System.EventHandler(this.ChkLayerVisibleCheckedChanged);
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.btnOpenFolder);
            this.groupBox2.Controls.Add(this.btnRemoveProxy);
            this.groupBox2.Controls.Add(this.lblHasProxy);
            this.groupBox2.Controls.Add(this.btnUpdateProxyMode);
            this.groupBox2.Controls.Add(this.txtImageName);
            this.groupBox2.Controls.Add(this.label8);
            this.groupBox2.Controls.Add(this.label10);
            this.groupBox2.Controls.Add(this.cboActiveProxyMode);
            this.groupBox2.Controls.Add(this.lblExtents);
            this.groupBox2.Controls.Add(this.label9);
            this.groupBox2.Controls.Add(this.txtSourceName);
            this.groupBox2.Controls.Add(this.lblCurrentProxyMode);
            this.groupBox2.Location = new System.Drawing.Point(6, 38);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(306, 287);
            this.groupBox2.TabIndex = 37;
            this.groupBox2.TabStop = false;
            this.groupBox2.Text = "Proxy mode";
            // 
            // btnOpenFolder
            // 
            this.btnOpenFolder.Location = new System.Drawing.Point(191, 109);
            this.btnOpenFolder.Name = "btnOpenFolder";
            this.btnOpenFolder.Size = new System.Drawing.Size(100, 21);
            this.btnOpenFolder.TabIndex = 36;
            this.btnOpenFolder.Text = "Open folder";
            this.btnOpenFolder.UseVisualStyleBackColor = true;
            this.btnOpenFolder.Click += new System.EventHandler(this.BtnOpenFolderClick);
            // 
            // btnRemoveProxy
            // 
            this.btnRemoveProxy.Location = new System.Drawing.Point(229, 171);
            this.btnRemoveProxy.Name = "btnRemoveProxy";
            this.btnRemoveProxy.Size = new System.Drawing.Size(63, 21);
            this.btnRemoveProxy.TabIndex = 35;
            this.btnRemoveProxy.Text = "Remove";
            this.btnRemoveProxy.UseVisualStyleBackColor = true;
            this.btnRemoveProxy.Click += new System.EventHandler(this.BtnRemoveProxyClick);
            // 
            // lblHasProxy
            // 
            this.lblHasProxy.AutoSize = true;
            this.lblHasProxy.Location = new System.Drawing.Point(11, 175);
            this.lblHasProxy.Name = "lblHasProxy";
            this.lblHasProxy.Size = new System.Drawing.Size(82, 13);
            this.lblHasProxy.TabIndex = 34;
            this.lblHasProxy.Text = "Has valid proxy:";
            // 
            // btnUpdateProxyMode
            // 
            this.btnUpdateProxyMode.Location = new System.Drawing.Point(228, 225);
            this.btnUpdateProxyMode.Name = "btnUpdateProxyMode";
            this.btnUpdateProxyMode.Size = new System.Drawing.Size(64, 21);
            this.btnUpdateProxyMode.TabIndex = 30;
            this.btnUpdateProxyMode.Text = "Update";
            this.btnUpdateProxyMode.UseVisualStyleBackColor = true;
            this.btnUpdateProxyMode.Click += new System.EventHandler(this.BtnUpdateProxyModeClick);
            // 
            // txtImageName
            // 
            this.txtImageName.Location = new System.Drawing.Point(14, 83);
            this.txtImageName.Name = "txtImageName";
            this.txtImageName.Size = new System.Drawing.Size(278, 20);
            this.txtImageName.TabIndex = 33;
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(11, 229);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(89, 13);
            this.label8.TabIndex = 25;
            this.label8.Text = "New proxy mode:";
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(11, 67);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(67, 13);
            this.label10.TabIndex = 32;
            this.label10.Text = "Image Name";
            // 
            // cboActiveProxyMode
            // 
            this.cboActiveProxyMode.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboActiveProxyMode.FormattingEnabled = true;
            this.cboActiveProxyMode.Location = new System.Drawing.Point(113, 225);
            this.cboActiveProxyMode.Name = "cboActiveProxyMode";
            this.cboActiveProxyMode.Size = new System.Drawing.Size(109, 21);
            this.cboActiveProxyMode.TabIndex = 26;
            // 
            // lblExtents
            // 
            this.lblExtents.AutoSize = true;
            this.lblExtents.Location = new System.Drawing.Point(11, 201);
            this.lblExtents.Name = "lblExtents";
            this.lblExtents.Size = new System.Drawing.Size(45, 13);
            this.lblExtents.TabIndex = 31;
            this.lblExtents.Text = "Extents:";
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(11, 18);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(72, 13);
            this.label9.TabIndex = 27;
            this.label9.Text = "Source Name";
            // 
            // txtSourceName
            // 
            this.txtSourceName.Location = new System.Drawing.Point(14, 34);
            this.txtSourceName.Name = "txtSourceName";
            this.txtSourceName.Size = new System.Drawing.Size(278, 20);
            this.txtSourceName.TabIndex = 28;
            // 
            // lblCurrentProxyMode
            // 
            this.lblCurrentProxyMode.AutoSize = true;
            this.lblCurrentProxyMode.Location = new System.Drawing.Point(11, 147);
            this.lblCurrentProxyMode.Name = "lblCurrentProxyMode";
            this.lblCurrentProxyMode.Size = new System.Drawing.Size(101, 13);
            this.lblCurrentProxyMode.TabIndex = 29;
            this.lblCurrentProxyMode.Text = "Current proxy mode:";
            // 
            // groupBox3
            // 
            this.groupBox3.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.groupBox3.Controls.Add(this.cboActiveBand);
            this.groupBox3.Controls.Add(this.label4);
            this.groupBox3.Controls.Add(this.chkAllowExternal);
            this.groupBox3.Controls.Add(this.btnRebuild);
            this.groupBox3.Controls.Add(this.label2);
            this.groupBox3.Controls.Add(this.cboColoringScheme);
            this.groupBox3.Controls.Add(this.cboColoring);
            this.groupBox3.Controls.Add(this.label1);
            this.groupBox3.Location = new System.Drawing.Point(6, 333);
            this.groupBox3.Name = "groupBox3";
            this.groupBox3.Size = new System.Drawing.Size(306, 265);
            this.groupBox3.TabIndex = 36;
            this.groupBox3.TabStop = false;
            this.groupBox3.Text = "ColorScheme";
            // 
            // cboActiveBand
            // 
            this.cboActiveBand.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboActiveBand.FormattingEnabled = true;
            this.cboActiveBand.Location = new System.Drawing.Point(24, 78);
            this.cboActiveBand.Name = "cboActiveBand";
            this.cboActiveBand.Size = new System.Drawing.Size(141, 21);
            this.cboActiveBand.TabIndex = 14;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(26, 62);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(67, 13);
            this.label4.TabIndex = 13;
            this.label4.Text = "Active band:";
            // 
            // chkAllowExternal
            // 
            this.chkAllowExternal.AutoSize = true;
            this.chkAllowExternal.Checked = true;
            this.chkAllowExternal.CheckState = System.Windows.Forms.CheckState.Checked;
            this.chkAllowExternal.Location = new System.Drawing.Point(25, 33);
            this.chkAllowExternal.Name = "chkAllowExternal";
            this.chkAllowExternal.Size = new System.Drawing.Size(157, 17);
            this.chkAllowExternal.TabIndex = 10;
            this.chkAllowExternal.Text = "Allow external color scheme";
            this.chkAllowExternal.UseVisualStyleBackColor = true;
            // 
            // btnRebuild
            // 
            this.btnRebuild.Location = new System.Drawing.Point(25, 224);
            this.btnRebuild.Name = "btnRebuild";
            this.btnRebuild.Size = new System.Drawing.Size(85, 27);
            this.btnRebuild.TabIndex = 2;
            this.btnRebuild.Text = "Rebuild";
            this.btnRebuild.UseVisualStyleBackColor = true;
            this.btnRebuild.Click += new System.EventHandler(this.BtnRebuildClick);
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(22, 173);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(88, 13);
            this.label2.TabIndex = 9;
            this.label2.Text = "Coloring scheme:";
            // 
            // cboColoringScheme
            // 
            this.cboColoringScheme.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboColoringScheme.FormattingEnabled = true;
            this.cboColoringScheme.Location = new System.Drawing.Point(25, 189);
            this.cboColoringScheme.Name = "cboColoringScheme";
            this.cboColoringScheme.Size = new System.Drawing.Size(141, 21);
            this.cboColoringScheme.TabIndex = 7;
            // 
            // cboColoring
            // 
            this.cboColoring.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboColoring.FormattingEnabled = true;
            this.cboColoring.Location = new System.Drawing.Point(25, 132);
            this.cboColoring.Name = "cboColoring";
            this.cboColoring.Size = new System.Drawing.Size(141, 21);
            this.cboColoring.TabIndex = 6;
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(22, 116);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(71, 13);
            this.label1.TabIndex = 8;
            this.label1.Text = "Coloring type:";
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox8);
            this.tabPage2.Controls.Add(this.groupBox7);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(326, 598);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "GDAL info";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox8
            // 
            this.groupBox8.Controls.Add(this.gdalInfoProxy);
            this.groupBox8.Location = new System.Drawing.Point(3, 298);
            this.groupBox8.Name = "groupBox8";
            this.groupBox8.Size = new System.Drawing.Size(317, 294);
            this.groupBox8.TabIndex = 1;
            this.groupBox8.TabStop = false;
            this.groupBox8.Text = "Gdal info grid proxy";
            // 
            // gdalInfoProxy
            // 
            this.gdalInfoProxy.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.gdalInfoProxy.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gdalInfoProxy.Location = new System.Drawing.Point(3, 16);
            this.gdalInfoProxy.Name = "gdalInfoProxy";
            this.gdalInfoProxy.Size = new System.Drawing.Size(311, 275);
            this.gdalInfoProxy.TabIndex = 1;
            this.gdalInfoProxy.Text = "";
            // 
            // groupBox7
            // 
            this.groupBox7.Controls.Add(this.gdalInfoSource);
            this.groupBox7.Location = new System.Drawing.Point(6, 11);
            this.groupBox7.Name = "groupBox7";
            this.groupBox7.Size = new System.Drawing.Size(309, 281);
            this.groupBox7.TabIndex = 0;
            this.groupBox7.TabStop = false;
            this.groupBox7.Text = "Gdal info grid source";
            // 
            // gdalInfoSource
            // 
            this.gdalInfoSource.BorderStyle = System.Windows.Forms.BorderStyle.None;
            this.gdalInfoSource.Dock = System.Windows.Forms.DockStyle.Fill;
            this.gdalInfoSource.Location = new System.Drawing.Point(3, 16);
            this.gdalInfoSource.Name = "gdalInfoSource";
            this.gdalInfoSource.Size = new System.Drawing.Size(303, 262);
            this.gdalInfoSource.TabIndex = 0;
            this.gdalInfoSource.Text = "";
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.groupBox4);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(326, 598);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Measuring";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // groupBox4
            // 
            this.groupBox4.Controls.Add(this.label14);
            this.groupBox4.Controls.Add(this.cboZoomBehavior);
            this.groupBox4.Controls.Add(this.label13);
            this.groupBox4.Controls.Add(this.cboCoordinates);
            this.groupBox4.Controls.Add(this.label11);
            this.groupBox4.Controls.Add(this.btnSetKnownExtents);
            this.groupBox4.Controls.Add(this.cboCountry);
            this.groupBox4.Controls.Add(this.label5);
            this.groupBox4.Controls.Add(this.cboScalebarUnits);
            this.groupBox4.Controls.Add(this.label12);
            this.groupBox4.Controls.Add(this.chkDisplayAngles);
            this.groupBox4.Controls.Add(this.optArea);
            this.groupBox4.Controls.Add(this.optDistance);
            this.groupBox4.Controls.Add(this.btnStopMeasuring);
            this.groupBox4.Controls.Add(this.btnMeasure);
            this.groupBox4.Location = new System.Drawing.Point(11, 12);
            this.groupBox4.Name = "groupBox4";
            this.groupBox4.Size = new System.Drawing.Size(310, 466);
            this.groupBox4.TabIndex = 23;
            this.groupBox4.TabStop = false;
            this.groupBox4.Text = "Measuring";
            // 
            // label14
            // 
            this.label14.AutoSize = true;
            this.label14.Location = new System.Drawing.Point(13, 336);
            this.label14.Name = "label14";
            this.label14.Size = new System.Drawing.Size(81, 13);
            this.label14.TabIndex = 37;
            this.label14.Text = "Zoom behavior:";
            // 
            // cboZoomBehavior
            // 
            this.cboZoomBehavior.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboZoomBehavior.FormattingEnabled = true;
            this.cboZoomBehavior.Location = new System.Drawing.Point(12, 352);
            this.cboZoomBehavior.Name = "cboZoomBehavior";
            this.cboZoomBehavior.Size = new System.Drawing.Size(179, 21);
            this.cboZoomBehavior.TabIndex = 36;
            this.cboZoomBehavior.SelectedIndexChanged += new System.EventHandler(this.cboZoomBehavior_SelectedIndexChanged);
            // 
            // label13
            // 
            this.label13.AutoSize = true;
            this.label13.Location = new System.Drawing.Point(13, 281);
            this.label13.Name = "label13";
            this.label13.Size = new System.Drawing.Size(95, 13);
            this.label13.TabIndex = 35;
            this.label13.Text = "Show coordinates:";
            // 
            // cboCoordinates
            // 
            this.cboCoordinates.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboCoordinates.FormattingEnabled = true;
            this.cboCoordinates.Location = new System.Drawing.Point(9, 297);
            this.cboCoordinates.Name = "cboCoordinates";
            this.cboCoordinates.Size = new System.Drawing.Size(179, 21);
            this.cboCoordinates.TabIndex = 34;
            this.cboCoordinates.SelectedIndexChanged += new System.EventHandler(this.cboCoordinates_SelectedIndexChanged);
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(13, 385);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(68, 13);
            this.label11.TabIndex = 33;
            this.label11.Text = "Map extents:";
            // 
            // btnSetKnownExtents
            // 
            this.btnSetKnownExtents.Location = new System.Drawing.Point(194, 428);
            this.btnSetKnownExtents.Name = "btnSetKnownExtents";
            this.btnSetKnownExtents.Size = new System.Drawing.Size(100, 32);
            this.btnSetKnownExtents.TabIndex = 31;
            this.btnSetKnownExtents.Text = "Zoom to";
            this.btnSetKnownExtents.UseVisualStyleBackColor = true;
            this.btnSetKnownExtents.Click += new System.EventHandler(this.btnSetKnownExtents_Click);
            // 
            // cboCountry
            // 
            this.cboCountry.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboCountry.FormattingEnabled = true;
            this.cboCountry.Location = new System.Drawing.Point(9, 401);
            this.cboCountry.Name = "cboCountry";
            this.cboCountry.Size = new System.Drawing.Size(279, 21);
            this.cboCountry.TabIndex = 32;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(13, 229);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(77, 13);
            this.label5.TabIndex = 23;
            this.label5.Text = "Scalebar units:";
            // 
            // cboScalebarUnits
            // 
            this.cboScalebarUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboScalebarUnits.FormattingEnabled = true;
            this.cboScalebarUnits.Location = new System.Drawing.Point(9, 245);
            this.cboScalebarUnits.Name = "cboScalebarUnits";
            this.cboScalebarUnits.Size = new System.Drawing.Size(179, 21);
            this.cboScalebarUnits.TabIndex = 22;
            this.cboScalebarUnits.SelectedIndexChanged += new System.EventHandler(this.CboScalebarUnitsSelectedIndexChanged);
            // 
            // label12
            // 
            this.label12.Location = new System.Drawing.Point(9, 131);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(285, 86);
            this.label12.TabIndex = 21;
            this.label12.Text = resources.GetString("label12.Text");
            // 
            // chkDisplayAngles
            // 
            this.chkDisplayAngles.AutoSize = true;
            this.chkDisplayAngles.Location = new System.Drawing.Point(16, 99);
            this.chkDisplayAngles.Name = "chkDisplayAngles";
            this.chkDisplayAngles.Size = new System.Drawing.Size(94, 17);
            this.chkDisplayAngles.TabIndex = 20;
            this.chkDisplayAngles.Text = "Display angles";
            this.chkDisplayAngles.UseVisualStyleBackColor = true;
            this.chkDisplayAngles.CheckedChanged += new System.EventHandler(this.ChkDisplayAnglesCheckedChanged);
            // 
            // optArea
            // 
            this.optArea.AutoSize = true;
            this.optArea.Location = new System.Drawing.Point(16, 65);
            this.optArea.Name = "optArea";
            this.optArea.Size = new System.Drawing.Size(47, 17);
            this.optArea.TabIndex = 19;
            this.optArea.Text = "Area";
            this.optArea.UseVisualStyleBackColor = true;
            // 
            // optDistance
            // 
            this.optDistance.AutoSize = true;
            this.optDistance.Checked = true;
            this.optDistance.Location = new System.Drawing.Point(16, 37);
            this.optDistance.Name = "optDistance";
            this.optDistance.Size = new System.Drawing.Size(67, 17);
            this.optDistance.TabIndex = 18;
            this.optDistance.TabStop = true;
            this.optDistance.Text = "Distance";
            this.optDistance.UseVisualStyleBackColor = true;
            // 
            // btnStopMeasuring
            // 
            this.btnStopMeasuring.Location = new System.Drawing.Point(209, 57);
            this.btnStopMeasuring.Name = "btnStopMeasuring";
            this.btnStopMeasuring.Size = new System.Drawing.Size(86, 32);
            this.btnStopMeasuring.TabIndex = 17;
            this.btnStopMeasuring.Text = "Stop";
            this.btnStopMeasuring.UseVisualStyleBackColor = true;
            this.btnStopMeasuring.Click += new System.EventHandler(this.BtnStopMeasuringClick);
            // 
            // btnMeasure
            // 
            this.btnMeasure.Location = new System.Drawing.Point(209, 19);
            this.btnMeasure.Name = "btnMeasure";
            this.btnMeasure.Size = new System.Drawing.Size(86, 32);
            this.btnMeasure.TabIndex = 16;
            this.btnMeasure.Text = "Start";
            this.btnMeasure.UseVisualStyleBackColor = true;
            this.btnMeasure.Click += new System.EventHandler(this.BtnMeasureClick);
            // 
            // lblProgress
            // 
            this.lblProgress.AutoSize = true;
            this.lblProgress.Location = new System.Drawing.Point(8, 624);
            this.lblProgress.Name = "lblProgress";
            this.lblProgress.Size = new System.Drawing.Size(0, 13);
            this.lblProgress.TabIndex = 22;
            // 
            // axMap1
            // 
            this.axMap1.Anchor = ((System.Windows.Forms.AnchorStyles)((((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Bottom) 
            | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.axMap1.Enabled = true;
            this.axMap1.Location = new System.Drawing.Point(10, 12);
            this.axMap1.Name = "axMap1";
            this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
            this.axMap1.Size = new System.Drawing.Size(588, 579);
            this.axMap1.TabIndex = 23;
            // 
            // TestGridForm1
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(942, 646);
            this.Controls.Add(this.axMap1);
            this.Controls.Add(this.lblProgress);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.progressBar1);
            this.Name = "TestGridForm1";
            this.Text = "TestGrid";
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabControl1.ResumeLayout(false);
            this.tabPage3.ResumeLayout(false);
            this.groupBox6.ResumeLayout(false);
            this.groupBox6.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.udMaxSizeWoProxy)).EndInit();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.groupBox3.ResumeLayout(false);
            this.groupBox3.PerformLayout();
            this.tabPage2.ResumeLayout(false);
            this.groupBox8.ResumeLayout(false);
            this.groupBox7.ResumeLayout(false);
            this.tabPage4.ResumeLayout(false);
            this.groupBox4.ResumeLayout(false);
            this.groupBox4.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.Button btnOpen;
        private System.Windows.Forms.ProgressBar progressBar1;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.Button btnClearMap;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.GroupBox groupBox6;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown udMaxSizeWoProxy;
        private System.Windows.Forms.ComboBox cboProxyMode;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cboProxyFormat;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.ComboBox cboActiveProxyMode;
        private System.Windows.Forms.Label label8;
        private System.Windows.Forms.TextBox txtSourceName;
        private System.Windows.Forms.Button btnUpdateProxyMode;
        private System.Windows.Forms.Label lblCurrentProxyMode;
        private System.Windows.Forms.Label lblExtents;
        private System.Windows.Forms.TextBox txtImageName;
        private System.Windows.Forms.Label label10;
        private System.Windows.Forms.GroupBox groupBox3;
        private System.Windows.Forms.ComboBox cboActiveBand;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.CheckBox chkAllowExternal;
        private System.Windows.Forms.Button btnRebuild;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.ComboBox cboColoringScheme;
        private System.Windows.Forms.ComboBox cboColoring;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.Label lblHasProxy;
        private System.Windows.Forms.Button btnOpenProxy;
        private System.Windows.Forms.Button btnReloadMapState;
        private System.Windows.Forms.Button btnRemoveProxy;
        private System.Windows.Forms.ListBox lstFilenames;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.GroupBox groupBox4;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.CheckBox chkDisplayAngles;
        private System.Windows.Forms.RadioButton optArea;
        private System.Windows.Forms.RadioButton optDistance;
        private System.Windows.Forms.Button btnStopMeasuring;
        private System.Windows.Forms.Button btnMeasure;
        private System.Windows.Forms.GroupBox groupBox8;
        private System.Windows.Forms.RichTextBox gdalInfoProxy;
        private System.Windows.Forms.GroupBox groupBox7;
        private System.Windows.Forms.RichTextBox gdalInfoSource;
        private System.Windows.Forms.Button btnOpenFolder;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.ComboBox cboScalebarUnits;
        private System.Windows.Forms.CheckBox chkLayerVisible;
        private System.Windows.Forms.Label lblProgress;
        private System.Windows.Forms.ComboBox cboCountry;
        private System.Windows.Forms.Button btnSetKnownExtents;
        private System.Windows.Forms.Label label13;
        private System.Windows.Forms.ComboBox cboCoordinates;
        private System.Windows.Forms.Label label11;
        private AxMapWinGIS.AxMap axMap1;
        private System.Windows.Forms.Label label14;
        private System.Windows.Forms.ComboBox cboZoomBehavior;
        private System.Windows.Forms.CheckBox chkUseFileManager;
    }
}