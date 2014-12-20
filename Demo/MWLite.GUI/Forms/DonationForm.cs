using System.Diagnostics;
using System.Net;
using MWLite.Core.UI;
using System;
using System.Collections.Generic;
using System.ComponentModel;
using System.Data;
using System.Drawing;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.Windows.Forms;
using WeifenLuo.WinFormsUI.Docking;
using System.Threading;

namespace MWLite.GUI.Forms
{
    public partial class DonationForm : DockContent
    {
        private const string MW5_URL =
            "http://www.mapwindow.org/documentation/mapwingis4.9/MapWindow49.html?utm_source=MWv49&utm_medium=cpc&utm_campaign=nagscreen";
        
        public DonationForm()
        {
            InitializeComponent();
            webBrowser1.Navigated += webBrowser1_Navigated;
        }

        //async void DoLoading()
        //{
        //    var client = new WebClient();
        //    webBrowser1.DocumentStream = await client.OpenReadTaskAsync(MW5_URL);
        //}

        void webBrowser1_Navigated(object sender, WebBrowserNavigatedEventArgs e)
        {
            Debug.WriteLine("Finished loading page: " + e.Url);
            
        }
    }
}
