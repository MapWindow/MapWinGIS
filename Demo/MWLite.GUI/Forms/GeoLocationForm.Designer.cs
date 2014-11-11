namespace MWLite.GUI.Forms
{
    partial class GeoLocationForm
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
            this.locationControl1 = new MWLite.GUI.Controls.LocationControl();
            this.button1 = new System.Windows.Forms.Button();
            this.SuspendLayout();
            // 
            // locationControl1
            // 
            this.locationControl1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.locationControl1.Location = new System.Drawing.Point(0, 0);
            this.locationControl1.Name = "locationControl1";
            this.locationControl1.Size = new System.Drawing.Size(375, 204);
            this.locationControl1.TabIndex = 4;
            // 
            // button1
            // 
            this.button1.DialogResult = System.Windows.Forms.DialogResult.Cancel;
            this.button1.Location = new System.Drawing.Point(66, 168);
            this.button1.Name = "button1";
            this.button1.Size = new System.Drawing.Size(35, 21);
            this.button1.TabIndex = 5;
            this.button1.UseVisualStyleBackColor = true;
            // 
            // GeoLocationForm
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.CancelButton = this.button1;
            this.ClientSize = new System.Drawing.Size(375, 204);
            this.Controls.Add(this.locationControl1);
            this.Controls.Add(this.button1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedSingle;
            this.MaximizeBox = false;
            this.MinimizeBox = false;
            this.Name = "GeoLocationForm";
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterParent;
            this.Text = "Set Map Extents";
            this.ResumeLayout(false);

        }

        #endregion

        private Controls.LocationControl locationControl1;
        private System.Windows.Forms.Button button1;
    }
}