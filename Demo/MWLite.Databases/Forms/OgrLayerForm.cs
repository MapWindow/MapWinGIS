using System;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.Events;
using MWLite.Core.UI;

namespace MWLite.Databases.Forms
{
    public partial class OgrLayerForm : Form
    {
        private OgrDatasource _datasource = new OgrDatasource();
        private ConnectionParams _connection = null;
        
        public event EventHandler<OgrLayerArgs> LayerAdded;

        protected virtual void OnLayerAdded(OgrLayerArgs e)
        {
            EventHandler<OgrLayerArgs> handler = LayerAdded;
            if (handler != null) handler(this, e);
        }

        public OgrLayerForm()
        {
            InitializeComponent();

            this.Shown += OgrLayerForm_Shown;
        }

        void OgrLayerForm_Shown(object sender, EventArgs e)
        {
            ChangeConnection();
        }

        private void PopulateList()
        {
            if (_datasource == null) return;

            listView1.Items.Clear();
            for (int i = 0; i < _datasource.LayerCount; i++)
            {
                var layer = _datasource.GetLayer(i);
                if (layer == null) continue;

                var item = listView1.Items.Add(layer.Name);
                item.SubItems.Add(layer.FeatureCount.ToString());
                item.SubItems.Add(layer.ShapeType.ToString());

                var gp = layer.GeoProjection;
                int epsg;
                string srid = gp.TryAutoDetectEpsg(out epsg) ? epsg.ToString() : gp.ExportToProj4();
                item.SubItems.Add(srid);
            }
        }

        private void btnChangeConnection_Click(object sender, EventArgs e)
        {
            ChangeConnection();
        }

        private void ChangeConnection()
        {
            using (var form = new OgrConnectionForm())
            {
                if (form.ShowDialog(this) == DialogResult.OK)
                {
                    _connection = form.ConnectionParams;
                    OpenDatasource();
                }
            }
        }

        private void OpenDatasource()
        {
            _datasource.Close();
            if (!OgrHelper.OpenDatasource(_datasource, _connection))
                return;
            PopulateList();
        }

        private void AddLayer()
        {
            if (listView1.SelectedItems.Count == 0)
            {
                MessageHelper.Info("No layer is selected.");
                return;
            }

            int index = listView1.SelectedIndices[0];
            var layer = _datasource.GetLayer(index);
            if (layer == null)
            {
                MessageHelper.Info("Failed to initialize layer.");
                return;
            }
            OnLayerAdded(new OgrLayerArgs() { Layer = layer });
        }

        private void btnAddLayer_Click(object sender, EventArgs e)
        {
            AddLayer();
        }

        private void listView1_MouseDoubleClick(object sender, MouseEventArgs e)
        {
            AddLayer();
        }
    }
}
