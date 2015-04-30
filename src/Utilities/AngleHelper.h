#pragma once
class AngleHelper
{
public:
	static CStringW FormatAngle(double angle, tkAngleFormat format, int anglePrecision, bool reducedBearing, bool adjustValue = true);
	static CStringW FormatBearing(double angle, tkBearingType bearingType, tkAngleFormat angleFormat, int anglePrecision);
	static CStringW GetReducedBearing(double azimuth, tkBearingType bearing, tkAngleFormat format, int anglePrecision);
};

