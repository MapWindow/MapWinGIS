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
        }

        private void Populate()
        {
            tableLayoutPanel1.AutoSizeMode = AutoSizeMode.GrowAndShrink;
            tableLayoutPanel1.AutoSize = true;
            tableLayoutPanel1.RowStyles.Clear();
            tableLayoutPanel1.RowCount = _sf.NumFields + 1;
            tableLayoutPanel1.RowStyles.Add(new RowStyle(SizeType.AutoSize));

            bool editing = _sf.InteractiveEditing;

            var watch = new Stopwatch();
            watch.Start();

            tableLayoutPanel1.SuspendLayout();
            int padding = editing ? 5 : 0;
            for (int i = 0; i < _sf.NumFields; i++)
            {
                string name = _sf.Field[i].Name;
                var fieldType = _sf.Field[i].Type;

                var lbl = new System.Windows.Forms.Label() {Text = name};
                lbl.Dock = DockStyle.Fill;
                lbl.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
                lbl.AutoSize = true;
                lbl.Padding = new Padding(padding);
                tableLayoutPanel1.Controls.Add(lbl, 0, i);

                lbl = new System.Windows.Forms.Label() { Text = GetShortFieldType(fieldType) };
                lbl.Dock = DockStyle.Fill;
                lbl.TextAlign = System.Drawing.ContentAlignment.MiddleLeft;
                lbl.AutoSize = true;
                lbl.Padding = new Padding(padding);
                tableLayoutPanel1.Controls.Add(lbl, 2, i);

                var value = _sf.CellValue[i, _shapeIndex] ?? GetDefaultValue(fieldType);
                Control control = null;
                if (editing)
                {
                    control = new ComboBox() {Text = value.ToString()};
                }
                else
                {
                    control = new System.Windows.Forms.Label() {Text = value.ToString(), 
                    TextAlign = ContentAlignment.MiddleLeft};

                }
                control.Dock = DockStyle.Fill;
                control.Margin = new Padding(padding);
                control.Tag = i;
                tableLayoutPanel1.Controls.Add(control, 1, i);

                control.Enabled = name.ToLower() != "mwshapeid";
            }

            var dummy = new System.Windows.Forms.Label();
            dummy.AutoSize = false;
            dummy.Height = 10;
            tableLayoutPanel1.Controls.Add(dummy, 0, _sf.NumFields);

            Debug.Print("Elapsed: " + watch.Elapsed);
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
                    return "[Int]";
                case FieldType.DOUBLE_FIELD:
                    return "[Dbl]";
                case FieldType.STRING_FIELD:
                default:
                    return "[Str]";
            }
        }

        private bool Save()
        {
            var list = tableLayoutPanel1.Controls.OfType<ComboBox>();
            foreach (var combo in list)
            {
                if (!combo.Enabled) continue;

                int fieldIndex = (int)combo.Tag;
                var fld = _sf.Field[fieldIndex];

                switch (fld.Type)
                {
                    case FieldType.STRING_FIELD:
                        {
                            _sf.EditCellValue(fieldIndex, _shapeIndex, combo.Text);
                            break;
                        }
                    case FieldType.INTEGER_FIELD:
                        {
                            int val;
                            if (!Int32.TryParse(combo.Text, out val))
                            {
                                combo.Focus();
                                MessageHelper.Info("Faield to parse integer value: " + combo.Text);
                                return false;
                            }
                            _sf.EditCellValue(fieldIndex, _shapeIndex, val);
                            break;
                        }
                    case FieldType.DOUBLE_FIELD:
                        {
                            double val;
                            if (!Double.TryParse(combo.Text, out val))
                            {
                                combo.Focus();
                                MessageHelper.Info("Faield to parse double value: " + combo.Text);
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
