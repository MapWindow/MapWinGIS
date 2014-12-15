
namespace Examples
{
    #region Usings
    using System;
    using System.Collections.Generic;
    using System.ComponentModel;
    using System.Data;
    using System.Drawing;
    using System.Linq;
    using System.Text;
    using System.Windows.Forms;
    using System.IO;
    using System.Reflection;
    using System.Diagnostics;
    using MapWinGIS;
    #endregion

    public partial class TestForm : Form
    {
        MapExamples m_examples;
        Description m_description;
        private string m_dataPath;
        private string m_iconPath;
        
        /// <summary>
        /// Creates a new instance of the form1
        /// </summary>
        public TestForm()
        {
            InitializeComponent();
            MapEvents.AttachMap(axMap1);
            m_examples = new MapExamples();
            m_examples.axMap1 = axMap1;
            m_description = new Description();
            FillList();
            this.treeView1.SelectedNode = this.treeView1.Nodes[0];
            GenerateExamples();
            btnClear_Click_1(null, null);

            axMap1.ProjectionMismatch += axMap1_ProjectionMismatch;
            axMap1.LayerReprojected += axMap1_LayerReprojected;
        }

        void axMap1_LayerReprojected(object sender, AxMapWinGIS._DMapEvents_LayerReprojectedEvent e)
        {
            Debug.WriteLine("Layer reprojected: " + e.layerHandle);
        }

        void axMap1_ProjectionMismatch(object sender, AxMapWinGIS._DMapEvents_ProjectionMismatchEvent e)
        {
            Debug.WriteLine("Projection mismatch:" + e.layerHandle);
        }

        #region Example list
        /// <summary>
        /// Fills the list from the examples directory.
        /// </summary>
        public void FillList()
        {
            m_dataPath = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\Data\";
            m_iconPath = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\icons\";
            if (!Directory.Exists(m_dataPath))
            {
                MessageBox.Show("The directory with the data wasn't found:\n" + m_dataPath);
                return;
            }

            this.treeView1.Nodes.Clear();
            string[] names = Enum.GetNames(typeof(ExampleGroup));
            foreach (string name in names)
            {
                if (name != "None")
                {
                    TreeNode node = this.treeView1.Nodes.Add(name);
                    node.Expand();
                }
            }
           
            IEnumerable<string> list = m_description.examples.Select(t => t.group.ToString()).Distinct();
            foreach (string s in list)
            {
                IEnumerable<Example> examples = m_description.examples.Where(t => t.group.ToString() == s).OrderBy(t => t.GuiName);
                foreach (Example ex in examples)
                {
                    TreeNode node = new TreeNode() { Text = ex.GuiName, Tag = ex };
                    foreach (TreeNode parent in this.treeView1.Nodes)
                    {
                        if (parent.Text == ex.group.ToString())
                        {
                            parent.Nodes.Add(node);
                        }
                    }
                }
            }

            this.treeView1.NodeMouseDoubleClick += delegate(object sender, TreeNodeMouseClickEventArgs e)
            {
                this.BtnRunClick(null, null);
            };

            this.treeView1.AfterSelect += new TreeViewEventHandler(treeView1_AfterSelect);
        }

        /// <summary>
        /// Shows description for the selected example
        /// </summary>
        void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            this.lblDescription.Text = "Description: ";
            TreeNode node = this.treeView1.SelectedNode;
            if (node != null && node.Tag != null)
            {
                Example ex = node.Tag as Example;
                this.lblDescription.Text += "The example demonstrates how to " + ex.description;
            }
        }
        #endregion

        #region Run example
        /// <summary>
        /// Runs the selected example
        /// </summary>
        private void BtnRunClick(object sender, EventArgs e)
        {
            this.btnClear_Click_1(null, null);
            
            TreeNode node = this.treeView1.SelectedNode;
            if (node != null && node.Tag != null)
            {
                Example ex = node.Tag as Example;
                if (ex != null)
                {
                    MethodInfo info = m_examples.GetType().GetMethod(ex.function);
                    if (info == null)
                    {
                        MessageBox.Show("Function wasn't found: " + ex.function);
                    }
                    else
                    {
                        ParameterInfo[] list = info.GetParameters();
                        if (list.Any())
                        {
                            object[] param = new object[list.Count()];
                            for (int i = 0; i < list.Count(); i++)
                            {
                                ParameterInfo item = list[i];
                                switch (item.ParameterType.ToString())
                                {
                                    case "AxMapWinGIS.AxMap":
                                        param[i] = this.axMap1;
                                        break;
                                    case "System.String":
                                        if (item.Name == "dataPath")
                                        {
                                            param[i] = m_dataPath;
                                        }
                                        if (item.Name == "iconPath")
                                        {
                                            param[i] = m_iconPath;
                                        }
                                        break;
                                    case "System.Windows.Forms.ToolStripStatusLabel":
                                        param[i] = this.label1;
                                        break;
                                }
                                //MessageBox.Show(item.ParameterType.ToString());
                            }
                            info.Invoke(m_examples, param);
                        }
                        else
                        {
                            info.Invoke(m_examples, null);
                        }
                    }
                }
            }
        }
        #endregion

        /// <summary>
        /// Generates a page with examples
        /// </summary>
        private void GenerateExamples()
        {
            string path = System.IO.Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\..\Interop.MapWinGIS\Related pages\examples_list.cs";
            StreamWriter writer = new StreamWriter(path);
            foreach (Example item in m_description.examples)
            {
                writer.Write(@"/// This example demonstrates how to " + item.description + "\n" + (item.image ?
                    @"///Here is a screenshot with the results of the code execution." + "\n": ""));
                if (item.image) {
                    writer.Write(@"/// \image html " + item.function + ".png\n");
                }
                writer.Write(@"/// \example " + item.function + ".cs\n\n");
            }
            writer.Flush();
        }

        /// <summary>
        /// Clears all layers and events
        /// </summary>
        private void btnClear_Click_1(object sender, EventArgs e)
        {
            axMap1.RemoveAllLayers();
            axMap1.ClearDrawings();
            axMap1.CursorMode = MapWinGIS.tkCursorMode.cmZoomIn;
            MapEvents.Clear();

            axMap1.Projection = tkMapProjection.PROJECTION_GOOGLE_MERCATOR;
            axMap1.ZoomBehavior = tkZoomBehavior.zbUseTileLevels;
            axMap1.TileProvider = tkTileProvider.OpenStreetMap;
            axMap1.GrabProjectionFromData = false;
            
            m_examples.m_timer.Stop();
        }

        /// <summary>
        /// Makes a snapshot of the map and saves it in the image folder with the name of the example
        /// </summary>
        private void btnSnapshot_Click(object sender, EventArgs e)
        {
            TreeNode node = this.treeView1.SelectedNode;
            if (node != null && node.Tag != null)
            {
                Example ex = node.Tag as Example;
                if (ex != null)
                {
                    MapWinGIS.Image img = (MapWinGIS.Image)this.axMap1.SnapShot(this.axMap1.Extents);
                    if (img != null)
                    {
                        string filename = Path.GetDirectoryName(Application.ExecutablePath) +
                                  @"..\..\..\..\Resources\images\" + ex.function + ".png";
                        img.Save(filename, false, MapWinGIS.ImageType.USE_FILE_EXTENSION, null);
                    }
                }
            }
        }
    }
}
