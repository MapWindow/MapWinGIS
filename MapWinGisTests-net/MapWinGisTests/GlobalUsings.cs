global using System;
global using MapWinGIS;
global using Xunit;
global using Xunit.Abstractions;
global using Shouldly;


namespace MapWinGisTests;
[CollectionDefinition(nameof(NotThreadSafeResourceCollection), DisableParallelization = true)]
public class NotThreadSafeResourceCollection { }
