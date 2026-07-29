using System.Windows.Forms;

namespace MapWinGisTests
{
	internal static class WinFormsTestRunner
	{
		public static void Run(Action action)
		{
			Exception? exception = null;

			var thread = new Thread(() => {
				try {
					Application.EnableVisualStyles();
					Application.SetCompatibleTextRenderingDefault(false);

					using var context = new ApplicationContext();
					action();
				} catch(Exception ex)
				{
					exception = ex;
				}
			});

			thread.SetApartmentState(ApartmentState.STA);
			thread.Start();
			thread.Join();

			if(exception != null)
				throw exception;
		}
	}
}
