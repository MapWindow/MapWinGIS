#include "stdafx.h"
#include "AngleHelper.h"
#include "GeometryHelper.h"

// *********************************************************
//		FormatAngle()
// *********************************************************	
CStringW AngleHelper::FormatAngle(double angle,
	tkAngleFormat angleType, int precision, bool reducedBearing, bool adjustValue) {
	if (adjustValue) {
		while (angle < -180.0) angle += 360.0;
		while (angle > 180.0) angle -= 360.0;
	}

	CStringW s;

	CStringW format;
	format.Format(L"%d", precision);
	format = L"%." + format + L"f";

	if (angleType == afRadians) {
		s.Format(format + L" " + m_globalSettings.GetLocalizedString(lsRadians), angle / 180.0 * pi_);
		return s;
	}

	if (angleType == tkAngleFormat::afDegrees) {
		s.Format(format + L"%.1f", angle);		//L"%.1f?
		return s;
	}

	// minutes
	format = "%02d";	// ignore fractional part for minutes and seconds
	CStringW degreeFormat = reducedBearing ? "%02d? " : "%d? ";

	int degrees = (int)floor(angle);
	double fMinutes = (angle - degrees) * 60.0;

	if (angleType == tkAngleFormat::afMinutes) {
		int minutes = Utility::Rint(fMinutes);
		if (minutes == 60) {
			minutes = 0;
			degrees += 1;
		}

		s.Format(degreeFormat + format + "'", degrees, minutes);
		return s;
	}

	// seconds
	if (angleType == afSeconds) {
		int minutes = (int)floor(fMinutes);

		double fSeconds = (angle - degrees - minutes / 60.0) * 3600.0;
		int seconds = Utility::Rint(fSeconds);
		if (seconds == 60) {
			seconds = 0;
			minutes += 1;

			if (minutes == 60) {
				minutes = 0;
				degrees += 1;
			}
		}

		s.Format(degreeFormat + "%02d' " + format + "\"", degrees, minutes, seconds);
		return s;
	}

	return L"";
}

// ***************************************************************
//		FormatBearing()
// ***************************************************************
CStringW AngleHelper::FormatBearing(double angle, tkBearingType bearingType, tkAngleFormat angleFormat, int precision) {
	switch (bearingType) {
	case btReducedNDE:
	case btReducedNED:
		return GetReducedBearing(angle, bearingType, angleFormat, precision);
	default:
		return FormatAngle(angle, angleFormat, precision, false, bearingType == btRelative);
	}
}

/****************************************************************************/
/*		GetReducedBearing()													*/
/****************************************************************************/
CStringW AngleHelper::GetReducedBearing(double azimuth, tkBearingType bearing, tkAngleFormat format, int precision) {
	int count = (int)floor(azimuth) / 360;
	double angle = azimuth - count * 360.0;
	count = (int)floor(angle) / 90;

	angle = ((count == 0) || (count == 2)) ? angle - count * 90 : (count + 1) * 90 - angle;

	if (bearing == btReducedNED) {
		tkLocalizedStrings ls;
		switch (count) {
		case 0:	ls = lsNorthEast;	break;
		case 1: ls = lsSouthEast;	break;
		case 2:	ls = lsSouthWest;	break;
		case 3: ls = lsNorthWest;	break;
		}

		CStringW s = m_globalSettings.GetLocalizedString(ls);
		s.Format(L"%s: %s", s, FormatAngle(angle, format, precision, true));
		return s;
	}

	if (bearing == btReducedNDE) {
		tkLocalizedStrings ls1;
		tkLocalizedStrings ls2;
		switch (count) {
		case 0:
			ls1 = lsNorth;
			ls2 = lsEast;
			break;
		case 1:
			ls1 = lsSouth;
			ls2 = lsEast;
			break;
		case 2:
			ls1 = lsSouth;
			ls2 = lsWest;
			break;
		case 3:
			ls1 = lsNorth;
			ls2 = lsWest;
			break;
		}

		CStringW s1 = m_globalSettings.GetLocalizedString(ls1);
		CStringW s2 = m_globalSettings.GetLocalizedString(ls2);
		s1.Format(L"%s %s %s", s1, FormatAngle(angle, format, precision, true), s2);
		return s1;
	}

	return L"";
}

// ***************************************************************
//		ToRad()
// ***************************************************************
double AngleHelper::ToRad(double angleDegree) {
	return angleDegree / 180.0 * pi_;
}
