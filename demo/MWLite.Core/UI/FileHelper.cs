using System.IO;
using System.Windows.Forms;

namespace MWLite.Core.UI
{
    public static class FileHelper
    {
    private static string GetFilter(FileType fileType)
        {
            switch (fileType)
            {
                case FileType.Project:
                    return "MWLite project state (*.mwxml)|*.mwxml";
                default:
                    return "All files|*.*";
            }
        }

        public static bool ShowOpenDialog(Form form, FileType fileType, out string filename)
        {
            filename = "";
            using (var dlg = new OpenFileDialog())
            {
                dlg.Filter = GetFilter(fileType);
                if (dlg.ShowDialog(form) == DialogResult.OK)
                {
                    filename = dlg.FileName;
                    return true;
                }
            }
            return false;
        }

        public static bool ShowSaveDialog(Form form, FileType fileType, ref string filename)
        {
            bool result = false;
            using (var dlg = new SaveFileDialog { Filter = GetFilter(fileType) })
            {
                if (!string.IsNullOrWhiteSpace(filename))
                {
                    dlg.InitialDirectory = Path.GetDirectoryName(filename);
                    dlg.FileName = Path.GetFileName(filename);
                }
                
                result = dlg.ShowDialog(form) == DialogResult.OK;
                filename = dlg.FileName;
            }
            return result;
        }
    }
}
