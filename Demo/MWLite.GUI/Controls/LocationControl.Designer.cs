namespace MWLite.GUI.Controls
{
    partial class LocationControl
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
            this.optKnownExtents = new System.Windows.Forms.RadioButton();
            this.optFindLocation = new System.Windows.Forms.RadioButton();
            this.txtFindLocation = new System.Windows.Forms.TextBox();
            this.cboKnownExtents = new System.Windows.Forms.ComboBox();
            this.btnApply = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // optKnownExtents
            // 
            this.optKnownExtents.AutoSize = true;
            this.optKnownExtents.Checked = true;
            this.optKnownExtents.Location = new System.Drawing.Point(17, 22);
            this.optKnownExtents.Name = "optKnownExtents";
            this.optKnownExtents.Size = new System.Drawing.Size(147, 17);
            this.optKnownExtents.TabIndex = 0;
            this.optKnownExtents.TabStop = true;
            this.optKnownExtents.Text = "Known extents (countries)";
            this.optKnownExtents.UseVisualStyleBackColor = true;
            // 
            // optFindLocation
            // 
            this.optFindLocation.AutoSize = true;
            this.optFindLocation.Enabled = false;
            this.optFindLocation.Location = new System.Drawing.Point(17, 94);
            this.optFindLocation.Name = "optFindLocation";
            this.optFindLocation.Size = new System.Drawing.Size(260, 17);
            this.optFindLocation.TabIndex = 1;
            this.optFindLocation.Text = "Find location (e.g. Boston, Spain, Black Sea, etc.)";
            this.optFindLocation.UseVisualStyleBackColor = true;
            // 
            // txtFindLocation
            // 
            this.txtFindLocation.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.txtFindLocation.Enabled = false;
            this.txtFindLocation.Location = new System.Drawing.Point(17, 117);
            this.txtFindLocation.Name = "txtFindLocation";
            this.txtFindLocation.Size = new System.Drawing.Size(258, 20);
            this.txtFindLocation.TabIndex = 3;
            // 
            // cboKnownExtents
            // 
            this.cboKnownExtents.Anchor = ((System.Windows.Forms.AnchorStyles)(((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Left) 
            | System.Windows.Forms.AnchorStyles.Right)));
            this.cboKnownExtents.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboKnownExtents.FormattingEnabled = true;
            this.cboKnownExtents.Location = new System.Drawing.Point(17, 45);
            this.cboKnownExtents.Name = "cboKnownExtents";
            this.cboKnownExtents.Size = new System.Drawing.Size(258, 21);
            this.cboKnownExtents.TabIndex = 4;
            // 
            // btnApply
            // 
            this.btnApply.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnApply.Location = new System.Drawing.Point(102, 157);
            this.btnApply.Name = "btnApply";
            this.btnApply.Size = new System.Drawing.Size(88, 30);
            this.btnApply.TabIndex = 5;
            this.btnApply.Text = "Apply";
            this.btnApply.UseVisualStyleBackColor = true;
            this.btnApply.Click += new System.EventHandler(this.btnApply_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.Anchor = ((System.Windows.Forms.AnchorStyles)((System.Windows.Forms.AnchorStyles.Top | System.Windows.Forms.AnchorStyles.Right)));
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(196, 157);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(88, 30);
            this.btnCancel.TabIndex = 6;
            this.btnCancel.Text = "Close";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // LocationControl
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnApply);
            this.Controls.Add(this.cboKnownExtents);
            this.Controls.Add(this.txtFindLocation);
            this.Controls.Add(this.optFindLocation);
            this.Controls.Add(this.optKnownExtents);
            this.Name = "LocationControl";
            this.Size = new System.Drawing.Size(293, 201);
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.RadioButton optKnownExtents;
        private System.Windows.Forms.RadioButton optFindLocation;
        private System.Windows.Forms.TextBox txtFindLocation;
        private System.Windows.Forms.ComboBox cboKnownExtents;
        private System.Windows.Forms.Button btnApply;
        private System.Windows.Forms.Button btnCancel;
    }
}
