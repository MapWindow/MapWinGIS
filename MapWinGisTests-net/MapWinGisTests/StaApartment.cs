using System.Runtime.ExceptionServices;
using System.Threading;
using System.Windows.Forms;

namespace MapWinGisTests;

/// <summary>
/// Owns a single, long-lived STA thread with an active WinForms message pump for the whole
/// lifetime of the test collection.
///
/// Why this exists: xUnit's <c>[StaFact]</c> rents (creates) and tears down a fresh STA thread
/// for every test. On the 32-bit (x86) test host this causes a loader-lock deadlock: while one
/// finished test's STA thread runs <c>DllMain(THREAD_DETACH)</c> for the native MapWinGIS/OCX
/// modules under the loader lock, the next test's STA thread runs <c>DllMain(THREAD_ATTACH)</c>,
/// and a COM/RPC worker unloads a DLL - all contending for the process-wide loader lock.
///
/// By running every AxMap test on one persistent STA thread, the native modules attach once at
/// startup and detach once at shutdown, eliminating the per-test thread attach/detach churn that
/// triggers the deadlock. The collection is already serialized (DisableParallelization = true),
/// so a single apartment is sufficient.
/// </summary>
public sealed class StaApartment : IDisposable
{
	private readonly Thread _thread;
	private readonly ManualResetEventSlim _ready = new(false);
	private Control _marshal = null!;
	private ApplicationContext _appContext = null!;

	/// <summary>
	/// The apartment created for the currently executing test collection. Set when the collection
	/// fixture is constructed and cleared on disposal. Used by <see cref="WinFormsTestRunner"/> to
	/// marshal test actions onto the shared STA thread.
	/// </summary>
	public static StaApartment? Current { get; private set; }

	public StaApartment()
	{
		_thread = new Thread(ThreadMain)
		{
			IsBackground = true,
			Name = "MapWinGIS STA Test Apartment",
		};
		_thread.SetApartmentState(ApartmentState.STA);
		_thread.Start();

		// Wait until the STA thread has created its marshaling control and started pumping.
		_ready.Wait();

		Current = this;
	}

	private void ThreadMain()
	{
		// A hidden control gives us a window handle on this STA thread to marshal work onto via
		// Control.Invoke. Accessing Handle forces handle creation even though the control is never
		// shown.
		_marshal = new Control();
		_ = _marshal.Handle;

		_appContext = new ApplicationContext();

		// Prime UI Automation on this STA thread while it is idle (before the message pump below
		// becomes busy servicing tests). On the 32-bit host, the first UI Automation activation
		// happens lazily inside Control.ReleaseUiaProvider during WM_DESTROY when a form is torn
		// down. That first-time activation makes a cross-apartment COM call into
		// UIAutomationCore!DoInit; if it happens while the STA thread is mid-teardown it deadlocks.
		// Forcing the initialization here - once, on an otherwise idle thread - means later form
		// disposals only release an already-initialized provider and never trigger that activation.
		WarmUpUiAutomation();

		_ready.Set();

		// Keep pumping messages for the whole test run so the ActiveX (AxMap) control's owning
		// apartment always has a live message loop.
		Application.Run(_appContext);
	}

	/// <summary>
	/// Forces first-time UI Automation / accessibility initialization on this STA thread by
	/// creating a throwaway control, realizing its accessible object, and then destroying it. Doing
	/// this once at startup avoids the lazy UIA activation that otherwise deadlocks on the 32-bit
	/// host during form teardown (see <see cref="ThreadMain"/>).
	/// </summary>
	private static void WarmUpUiAutomation()
	{
		try {
			using var warmup = new Control();
			// Force handle creation so the control has a real HWND.
			_ = warmup.Handle;

			// Touching AccessibilityObject realizes the control's accessible/UIA provider, which
			// drives the same UIAutomationCore initialization that Control.ReleaseUiaProvider hits
			// during WM_DESTROY. Doing it here, on an idle STA thread, primes that init safely so
			// later form teardown only releases an already-initialized provider.
			_ = warmup.AccessibilityObject;

			// Disposing now runs the release path once, while the thread is idle rather than
			// mid-teardown.
		} catch {
			// Warm-up is best-effort; it must not fail apartment startup.
		}
	}

	/// <summary>
	/// Runs <paramref name="action"/> synchronously on the shared STA thread, marshaling any
	/// exception back to the caller.
	/// </summary>
	public void Invoke(Action action)
	{
		ExceptionDispatchInfo? exception = null;

		_marshal.Invoke(new Action(() =>
		{
			try {
				action();
			} catch(Exception ex) {
				exception = ExceptionDispatchInfo.Capture(ex);
			}
		}));

		exception?.Throw();
	}

	public void Dispose()
	{
		if(_thread.IsAlive)
		{
			try {
				_marshal.Invoke(new Action(() =>
				{
					// Drain COM RCWs and run their finalizers on THIS STA thread while it is still
					// alive and pumping, so native COM release does not run cross-apartment.
					GC.Collect();
					GC.WaitForPendingFinalizers();
					GC.Collect();
					GC.WaitForPendingFinalizers();

					_marshal.Dispose();

					// Exit the message loop so the STA thread ends and runs
					// DllMain(THREAD_DETACH) exactly once, with nothing else contending for the
					// loader lock.
					_appContext.ExitThread();
				}));
			} catch {
				/* ignore teardown errors */
			}

			_thread.Join(TimeSpan.FromSeconds(30));
		}

		Current = null;
		_ready.Dispose();
	}
}
