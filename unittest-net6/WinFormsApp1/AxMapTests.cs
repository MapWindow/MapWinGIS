namespace WinFormsApp1;
public sealed partial class Form1
{
    public Version GetMapWinGisVersion()
    {
        var versionString = axMap1.VersionNumber;
        var ver = Version.Parse(versionString);
        return ver;
    }
}
