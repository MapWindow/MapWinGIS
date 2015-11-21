// adapted from https ://github.com/qgis/QGIS/blob/master/src/core/qgsexpression.cpp

#include "stdafx.h"
#include "Functions.h"
#include "ShapeHelper.h"

namespace parser
{
	std::vector<CustomFunction*> functions;
	map<CString, CustomFunction*> fnMap;

#pragma region Math functions

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
		// TODO: split into separate functions
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

	bool fcnFloor(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(floor(args[0]->dbl()));
		return true;
	}

	bool fcnCeil(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(ceil(args[0]->dbl()));
		return true;
	}

	bool fcnPi(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(pi_);
		return true;
	}

#pragma endregion

#pragma region String functions

	bool fcnLower(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.str(args[0]->str().MakeLower());
		return true;
	}

	bool fcnUpper(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.str(args[0]->str().MakeUpper());
		return true;
	}

	bool fcnTitle(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW arg = args[0]->str();

		CStringW r;

		int position = 0;
		CStringW word = arg.Tokenize(L" ", position);

		while (word.GetLength() > 0)
		{
			if (word.GetLength() > 1)
			{
				r += word.Left(1).MakeUpper() + word.Mid(1).MakeLower() + " ";
			}

			word = arg.Tokenize(L" ", position);
		}

		if (r.GetLength() > 0)
		{
			r = r.Left(r.GetLength() - 1);
		}
		
		result.str(r);
		return true;
	}

	bool fcnTrim(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.str(args[0]->str().Trim());
		return true;
	}

	bool fcnLength(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		result.dbl(args[0]->str().GetLength());
		return true;
	}

	bool fcnReplace(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW s = args[0]->str();
		s.Replace(args[1]->str(), args[2]->str());
		result.str(s);
		return true;
	}

	bool fcnSubstr(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW s = args[0]->str();

		// TODO: split into separate functions
		if (args.size() == 2)
		{
			result.str(s.Mid(static_cast<int>(args[1]->dbl())));
		}
		else if (args.size() == 3)
		{
			result.str(s.Mid(static_cast<int>(args[1]->dbl()), static_cast<int>(args[2]->dbl())));
		}
		else 
		{
			return false;
		}
		
		return true;
	}

	bool fcnConcat(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW s;

		for (size_t i = 0; i < args.size(); i++)
		{
			s += args[i]->str();
		}

		result.str(s);

		return true;
	}

	bool fcnStrpos(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		int pos = args[0]->str().Find(args[1]->str());
		result.dbl(pos);
		return true;
	}

	bool fcnLeft(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW s = args[0]->str().Left(static_cast<int>(args[1]->dbl()));
		result.str(s);
		return true;
	}

	bool fcnRight(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CStringW s = args[0]->str().Right(static_cast<int>(args[1]->dbl()));
		result.str(s);
		return true;
	}

#pragma endregion

#pragma region Common implementations

	bool GetXYCore(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result, bool isX)
	{
		if (!shape) return false;

		double x, y;
		VARIANT_BOOL vb;
		shape->get_XY(0, &x, &y, &vb);

		if (vb)
		{
			result.dbl(isX ? x : y);
			return true;
		}

		return false;
	}

	bool GetXYatCore(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result, bool isX)
	{
		if (!shape) return false;

		long index = static_cast<long>(args[0]->dbl());

		double x, y;
		VARIANT_BOOL vb;
		shape->get_XY(index, &x, &y, &vb);

		if (vb)
		{
			result.dbl(isX ? x : y);
			return true;
		}

		return false;
	}

#pragma endregion	

#pragma region Geometry functions

	bool fcnGeometryToWkt(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (!shape) return false;

		CComBSTR bstr;
		shape->ExportToWKT(&bstr);

		USES_CONVERSION;
		result.str(OLE2A(bstr));

		return true;
	}

	bool fcnGeometryArea(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (!shape) return false;

		ShpfileType shpType = ShapeHelper::GetShapeType2D(shape);
		if (shpType != SHP_POLYGON)
		{
			result.dbl(0.0);
			return true;
		}

		double area;
		shape->get_Area(&area);
		result.dbl(area);

		return true;
	}

