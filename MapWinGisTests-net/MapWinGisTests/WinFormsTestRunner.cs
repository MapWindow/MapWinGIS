using System.Runtime.ExceptionServices;
using System.Windows.Forms;
using WinFormsApp1;

namespace MapWinGisTests
{
	internal static class WinFormsTestRunner
	{
		public static void Run(Action<Form1> action, int timeoutMilliseconds = 120000)
		{
			if(Thread.CurrentThread.GetApartmentState() != ApartmentState.STA)
			{
				throw new InvalidOperationException(
					"WinFormsTestRunner.Run must be called on an STA thread (use [StaFact]/[StaTheory]).");
			}

			// On GitHub Actions there is no interactive desktop session, so showing a real
			// window and running a WinForms message loop (Application.Run) causes the AxMap
			// OCX to deadlock during activation/first paint. Instead, create the control's
			// window handle off-screen and invoke the action directly on this STA thread.
			if(Form1.IsRunningOnGitHubActions)
			{
				RunHeadless(action);
				return;
			}

			RunWithMessageLoop(action, timeoutMilliseconds);
		}

		private static void RunHeadless(Action<Form1> action)
		{
			var form = new Form1();

			try {
				// Force handle creation without ever showing the form. CreateControl()
				// (called inside EnsureMapControlCreated) builds the native window handle
				// for the form and the AxMap OCX without a Show()/Application.Run().
				form.EnsureMapControlCreated();

				// Pump any queued messages the OCX posted during handle creation.
				Application.DoEvents();

				action(form);
			} finally
			{
				// Dispose the control and release all COM references on THIS STA thread,
				// draining the message pump, before the StaFact apartment is torn down.
				DisposeAndDrainStaApartment(form);
			}
		}

		public static void RunWithMessageLoop(Action<Form1> action, int timeoutMilliseconds = 120000)
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

			var form = new Form1();

			try {
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
			}
			finally
			{
				// Dispose the control and release all COM references on THIS STA thread,
				// draining the message pump, before the StaFact apartment is torn down.
				DisposeAndDrainStaApartment(form);
			}

			if(timedOut && exception == null) {
				throw new TimeoutException(
					$"WinForms test did not complete within {timeoutMilliseconds} ms.");
			}

			exception?.Throw();
		}

		/// <summary>
		/// Performs deterministic, in-apartment teardown of the WinForms/AxMap control on the
		/// current STA thread. The native MapWinGIS OCX creates COM objects in this apartment;
		/// releasing them here (rather than letting the GC finalizer thread do it later, from a
		/// different apartment) avoids the cross-apartment release that deadlocks on the loader
		/// lock while the StaFact STA thread is being torn down (observed as a hang on x86).
		///
		/// The sequence is:
		///  1. Dispose the form (runs Form1_FormClosing: RemoveAllLayers + axMap1.Dispose()).
		///  2. Pump the message queue so any WM_* the OCX posted during teardown is processed
		///     while this thread is still pumping.
		///  3. Force COM RCW collection and run finalizers in this apartment.
		///  4. Pump once more so release-driven messages complete before the thread exits.
		/// </summary>
		private static void DisposeAndDrainStaApartment(Form form)
		{
			try {
				if(!form.IsDisposed)
					form.Dispose();
			} catch {
				/* ignore disposal errors during teardown */
			}

			// Drain messages the OCX posted while being disposed.
			Application.DoEvents();

			// Release COM RCWs and run their finalizers on THIS STA thread.
			GC.Collect();
			GC.WaitForPendingFinalizers();
			GC.Collect();
			GC.WaitForPendingFinalizers();

			// Pump once more so any release-driven messages are handled before the
			// STA thread exits and runs DllMain(THREAD_DETACH) under the loader lock.
			Application.DoEvents();
		}
	}
}
