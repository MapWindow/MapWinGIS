//********************************************************************************************************
//File name: Projections.h
//Description: Sets up the projection definitions to be used by MapWindow and MapWinGIS.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specific language governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//Projection names and WKT definitions are taken from the ESRI projection database.
//
//********************************************************************************************************

#ifndef PROJECTIONS_H
#define PROJECTIONS_H

#include "stdafx.h"
#include "ogr_api.h"

using namespace std;

class ProjectionTools
{
public:
	void ToESRIWKTFromProj4(char ** wkt, char * prj4);
	void GetProj4FromPRJFile(char * prjfileName, char ** prj4);
	int countSpaces(char * str);
	bool compareProj4WithRound5(char * proj1, char * proj2);
	double round(double doValue, int nPrecision);
	bool IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b);
	bool SupportsWorldWideTransform(IGeoProjection* mapProjection, IGeoProjection* wgsProjection);
};

#endif