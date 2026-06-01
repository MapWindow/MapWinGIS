using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using JetBrains.Annotations;

namespace Shouldly
{
	public static class ShouldBeExtensions
	{
		//ContractAnnotation("actual:notnull => halt")]
		public static void ShouldBeEqualWithin(this double? actual, double expected, double withinAmount, string? customMessage = null)
		{
			if(!actual.HasValue)
				throw new ShouldAssertException("actual value is null");

			if(Math.Abs(actual.Value - expected) > withinAmount)
				throw new ShouldAssertException($"{actual.Value} is not equal to {expected} withing {withinAmount}");
		}
	}
}
