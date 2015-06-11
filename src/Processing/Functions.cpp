// adapted from https ://github.com/qgis/QGIS/blob/master/src/core/qgsexpression.cpp

#include "stdafx.h"
#include "Functions.h"

namespace parser
{
	std::vector<CFunction*> functions;
	map<CString, CFunction*> fnMap;

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
		CString arg = args[0]->str();

		CString r;

		int position = 0;
		CString word = arg.Tokenize(" ", position);

		while (word.GetLength() > 0)
		{
			if (word.GetLength() > 1)
			{
				r += word.Left(1).MakeUpper() + word.Mid(1).MakeLower() + " ";
			}

			word = arg.Tokenize(" ", position);
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
		CString s = args[0]->str();
		s.Replace(args[1]->str(), args[2]->str());
		result.str(s);
		return true;
	}

	bool fcnSubstr(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CString s = args[0]->str();

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
		CString s;

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
		CString s = args[0]->str().Left(static_cast<int>(args[1]->dbl()));
		result.str(s);
		return true;
	}

	bool fcnRight(const vector<CExpressionValue*>& args, IShape* shape, CExpressionValue& result)
	{
		CString s = args[0]->str().Right(static_cast<int>(args[1]->dbl()));
		result.str(s);
		return true;
	}

	void CreateFunctions()
	{
		functions.clear();

		functions.push_back(new CFunction("sqrt", 1, fcnSqrt, "Math"));
		functions.push_back(new CFunction("abs", 1, fcnAbs, "Math"));
		functions.push_back(new CFunction("cos", 1, fcnCos, "Math"));
		functions.push_back(new CFunction("sin", 1, fcnSin, "Math"));
		functions.push_back(new CFunction("tan", 1, fcnTan, "Math"));
		functions.push_back(new CFunction("asin", 1, fcnAsin, "Math"));
		functions.push_back(new CFunction("acos", 1, fcnAcos, "Math"));
		functions.push_back(new CFunction("atan", 1, fcnAtan, "Math"));
		functions.push_back(new CFunction("atan2", 2, fcnAtan2, "Math"));
		functions.push_back(new CFunction("exp", 1, fcnExp, "Math"));
		functions.push_back(new CFunction("ln", 1, fcnLn, "Math"));
		functions.push_back(new CFunction("log10", 1, fcnLog10, "Math"));
		functions.push_back(new CFunction("log", 2, fcnLog, "Math"));
		functions.push_back(new CFunction("round", -1, fcnRound, "Math"));
		functions.push_back(new CFunction("rand", 2, fcnRand, "Math"));
		functions.push_back(new CFunction("randf", 2, fcnRandF, "Math"));
		functions.push_back(new CFunction("max", -1, fcnMax, "Math"));
		functions.push_back(new CFunction("min", -1, fcnMin, "Math"));
		functions.push_back(new CFunction("clamp", 3, fcnClamp, "Math"));
		functions.push_back(new CFunction("floor", 1, fcnFloor, "Math"));
		functions.push_back(new CFunction("ceil", 1, fcnCeil, "Math"));
		functions.push_back(new CFunction("pi;$pi", 0, fcnPi, "Math"));
		/*functions.push_back(new CFunction("scale_linear", 5, fcnLinearScale, "Math"));
		functions.push_back(new CFunction("scale_exp", 6, fcnExpScale, "Math")); */

		functions.push_back(new CFunction("lower", 1, fcnLower, "String"));
		functions.push_back(new CFunction("upper", 1, fcnUpper, "String"));
		functions.push_back(new CFunction("title", 1, fcnTitle, "String"));
		functions.push_back(new CFunction("trim", 1, fcnTrim, "String"));
		functions.push_back(new CFunction("length", 1, fcnLength, "String"));
		functions.push_back(new CFunction("replace", 3, fcnReplace, "String"));
		functions.push_back(new CFunction("substr", 3, fcnSubstr, "String"));
		functions.push_back(new CFunction("concat", -1, fcnConcat, "String"));
		functions.push_back(new CFunction("strpos", 2, fcnStrpos, "String"));
		functions.push_back(new CFunction("left", 2, fcnLeft, "String"));
		functions.push_back(new CFunction("right", 2, fcnRight, "String"));
		/*functions.push_back(new CFunction("regexp_replace", 3, fcnRegexpReplace, "String"));
		functions.push_back(new CFunction("wordwrap", -1, fcnWordwrap, "String"));
		functions.push_back(new CFunction("regexp_substr", 2, fcnRegexpSubstr, "String"));
		functions.push_back(new CFunction("format", -1, fcnFormatString, "String"));
		functions.push_back(new CFunction("format_number", 2, fcnFormatNumber, "String"));
		functions.push_back(new CFunction("format_date", 2, fcnFormatDate, "String"));
		functions.push_back(new CFunction("rpad", 3, fcnRPad, "String"));
		functions.push_back(new CFunction("lpad", 3, fcnLPad, "String"));*/

		/*functions.push_back(new CFunction("to_int;toint", 1, fcnToInt, "Conversions"));
		functions.push_back(new CFunction("to_real;toreal", 1, fcnToReal, "Conversions"));
		functions.push_back(new CFunction("to_string;tostring", 1, fcnToString, "Conversions"));
		functions.push_back(new CFunction("to_datetime;todatetime", 1, fcnToDateTime, "Conversions"));
		functions.push_back(new CFunction("to_date;todate", 1, fcnToDate, "Conversions"));
		functions.push_back(new CFunction("to_time;totime", 1, fcnToTime, "Conversions"));
		functions.push_back(new CFunction("to_interval;tointerval", 1, fcnToInterval, "Conversions"));

		functions.push_back(new CFunction("coalesce", -1, fcnCoalesce, "Conditionals"));
		functions.push_back(new CFunction("if", 3, fcnIf, "Conditionals"));
		functions.push_back(new CFunction("regexp_match", 2, fcnRegexpMatch, "Conditionals"));

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
		functions.push_back(new CFunction("color_cmyka", 5, fncColorCmyka, "Color"));

		functions.push_back(new CFunction("$geometry", 0, fcnGeometry, "Geometry", true));
		functions.push_back(new CFunction("$area", 0, fcnGeomArea, "Geometry", true));
		functions.push_back(new CFunction("$length", 0, fcnGeomLength, "Geometry", true));
		functions.push_back(new CFunction("$perimeter", 0, fcnGeomPerimeter, "Geometry", true));
		functions.push_back(new CFunction("$x", 0, fcnX, "Geometry", true));
		functions.push_back(new CFunction("$y", 0, fcnY, "Geometry", true));
		functions.push_back(new CFunction("x_at;xat", 1, fcnXat, "Geometry", true));
		functions.push_back(new CFunction("y_at;yat", 1, fcnYat, "Geometry", true));
		functions.push_back(new CFunction("x_min;xmin", 1, fcnXMin, "Geometry", true));
		functions.push_back(new CFunction("x_max;xmax", 1, fcnXMax, "Geometry", true));
		functions.push_back(new CFunction("y_min;ymin", 1, fcnYMin, "Geometry", true));
		functions.push_back(new CFunction("y_max;ymax", 1, fcnYMax, "Geometry", true));
		functions.push_back(new CFunction("geom_from_wkt;geomFromWKT", 1, fcnGeomFromWKT, "Geometry"));
		functions.push_back(new CFunction("geom_from_gml;geomFromGML", 1, fcnGeomFromGML, "Geometry"));
		functions.push_back(new CFunction("intersects_bbox;bbox", 2, fcnBbox, "Geometry"));
		functions.push_back(new CFunction("disjoint", 2, fcnDisjoint, "Geometry"));
		functions.push_back(new CFunction("intersects", 2, fcnIntersects, "Geometry"));
		functions.push_back(new CFunction("touches", 2, fcnTouches, "Geometry"));
		functions.push_back(new CFunction("crosses", 2, fcnCrosses, "Geometry"));
		functions.push_back(new CFunction("contains", 2, fcnContains, "Geometry"));
		functions.push_back(new CFunction("overlaps", 2, fcnOverlaps, "Geometry"));
		functions.push_back(new CFunction("within", 2, fcnWithin, "Geometry"));
		functions.push_back(new CFunction("buffer", -1, fcnBuffer, "Geometry"));
		functions.push_back(new CFunction("centroid", 1, fcnCentroid, "Geometry"));
		functions.push_back(new CFunction("bounds", 1, fcnBounds, "Geometry", true));
		functions.push_back(new CFunction("bounds_width", 1, fcnBoundsWidth, "Geometry", true));
		functions.push_back(new CFunction("bounds_height", 1, fcnBoundsHeight, "Geometry", true));
		functions.push_back(new CFunction("convex_hull;convexHull", 1, fcnConvexHull, "Geometry"));
		functions.push_back(new CFunction("difference", 2, fcnDifference, "Geometry"));
		functions.push_back(new CFunction("distance", 2, fcnDistance, "Geometry"));
		functions.push_back(new CFunction("intersection", 2, fcnIntersection, "Geometry"));
		functions.push_back(new CFunction("sym_difference;symDifference", 2, fcnSymDifference, "Geometry"));
		functions.push_back(new CFunction("combine", 2, fcnCombine, "Geometry"));
		functions.push_back(new CFunction("union", 2, fcnCombine, "Geometry"));
		functions.push_back(new CFunction("geom_to_wkt;geomToWKT", -1, fcnGeomToWKT, "Geometry"));
		functions.push_back(new CFunction("geometry", 1, fcnGetGeometry, "Geometry"));
		functions.push_back(new CFunction("transform", 3, fcnTransformGeometry, "Geometry"));

		functions.push_back(new CFunction("$rownum", 0, fcnRowNumber, "Record"));
		functions.push_back(new CFunction("$id", 0, fcnFeatureId, "Record"));
		functions.push_back(new CFunction("$currentfeature", 0, fcnFeature, "Record"));
		functions.push_back(new CFunction("$scale", 0, fcnScale, "Record"));
		functions.push_back(new CFunction("uuid;$uuid", 0, fcnUuid, "Record"));
		functions.push_back(new CFunction("get_feature;getFeature", 3, fcnGetFeature, "Record"));
		functions.push_back(new CFunction("attribute", 2, fcnAttribute, "Record"));
		functions.push_back(new CFunction("_specialcol_", 1, fcnSpecialColumn, "Special"));*/
	}

	void BuildMap()
	{
		fnMap.clear();

		for (size_t i = 0; i < functions.size(); i++)
		{
			CFunction* fn = functions[i];
			vector<CString>* aliases = fn->GetAliases();

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

	void ReleaseFunctions()
	{
		for (size_t i = 0; i < functions.size(); i++)
		{
			delete functions[i];
		}

		functions.clear();

		fnMap.clear();
	}

	void InitializeFunctions()
	{
		CreateFunctions();

		BuildMap();
	}

	CFunction* GetFunction(CString name)
	{
		map<CString, CFunction*>::iterator it = fnMap.find(name.MakeLower());
		return it != fnMap.end() ? it->second : NULL; 
	}
}