	bool fcnGeometryLength(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (!shape) return false;

		ShpfileType shpType = ShapeHelper::GetShapeType2D(shape);
		if (shpType == SHP_POLYLINE )
		{
			double length;
			shape->get_Length(&length);
			result.dbl(length);
			return true;
		}
	
		result.dbl(0.0);
		return true;
	}

	bool fcnGeometryPerimeter(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (!shape) return false;

		ShpfileType shpType = ShapeHelper::GetShapeType2D(shape);
		if (shpType == SHP_POLYGON)
		{
			double area;
			shape->get_Perimeter(&area);
			result.dbl(area);
			return true;
		}

		result.dbl(0.0);
		return true;
	}

	bool fcnX(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		return GetXYCore(args, shape, result, true);
	}

	bool fcnY(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		return GetXYCore(args, shape, result, false);
	}

	bool fcnXat(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		return GetXYatCore(args, shape, result, true);
	}

	bool fcnYat(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		return GetXYatCore(args, shape, result, false);
	}

#pragma endregion

#pragma region Conversion
	
	bool fcnToInt(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		switch (args[0]->type())
		{
			case tkValueType::vtDouble:
				result.dbl(round(args[0]->dbl()));
				break;
			case tkValueType::vtBoolean:
				result.dbl(args[0]->bln() ? 1.0 : 0.0);
				break;
			case tkValueType::vtString:
				result.dbl(_wtoi(args[0]->str()));		// if it's not valid string, return 0
				break;
			case tkValueType::vtFloatArray:
				return false;
		}
		
		return true;
	}

	bool fcnToReal(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		switch (args[0]->type())
		{
			case tkValueType::vtDouble:
				result.dbl(args[0]->dbl());
				break;
			case tkValueType::vtBoolean:
				result.dbl(args[0]->bln() ? 1.0 : 0.0);
				break;
			case tkValueType::vtString:
				result.dbl(Utility::wtof_custom(args[0]->str()));
				break;
			case tkValueType::vtFloatArray:
				return false;
		}

		return true;
	}

	bool fcnToString(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		switch (args[0]->type())
		{
			case tkValueType::vtDouble:
				{
					CStringW s;
					s.Format(L"%g", args[0]->dbl());					// set format as a parameter
					result.str(s);
				}
				break;
			case tkValueType::vtBoolean:
				result.str(args[0]->bln() ? "True" : "False");      // TODO: should they be localized ?
				break;
			case tkValueType::vtString:
				result.str(args[0]->str());
				break;
			case tkValueType::vtFloatArray:
				return false;
		}

		return true;
	}

	bool fcnIf(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		if (args.size() != 3) return false;

		if (args[0]->bln())
		{
			result.copyFrom(*args[1]);
		}
		else
		{
			result.copyFrom(*args[2]);
		}

		return true;
	}

#pragma endregion

