using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Diagnostics;
using System.Linq;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.UI;
using System.IO;

namespace MWLite.ShapeEditor.Forms
{
    public partial class EditFieldsForm : Form
    {
        BindingList<FieldClass> _fields  = null;
        
        public EditFieldsForm(BindingList<FieldClass> fields)
        {
            InitializeComponent();

            _fields = fields;
            cmnName.DataPropertyName = "Name";
            cmnType.DataPropertyName = "Type";
            cmnType.DataSource = Enum.GetValues(typeof(FieldType));
            cmnWidth.DataPropertyName = "Width";
            cmnPrecision.DataPropertyName = "Precision";
            dataGridView1.AutoGenerateColumns = false;
            dataGridView1.DefaultValuesNeeded += dataGridView1_DefaultValuesNeeded;
            dataGridView1.DataSource = fields;
            this.Shown += EditFieldsForm_Shown;
        }

        void EditFieldsForm_Shown(object sender, EventArgs e)
        {
            dataGridView1.Focus();
            dataGridView1.CurrentCell = dataGridView1.Rows[0].Cells[0];
            dataGridView1.BeginEdit(true);
        }

        void dataGridView1_DefaultValuesNeeded(object sender, DataGridViewRowEventArgs e)
        {
            e.Row.Cells[0].Value = "<New field>";
        }

        private void btnClear_Click(object sender, EventArgs e)
        {
            _fields.Clear();
        }

        private void btnRemove_Click(object sender, EventArgs e)
        {
            var row = dataGridView1.CurrentRow;
            if (row != null)
            {
                _fields.RemoveAt(row.Index);
            }
        }

        private void dataGridView1_CurrentCellChanged(object sender, EventArgs e)
        {
            var row = dataGridView1.CurrentRow;
            btnRemove.Enabled = row != null && !row.IsNewRow;
        }
    }
}
