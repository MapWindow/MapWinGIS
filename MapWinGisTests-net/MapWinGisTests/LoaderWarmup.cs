using System.Runtime.CompilerServices;

namespace MapWinGisTests;

/// <summary>
/// Pre-resolves lazy/forwarded native DLL loads on the process startup thread, before the CLR spins
/// up its STA apartment, tiered-compilation, GDI+ and thread-pool threads.
/// </summary>
/// <remarks>
/// <para>
/// On x86 the test host could hang with a Windows loader-lock deadlock. The trigger was a
/// <see cref="File.Copy(string,string)"/> call in a test fixture: it uses the Win32 <c>CopyFileEx</c>
/// API which, to copy ACLs, <b>delay-loads the forwarded</b> <c>api-ms-win-security-provider</c> DLL.
/// That first-time forwarded load ran <i>inside</i> <c>LdrpLoadForwardedDll</c> at the same moment
/// other runtime threads were being created (each needing <c>LdrpDrainWorkQueue</c>), closing a
/// loader-lock cycle that never resolved. (x64 didn't hang because that module was already resolved.)
/// </para>
/// <para>
/// A <see cref="ModuleInitializer"/> runs while the process is still effectively single-threaded, so
/// performing the same <c>CopyFileEx</c> path here forces the forwarded DLL to resolve safely, outside
/// the concurrent window. The fixtures themselves were also switched to a plain stream copy
/// (<see cref="Helpers.StreamCopyFile"/>) which avoids the path altogether; this warm-up is
/// defense-in-depth for any other code that still calls <see cref="File.Copy(string,string)"/>.
/// </para>
/// </remarks>
internal static class LoaderWarmup
{
	[ModuleInitializer]
	internal static void Initialize()
	{
		ConfigureGdal();

		try {
			// Drive the exact CopyFileEx -> LdrpLoadForwardedDll path once, single-threaded, so the
			// forwarded security-provider DLL is fully resolved before any other threads start.
			var source = Path.Combine(Path.GetTempPath(), $"mwg_warmup_{Guid.NewGuid():N}.tmp");
			var destination = Path.Combine(Path.GetTempPath(), $"mwg_warmup_{Guid.NewGuid():N}.tmp");

			File.WriteAllBytes(source, [0]);
			File.Copy(source, destination, overwrite: true);

			TryDelete(source);
			TryDelete(destination);
		} catch {
			// Warm-up is best-effort. Never fail module initialization because of it.
		}
	}

	private static void TryDelete(string path)
	{
		try {
			if (File.Exists(path)) File.Delete(path);
		} catch {
			// ignore cleanup failures
		}
	}

	private static void ConfigureGdal()
	{
		try {
			// Forces GDAL to run raster operations single-threaded by setting the <c>GDAL_NUM_THREADS</c>
			// /// environment variable before any native GDAL/OCX module is loaded.
			if(string.IsNullOrEmpty(Environment.GetEnvironmentVariable("GDAL_NUM_THREADS")))
				Environment.SetEnvironmentVariable("GDAL_NUM_THREADS", "1");
		} catch {
			// Ignore
		}
	}
}
