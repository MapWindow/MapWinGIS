// ReSharper disable ArrangeAccessorOwnerBody
// ReSharper disable DelegateSubtraction
// ReSharper disable PossibleInvalidCastExceptionInForeachLoop
// ReSharper disable CheckNamespace

// ReSharper disable MergeSequentialChecks
// ReSharper disable SuggestVarOrType_SimpleTypes
// ReSharper disable SuggestVarOrType_Elsewhere
// ReSharper disable SuggestVarOrType_BuiltInTypes
namespace Examples
{
    #region Usings
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Windows.Forms;
    using System.IO;
    using System.Reflection;
    using System.Diagnostics;
    using MapWinGIS;
    #endregion

    public partial class TestForm : Form
    {
        private readonly MapExamples m_examples;
        private readonly Description m_description;
        private string m_dataPath;
        private string m_iconPath;
        
        /// <inheritdoc />
        /// <summary>
        /// Creates a new instance of the form1
        /// </summary>
        public TestForm()
        {
            InitializeComponent();

            MapEvents.AttachMap(axMap1);
            m_examples = new MapExamples {axMap1 = axMap1};
            m_description = new Description();
            FillList();
            treeView1.SelectedNode = treeView1.Nodes[0];
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
        private void FillList()
        {
            m_dataPath = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\Data\";
            m_iconPath = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\icons\";
            if (!Directory.Exists(m_dataPath))
            {
                MessageBox.Show(@"The directory with the data wasn't found:\n" + m_dataPath);
                return;
            }

            treeView1.Nodes.Clear();
            string[] names = Enum.GetNames(typeof(ExampleGroup));
            foreach (string name in names)
            {
                if (name == "None") continue;
                
                TreeNode node = treeView1.Nodes.Add(name);
                node.Expand();
            }
           
            IEnumerable<string> list = m_description.examples.Select(t => t.group.ToString()).Distinct();
            foreach (string s in list)
            {
                IEnumerable<Example> examples = m_description.examples.Where(t => t.group.ToString() == s).OrderBy(t => t.GuiName);
                foreach (Example ex in examples)
                {
                    TreeNode node = new TreeNode() { Text = ex.GuiName, Tag = ex };
                    foreach (TreeNode parent in treeView1.Nodes)
                    {
                        if (parent.Text == ex.group.ToString())
                        {
                            parent.Nodes.Add(node);
                        }
                    }
                }
            }

            treeView1.NodeMouseDoubleClick += delegate
            {
                BtnRunClick();
            };

            treeView1.AfterSelect += treeView1_AfterSelect;
        }

        /// <summary>
        /// Shows description for the selected example
        /// </summary>
        private void treeView1_AfterSelect(object sender, TreeViewEventArgs e)
        {
            lblDescription.Text = @"Description: ";
            TreeNode node = treeView1.SelectedNode;
            if (node == null || node.Tag == null) return;

            Example ex = node.Tag as Example;
            if (ex != null) lblDescription.Text += @"The example demonstrates how to " + ex.description;
        }
        #endregion

        #region Run example
        /// <summary>
        /// Runs the selected example
        /// </summary>
        private void BtnRunClick()
        {
            btnClear_Click_1(null, null);
            
            TreeNode node = treeView1.SelectedNode;
            if (node == null || node.Tag == null) return;

            Example ex = node.Tag as Example;
            if (ex == null) return;

            MethodInfo info = m_examples.GetType().GetMethod(ex.function);
            if (info == null)
            {
                MessageBox.Show(@"Function wasn't found: " + ex.function);
                return;
            }

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
                            param[i] = axMap1;
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
                            param[i] = label1;
                            break;
                    }
                }
                info.Invoke(m_examples, param);
            }
            else
            {
                info.Invoke(m_examples, null);
            }
        }
        #endregion

        /// <summary>
        /// Generates a page with examples
        /// </summary>
        private void GenerateExamples()
        {
            string path = Path.GetDirectoryName(Application.ExecutablePath) + @"..\..\..\..\..\Interop.MapWinGIS\Related pages\examples_list.cs";
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
            axMap1.CursorMode = tkCursorMode.cmZoomIn;
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
            TreeNode node = treeView1.SelectedNode;
            if (node == null || node.Tag == null) return;

            Example ex = node.Tag as Example;
            if (ex == null) return;

            Image img = axMap1.SnapShot(axMap1.Extents);
            if (img == null) return;

            string filename = Path.GetDirectoryName(Application.ExecutablePath) +
                              @"..\..\..\..\Resources\images\" + ex.function + ".png";
            img.Save(filename, false, MapWinGIS.ImageType.USE_FILE_EXTENSION, null);
        }
    }
}
