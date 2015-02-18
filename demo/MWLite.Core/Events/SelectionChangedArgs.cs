using System;

namespace MWLite.Core.Events
{
    public class SelectionChangedArgs: EventArgs
    {
        public int LayerHandle;
        public int SelectionCount;
    }
}
