// ********************************************************************************************************
// <copyright file="MapWindow.Legend.cs" company="MapWindow.org">
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

using MapWindow.Legend.Classes;

namespace MapWindow.Legend.Forms
{
    using System;
    using System.Windows.Forms;
    using MapWinGIS;
    using System.Drawing;

    partial class frmSymbologyMain
    {
        /// <summary>
        /// Sets the state of controls on the general tab on loading
        /// </summary>
        private void InitModeTab()
        {
            if (Globals.ShapefileType2D(m_shapefile.ShapefileType) == ShpfileType.SHP_POINT)
            {
                cboCollisionMode.Enabled = true;
                cboCollisionMode.Items.Clear();
                cboCollisionMode.Items.Add("Allow collisions");
                cboCollisionMode.Items.Add("Avoid point vs point collisions");
                cboCollisionMode.Items.Add("Avoid point vs label collisions");
                cboCollisionMode.SelectedIndex = (int)m_shapefile.CollisionMode;
            }
            else
            {
                cboCollisionMode.Enabled = false;
            }

            chkFastMode.Checked = m_shapefile.FastMode;
            chkSpatialIndex.Checked = m_shapefile.UseSpatialIndex && m_shapefile.IsSpatialIndexValid();
            chkEditMode.Checked = m_shapefile.EditingShapes;
            udMinDrawingSize.SetValue((double)m_shapefile.MinDrawingSize);
            udMinLabelingSize.SetValue((double)m_shapefile.Labels.MinDrawingSize);

            // displaying help string default help
            chkFastMode_Enter(chkFastMode, null);
        }
        
        /// <summary>
        /// Toggles fast edit mode for the shapefile
        /// </summary>
        private void chkFastEditingMode_CheckedChanged(object sender, EventArgs e)
        {
            if (_noEvents)
            {
                return;
            }

            MapWinGIS.ICallback oldCallback = null;

            if (chkFastMode.Checked)
            {
                this.Enabled = false;
                this.Cursor = Cursors.WaitCursor;
                oldCallback = m_shapefile.GlobalCallback;
                CallbackLocal callback = new CallbackLocal(this.progressBar1);
                m_shapefile.GlobalCallback = callback;
                this.progressBar1.Visible = true;
                //groupModeDescription.Height = 267;
            }

            m_shapefile.FastMode = chkFastMode.Checked;

            if (chkFastMode.Checked)
            {
                this.Cursor = Cursors.Default;
                this.Enabled = true;
                m_shapefile.GlobalCallback = oldCallback;
                this.progressBar1.Visible = false;
                //groupModeDescription.Height = 293;
            }

            RedrawMap();
        }

        /// <summary>
        /// Start and stops the edit mode for the shapefile
        /// </summary>
        private void chkEditMode_CheckedChanged(object sender, EventArgs e)
        {
            if (_noEvents)
            {
                return;
            }

            MapWinGIS.ICallback oldCallback = null;

            bool Start = chkEditMode.Checked && !m_shapefile.EditingShapes;
            bool Stop = !chkEditMode.Checked && m_shapefile.EditingShapes;

            bool save = false;
            if (Stop)
            {
                DialogResult result = MessageBox.Show("Do you want to save changes to the shapefile?", "MapWindow 4", MessageBoxButtons.YesNoCancel, MessageBoxIcon.Question);
                if (result == DialogResult.Cancel)
                {
                    chkEditMode.Checked = true;
                    return;
                }
                save = (result == DialogResult.Yes);
            }

            if (Start) //|| Stop)
            {
                this.Enabled = false;
                this.Cursor = Cursors.WaitCursor;
                oldCallback = m_shapefile.GlobalCallback;
                CallbackLocal callback = new CallbackLocal(this.progressBar1);
                m_shapefile.GlobalCallback = callback;
                this.progressBar1.Visible = true;
            }

            if (Start)
            {
                m_shapefile.StartEditingShapes(true, null);
            }
            if (Stop)
            {
                m_shapefile.StopEditingShapes(save, true, null);
            }

            if (Start) //|| Stop)
            {
                this.Cursor = Cursors.Default;
                this.Enabled = true;
                m_shapefile.GlobalCallback = oldCallback;
                this.progressBar1.Visible = false;
                RedrawMap();
            }
        }

