using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.GUI.Helpers;
using MWLite.Symbology.Classes;

namespace MWLite.GUI.Forms
{
    public partial class MeasuringForm : Form
    {
        private readonly IMeasuring _measuring;
        private static int _selectedTab = -1;

        public MeasuringForm(IMeasuring measuring)
        {
            if (measuring == null) throw new ArgumentNullException("measuring");
            _measuring = measuring;

            InitializeComponent();

            ModelToUi();

            tabControl1.SelectedIndex = _selectedTab;

            FormClosed += (s, e) => _selectedTab = tabControl1.SelectedIndex;
        }

        private void ModelToUi()
        {
            cboAngleFormat.AddItemsFromEnum<tkAngleFormat>();
            cboBearingType.AddItemsFromEnum<tkBearingType>();
            cboLengthUnits.AddItemsFromEnum<tkLengthDisplayMode>();
            cboAreaUnits.AddItemsFromEnum<tkAreaDisplayMode>();

            cboAngleFormat.SetValue(_measuring.AngleFormat);
            cboBearingType.SetValue(_measuring.BearingType);
            cboLengthUnits.SetValue(_measuring.LengthUnits);
            cboAreaUnits.SetValue(_measuring.AreaUnits);

            chkShowBearing.Checked = _measuring.ShowBearing;
            chkShowLength.Checked = _measuring.ShowLength;
            chkShowTotalLength.Checked = _measuring.ShowTotalLength;

            udLengthPrecision.Value = _measuring.LengthPrecision;
            udBearingPrecision.Value = _measuring.AnglePrecision;
            udAreaPrecision.Value = _measuring.AreaPrecision;

            chkShowPoints.Checked = _measuring.PointsVisible;
            chkShowPointLabels.Checked = _measuring.PointLabelsVisible;

            clpFillColor.Color = Colors.UintToColor(_measuring.FillColor);
            clpLineColor.Color = Colors.UintToColor(_measuring.LineColor);

            cboLineStyle.SelectedIndex = (int)_measuring.LineStyle;
            cboLineWidth.SelectedIndex = (int) (_measuring.LineWidth - 1);

            fillTransparency.Value = _measuring.FillTransparency;
        }

        private void UiToModel()
        {
            _measuring.ShowBearing = chkShowBearing.Checked;
            _measuring.ShowLength = chkShowLength.Checked;
            _measuring.ShowTotalLength = chkShowTotalLength.Checked;

            _measuring.AngleFormat = cboAngleFormat.GetValue<tkAngleFormat>();
            _measuring.BearingType = cboBearingType.GetValue<tkBearingType>();
            _measuring.LengthUnits = cboLengthUnits.GetValue<tkLengthDisplayMode>();
            _measuring.AreaUnits = cboAreaUnits.GetValue<tkAreaDisplayMode>();

            _measuring.LengthPrecision = (int) udLengthPrecision.Value;
            _measuring.AnglePrecision = (int) udBearingPrecision.Value;
            _measuring.AreaPrecision = (int) udAreaPrecision.Value;

            _measuring.PointsVisible = chkShowPoints.Checked;
            _measuring.PointLabelsVisible = chkShowPointLabels.Checked;

            _measuring.FillColor = Colors.ColorToUInteger(clpFillColor.Color);
            _measuring.LineColor = Colors.ColorToUInteger(clpLineColor.Color);
            _measuring.FillTransparency = fillTransparency.Value;

            _measuring.LineWidth = cboLineWidth.SelectedIndex + 1;
            _measuring.LineStyle = (tkDashStyle)cboLineStyle.SelectedIndex;
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            UiToModel();
            DialogResult = DialogResult.OK;
        }
    }
}
