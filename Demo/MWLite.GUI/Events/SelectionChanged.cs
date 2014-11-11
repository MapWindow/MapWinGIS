using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;

namespace MWLite.GUI.Events
{
    public class SelectionChanged: EventArgs
    {
        public int LayerHandle;
        public int SelectionCount;
    }
}
