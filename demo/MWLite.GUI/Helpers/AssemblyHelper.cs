using System.Reflection;

namespace MWLite.GUI.Helpers
{
    internal static class AssemblyHelper
    {
        public static string AppName
        {
            get { return Assembly.GetExecutingAssembly().GetName().Name; }
        }
    }
}
