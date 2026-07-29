using System.Windows.Forms;
using WinFormsApp1;

namespace MapWinGisTests
{
	internal static class WinFormsTestRunner
	{
		public static void Run(Action<Form1> action)
		{
			Exception? exception = null;

			var thread = new Thread(() => {
				using var form = new Form1();

				form.Shown += (_, _) => {
					try {
						form.EnsureMapControlCreated();
						Application.DoEvents();

						action(form);
					} catch(Exception ex) {
						exception = ex;
					} finally {
						form.Close();
					}
				};
				Application.Run(form);
			});

			thread.SetApartmentState(ApartmentState.STA);
			thread.Start();
			thread.Join();

			if(exception != null)
				throw exception;
		}
	}
}
