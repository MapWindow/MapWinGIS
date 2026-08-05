global using System;
global using MapWinGIS;
global using Xunit;
global using Shouldly;

namespace MapWinGisTests;

/// <summary>
/// Shared fixture for the non-thread-safe collection. Its <see cref="Dispose"/> runs once
/// after every test class in the collection has finished, while the .NET runtime is still
/// alive. It forces all MapWinGIS COM RCWs to be collected and their finalizers to run now,
/// so native COM release does not re-enter managed code during process shutdown after the
/// runtime thread state has been destroyed (which crashes the test host).
/// </summary>
public sealed class NotThreadSafeResourceFixture : IDisposable
{
    public void Dispose()
    {
        // Make sure the global native callback registration is cleared.
        _ = new GlobalSettings { ApplicationCallback = null };

        // Release all COM RCWs and run their finalizers while the runtime is still alive.
        GC.Collect();
        GC.WaitForPendingFinalizers();
        GC.Collect();
        GC.WaitForPendingFinalizers();
    }
}

[CollectionDefinition(nameof(NotThreadSafeResourceCollection), DisableParallelization = true)]
public class NotThreadSafeResourceCollection
    : ICollectionFixture<NotThreadSafeResourceFixture>,
      ICollectionFixture<StaApartment>
{ }
