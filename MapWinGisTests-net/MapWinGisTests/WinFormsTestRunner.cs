using System.Runtime.ExceptionServices;
using System.Windows.Forms;
using WinFormsApp1;

namespace MapWinGisTests
{
	internal static class WinFormsTestRunner
	{
		public static void Run(Action<Form1> action, int timeoutMilliseconds = 120000)
		{
			// Runs the test action on the current thread, which is already an STA thread
			// with a message pump supplied by Xunit.StaFact. A single, non-nested message
			// loop is used here so the ActiveX (AxMap) control's owning thread keeps
			// pumping messages while the action executes. This avoids the nested
			// STA-thread + blocking-wait deadlock that occurred when the action re-entered
			// the window/activation code (e.g. via form.Show()).
			if(Thread.CurrentThread.GetApartmentState() != ApartmentState.STA) {
				throw new InvalidOperationException(
					"WinFormsTestRunner.Run must be called on an STA thread (use [StaFact]/[StaTheory]).");
			}

			ExceptionDispatchInfo? exception = null;
			var timedOut = false;

			using var form = new Form1();

			// Close the form (and thus end the message loop) if the action exceeds the timeout.
			using var timeoutTimer = new System.Windows.Forms.Timer { Interval = timeoutMilliseconds };
			timeoutTimer.Tick += (_, _) =>
			{
				timeoutTimer.Stop();
				timedOut = true;
				form.Close();
			};

			form.Shown += (_, _) =>
			{
				// Post the action so it runs after Shown handling completes, while the
				// message loop keeps pumping.
				form.BeginInvoke(() =>
				{
					try {
						form.EnsureMapControlCreated();
						Application.DoEvents();

						action(form);
					} catch(Exception ex) {
						exception = ExceptionDispatchInfo.Capture(ex);
					} finally {
						timeoutTimer.Stop();
						form.Close();
					}
				});
			};

			timeoutTimer.Start();
			Application.Run(form);

			if(timedOut && exception == null) {
				throw new TimeoutException(
					$"WinForms test did not complete within {timeoutMilliseconds} ms.");
			}

			exception?.Throw();
		}
	}
}
