using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using MWLite.Core;
using MWLite.Core.Exts;
using MWLite.ShapeEditor;

namespace MWLite.GUI.Classes
{
    internal class Project: ProjectBase, IProject
    {
        public bool TryClose()
        {
            if (!Editor.StopAllEditing())
                return false;
            
            if (TryCloseProject())
            {
                App.Map.ShapeEditor.Clear();
                App.Legend.Groups.Clear();
                App.Legend.Layers.Clear();
                App.Map.SetDefaultExtents();
                return true;
            }
            return false;
        }

        public void Load(string filename)
        {
            LoadProject(filename);
        }

        public void SaveAs()
        {
            SaveProjectAs();
        }

        public bool Save()
        {
            return SaveCurrentProject();
        }

        public bool IsEmpty
        {
            get { return Filename.Length == 0; }
        }

        public string GetPath()
        {
            return Filename;
        }

        public ProjectState GetState()
        {
            return State;
        }

    }
}