        /// <summary>
        /// Creates spatial index for the shapefile. Toggles it's usage.
        /// </summary>
        private void chkSpatialIndex_CheckedChanged(object sender, EventArgs e)
        {
            if (_noEvents)
            {
                return;
            }

            if (chkSpatialIndex.Checked)
            {
                if (!m_shapefile.HasSpatialIndex)
                {
                    this.Enabled = false;
                    this.Cursor = Cursors.WaitCursor;
                    if (m_shapefile.CreateSpatialIndex(m_shapefile.Filename))
                    {
                        MessageBox.Show("Spatial index was successfully created");
                    }
                    this.Enabled = true;
                    this.Cursor = Cursors.Default;
                }
                else
                {
                    m_shapefile.UseSpatialIndex = true;
                }
            }
            else
            {
                m_shapefile.UseSpatialIndex = false;
            }

            RedrawMap();
        }

        /// <summary>
        /// Changes the minimum size of object in pixels to be drawn
        /// </summary>
        private void udMinDrawingSize_ValueChanged(object sender, EventArgs e)
        {
            m_shapefile.MinDrawingSize = (int)udMinDrawingSize.Value;
            RedrawMap();
        }

        /// <summary>
        /// Changes the minimum size of the object to label
        /// </summary>
        private void udMinLabelingSize_ValueChanged(object sender, EventArgs e)
        {
            m_shapefile.Labels.MinDrawingSize = (int)udMinLabelingSize.Value;
            RedrawMap();
        }

        private void chkRedrawMap_CheckedChanged(object sender, EventArgs e)
        {
            //if (_noEvents) return;

            if (!chkRedrawMap.Checked)
            {
                //m_mapWin.View.LockMap();
                //Globals.Legend.Lock();
            }
            else
            {
                //m_mapWin.View.UnlockMap();
                //Globals.Legend.Unlock();
            }

            //m_redrawModeIsChanging = true;
            //GUI2Settings(null, null);
            //RedrawMap();
            //m_redrawModeIsChanging = false;
        }

        private void chkFastMode_MouseMove(object sender, MouseEventArgs e)
        {
            chkFastMode_Enter(sender, null);
        }

        private void chkFastMode_Enter(object sender, EventArgs e)
        {
            string s = string.Empty;
            if ((Control)sender == (Control)chkEditMode)
            {
                s += "Editing mode: starts or stops the editing session for the shapefile. The changes can be saved or discarded while closing.";
            }
            else if ((Control)sender == (Control)chkFastMode)
            {
                s += "Fast mode: loads shape data in the memory for faster drawing. There are certain limitations when using it coupled with editing mode.";
            }
            else if ((Control)sender == (Control)chkSpatialIndex)
            {
                s += "Spatial index: creates R-tree for faster search. Affects drawing and selection at close scales. Creates 2 files with .mwd and .mwx extentions in the shapefile folder. Isn't used for editing mode.";
            }
            else if ((Control)sender == (Control)udMinDrawingSize)
            {
                s += "Minimal drawing size: if the size polyline or polygon at current scale in pixels is less than this value, it will be drawn as a single dot.";
            }
            else if ((Control)sender == (Control)udMinLabelingSize)
            {
                s += "Minimal labeling size: a polygon or polyline will be labeled only if it's size in pixels greater than this value.";
            }
            else if ((Control)sender == (Control)cboCollisionMode)
            {
                s += "Collision mode: detemines whether point symbols can be drawn one above the another. Also if the collisions of points with labels are allowed.";
            }   
            else
            {
                // nothing
            }

            txtModeDescription.Text = s;

            Font font = new Font("Arial", 10.0f);
            txtModeDescription.SelectAll();
            txtModeDescription.SelectionFont = font;

            string[] str = { "Fast mode:", "Editing mode:", "Spatial index:", "Minimal drawing size: ", "Minimal labeling size:", "GDI mode for labels:", "Collision mode" };
            font = new Font("Arial", 10.0f, FontStyle.Bold);

            for (int i = 0; i < str.Length; i++)
            {
                int position = txtModeDescription.Text.IndexOf(str[i]);
                if (position >= 0)
                {
                    txtModeDescription.Select(position, str[i].Length);
                    txtModeDescription.SelectionFont = font;
                }
            }
        }
    }
}