	// *****************************************************************
	//		CreateFunctions()
	// *****************************************************************
	void CreateFunctions()
	{
		functions.clear();

		functions.push_back(new CustomFunction(fnSqrt, "sqrt", 1, fcnSqrt, fgMath));
		functions.push_back(new CustomFunction(fnAbs, "abs", 1, fcnAbs, fgMath));
		functions.push_back(new CustomFunction(fnCos, "cos", 1, fcnCos, fgMath));
		functions.push_back(new CustomFunction(fnSin, "sin", 1, fcnSin, fgMath));
		functions.push_back(new CustomFunction(fnTan, "tan", 1, fcnTan, fgMath));
		functions.push_back(new CustomFunction(fnAsin, "asin", 1, fcnAsin, fgMath));
		functions.push_back(new CustomFunction(fnAcos, "acos", 1, fcnAcos, fgMath));
		functions.push_back(new CustomFunction(fnAtan, "atan", 1, fcnAtan, fgMath));
		functions.push_back(new CustomFunction(fnAtan2, "atan2", 2, fcnAtan2, fgMath));
		functions.push_back(new CustomFunction(fnExp, "exp", 1, fcnExp, fgMath));
		functions.push_back(new CustomFunction(fnLn, "ln", 1, fcnLn, fgMath));
		functions.push_back(new CustomFunction(fnLog10, "log10", 1, fcnLog10, fgMath));
		functions.push_back(new CustomFunction(fnLog, "log", 2, fcnLog, fgMath));
		functions.push_back(new CustomFunction(fnRound, "round", -1, fcnRound, fgMath));
		functions.push_back(new CustomFunction(fnRand, "rand", 2, fcnRand, fgMath));
		functions.push_back(new CustomFunction(fnRandf, "randf", 2, fcnRandF, fgMath));
		functions.push_back(new CustomFunction(fnMax, "max", -1, fcnMax, fgMath));
		functions.push_back(new CustomFunction(fnMin, "min", -1, fcnMin, fgMath));
		functions.push_back(new CustomFunction(fnClamp, "clamp", 3, fcnClamp, fgMath));
		functions.push_back(new CustomFunction(fnFloor, "floor", 1, fcnFloor, fgMath));
		functions.push_back(new CustomFunction(fnCeil, "ceil", 1, fcnCeil, fgMath));
		functions.push_back(new CustomFunction(fnPi, "pi;$pi", 0, fcnPi, fgMath));
		/*functions.push_back(new CFunction("scale_linear", 5, fcnLinearScale, fgMath));
		functions.push_back(new CFunction("scale_exp", 6, fcnExpScale, fgMath)); */

		functions.push_back(new CustomFunction(fnLower, "lower", 1, fcnLower, fgStrings));
		functions.push_back(new CustomFunction(fnUpper, "upper", 1, fcnUpper, fgStrings));
		functions.push_back(new CustomFunction(fnTitleCase, "title", 1, fcnTitle, fgStrings));
		functions.push_back(new CustomFunction(fnTrim, "trim", 1, fcnTrim, fgStrings));
		functions.push_back(new CustomFunction(fnLen,"length", 1, fcnLength, fgStrings));
		functions.push_back(new CustomFunction(fnReplace, "replace", 3, fcnReplace, fgStrings));
		functions.push_back(new CustomFunction(fnSubstr, "substr", 3, fcnSubstr, fgStrings));
		functions.push_back(new CustomFunction(fnConcat, "concat", -1, fcnConcat, fgStrings));
		functions.push_back(new CustomFunction(fnStrpos, "strpos", 2, fcnStrpos, fgStrings));
		functions.push_back(new CustomFunction(fnLeft, "left", 2, fcnLeft, fgStrings));
		functions.push_back(new CustomFunction(fnRight, "right", 2, fcnRight, fgStrings));
		/*functions.push_back(new CFunction("regexp_replace", 3, fcnRegexpReplace, fgStrings));
		functions.push_back(new CFunction("wordwrap", -1, fcnWordwrap, fgStrings));
		functions.push_back(new CFunction("regexp_substr", 2, fcnRegexpSubstr, fgStrings));
		functions.push_back(new CFunction("format", -1, fcnFormatString, fgStrings));
		functions.push_back(new CFunction("format_number", 2, fcnFormatNumber, fgStrings));
		functions.push_back(new CFunction("format_date", 2, fcnFormatDate, fgStrings));
		functions.push_back(new CFunction("rpad", 3, fcnRPad, fgStrings));
		functions.push_back(new CFunction("lpad", 3, fcnLPad, fgStrings));*/

		functions.push_back(new CustomFunction(fnToInt, "to_int;toint", 1, fcnToInt, fgConversion));
		functions.push_back(new CustomFunction(fnToReal, "to_real;toreal", 1, fcnToReal, fgConversion));
		functions.push_back(new CustomFunction(fnToString, "to_string;tostring", 1, fcnToString, fgConversion));
		/* functions.push_back(new CFunction("to_datetime;todatetime", 1, fcnToDateTime, fgConversion));
		functions.push_back(new CFunction("to_date;todate", 1, fcnToDate, fgConversion));
		functions.push_back(new CFunction("to_time;totime", 1, fcnToTime, fgConversion));
		functions.push_back(new CFunction("to_interval;tointerval", 1, fcnToInterval, fgConversion)); */

		
		functions.push_back(new CustomFunction(fnIf, "if;iif", 3, fcnIf, fgBranching));

		/*
		functions.push_back(new CFunction("coalesce", -1, fcnCoalesce, "Conditions"));
		functions.push_back(new CFunction("regexp_match", 2, fcnRegexpMatch, "Conditions")); */

		/*
		functions.push_back(new CFunction("now;$now", 0, fcnNow, "Date and Time"));
		functions.push_back(new CFunction("age", 2, fcnAge, "Date and Time"));
		functions.push_back(new CFunction("year", 1, fcnYear, "Date and Time"));
		functions.push_back(new CFunction("month", 1, fcnMonth, "Date and Time"));
		functions.push_back(new CFunction("week", 1, fcnWeek, "Date and Time"));
		functions.push_back(new CFunction("day", 1, fcnDay, "Date and Time"));
		functions.push_back(new CFunction("hour", 1, fcnHour, "Date and Time"));
		functions.push_back(new CFunction("minute", 1, fcnMinute, "Date and Time"));
		functions.push_back(new CFunction("second", 1, fcnSeconds, "Date and Time"));

		functions.push_back(new CFunction("color_rgb", 3, fcnColorRgb, "Color"));
		functions.push_back(new CFunction("color_rgba", 4, fncColorRgba, "Color"));
		functions.push_back(new CFunction("ramp_color", 2, fcnRampColor, "Color"));
		functions.push_back(new CFunction("color_hsl", 3, fcnColorHsl, "Color"));
		functions.push_back(new CFunction("color_hsla", 4, fncColorHsla, "Color"));
		functions.push_back(new CFunction("color_hsv", 3, fcnColorHsv, "Color"));
		functions.push_back(new CFunction("color_hsva", 4, fncColorHsva, "Color"));
		functions.push_back(new CFunction("color_cmyk", 4, fcnColorCmyk, "Color"));
		functions.push_back(new CFunction("color_cmyka", 5, fncColorCmyka, "Color"));*/

		functions.push_back(new CustomFunction(fnGeometryToWkt, "$geomToWkt", 0, fcnGeometryToWkt, fgGeometry, true));
		functions.push_back(new CustomFunction(fnArea, "$area", 0, fcnGeometryArea, fgGeometry, true));
		functions.push_back(new CustomFunction(fnLength, "$length", 0, fcnGeometryLength, fgGeometry, true));
		functions.push_back(new CustomFunction(fnPerimeter, "$perimeter", 0, fcnGeometryPerimeter, fgGeometry, true));
		functions.push_back(new CustomFunction(fnX, "$x", 0, fcnX, fgGeometry, true));
		functions.push_back(new CustomFunction(fnY, "$y", 0, fcnY, fgGeometry, true));
		functions.push_back(new CustomFunction(fnXat, "x_at;xat", 1, fcnXat, fgGeometry, true));
		functions.push_back(new CustomFunction(fnYat, "y_at;yat", 1, fcnYat, fgGeometry, true));

		/*
		functions.push_back(new CFunction("x_min;xmin", 1, fcnXMin, fgGeometry, true));
		functions.push_back(new CFunction("x_max;xmax", 1, fcnXMax, fgGeometry, true));
		functions.push_back(new CFunction("y_min;ymin", 1, fcnYMin, fgGeometry, true));
		functions.push_back(new CFunction("y_max;ymax", 1, fcnYMax, fgGeometry, true));
		functions.push_back(new CFunction("geom_from_wkt;geomFromWKT", 1, fcnGeomFromWKT, fgGeometry));
		functions.push_back(new CFunction("geom_from_gml;geomFromGML", 1, fcnGeomFromGML, fgGeometry));
		functions.push_back(new CFunction("intersects_bbox;bbox", 2, fcnBbox, fgGeometry));
		functions.push_back(new CFunction("disjoint", 2, fcnDisjoint, fgGeometry));
		functions.push_back(new CFunction("intersects", 2, fcnIntersects, fgGeometry));
		functions.push_back(new CFunction("touches", 2, fcnTouches, fgGeometry));
		functions.push_back(new CFunction("crosses", 2, fcnCrosses, fgGeometry));
		functions.push_back(new CFunction("contains", 2, fcnContains, fgGeometry));
		functions.push_back(new CFunction("overlaps", 2, fcnOverlaps, fgGeometry));
		functions.push_back(new CFunction("within", 2, fcnWithin, fgGeometry));
		functions.push_back(new CFunction("buffer", -1, fcnBuffer, fgGeometry));
		functions.push_back(new CFunction("centroid", 1, fcnCentroid, fgGeometry));
		functions.push_back(new CFunction("bounds", 1, fcnBounds, fgGeometry, true));
		functions.push_back(new CFunction("bounds_width", 1, fcnBoundsWidth, fgGeometry, true));
		functions.push_back(new CFunction("bounds_height", 1, fcnBoundsHeight, fgGeometry, true));
		functions.push_back(new CFunction("convex_hull;convexHull", 1, fcnConvexHull, fgGeometry));
		functions.push_back(new CFunction("difference", 2, fcnDifference, fgGeometry));
		functions.push_back(new CFunction("distance", 2, fcnDistance, fgGeometry));
		functions.push_back(new CFunction("intersection", 2, fcnIntersection, fgGeometry));
		functions.push_back(new CFunction("sym_difference;symDifference", 2, fcnSymDifference, fgGeometry));
		functions.push_back(new CFunction("combine", 2, fcnCombine, fgGeometry));
		functions.push_back(new CFunction("union", 2, fcnCombine, fgGeometry));
		functions.push_back(new CFunction("geom_to_wkt;geomToWKT", -1, fcnGeomToWKT, fgGeometry));
		functions.push_back(new CFunction(fgGeometry, 1, fcnGetGeometry, fgGeometry));
		functions.push_back(new CFunction("transform", 3, fcnTransformGeometry, fgGeometry));*/

		/*
		functions.push_back(new CFunction("$rownum", 0, fcnRowNumber, "Record"));
		functions.push_back(new CFunction("$id", 0, fcnFeatureId, "Record"));
		functions.push_back(new CFunction("$currentfeature", 0, fcnFeature, "Record"));
		functions.push_back(new CFunction("$scale", 0, fcnScale, "Record"));
		functions.push_back(new CFunction("uuid;$uuid", 0, fcnUuid, "Record"));
		functions.push_back(new CFunction("get_feature;getFeature", 3, fcnGetFeature, "Record"));
		functions.push_back(new CFunction("attribute", 2, fcnAttribute, "Record"));
		functions.push_back(new CFunction("_specialcol_", 1, fcnSpecialColumn, "Special"));*/
	}

