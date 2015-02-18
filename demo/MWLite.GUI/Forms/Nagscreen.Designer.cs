namespace MWLite.GUI.Forms
{
    partial class Nagscreen
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
            this.splitContainer1 = new System.Windows.Forms.SplitContainer();
            this.webBrowser1 = new System.Windows.Forms.WebBrowser();
            this.ShowOnStartupCheckbox = new System.Windows.Forms.CheckBox();
            this.ProgressLabel = new System.Windows.Forms.Label();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).BeginInit();
            this.splitContainer1.Panel1.SuspendLayout();
            this.splitContainer1.Panel2.SuspendLayout();
            this.splitContainer1.SuspendLayout();
            this.SuspendLayout();
            // 
            // splitContainer1
            // 
            this.splitContainer1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.splitContainer1.FixedPanel = System.Windows.Forms.FixedPanel.Panel2;
            this.splitContainer1.IsSplitterFixed = true;
            this.splitContainer1.Location = new System.Drawing.Point(0, 0);
            this.splitContainer1.Name = "splitContainer1";
            this.splitContainer1.Orientation = System.Windows.Forms.Orientation.Horizontal;
            // 
            // splitContainer1.Panel1
            // 
            this.splitContainer1.Panel1.Controls.Add(this.webBrowser1);
            // 
            // splitContainer1.Panel2
            // 
            this.splitContainer1.Panel2.Controls.Add(this.ProgressLabel);
            this.splitContainer1.Panel2.Controls.Add(this.ShowOnStartupCheckbox);
            this.splitContainer1.Size = new System.Drawing.Size(750, 500);
            this.splitContainer1.SplitterDistance = 455;
            this.splitContainer1.TabIndex = 1;
            // 
            // webBrowser1
            // 
            this.webBrowser1.AllowNavigation = false;
            this.webBrowser1.AllowWebBrowserDrop = false;
            this.webBrowser1.Dock = System.Windows.Forms.DockStyle.Fill;
            this.webBrowser1.IsWebBrowserContextMenuEnabled = false;
            this.webBrowser1.Location = new System.Drawing.Point(0, 0);
            this.webBrowser1.MinimumSize = new System.Drawing.Size(20, 20);
            this.webBrowser1.Name = "webBrowser1";
            this.webBrowser1.ScriptErrorsSuppressed = true;
            this.webBrowser1.Size = new System.Drawing.Size(750, 455);
            this.webBrowser1.TabIndex = 1;
            this.webBrowser1.Url = new System.Uri("http://www.mapwindow.org/documentation/mapwingis4.9/MapWindow49.html?utm_source=M" +
        "Wv49&utm_medium=cpc&utm_campaign=nagscreen", System.UriKind.Absolute);
            this.webBrowser1.WebBrowserShortcutsEnabled = false;
            this.webBrowser1.DocumentCompleted += new System.Windows.Forms.WebBrowserDocumentCompletedEventHandler(this.webBrowser1_DocumentCompleted);
            // 
            // ShowOnStartupCheckbox
            // 
            this.ShowOnStartupCheckbox.AutoSize = true;
            this.ShowOnStartupCheckbox.Checked = true;
            this.ShowOnStartupCheckbox.CheckState = System.Windows.Forms.CheckState.Checked;
            this.ShowOnStartupCheckbox.Location = new System.Drawing.Point(12, 11);
            this.ShowOnStartupCheckbox.Name = "ShowOnStartupCheckbox";
            this.ShowOnStartupCheckbox.Size = new System.Drawing.Size(103, 17);
            this.ShowOnStartupCheckbox.TabIndex = 0;
            this.ShowOnStartupCheckbox.Text = "Show on startup";
            this.ShowOnStartupCheckbox.UseVisualStyleBackColor = true;
            this.ShowOnStartupCheckbox.CheckedChanged += new System.EventHandler(this.ShowOnStartupCheckbox_CheckedChanged);
            // 
            // ProgressLabel
            // 
            this.ProgressLabel.AutoSize = true;
            this.ProgressLabel.Location = new System.Drawing.Point(121, 12);
            this.ProgressLabel.Name = "ProgressLabel";
            this.ProgressLabel.Size = new System.Drawing.Size(0, 13);
            this.ProgressLabel.TabIndex = 1;
            this.ProgressLabel.TextChanged += new System.EventHandler(this.ProgressLabel_TextChanged);
            // 
            // Nagscreen
            // 
            this.AutoScaleDimensions = new System.Drawing.SizeF(6F, 13F);
            this.AutoScaleMode = System.Windows.Forms.AutoScaleMode.Font;
            this.ClientSize = new System.Drawing.Size(750, 500);
            this.Controls.Add(this.splitContainer1);
            this.FormBorderStyle = System.Windows.Forms.FormBorderStyle.FixedToolWindow;
            this.MinimizeBox = false;
            this.MinimumSize = new System.Drawing.Size(750, 500);
            this.Name = "Nagscreen";
            this.ShowIcon = false;
            this.ShowInTaskbar = false;
            this.StartPosition = System.Windows.Forms.FormStartPosition.CenterScreen;
            this.Text = "Windows v4.9 Lite";
            this.splitContainer1.Panel1.ResumeLayout(false);
            this.splitContainer1.Panel2.ResumeLayout(false);
            this.splitContainer1.Panel2.PerformLayout();
            ((System.ComponentModel.ISupportInitialize)(this.splitContainer1)).EndInit();
            this.splitContainer1.ResumeLayout(false);
            this.ResumeLayout(false);

        }

        #endregion

        private System.Windows.Forms.SplitContainer splitContainer1;
        private System.Windows.Forms.WebBrowser webBrowser1;
        private System.Windows.Forms.CheckBox ShowOnStartupCheckbox;
        public System.Windows.Forms.Label ProgressLabel;
    }
}