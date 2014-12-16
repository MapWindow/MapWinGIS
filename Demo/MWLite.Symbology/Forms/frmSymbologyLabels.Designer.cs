// ********************************************************************************************************
// <copyright file="MWLite.Symbology.cs" company="MapWindow.org">
// Copyright (c) MapWindow.org. All rights reserved.
// </copyright>
// The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
// you may not use this file except in compliance with the License. You may obtain a copy of the License at 
// http:// Www.mozilla.org/MPL/ 
// Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
// ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
// limitations under the License. 
// 
// The Initial Developer of this version of the Original Code is Sergei Leschinski
// 
// Contributor(s): (Open source contributors should list themselves and their modifications here). 
// Change Log: 
// Date            Changed By      Notes
// ********************************************************************************************************

using MWLite.Symbology.Forms.Labels;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms
{
    using System;
    using System.Windows.Forms;
    using MWLite.Symbology.Classes;
    using MWLite.Symbology;

    partial class frmSymbologyMain
    {
        /// <summary>
        /// Initializes labels tab
        /// </summary>
        private void InitLabelsTab()
        {
            MapWinGIS.Labels lb = _shapefile.Labels;
            chkShowLabels.Checked = lb.Visible;
            
            chkLabelFrame.Checked = lb.FrameVisible;
            clpLabelFrame.Color = Colors.UintToColor(lb.FrameBackColor);
            udLabelFontSize.SetValue(lb.FontSize);
        }

        /// <summary>
        /// Generating of labels
        /// </summary>
        private void btnGenerateLabels_Click(object sender, EventArgs e)
        {
            //frmGenerateLabels form = new frmGenerateLabels(m_mapWin, _shapefile, layer, LabelAction.ChangeAll);

            LabelStyleForm form = new LabelStyleForm(_legend, _shapefile, _layerHandle);
            if (form.ShowDialog(this) == DialogResult.OK)
            {           
                _shapefile.Labels.Visible = true;
                DrawLabelsPreview();
                RefreshControlsState(null, null);
                RedrawMap();
            }
            form.Dispose();
        }

        /// <summary>
        /// Updates preview of the labels
        /// </summary>
        private void DrawLabelsPreview()
        {
            LabelUtilities.DrawPreview(_shapefile.Labels.Options, _shapefile, pctLabelPreview, false);
        }

        /// <summary>
        /// Changing the default style of labels
        /// </summary>
        private void btnLabelsAppearance_Click(object sender, EventArgs e)
        {
            LabelStyleForm styleFormForm = new LabelStyleForm(_legend, _shapefile, _layerHandle );
            styleFormForm.ShowDialog();

            // updating controls (even if cancel was hit, a user could have applied the options)
            MapWinGIS.LabelCategory options = _shapefile.Labels.Options;
            udLabelFontSize.Value = options.FontSize;
            clpLabelFrame.Color = Colors.UintToColor(options.FrameBackColor);
            chkLabelFrame.Checked = options.FrameVisible;
            chkShowLabels.Checked = options.Visible;

            RefreshControlsState(null, null);

            RedrawMap();

            // refreshing preview
            DrawLabelsPreview();
            
            styleFormForm.Dispose();
        }

        /// <summary>
        /// Deletes all the labels
        /// </summary>
        private void btnLabelsClear_Click(object sender, EventArgs e)
        {
            if (MessageBox.Show("Do you want to delete labels?", LegendControl.Legend.AppName, MessageBoxButtons.YesNo, MessageBoxIcon.Question) == DialogResult.Yes)
            {
                _shapefile.Labels.Clear();
                _shapefile.Labels.Expression = "";
                DrawLabelsPreview();
                RefreshControlsState(null, null);
                RedrawMap();
            }
        }

        /// /// <summary>
        /// Changes label font size. We shall make changes for the categories as well in case a user wants to
        /// </summary>
        private void udLabelFontSize_ValueChanged(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            _shapefile.Labels.FontSize = (int)udLabelFontSize.Value;
            DrawLabelsPreview();
            RedrawMap();
        }

        /// <summary>
        /// Changes label frame color. We shall make changes for the categories as well in case a user wants to
        /// </summary>
        private void clpLabelFrame_SelectedColorChanged(object sender, EventArgs e)
        {
            if (_noEvents)
                return;
           
            _shapefile.Labels.FrameBackColor = Colors.ColorToUInteger(clpLabelFrame.Color);
            DrawLabelsPreview();
            RedrawMap();
        }

        /// <summary>
        /// Changes label frame visiblity. We shall make changes for the categories as well in case a user wants to
        /// </summary>
        private void chkLabelFrame_CheckedChanged(object sender, EventArgs e)
        {
            if (_noEvents)
                return;

            _shapefile.Labels.FrameVisible = chkLabelFrame.Checked;
            DrawLabelsPreview();
            RedrawMap();
        }

        /// <summary>
        /// Sets the properties of the labels based upon user input
        /// </summary>
        private void UpdateLabels()
        {
            MapWinGIS.Labels lb = _shapefile.Labels;
            lb.Visible = chkShowLabels.Checked;

            // categories will have the same alignment
            for (int i = 0; i < _shapefile.Labels.NumCategories; i++)
            {
                MapWinGIS.LabelCategory cat = _shapefile.Labels.get_Category(i);
                cat.Alignment = lb.Alignment;
                cat.OffsetX = lb.OffsetX;
                cat.OffsetY = lb.OffsetY;
            }

            DrawLabelsPreview();
        }
    }
}
