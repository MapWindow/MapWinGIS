using System.Drawing;

namespace MWLite.Symbology.Classes
{
    internal static class Utilities
    {
        private static Bitmap bmp = new Bitmap(1, 1);
        private static Graphics g = null;
        public static Graphics FakeGraphics
        {
            get { return g ?? (g = Graphics.FromImage(bmp)); }
        }
    }
}
