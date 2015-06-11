// adapted from https ://github.com/qgis/QGIS/blob/master/src/core/qgsexpression.cpp

#include "stdafx.h"
#include "Functions.h"

namespace parser
{
	std::vector<CFunction> functions;

	bool fcnSqrt(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(sqrt(args[0]->dbl()));
		return true;
	}

	bool fcnAbs(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(abs(args[0]->dbl()));
		return true;
	}

	bool fcnCos(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(cos(args[0]->dbl()));
		return true;
	}

	bool fcnSin(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(sin(args[0]->dbl()));
		return true;
	}

	bool fcnTan(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(tan(args[0]->dbl()));
		return true;
	}

	bool fcnAsin(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(asin(args[0]->dbl()));
		return true;
	}

	bool fcnAcos(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(acos(args[0]->dbl()));
		return true;
	}

	bool fcnAtan(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(atan(args[0]->dbl()));
		return true;
	}

	bool fcnAtan2(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(atan2(args[0]->dbl(), args[1]->dbl()));
		return true;
	}

	bool fcnExp(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(exp(args[0]->dbl()));
		return true;
	}

	bool fcnLn(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(log(args[0]->dbl()));
		return true;
	}

	bool fcnLog10(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(log10(args[0]->dbl()));
		return true;
	}

	bool fcnLog(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		double b = args[0]->dbl();
		double x = args[1]->dbl();

		if (x <= 0 || b <= 0) {
			return false;
		}

		result.dbl(log(x) / log(b));
		
		return true;
	}

	bool fcnRound(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (args.size() == 2)
		{
			double number = args[0]->dbl();
			double scaler = pow(10.0, args[1]->dbl());
			result.dbl(round(number * scaler) / scaler);
			return true;
		}

		if (args.size() == 1)
		{
			result.dbl(round(args[0]->dbl()));
			return true;
		}

		return false;
	}
	
	bool fcnRand(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		double min = args[0]->dbl();
		double max = args[1]->dbl();
		
		if (max < min) return false;

		result.dbl(min + (rand() % (int)(max - min + 1)));

		return true;
	}

	bool fcnRandF(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		double min = args[0]->dbl();
		double max = args[1]->dbl();

		if (max < min) return false;

		double f = (double)rand() / RAND_MAX;
		result.dbl(min + f * (max - min));

		return true;
	}

	bool fcnMax(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (args.size() == 0) return false;

		double maxVal = -DBL_MAX;

		for (size_t i = 0; i < args.size(); ++i)
		{
			double testVal = args[i]->dbl();
			if (testVal > maxVal)
			{
				maxVal = testVal;
			}
		}

		result.dbl(maxVal);

		return true;
	}

	bool fcnMin(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (args.size() == 0) return false;

		double minVal = DBL_MAX;

		for (size_t i = 0; i < args.size(); ++i)
		{
			double testVal = args[i]->dbl();
			if (testVal < minVal)
			{
				minVal = testVal;
			}
		}

		result.dbl(minVal);

		return true;
	}

	bool fcnClamp(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		double minValue = args[0]->dbl();
		double testValue = args[1]->dbl();
		double maxValue = args[2]->dbl();

		if (testValue <= minValue)
		{
			result.dbl(minValue);
		}
		else if (testValue >= maxValue)
		{
			result.dbl(maxValue);
		}
		else
		{
			result.dbl(testValue);
		}

		return true;
	}

