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

namespace MWLite.Symbology.Forms.Labels
{
    public partial class AddLabelsForm : Form
    {
        // shapefile to generate labels for
        private MapWinGIS.Shapefile m_shapefile = null;

        internal tkLabelAlignment m_alignment; // = tkLabelAlignment.laCenter;
        
        /// <summary>
        /// Creates a new instance of the AddLabelsForm
        /// </summary>
        public AddLabelsForm(MapWinGIS.Shapefile sf, tkLabelAlignment alignment)
        {
            InitializeComponent();

            if (sf == null)
            {
                throw new Exception("AddLabelsForm: Unexpected null parameter");
            }

            m_alignment = alignment;
            m_shapefile = sf;
            cboLineOrientation.Enabled = false;
            panel1.Visible = false;

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
                groupBox4.Top -= 30;
                this.Height -= 30;

                optPosition2.Checked = true;        // TODO: choose according Labels.Positioning
            }
            else if (type == ShpfileType.SHP_POLYLINE)
            {
                optPosition1.Text = "First segment";
                optPosition2.Text = "Last segment";
                optPosition3.Text = "Middle segment";
                optPosition4.Text = "The longest segment";

                optPosition1.Tag = tkLabelPositioning.lpFirstSegment;
                optPosition2.Tag = tkLabelPositioning.lpLastSegment;
                optPosition3.Tag = tkLabelPositioning.lpMiddleSegment;
                optPosition4.Tag = tkLabelPositioning.lpLongestSegement;

                optPosition4.Checked = true;      // TODO: choose according Labels.Positioning
                cboLineOrientation.Enabled = true;
            }
            else
            {
                panel1.Visible = true;
                optPosition1.Visible = false;
                optPosition2.Visible = false;
                optPosition3.Visible = false;
                optPosition4.Visible = false;
                this.Height -= 100;
                groupBox4.Visible = false;

                optAlignBottomCenter.Checked = (m_alignment == tkLabelAlignment.laBottomCenter);
                optAlignBottomLeft.Checked = (m_alignment == tkLabelAlignment.laBottomLeft);
                optAlignBottomRight.Checked = (m_alignment == tkLabelAlignment.laBottomRight);
                optAlignCenterLeft.Checked = (m_alignment == tkLabelAlignment.laCenterLeft);
                optAlignCenterRight.Checked = (m_alignment == tkLabelAlignment.laCenterRight || m_alignment == tkLabelAlignment.laCenter);
                optAlignTopCenter.Checked = (m_alignment == tkLabelAlignment.laTopCenter);
                optAlignTopLeft.Checked = (m_alignment == tkLabelAlignment.laTopLeft);
                optAlignTopRight.Checked = (m_alignment == tkLabelAlignment.laTopRight);
                
                // Applicable for polyline and polygon shapefiles
                //throw new Exception("AddLabelsForm: invalid shapefile type");
            }

            // line orientation
            cboLineOrientation.Items.Clear();
            cboLineOrientation.Items.Add("Horizontal");
            cboLineOrientation.Items.Add("Parallel");
            cboLineOrientation.Items.Add("Perpendicular");
            cboLineOrientation.SelectedIndex = 1;
        }

        /// <summary>
        /// Generates labels with specified positions
        /// </summary>
        private void btnOk_Click(object sender, EventArgs e)
        {
            // callback and wait cursor
            ICallback cBackOld = m_shapefile.GlobalCallback;
            Callback cback = new Callback();
            m_shapefile.GlobalCallback = cback;
            this.Enabled = false;
            this.Cursor = Cursors.WaitCursor;

            MapWinGIS.Labels lb = m_shapefile.Labels;
            tkLabelPositioning positioning = get_LabelPositioning();
            lb.LineOrientation = (tkLineLabelOrientation)cboLineOrientation.SelectedIndex;
            
            try
            {
                // generation
                m_shapefile.GenerateLabels(-1, positioning, !chkLabelEveryPart.Checked);
                m_shapefile.Labels.SavingMode = tkSavingMode.modeXMLOverwrite;  // .lbl file should be updated

                ShpfileType type = Globals.ShapefileType2D(m_shapefile.ShapefileType);
                if (type == ShpfileType.SHP_POINT || type == ShpfileType.SHP_MULTIPOINT)
                {
                    if (optAlignBottomCenter.Checked) m_alignment = tkLabelAlignment.laBottomCenter;
                    if (optAlignBottomLeft.Checked) m_alignment = tkLabelAlignment.laBottomLeft;
                    if (optAlignBottomRight.Checked) m_alignment = tkLabelAlignment.laBottomRight;
                    if (optAlignCenter.Checked) m_alignment = tkLabelAlignment.laCenter;
                    if (optAlignCenterLeft.Checked) m_alignment = tkLabelAlignment.laCenterLeft;
                    if (optAlignCenterRight.Checked) m_alignment = tkLabelAlignment.laCenterRight;
                    if (optAlignTopCenter.Checked) m_alignment = tkLabelAlignment.laTopCenter;
                    if (optAlignTopLeft.Checked) m_alignment = tkLabelAlignment.laTopLeft;
                    if (optAlignTopRight.Checked) m_alignment = tkLabelAlignment.laTopRight;
                }

                // updating references to categories
                if (lb.NumCategories > 0)
                {
                    for (int i = 0; i < lb.Count; i++)
                    {
                        MapWinGIS.Label label = lb.get_Label(i, 0);
                        label.Category = m_shapefile.get_ShapeCategory(i);
                    }
                }
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
        ///  Returns positioning method
        /// </summary>
        private tkLabelPositioning get_LabelPositioning()
        {
            ShpfileType type = Globals.ShapefileType2D(m_shapefile.ShapefileType);
            if (type != ShpfileType.SHP_POINT || type == ShpfileType.SHP_MULTIPOINT)
            {
                if (optPosition4.Checked)
                    return (tkLabelPositioning)optPosition4.Tag;
                if (optPosition3.Checked)
                    return (tkLabelPositioning)optPosition3.Tag;
                if (optPosition2.Checked)
                    return (tkLabelPositioning)optPosition2.Tag;
                if (optPosition1.Checked)
                    return (tkLabelPositioning)optPosition1.Tag;
            }
            
            return tkLabelPositioning.lpCentroid;
        }
    }
}
