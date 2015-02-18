using System;
using System.Collections;
using System.Windows.Forms;
using MapWinGIS;

namespace Examples
{
    public partial class MapExamples
    {
        private void MergeImages(object sender, EventArgs e)
        {
            FolderBrowserDialog dlg = new FolderBrowserDialog();
            dlg.Description = "Select the directory with images to merge";
            if (dlg.ShowDialog() == DialogResult.OK)
            {
                System.IO.DirectoryInfo dir = new System.IO.DirectoryInfo(dlg.SelectedPath);
                System.IO.FileInfo[] files = dir.GetFiles();

                ArrayList list = new ArrayList();
                for (int i = 0; i < files.Length; i++)
                {
                    if (files[i].Extension.ToLower() == ".png")
                    {
                        list.Add(files[i].FullName);
                    }
                }

                if (list.Count <= 1)
                {
                    MessageBox.Show("There is no PNG files to merge in the specified directory");
                    return;
                }
                else
                {
                    string[] inputNames = (string[])list.ToArray(typeof(string));

                    Utils utils = new Utils();
                    if (utils.MergeImages(inputNames, "c:\\result.png"))
                    {
                        MessageBox.Show("Success");
                    }
                }
            }
        }
    }
}