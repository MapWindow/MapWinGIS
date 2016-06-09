using MWLite.GUI.Controls;
using MWLite.Symbology.Controls;

namespace MWLite.GUI.Forms
{
    partial class MeasuringForm
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
            this.chkShowBearing = new System.Windows.Forms.CheckBox();
            this.chkShowLength = new System.Windows.Forms.CheckBox();
            this.cboBearingType = new System.Windows.Forms.ComboBox();
            this.udBearingPrecision = new System.Windows.Forms.NumericUpDown();
            this.label3 = new System.Windows.Forms.Label();
            this.cboAngleFormat = new System.Windows.Forms.ComboBox();
            this.label2 = new System.Windows.Forms.Label();
            this.label1 = new System.Windows.Forms.Label();
            this.chkShowTotalLength = new System.Windows.Forms.CheckBox();
            this.udLengthPrecision = new System.Windows.Forms.NumericUpDown();
            this.label4 = new System.Windows.Forms.Label();
            this.label6 = new System.Windows.Forms.Label();
            this.cboLengthUnits = new System.Windows.Forms.ComboBox();
            this.btnOk = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.tabControl1 = new System.Windows.Forms.TabControl();
            this.tabPage2 = new System.Windows.Forms.TabPage();
            this.groupBox1 = new System.Windows.Forms.GroupBox();
            this.tabPage3 = new System.Windows.Forms.TabPage();
            this.cboAreaUnits = new System.Windows.Forms.ComboBox();
            this.label5 = new System.Windows.Forms.Label();
            this.label7 = new System.Windows.Forms.Label();
            this.udAreaPrecision = new System.Windows.Forms.NumericUpDown();
            this.tabPage1 = new System.Windows.Forms.TabPage();
            this.groupBox2 = new System.Windows.Forms.GroupBox();
            this.tabPage4 = new System.Windows.Forms.TabPage();
            this.label12 = new System.Windows.Forms.Label();
            this.label11 = new System.Windows.Forms.Label();
            this.fillTransparency = new MWLite.Symbology.Controls.TransparencyControl();
            this.label10 = new System.Windows.Forms.Label();
            this.clpFillColor = new MWLite.Symbology.Controls.ColorPicker.Office2007ColorPicker(this.components);
            this.label9 = new System.Windows.Forms.Label();
            this.label8 = new System.Windows.Forms.Label();
            this.clpLineColor = new MWLite.Symbology.Controls.ColorPicker.Office2007ColorPicker(this.components);
            this.imageList1 = new System.Windows.Forms.ImageList(this.components);
            this.cboLineWidth = new ImageCombo();
            this.cboLineStyle = new ImageCombo();
            this.chkShowPoints = new System.Windows.Forms.CheckBox();
            this.chkShowPointLabels = new System.Windows.Forms.CheckBox();
            ((System.ComponentModel.ISupportInitialize)(this.udBearingPrecision)).BeginInit();
            ((System.ComponentModel.ISupportInitialize)(this.udLengthPrecision)).BeginInit();
            this.tabControl1.SuspendLayout();
            this.tabPage2.SuspendLayout();
            this.groupBox1.SuspendLayout();
            this.tabPage3.SuspendLayout();
            ((System.ComponentModel.ISupportInitialize)(this.udAreaPrecision)).BeginInit();
            this.tabPage1.SuspendLayout();
            this.groupBox2.SuspendLayout();
            this.tabPage4.SuspendLayout();
            this.SuspendLayout();
            // 
            // chkShowBearing
            // 
            this.chkShowBearing.AutoSize = true;
            this.chkShowBearing.Location = new System.Drawing.Point(27, 23);
            this.chkShowBearing.Name = "chkShowBearing";
            this.chkShowBearing.Size = new System.Drawing.Size(91, 17);
            this.chkShowBearing.TabIndex = 0;
            this.chkShowBearing.Text = "Show bearing";
            this.chkShowBearing.UseVisualStyleBackColor = true;
            // 
            // chkShowLength
            // 
            this.chkShowLength.AutoSize = true;
            this.chkShowLength.Location = new System.Drawing.Point(27, 23);
            this.chkShowLength.Name = "chkShowLength";
            this.chkShowLength.Size = new System.Drawing.Size(85, 17);
            this.chkShowLength.TabIndex = 1;
            this.chkShowLength.Text = "Show length";
            this.chkShowLength.UseVisualStyleBackColor = true;
            // 
            // cboBearingType
            // 
            this.cboBearingType.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboBearingType.FormattingEnabled = true;
            this.cboBearingType.Location = new System.Drawing.Point(158, 19);
            this.cboBearingType.Name = "cboBearingType";
            this.cboBearingType.Size = new System.Drawing.Size(121, 21);
            this.cboBearingType.TabIndex = 2;
            // 
            // udBearingPrecision
            // 
            this.udBearingPrecision.Location = new System.Drawing.Point(159, 101);
            this.udBearingPrecision.Name = "udBearingPrecision";
            this.udBearingPrecision.Size = new System.Drawing.Size(120, 20);
            this.udBearingPrecision.TabIndex = 7;
            // 
            // label3
            // 
            this.label3.AutoSize = true;
            this.label3.Location = new System.Drawing.Point(21, 103);
            this.label3.Name = "label3";
            this.label3.Size = new System.Drawing.Size(50, 13);
            this.label3.TabIndex = 6;
            this.label3.Text = "Precision";
            // 
            // cboAngleFormat
            // 
            this.cboAngleFormat.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboAngleFormat.FormattingEnabled = true;
            this.cboAngleFormat.Location = new System.Drawing.Point(158, 59);
            this.cboAngleFormat.Name = "cboAngleFormat";
            this.cboAngleFormat.Size = new System.Drawing.Size(121, 21);
            this.cboAngleFormat.TabIndex = 5;
            // 
            // label2
            // 
            this.label2.AutoSize = true;
            this.label2.Location = new System.Drawing.Point(21, 62);
            this.label2.Name = "label2";
            this.label2.Size = new System.Drawing.Size(66, 13);
            this.label2.TabIndex = 4;
            this.label2.Text = "Angle format";
            // 
            // label1
            // 
            this.label1.AutoSize = true;
            this.label1.Location = new System.Drawing.Point(20, 22);
            this.label1.Name = "label1";
            this.label1.Size = new System.Drawing.Size(66, 13);
            this.label1.TabIndex = 3;
            this.label1.Text = "Bearing type";
            // 
            // chkShowTotalLength
            // 
            this.chkShowTotalLength.AutoSize = true;
            this.chkShowTotalLength.Location = new System.Drawing.Point(25, 102);
            this.chkShowTotalLength.Name = "chkShowTotalLength";
            this.chkShowTotalLength.Size = new System.Drawing.Size(108, 17);
            this.chkShowTotalLength.TabIndex = 8;
            this.chkShowTotalLength.Text = "Show total length";
            this.chkShowTotalLength.UseVisualStyleBackColor = true;
            // 
            // udLengthPrecision
            // 
            this.udLengthPrecision.Location = new System.Drawing.Point(161, 67);
            this.udLengthPrecision.Name = "udLengthPrecision";
            this.udLengthPrecision.Size = new System.Drawing.Size(120, 20);
            this.udLengthPrecision.TabIndex = 7;
            // 
            // label4
            // 
            this.label4.AutoSize = true;
            this.label4.Location = new System.Drawing.Point(23, 69);
            this.label4.Name = "label4";
            this.label4.Size = new System.Drawing.Size(50, 13);
            this.label4.TabIndex = 6;
            this.label4.Text = "Precision";
            // 
            // label6
            // 
            this.label6.AutoSize = true;
            this.label6.Location = new System.Drawing.Point(22, 33);
            this.label6.Name = "label6";
            this.label6.Size = new System.Drawing.Size(65, 13);
            this.label6.TabIndex = 3;
            this.label6.Text = "Length units";
            // 
            // cboLengthUnits
            // 
            this.cboLengthUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboLengthUnits.FormattingEnabled = true;
            this.cboLengthUnits.Location = new System.Drawing.Point(160, 30);
            this.cboLengthUnits.Name = "cboLengthUnits";
            this.cboLengthUnits.Size = new System.Drawing.Size(121, 21);
            this.cboLengthUnits.TabIndex = 2;
            // 
            // btnOk
            // 
            this.btnOk.Location = new System.Drawing.Point(173, 236);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(90, 26);
            this.btnOk.TabIndex = 5;
            this.btnOk.Text = "Ok";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.btnOk_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(269, 236);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(85, 26);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // tabControl1
            // 
            this.tabControl1.Controls.Add(this.tabPage2);
            this.tabControl1.Controls.Add(this.tabPage3);
            this.tabControl1.Controls.Add(this.tabPage1);
            this.tabControl1.Controls.Add(this.tabPage4);
            this.tabControl1.Location = new System.Drawing.Point(8, 3);
            this.tabControl1.Name = "tabControl1";
            this.tabControl1.SelectedIndex = 0;
            this.tabControl1.Size = new System.Drawing.Size(355, 231);
            this.tabControl1.TabIndex = 7;
            // 
            // tabPage2
            // 
            this.tabPage2.Controls.Add(this.groupBox1);
            this.tabPage2.Controls.Add(this.chkShowLength);
            this.tabPage2.Location = new System.Drawing.Point(4, 22);
            this.tabPage2.Name = "tabPage2";
            this.tabPage2.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage2.Size = new System.Drawing.Size(347, 205);
            this.tabPage2.TabIndex = 1;
            this.tabPage2.Text = "Length";
            this.tabPage2.UseVisualStyleBackColor = true;
            // 
            // groupBox1
            // 
            this.groupBox1.Controls.Add(this.cboLengthUnits);
            this.groupBox1.Controls.Add(this.chkShowTotalLength);
            this.groupBox1.Controls.Add(this.label4);
            this.groupBox1.Controls.Add(this.label6);
            this.groupBox1.Controls.Add(this.udLengthPrecision);
            this.groupBox1.Location = new System.Drawing.Point(16, 46);
            this.groupBox1.Name = "groupBox1";
            this.groupBox1.Size = new System.Drawing.Size(312, 136);
            this.groupBox1.TabIndex = 9;
            this.groupBox1.TabStop = false;
            // 
            // tabPage3
            // 
            this.tabPage3.Controls.Add(this.cboAreaUnits);
            this.tabPage3.Controls.Add(this.label5);
            this.tabPage3.Controls.Add(this.label7);
            this.tabPage3.Controls.Add(this.udAreaPrecision);
            this.tabPage3.Location = new System.Drawing.Point(4, 22);
            this.tabPage3.Name = "tabPage3";
            this.tabPage3.Size = new System.Drawing.Size(347, 205);
            this.tabPage3.TabIndex = 2;
            this.tabPage3.Text = "Area";
            this.tabPage3.UseVisualStyleBackColor = true;
            // 
            // cboAreaUnits
            // 
            this.cboAreaUnits.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboAreaUnits.FormattingEnabled = true;
            this.cboAreaUnits.Location = new System.Drawing.Point(132, 31);
            this.cboAreaUnits.Name = "cboAreaUnits";
            this.cboAreaUnits.Size = new System.Drawing.Size(150, 21);
            this.cboAreaUnits.TabIndex = 2;
            // 
            // label5
            // 
            this.label5.AutoSize = true;
            this.label5.Location = new System.Drawing.Point(24, 70);
            this.label5.Name = "label5";
            this.label5.Size = new System.Drawing.Size(50, 13);
            this.label5.TabIndex = 6;
            this.label5.Text = "Precision";
            // 
            // label7
            // 
            this.label7.AutoSize = true;
            this.label7.Location = new System.Drawing.Point(23, 34);
            this.label7.Name = "label7";
            this.label7.Size = new System.Drawing.Size(54, 13);
            this.label7.TabIndex = 3;
            this.label7.Text = "Area units";
            // 
            // udAreaPrecision
            // 
            this.udAreaPrecision.Location = new System.Drawing.Point(132, 68);
            this.udAreaPrecision.Name = "udAreaPrecision";
            this.udAreaPrecision.Size = new System.Drawing.Size(150, 20);
            this.udAreaPrecision.TabIndex = 7;
            // 
            // tabPage1
            // 
            this.tabPage1.Controls.Add(this.groupBox2);
            this.tabPage1.Controls.Add(this.chkShowBearing);
            this.tabPage1.Location = new System.Drawing.Point(4, 22);
            this.tabPage1.Name = "tabPage1";
            this.tabPage1.Padding = new System.Windows.Forms.Padding(3);
            this.tabPage1.Size = new System.Drawing.Size(347, 205);
            this.tabPage1.TabIndex = 0;
            this.tabPage1.Text = "Bearing";
            this.tabPage1.UseVisualStyleBackColor = true;
            // 
            // groupBox2
            // 
            this.groupBox2.Controls.Add(this.udBearingPrecision);
            this.groupBox2.Controls.Add(this.label1);
            this.groupBox2.Controls.Add(this.cboBearingType);
            this.groupBox2.Controls.Add(this.label2);
            this.groupBox2.Controls.Add(this.label3);
            this.groupBox2.Controls.Add(this.cboAngleFormat);
            this.groupBox2.Location = new System.Drawing.Point(16, 46);
            this.groupBox2.Name = "groupBox2";
            this.groupBox2.Size = new System.Drawing.Size(304, 142);
            this.groupBox2.TabIndex = 8;
            this.groupBox2.TabStop = false;
            // 
            // tabPage4
            // 
            this.tabPage4.Controls.Add(this.chkShowPointLabels);
            this.tabPage4.Controls.Add(this.chkShowPoints);
            this.tabPage4.Controls.Add(this.cboLineStyle);
            this.tabPage4.Controls.Add(this.cboLineWidth);
            this.tabPage4.Controls.Add(this.label12);
            this.tabPage4.Controls.Add(this.label11);
            this.tabPage4.Controls.Add(this.fillTransparency);
            this.tabPage4.Controls.Add(this.label10);
            this.tabPage4.Controls.Add(this.clpFillColor);
            this.tabPage4.Controls.Add(this.label9);
            this.tabPage4.Controls.Add(this.label8);
            this.tabPage4.Controls.Add(this.clpLineColor);
            this.tabPage4.Location = new System.Drawing.Point(4, 22);
            this.tabPage4.Name = "tabPage4";
            this.tabPage4.Size = new System.Drawing.Size(347, 205);
            this.tabPage4.TabIndex = 3;
            this.tabPage4.Text = "Appearance";
            this.tabPage4.UseVisualStyleBackColor = true;
            // 
            // label12
            // 
            this.label12.AutoSize = true;
            this.label12.Location = new System.Drawing.Point(173, 74);
            this.label12.Name = "label12";
            this.label12.Size = new System.Drawing.Size(51, 13);
            this.label12.TabIndex = 7;
            this.label12.Text = "Line style";
            // 
            // label11
            // 
            this.label11.AutoSize = true;
            this.label11.Location = new System.Drawing.Point(173, 33);
            this.label11.Name = "label11";
            this.label11.Size = new System.Drawing.Size(55, 13);
            this.label11.TabIndex = 6;
            this.label11.Text = "Line width";
            // 
            // transparencyControl1
            // 
            this.fillTransparency.BandColor = System.Drawing.Color.Empty;
            this.fillTransparency.Location = new System.Drawing.Point(130, 113);
            this.fillTransparency.MaximumSize = new System.Drawing.Size(1024, 32);
            this.fillTransparency.MinimumSize = new System.Drawing.Size(128, 32);
            this.fillTransparency.Name = "fillTransparency";
            this.fillTransparency.Size = new System.Drawing.Size(190, 32);
            this.fillTransparency.TabIndex = 5;
            this.fillTransparency.Value = ((byte)(255));
            // 
            // label10
            // 
            this.label10.AutoSize = true;
            this.label10.Location = new System.Drawing.Point(22, 113);
            this.label10.Name = "label10";
            this.label10.Size = new System.Drawing.Size(83, 13);
            this.label10.TabIndex = 4;
            this.label10.Text = "Fill transparency";
            // 
            // clpFillColor
            // 
            this.clpFillColor.Color = System.Drawing.Color.Black;
            this.clpFillColor.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.clpFillColor.DropDownHeight = 1;
            this.clpFillColor.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.clpFillColor.FormattingEnabled = true;
            this.clpFillColor.IntegralHeight = false;
            this.clpFillColor.Items.AddRange(new object[] {
            "Color"});
            this.clpFillColor.Location = new System.Drawing.Point(79, 71);
            this.clpFillColor.Name = "clpFillColor";
            this.clpFillColor.Size = new System.Drawing.Size(79, 21);
            this.clpFillColor.TabIndex = 3;
            // 
            // label9
            // 
            this.label9.AutoSize = true;
            this.label9.Location = new System.Drawing.Point(22, 74);
            this.label9.Name = "label9";
            this.label9.Size = new System.Drawing.Size(45, 13);
            this.label9.TabIndex = 2;
            this.label9.Text = "Fill color";
            // 
            // label8
            // 
            this.label8.AutoSize = true;
            this.label8.Location = new System.Drawing.Point(20, 33);
            this.label8.Name = "label8";
            this.label8.Size = new System.Drawing.Size(53, 13);
            this.label8.TabIndex = 1;
            this.label8.Text = "Line color";
            // 
            // clpLineColor
            // 
            this.clpLineColor.Color = System.Drawing.Color.Black;
            this.clpLineColor.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.clpLineColor.DropDownHeight = 1;
            this.clpLineColor.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.clpLineColor.FormattingEnabled = true;
            this.clpLineColor.IntegralHeight = false;
            this.clpLineColor.Items.AddRange(new object[] {
            "Color"});
            this.clpLineColor.Location = new System.Drawing.Point(79, 30);
            this.clpLineColor.Name = "clpLineColor";
            this.clpLineColor.Size = new System.Drawing.Size(79, 21);
            this.clpLineColor.TabIndex = 0;
            // 
            // imageList1
            // 
            this.imageList1.ColorDepth = System.Windows.Forms.ColorDepth.Depth8Bit;
            this.imageList1.ImageSize = new System.Drawing.Size(16, 16);
            this.imageList1.TransparentColor = System.Drawing.Color.Transparent;
            // 
            // cboLineWidth
            // 
            this.cboLineWidth.Color1 = System.Drawing.Color.Gray;
            this.cboLineWidth.Color2 = System.Drawing.Color.Gray;
            this.cboLineWidth.ComboStyle = MWLite.Symbology.Controls.ImageComboStyle.LineWidth;
            this.cboLineWidth.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.cboLineWidth.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboLineWidth.FormattingEnabled = true;
            this.cboLineWidth.Location = new System.Drawing.Point(234, 30);
            this.cboLineWidth.Name = "cboLineWidth";
            this.cboLineWidth.OutlineColor = System.Drawing.Color.Black;
            this.cboLineWidth.Size = new System.Drawing.Size(86, 21);
            this.cboLineWidth.TabIndex = 8;
            // 
            // cboLineStyle
            // 
            this.cboLineStyle.Color1 = System.Drawing.Color.Gray;
            this.cboLineStyle.Color2 = System.Drawing.Color.Gray;
            this.cboLineStyle.ComboStyle = MWLite.Symbology.Controls.ImageComboStyle.LineStyle;
            this.cboLineStyle.DrawMode = System.Windows.Forms.DrawMode.OwnerDrawFixed;
            this.cboLineStyle.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboLineStyle.FormattingEnabled = true;
            this.cboLineStyle.Location = new System.Drawing.Point(234, 71);
            this.cboLineStyle.Name = "cboLineStyle";
            this.cboLineStyle.OutlineColor = System.Drawing.Color.Black;
            this.cboLineStyle.Size = new System.Drawing.Size(86, 21);
            this.cboLineStyle.TabIndex = 9;
            // 
            // chkShowPoints
            // 
            this.chkShowPoints.AutoSize = true;
            this.chkShowPoints.Location = new System.Drawing.Point(27, 165);
            this.chkShowPoints.Name = "chkShowPoints";
            this.chkShowPoints.Size = new System.Drawing.Size(84, 17);
            this.chkShowPoints.TabIndex = 10;
            this.chkShowPoints.Text = "Show points";
            this.chkShowPoints.UseVisualStyleBackColor = true;
            // 
            // chkShowPointLabels
            // 
            this.chkShowPointLabels.AutoSize = true;
            this.chkShowPointLabels.Location = new System.Drawing.Point(128, 165);
            this.chkShowPointLabels.Name = "chkShowPointLabels";
            this.chkShowPointLabels.Size = new System.Drawing.Size(109, 17);
            this.chkShowPointLabels.TabIndex = 11;
            this.chkShowPointLabels.Text = "Show point labels";
            this.chkShowPointLabels.UseVisualStyleBackColor = true;
            // 
            // MeasuringForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(366, 268);
            this.Controls.Add(this.tabControl1);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOk);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedDialog;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "MeasuringForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Measuring options";
            ((System.ComponentModel.ISupportInitialize)(this.udBearingPrecision)).EndInit();
            ((System.ComponentModel.ISupportInitialize)(this.udLengthPrecision)).EndInit();
            this.tabControl1.ResumeLayout(false);
            this.tabPage2.ResumeLayout(false);
            this.tabPage2.PerformLayout();
            this.groupBox1.ResumeLayout(false);
            this.groupBox1.PerformLayout();
            this.tabPage3.ResumeLayout(false);
            this.tabPage3.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.udAreaPrecision)).EndInit();
            this.tabPage1.ResumeLayout(false);
            this.tabPage1.PerformLayout();
            this.groupBox2.ResumeLayout(false);
            this.groupBox2.PerformLayout();
            this.tabPage4.ResumeLayout(false);
            this.tabPage4.PerformLayout();
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.CheckBox chkShowBearing;
        private System.Windows.Forms.CheckBox chkShowLength;
        private System.Windows.Forms.ComboBox cboBearingType;
        private System.Windows.Forms.NumericUpDown udBearingPrecision;
        private System.Windows.Forms.Label label3;
        private System.Windows.Forms.ComboBox cboAngleFormat;
        private System.Windows.Forms.Label label2;
        private System.Windows.Forms.Label label1;
        private System.Windows.Forms.CheckBox chkShowTotalLength;
        private System.Windows.Forms.NumericUpDown udLengthPrecision;
        private System.Windows.Forms.Label label4;
        private System.Windows.Forms.Label label6;
        private System.Windows.Forms.ComboBox cboLengthUnits;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.TabControl tabControl1;
        private System.Windows.Forms.TabPage tabPage2;
        private System.Windows.Forms.GroupBox groupBox1;
        private System.Windows.Forms.TabPage tabPage3;
        private System.Windows.Forms.TabPage tabPage1;
        private System.Windows.Forms.GroupBox groupBox2;
        private System.Windows.Forms.TabPage tabPage4;
        private System.Windows.Forms.ComboBox cboAreaUnits;
        private System.Windows.Forms.Label label5;
        private System.Windows.Forms.Label label7;
        private System.Windows.Forms.NumericUpDown udAreaPrecision;
        private System.Windows.Forms.Label label10;
        private Symbology.Controls.ColorPicker.Office2007ColorPicker clpFillColor;
        private System.Windows.Forms.Label label9;
        private System.Windows.Forms.Label label8;
        private Symbology.Controls.ColorPicker.Office2007ColorPicker clpLineColor;
        private MWLite.Symbology.Controls.TransparencyControl fillTransparency;
        private System.Windows.Forms.Label label12;
        private System.Windows.Forms.Label label11;
        private System.Windows.Forms.ImageList imageList1;
        private System.Windows.Forms.CheckBox chkShowPointLabels;
        private System.Windows.Forms.CheckBox chkShowPoints;
        private ImageCombo cboLineStyle;
        private ImageCombo cboLineWidth;
    }
}