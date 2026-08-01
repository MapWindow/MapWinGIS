using System.Runtime.ExceptionServices;
using System.Windows.Forms;
using WinFormsApp1;

namespace MapWinGisTests
{
	internal static class WinFormsTestRunner
	{
		public static void Run(Action<Form1> action, int timeoutMilliseconds = 120000)
		{
			ExceptionDispatchInfo? exception = null;

			var completed = new ManualResetEventSlim(false);

			var thread = new Thread(() => {
				try {
					using var form = new Form1();

					form.Shown += (_, _) =>
					{
						form.BeginInvoke(() =>
						{
							try {
								form.EnsureMapControlCreated();
								Application.DoEvents();

								action(form);
							} catch(Exception ex) {
								exception = ExceptionDispatchInfo.Capture(ex);
							} finally {
								completed.Set();
								form.Close();
							}
						});
					};
					Application.Run(form);
				} catch(Exception ex) {
					exception = ExceptionDispatchInfo.Capture(ex);
					completed.Set();
				}
			});

			thread.SetApartmentState(ApartmentState.STA);
			thread.Start();

			if(!completed.Wait(timeoutMilliseconds)) {
				throw new TimeoutException(
					$"WinForms test did not complete within {timeoutMilliseconds} ms.");
			}

			thread.Join();

			exception?.Throw();
		}
	}
}
