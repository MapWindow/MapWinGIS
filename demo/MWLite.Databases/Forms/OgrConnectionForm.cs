using System;
using System.Windows.Forms;
using MapWinGIS;
using MWLite.Core.UI;

namespace MWLite.Databases.Forms
{
    public partial class OgrConnectionForm : Form
    {
        private static ConnectionParams _param;

        static OgrConnectionForm()
        {
            _param = new ConnectionParams();
            _param.InitDefaultPostGis();
        }

        public OgrConnectionForm()
        {
            InitializeComponent();
            Populate(_param);
        }
        
        public OgrConnectionForm(ConnectionParams param)
        {
            InitializeComponent();

            Populate(param);
        }

        private void Populate(ConnectionParams param)
        {
            if (param == null) return;
            
            txtHost.Text = param.Host;
            txtPort.Text = param.Port.ToString();
            txtDatabase.Text = param.Database;
            txtUserName.Text = param.UserName;
            txtPassword.Text = param.Password;
        }

        private bool ValidateInput()
        { 
            if (string.IsNullOrWhiteSpace(txtHost.Text))
            {
                MessageHelper.Warn("No host name is provided.");
                return false;
            }

            int port;
            if (!Int32.TryParse(txtPort.Text, out port))
            {
                MessageHelper.Warn("Port must be an integer number.");
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtUserName.Text))
            {
                MessageHelper.Warn("No user name is provided.");
                return false;
            }

            if (string.IsNullOrWhiteSpace(txtDatabase.Text))
            {
                MessageHelper.Warn("No database name is provided.");
                return false;
            }

            return true;
        }

        public ConnectionParams ConnectionParams
        {
            get
            {
                if (!ValidateInput()) return null;
                return new ConnectionParams()
                {
                    Host = txtHost.Text,
                    Port = Int32.Parse(txtPort.Text),
                    Database = txtDatabase.Text,
                    UserName = txtUserName.Text,
                    Password = txtPassword.Text
                };
            }
        }

        private bool TestConnection(bool silent)
        {
            var param = ConnectionParams;
            if (param == null) return false;
            string cs = param.GetPostGisConnection();
            
            bool result = false;
            var ds = new OgrDatasource();

            if (!ds.Open(cs))
            {
                MessageHelper.Warn("Failed to open connection: " + ds.GdalLastErrorMsg);
            }
            else
            {
                if (!silent)
                    MessageHelper.Info("Connected successfully");
                result = true;
            }
            ds.Close();
            return result;
        }

        private void btnTestConnection_Click(object sender, EventArgs e)
        {
            TestConnection(false);
        }

        private void btnOk_Click(object sender, EventArgs e)
        {
            if (TestConnection(true))
            {
                _param = ConnectionParams;
                DialogResult = DialogResult.OK;
            }
        }
    }
}
