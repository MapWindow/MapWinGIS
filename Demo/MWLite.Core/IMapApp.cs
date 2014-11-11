using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Windows.Forms;
using MapWindow.Legend;
using MapWindow.Legend.Controls.Legend;

namespace MWLite.Core
{
    public interface IMapApp
    {
        Legend Legend { get; }
        AxMapWinGIS.AxMap Map { get; }
        void RefreshUI();
        void AddMenu(ToolStripMenuItem menu);
        void AddToolbar(ToolStrip toolbar);
        IProject Project { get; }
        
    }

    public interface IProject
    {
        bool IsEmpty {get;}
        string GetPath();
        ProjectState GetState();
        bool Save();
        void SaveAs();
        void Load(string filename);
        bool TryClose();
        event EventHandler<EventArgs> ProjectChanged;
    }
}
