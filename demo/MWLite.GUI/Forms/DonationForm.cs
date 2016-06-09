using System.Diagnostics;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;

namespace MWLite.GUI.Forms
{
    public partial class DonationForm : DockContent
    {
        //private const string MW5_URL =
        //    "http://www.mapwindow.org/documentation/mapwingis4.9/MapWindow49.html?utm_source=MWv49&utm_medium=cpc&utm_campaign=nagscreen";
        
        public DonationForm()
        {
            InitializeComponent();
            
        }

        //async void DoLoading()
        //{
        //    var client = new WebClient();
        //    webBrowser1.DocumentStream = await client.OpenReadTaskAsync(MW5_URL);
        //}
    }
}
