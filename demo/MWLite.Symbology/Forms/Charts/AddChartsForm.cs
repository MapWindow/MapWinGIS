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

using System;
using System.Windows.Forms;
using MWLite.Symbology.Classes;
using MapWinGIS;
using MWLite.Symbology.Classes;

namespace MWLite.Symbology.Forms.Charts
{
    public partial class AddChartsForm : Form
    {
        // the shapefile to generate charts for
        private MapWinGIS.Shapefile m_shapefile = null;

        /// <summary>
        /// Creates a new instance of the AddChartsForm class
        /// </summary>
        public AddChartsForm(MapWinGIS.Shapefile sf)
        {
            InitializeComponent();

            m_shapefile = sf;

            ShpfileType type = Globals.ShapefileType2D(m_shapefile.ShapefileType);
            if (type == ShpfileType.SHP_POLYGON)
            {
                optPosition1.Text = "Center";
                optPosition2.Text = "Centroid";
                optPosition3.Text = "Interior point";
                optPosition4.Visible = false;

                optPosition1.Tag = tkLabelPositioning.lpCenter;
                optPosition2.Tag = tkLabelPositioning.lpCentroid;
                optPosition3.Tag = tkLabelPositioning.lpInteriorPoint;

                groupBox5.Height -= 30;
                this.Height -= 30;

                optPosition2.Checked = true;
            }
            else if (type == ShpfileType.SHP_POLYLINE)
            {
                optPosition1.Text = "First segment";
                optPosition2.Text = "Last segment";
                optPosition3.Text = "Middle segment";
                optPosition4.Text = "Longest segment";

                optPosition1.Tag = tkLabelPositioning.lpFirstSegment;
                optPosition2.Tag = tkLabelPositioning.lpLastSegment;
                optPosition3.Tag = tkLabelPositioning.lpMiddleSegment;
                optPosition4.Tag = tkLabelPositioning.lpLongestSegement;

                optPosition4.Checked = true;
            }
        }

        /// <summary>
        /// Calculates chart positions
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            ICallback cBackOld = m_shapefile.GlobalCallback;
            Callback cback = new Callback();
            m_shapefile.GlobalCallback = cback;
            this.Enabled = false;
            this.Cursor = Cursors.WaitCursor;

            try
            {
                tkLabelPositioning positioning = tkLabelPositioning.lpCentroid;
                if (optPosition4.Checked)
                    positioning = (tkLabelPositioning)optPosition4.Tag;
                if (optPosition3.Checked)
                    positioning = (tkLabelPositioning)optPosition3.Tag;
                if (optPosition2.Checked)
                    positioning = (tkLabelPositioning)optPosition2.Tag;
                if (optPosition1.Checked)
                    positioning = (tkLabelPositioning)optPosition1.Tag;

                m_shapefile.Charts.Generate(positioning);
                m_shapefile.Charts.SavingMode = tkSavingMode.modeXMLOverwrite;
            }
            finally
            {
                this.Enabled = true;
                this.Cursor = Cursors.Default;
                cback.Clear();
                m_shapefile.GlobalCallback = cBackOld;
            }
        }

        /// <summary>
        /// Closes the form
        /// </summary>
        private void btnCancel_Click(object sender, EventArgs e)
        {

        }
    }
}
