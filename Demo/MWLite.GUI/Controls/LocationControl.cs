using System;
using System.Linq;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core;
using MWLite.Core.GeoLocation;
using MWLite.Core.UI;
using MWLite.GUI.Events;
using MWLite.GUI.Helpers;

namespace MWLite.GUI.Controls
{
    public partial class LocationControl : UserControl
    {
        public event EventHandler<NewExtentsEventArgs> NewExtents;

        protected virtual void OnNewExtents(NewExtentsEventArgs e)
        {
            var handler = NewExtents;
            if (handler != null) handler(this, e);
        }

        public LocationControl()
        {
            InitializeComponent();

            optKnownExtents.CheckedChanged += (s, e) => RefreshControls();

            var list = Enum.GetValues(typeof (tkKnownExtents));
            cboKnownExtents.DataSource = list.OfType<tkKnownExtents>().ToList();
            cboKnownExtents.SelectedIndex = 0;

            txtFindLocation.KeyDown += (s, e) => { if (e.KeyCode == Keys.Enter) Run(); };

            RefreshControls();
        }

        private void RefreshControls()
        {
            cboKnownExtents.Enabled = optKnownExtents.Checked;
            txtFindLocation.Enabled = optFindLocation.Checked;
        }

        private void btnApply_Click(object sender, EventArgs e)
        {
            Run();
        }

        private void Run()
        {
            if (optFindLocation.Checked)
            {
                if (string.IsNullOrWhiteSpace(txtFindLocation.Text))
                {
                    MessageHelper.Info("Enter the name of location.");
                    return;
                }

                try
                {
                    var box = GeoLocationHelper.FindLocation(txtFindLocation.Text);
                    OnNewExtents(new NewExtentsEventArgs()
                    {
                        ExtentsType = ExtentType.Geogrpahic,
                        GeographicExtents = box
                    });
                }
                catch (Exception ex)
                {
                    MessageHelper.Warn(ex.Message);
                }
            }
            if (optKnownExtents.Checked)
            {
                OnNewExtents(new NewExtentsEventArgs()
                {
                    ExtentsType = ExtentType.Known,
                    KnownExtents = (tkKnownExtents)cboKnownExtents.SelectedItem
                });
            }
        }
    }
}
