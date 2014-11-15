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
    public partial class NewLayerForm : Form
    {
        private static string _lastPath = "";
        private BindingList<FieldClass> _fields = null;
        private Shapefile _shapefile = null;
        
        public NewLayerForm()
        {
            InitializeComponent();

            var values = Enum.GetValues(typeof (ShpfileType)).Cast<ShpfileType>().ToArray();
            Array.Sort(values, (x, y) => String.Compare(x.ToString(), y.ToString(), StringComparison.Ordinal));
            comboBox1.Items.AddRange(values.Cast<object>().ToArray());
            comboBox1.SelectedIndex = 4;

            txtPath.Text = _lastPath.Length > 0 ?_lastPath : Path.GetDirectoryName(App.Instance.Project.GetPath());
        }

        #region Properties

        private ShpfileType ShapefileType
        {
            get { return (ShpfileType)comboBox1.SelectedItem; }
        }

        private string Filename
        {
            get { return txtPath.Text + "\\" + txtName.Text + ".shp";  }
        }

        private IEnumerable<Field> Fields
        {
            get { return _fields; }
        }

        public Shapefile Shapefile
        {
            get { return _shapefile; }
        }

        #endregion

        #region Methods

        private void btnSelectPath_Click(object sender, EventArgs e)
        {
            var dlg = new FolderBrowserDialog {SelectedPath = txtPath.Text};

            if (dlg.ShowDialog(App.Instance as Form) == DialogResult.OK)
            {
                txtPath.Text = dlg.SelectedPath;
            }
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (string.IsNullOrWhiteSpace(txtName.Text))
            {
                MessageHelper.Info("Enter layer name.");
                return;
            }

            var types = new[] {ShpfileType.SHP_MULTIPATCH, ShpfileType.SHP_NULLSHAPE};
            if (types.Contains(ShapefileType))
            {
                MessageHelper.Info("Unsupported shape type.");
                return;
            }

            if (File.Exists(Filename))
            {
                MessageHelper.Info("File with such name already exists.");
                return;
            }

            _lastPath = txtPath.Text;

            var fields = new BindingList<FieldClass>();
            using (var form = new EditFieldsForm(fields))
            {
                if (form.ShowDialog(this) == DialogResult.OK)
                {
                    _fields = fields;
                    CreateShapefile();
                    DialogResult = DialogResult.OK;
                }
            }
        }

        private void CreateShapefile()
        {
            _shapefile = new Shapefile();
            _shapefile.CreateNewWithShapeID(Filename, ShapefileType);
            _shapefile.InteractiveEditing = true;
            foreach (var fld in Fields)
            {
                _shapefile.EditInsertField(fld, _shapefile.NumFields);
            }
        }

        #endregion
    }
}
