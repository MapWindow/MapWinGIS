namespace MWLite.GUI.Forms
{
    partial class SetProjectionForm
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
            this.txtDefinition = new System.Windows.Forms.TextBox();
            this.btnOk = new System.Windows.Forms.Button();
            this.btnCancel = new System.Windows.Forms.Button();
            this.optEmpty = new System.Windows.Forms.RadioButton();
            this.optWellKnown = new System.Windows.Forms.RadioButton();
            this.cboWellKnown = new System.Windows.Forms.ComboBox();
            this.optDefinition = new System.Windows.Forms.RadioButton();
            this.SuspendLayout();
            // 
            // txtDefinition
            // 
            this.txtDefinition.Location = new System.Drawing.Point(16, 117);
            this.txtDefinition.Multiline = true;
            this.txtDefinition.Name = "txtDefinition";
            this.txtDefinition.Size = new System.Drawing.Size(458, 76);
            this.txtDefinition.TabIndex = 0;
            // 
            // btnOk
            // 
            this.btnOk.Location = new System.Drawing.Point(318, 210);
            this.btnOk.Name = "btnOk";
            this.btnOk.Size = new System.Drawing.Size(75, 23);
            this.btnOk.TabIndex = 1;
            this.btnOk.Text = "Ok";
            this.btnOk.UseVisualStyleBackColor = true;
            this.btnOk.Click += new System.EventHandler(this.button1_Click);
            // 
            // btnCancel
            // 
            this.btnCancel.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.btnCancel.Location = new System.Drawing.Point(399, 210);
            this.btnCancel.Name = "btnCancel";
            this.btnCancel.Size = new System.Drawing.Size(75, 23);
            this.btnCancel.TabIndex = 2;
            this.btnCancel.Text = "Cancel";
            this.btnCancel.UseVisualStyleBackColor = true;
            // 
            // optEmpty
            // 
            this.optEmpty.AutoSize = true;
            this.optEmpty.Location = new System.Drawing.Point(16, 19);
            this.optEmpty.Name = "optEmpty";
            this.optEmpty.Size = new System.Drawing.Size(219, 17);
            this.optEmpty.TabIndex = 5;
            this.optEmpty.TabStop = true;
            this.optEmpty.Text = "Empty (will be grabbed from the first layer)";
            this.optEmpty.UseVisualStyleBackColor = true;
            // 
            // optWellKnown
            // 
            this.optWellKnown.AutoSize = true;
            this.optWellKnown.Location = new System.Drawing.Point(16, 52);
            this.optWellKnown.Name = "optWellKnown";
            this.optWellKnown.Size = new System.Drawing.Size(130, 17);
            this.optWellKnown.TabIndex = 6;
            this.optWellKnown.TabStop = true;
            this.optWellKnown.Text = "Well known projection";
            this.optWellKnown.UseVisualStyleBackColor = true;
            // 
            // cboWellKnown
            // 
            this.cboWellKnown.DropDownStyle = System.Windows.Forms.ComboBoxStyle.DropDownList;
            this.cboWellKnown.FormattingEnabled = true;
            this.cboWellKnown.Location = new System.Drawing.Point(168, 51);
            this.cboWellKnown.Name = "cboWellKnown";
            this.cboWellKnown.Size = new System.Drawing.Size(268, 21);
            this.cboWellKnown.TabIndex = 7;
            // 
            // optDefinition
            // 
            this.optDefinition.AutoSize = true;
            this.optDefinition.Checked = true;
            this.optDefinition.Location = new System.Drawing.Point(16, 85);
            this.optDefinition.Name = "optDefinition";
            this.optDefinition.Size = new System.Drawing.Size(358, 17);
            this.optDefinition.TabIndex = 8;
            this.optDefinition.TabStop = true;
            this.optDefinition.Text = "Enter projection definition in any form (e.g. PROJ4, WKT, EPSG code):";
            this.optDefinition.UseVisualStyleBackColor = true;
            // 
            // SetProjectionForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.btnCancel;
            this.ClientSize = new System.Drawing.Size(486, 248);
            this.Controls.Add(this.optDefinition);
            this.Controls.Add(this.cboWellKnown);
            this.Controls.Add(this.optWellKnown);
            this.Controls.Add(this.optEmpty);
            this.Controls.Add(this.btnCancel);
            this.Controls.Add(this.btnOk);
            this.Controls.Add(this.txtDefinition);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "SetProjectionForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Set Map Projection";
            this.ResumeLayout(false);
            this.PerformLayout();

        }

        #endregion

        private System.Windows.Forms.TextBox txtDefinition;
        private System.Windows.Forms.Button btnOk;
        private System.Windows.Forms.Button btnCancel;
        private System.Windows.Forms.RadioButton optEmpty;
        private System.Windows.Forms.RadioButton optWellKnown;
        private System.Windows.Forms.ComboBox cboWellKnown;
        private System.Windows.Forms.RadioButton optDefinition;
    }
}