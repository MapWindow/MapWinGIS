using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Drawing;
using System.Linq;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.UI;
using System.IO;

namespace MWLite.ShapeEditor.Forms
{
    public partial class AttributesForm : Form
    {
        private readonly Shapefile _sf;
        private readonly int _shapeIndex;

        public AttributesForm(Shapefile sf, int shapeIndex)
        {
            _sf = sf;
            _shapeIndex = shapeIndex;
            InitializeComponent();
            Populate();

            this.Shown += AttributesForm_Shown;
        }

        void AttributesForm_Shown(object sender, EventArgs e)
        {
            if (!_sf.InteractiveEditing)
            {
                btnOk.Focus();
            }
        }

        private void Populate()
        {
            tableLayoutPanel1.SuspendLayout();

            bool editing = _sf.InteractiveEditing;

            tableLayoutPanel1.RowStyles.Clear();
            tableLayoutPanel1.ColumnStyles.Clear();

            tableLayoutPanel1.AutoSizeMode = AutoSizeMode.GrowAndShrink;
            tableLayoutPanel1.AutoSize = true;
            
            tableLayoutPanel1.RowCount = _sf.NumFields;
            tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.AutoSize));

            double maxRows = 15.0;
            int columCount = (int)Math.Ceiling(_sf.NumFields / maxRows);
            tableLayoutPanel1.ColumnCount = columCount*3;
            for (int n = 0; n < columCount; n++)
            {
                tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle());
                tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle(SizeType.Percent, 100F));
                tableLayoutPanel1.ColumnStyles.Add(new ColumnStyle());
            }
            
            int padding = editing ? 5 : 0;
            for (int i = 0; i < _sf.NumFields; i++)
            {
                int cmnIndex = (int)(Math.Floor(i / maxRows) * 3);
                int rowIndex = i % (int)maxRows;

                string name = _sf.Field[i].Name.ToUpper();
                var fieldType = _sf.Field[i].Type;

                var lbl = new System.Windows.Forms.Label() {Text = name};
                lbl.Dock = DockStyle.Fill;
                lbl.TextAlign = ContentAlignment.MiddleLeft;
                lbl.AutoSize = true;
                lbl.Padding = new Padding(padding);
                lbl.BackColor = Color.White;
                tableLayoutPanel1.Controls.Add(lbl, cmnIndex, rowIndex);

                lbl = new System.Windows.Forms.Label() { Text = editing ? GetShortFieldType(fieldType) : "" };
                lbl.Dock = DockStyle.Fill;
                lbl.TextAlign = ContentAlignment.MiddleLeft;
                lbl.AutoSize = true;
                lbl.Padding = new Padding(0, padding, 0, padding);
                tableLayoutPanel1.Controls.Add(lbl, cmnIndex + 2, rowIndex);

                var value = _sf.CellValue[i, _shapeIndex] ?? GetDefaultValue(fieldType);
                var control = new TextBox() { ReadOnly = !editing,  Text = value.ToString() };;
                control.Dock = DockStyle.Fill;
                control.Padding = new Padding(padding);
                control.Tag = i;
                tableLayoutPanel1.Controls.Add(control, cmnIndex + 1, rowIndex);

                control.Enabled = name.ToLower() != "mwshapeid";
            }
            tableLayoutPanel1.ResumeLayout();
        }

        private object GetDefaultValue(FieldType type)
        {
            switch (type)
            {
                case FieldType.INTEGER_FIELD:
                    return 0;
                case FieldType.DOUBLE_FIELD:
                    return 0.0;
                case FieldType.STRING_FIELD:
                default:
                    return "";
            }
        }

        private string GetShortFieldType(FieldType type)
        {
            switch (type)
            {
                case FieldType.INTEGER_FIELD:
                    return "i";
                case FieldType.DOUBLE_FIELD:
                    return "d";
                case FieldType.STRING_FIELD:
                default:
                    return "s";
            }
        }

        private bool Save()
        {
            var list = tableLayoutPanel1.Controls.OfType<TextBox>();
            foreach (var txt in list)
            {
                if (!txt.Enabled) continue;

                int fieldIndex = (int)txt.Tag;
                var fld = _sf.Field[fieldIndex];

                switch (fld.Type)
                {
                    case FieldType.STRING_FIELD:
                        {
                            _sf.EditCellValue(fieldIndex, _shapeIndex, txt.Text);
                            break;
                        }
                    case FieldType.INTEGER_FIELD:
                        {
                            int val;
                            if (!Int32.TryParse(txt.Text, out val))
                            {
                                txt.Focus();
                                MessageHelper.Info("Failed to parse integer value: " + txt.Text);
                                return false;
                            }
                            _sf.EditCellValue(fieldIndex, _shapeIndex, val);
                            break;
                        }
                    case FieldType.DOUBLE_FIELD:
                        {
                            double val;
                            if (!Double.TryParse(txt.Text, out val))
                            {
                                txt.Focus();
                                MessageHelper.Info("Faield to parse double value: " + txt.Text);
                                return false;
                            }
                            _sf.EditCellValue(fieldIndex, _shapeIndex, val);
                            break;
                        }
                }
            }
            return true;
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (Save())
                DialogResult = DialogResult.OK;
        }
    }
}
