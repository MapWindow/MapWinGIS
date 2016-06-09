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
          this.axMap1 = new AxMapWinGIS.AxMap();
          this.button1 = new System.Windows.Forms.Button();
          ((System.ComponentModel.ISupportInitialize)(this.axMap1)).BeginInit();
          this.SuspendLayout();
          // 
          // axMap1
          // 
          this.axMap1.Enabled = true;
          this.axMap1.Location = new System.Drawing.Point(45, 37);
          this.axMap1.Name = "axMap1";
          this.axMap1.OcxState = ((System.Windows.Forms.AxHost.State)(resources.GetObject("axMap1.OcxState")));
          this.axMap1.Size = new System.Drawing.Size(211, 215);
          this.axMap1.TabIndex = 0;
          // 
          // button1
          // 
          this.button1.Location = new System.Drawing.Point(339, 69);
          this.button1.Name = "button1";
          this.button1.Size = new System.Drawing.Size(136, 31);
          this.button1.TabIndex = 1;
          this.button1.Text = "ClipGridWithPolygon";
          this.button1.UseVisualStyleBackColor = true;
          this.button1.Click += new System.EventHandler(this.button1_Click);
          // 
          // Form1
          // 
          this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
          this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
          this.ClientSize = new System.Drawing.Size(510, 388);
          this.Controls.Add(this.button1);
          this.Controls.Add(this.axMap1);
          this.Name = "Form1";
          this.Text = "Form1";
          ((System.ComponentModel.ISupportInitialize)(this.axMap1)).EndInit();
          this.ResumeLayout(false);

        }

        #endregion

        private AxMapWinGIS.AxMap axMap1;
        private System.Windows.Forms.Button button1;




    }
}