	// *****************************************************************
	//		BuildMap()
	// *****************************************************************
	void BuildMap()
	{
		fnMap.clear();

		for (size_t i = 0; i < functions.size(); i++)
		{
			CustomFunction* fn = functions[i];
			vector<CString>* aliases = fn->getAliases();

			for (size_t j = 0; j < aliases->size(); j++)
			{
				CString key = (*aliases)[j].MakeLower();

				if (fnMap.find(key) != fnMap.end())
				{
					CallbackHelper::AssertionFailed("Duplicate name of the function: " + key);
				}
				else
				{
					fnMap[key] = fn;
				}
			}
		}
	}

	// *****************************************************************
	//		ReleaseFunctions()
	// *****************************************************************
	void ReleaseFunctions()
	{
		for (size_t i = 0; i < functions.size(); i++)
		{
			delete functions[i];
		}

		functions.clear();

		fnMap.clear();
	}

	// *****************************************************************
	//		InitializeFunctions()
	// *****************************************************************
	void InitializeFunctions()
	{
		CreateFunctions();

		BuildMap();
	}

	// *****************************************************************
	//		GetFunction()
	// *****************************************************************
	CustomFunction* GetFunction(CString name)
	{
		map<CString, CustomFunction*>::iterator it = fnMap.find(name.MakeLower());
		return it != fnMap.end() ? it->second : NULL; 
	}
}