	void InitializeFunctions()
	{
		functions.clear();

		functions.push_back(CFunction("sqrt", 1, fcnSqrt, "Math"));
		functions.push_back(CFunction("abs", 1, fcnAbs, "Math"));
		functions.push_back(CFunction("cos", 1, fcnCos, "Math"));
		functions.push_back(CFunction("sin", 1, fcnSin, "Math"));
		functions.push_back(CFunction("tan", 1, fcnTan, "Math"));
		functions.push_back(CFunction("asin", 1, fcnAsin, "Math"));
		functions.push_back(CFunction("acos", 1, fcnAcos, "Math"));
		functions.push_back(CFunction("atan", 1, fcnAtan, "Math"));
		functions.push_back(CFunction("atan2", 2, fcnAtan2, "Math"));
		functions.push_back(CFunction("exp", 1, fcnExp, "Math"));
		functions.push_back(CFunction("ln", 1, fcnLn, "Math"));
		functions.push_back(CFunction("log10", 1, fcnLog10, "Math"));
		functions.push_back(CFunction("log", 2, fcnLog, "Math"));
		functions.push_back(CFunction("round", -1, fcnRound, "Math"));
		functions.push_back(CFunction("rand", 2, fcnRand, "Math"));
		functions.push_back(CFunction("randf", 2, fcnRandF, "Math"));
		functions.push_back(CFunction("max", -1, fcnMax, "Math"));
		functions.push_back(CFunction("min", -1, fcnMin, "Math"));
		functions.push_back(CFunction("clamp", 3, fcnClamp, "Math"));
		//functions.push_back(CFunction("floor", 1, fcnFloor, "Math"));
		//functions.push_back(CFunction("ceil", 1, fcnCeil, "Math"));
		//functions.push_back(CFunction("pi;$pi", 0, fcnPi, "Math"));
		/*functions.push_back(CFunction("lower", 1, fcnLower, "String"));
		functions.push_back(CFunction("upper", 1, fcnUpper, "String"));
		functions.push_back(CFunction("title", 1, fcnTitle, "String"));
		functions.push_back(CFunction("trim", 1, fcnTrim, "String"));
		functions.push_back(CFunction("wordwrap", -1, fcnWordwrap, "String"));
		functions.push_back(CFunction("length", 1, fcnLength, "String"));
		functions.push_back(CFunction("replace", 3, fcnReplace, "String"));
		functions.push_back(CFunction("regexp_replace", 3, fcnRegexpReplace, "String"));
		functions.push_back(CFunction("regexp_substr", 2, fcnRegexpSubstr, "String"));
		functions.push_back(CFunction("substr", 3, fcnSubstr, "String"));
		functions.push_back(CFunction("concat", -1, fcnConcat, "String"));
		functions.push_back(CFunction("strpos", 2, fcnStrpos, "String"));
		functions.push_back(CFunction("left", 2, fcnLeft, "String"));
		functions.push_back(CFunction("right", 2, fcnRight, "String"));
		functions.push_back(CFunction("rpad", 3, fcnRPad, "String"));
		functions.push_back(CFunction("lpad", 3, fcnLPad, "String"));
		functions.push_back(CFunction("format", -1, fcnFormatString, "String"));
		functions.push_back(CFunction("format_number", 2, fcnFormatNumber, "String"));
		functions.push_back(CFunction("format_date", 2, fcnFormatDate, "String"));*/

		/*functions.push_back(CFunction("scale_linear", 5, fcnLinearScale, "Math"));
		functions.push_back(CFunction("scale_exp", 6, fcnExpScale, "Math"));
		functions.push_back(CFunction("to_int;toint", 1, fcnToInt, "Conversions"));
		functions.push_back(CFunction("to_real;toreal", 1, fcnToReal, "Conversions"));
		functions.push_back(CFunction("to_string;tostring", 1, fcnToString, "Conversions"));
		functions.push_back(CFunction("to_datetime;todatetime", 1, fcnToDateTime, "Conversions"));
		functions.push_back(CFunction("to_date;todate", 1, fcnToDate, "Conversions"));
		functions.push_back(CFunction("to_time;totime", 1, fcnToTime, "Conversions"));
		functions.push_back(CFunction("to_interval;tointerval", 1, fcnToInterval, "Conversions"));
		functions.push_back(CFunction("coalesce", -1, fcnCoalesce, "Conditionals"));
		functions.push_back(CFunction("if", 3, fcnIf, "Conditionals"));
		functions.push_back(CFunction("regexp_match", 2, fcnRegexpMatch, "Conditionals"));
		functions.push_back(CFunction("now;$now", 0, fcnNow, "Date and Time"));
		functions.push_back(CFunction("age", 2, fcnAge, "Date and Time"));
		functions.push_back(CFunction("year", 1, fcnYear, "Date and Time"));
		functions.push_back(CFunction("month", 1, fcnMonth, "Date and Time"));
		functions.push_back(CFunction("week", 1, fcnWeek, "Date and Time"));
		functions.push_back(CFunction("day", 1, fcnDay, "Date and Time"));
		functions.push_back(CFunction("hour", 1, fcnHour, "Date and Time"));
		functions.push_back(CFunction("minute", 1, fcnMinute, "Date and Time"));
		functions.push_back(CFunction("second", 1, fcnSeconds, "Date and Time"));
		
		functions.push_back(CFunction("color_rgb", 3, fcnColorRgb, "Color"));
		functions.push_back(CFunction("color_rgba", 4, fncColorRgba, "Color"));
		functions.push_back(CFunction("ramp_color", 2, fcnRampColor, "Color"));
		functions.push_back(CFunction("color_hsl", 3, fcnColorHsl, "Color"));
		functions.push_back(CFunction("color_hsla", 4, fncColorHsla, "Color"));
		functions.push_back(CFunction("color_hsv", 3, fcnColorHsv, "Color"));
		functions.push_back(CFunction("color_hsva", 4, fncColorHsva, "Color"));
		functions.push_back(CFunction("color_cmyk", 4, fcnColorCmyk, "Color"));
		functions.push_back(CFunction("color_cmyka", 5, fncColorCmyka, "Color"));
		functions.push_back(CFunction("$geometry", 0, fcnGeometry, "GeometryGroup", true));
		functions.push_back(CFunction("$area", 0, fcnGeomArea, "GeometryGroup", true));
		functions.push_back(CFunction("$length", 0, fcnGeomLength, "GeometryGroup", true));
		functions.push_back(CFunction("$perimeter", 0, fcnGeomPerimeter, "GeometryGroup", true));
		functions.push_back(CFunction("$x", 0, fcnX, "GeometryGroup", true));
		functions.push_back(CFunction("$y", 0, fcnY, "GeometryGroup", true));
		functions.push_back(CFunction("x_at;xat", 1, fcnXat, "GeometryGroup", true));
		functions.push_back(CFunction("y_at;yat", 1, fcnYat, "GeometryGroup", true));
		functions.push_back(CFunction("x_min;xmin", 1, fcnXMin, "GeometryGroup", true));
		functions.push_back(CFunction("x_max;xmax", 1, fcnXMax, "GeometryGroup", true));
		functions.push_back(CFunction("y_min;ymin", 1, fcnYMin, "GeometryGroup", true));
		functions.push_back(CFunction("y_max;ymax", 1, fcnYMax, "GeometryGroup", true));
		functions.push_back(CFunction("geom_from_wkt;geomFromWKT", 1, fcnGeomFromWKT, "GeometryGroup"));
		functions.push_back(CFunction("geom_from_gml;geomFromGML", 1, fcnGeomFromGML, "GeometryGroup"));
		functions.push_back(CFunction("intersects_bbox;bbox", 2, fcnBbox, "GeometryGroup"));
		functions.push_back(CFunction("disjoint", 2, fcnDisjoint, "GeometryGroup"));
		functions.push_back(CFunction("intersects", 2, fcnIntersects, "GeometryGroup"));
		functions.push_back(CFunction("touches", 2, fcnTouches, "GeometryGroup"));
		functions.push_back(CFunction("crosses", 2, fcnCrosses, "GeometryGroup"));
		functions.push_back(CFunction("contains", 2, fcnContains, "GeometryGroup"));
		functions.push_back(CFunction("overlaps", 2, fcnOverlaps, "GeometryGroup"));
		functions.push_back(CFunction("within", 2, fcnWithin, "GeometryGroup"));
		functions.push_back(CFunction("buffer", -1, fcnBuffer, "GeometryGroup"));
		functions.push_back(CFunction("centroid", 1, fcnCentroid, "GeometryGroup"));
		functions.push_back(CFunction("bounds", 1, fcnBounds, "GeometryGroup", true));
		functions.push_back(CFunction("bounds_width", 1, fcnBoundsWidth, "GeometryGroup", true));
		functions.push_back(CFunction("bounds_height", 1, fcnBoundsHeight, "GeometryGroup", true));
		functions.push_back(CFunction("convex_hull;convexHull", 1, fcnConvexHull, "GeometryGroup"));
		functions.push_back(CFunction("difference", 2, fcnDifference, "GeometryGroup"));
		functions.push_back(CFunction("distance", 2, fcnDistance, "GeometryGroup"));
		functions.push_back(CFunction("intersection", 2, fcnIntersection, "GeometryGroup"));
		functions.push_back(CFunction("sym_difference;symDifference", 2, fcnSymDifference, "GeometryGroup"));
		functions.push_back(CFunction("combine", 2, fcnCombine, "GeometryGroup"));
		functions.push_back(CFunction("union", 2, fcnCombine, "GeometryGroup"));
		functions.push_back(CFunction("geom_to_wkt;geomToWKT", -1, fcnGeomToWKT, "GeometryGroup"));
		functions.push_back(CFunction("geometry", 1, fcnGetGeometry, "GeometryGroup"));
		functions.push_back(CFunction("transform", 3, fcnTransformGeometry, "GeometryGroup"));
		functions.push_back(CFunction("$rownum", 0, fcnRowNumber, "Record"));
		functions.push_back(CFunction("$id", 0, fcnFeatureId, "Record"));
		functions.push_back(CFunction("$currentfeature", 0, fcnFeature, "Record"));
		functions.push_back(CFunction("$scale", 0, fcnScale, "Record"));
		functions.push_back(CFunction("uuid;$uuid", 0, fcnUuid, "Record"));
		functions.push_back(CFunction("get_feature;getFeature", 3, fcnGetFeature, "Record"));
		functions.push_back(CFunction("attribute", 2, fcnAttribute, "Record"));
		functions.push_back(CFunction("_specialcol_", 1, fcnSpecialColumn, "Special"));*/
	}
}