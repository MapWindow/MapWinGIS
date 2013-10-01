//********************************************************************************************************
//File name: Projections.h
//Description: Sets up the projection definitions to be used by MapWindow and MapWinGIS.
//********************************************************************************************************
//The contents of this file are subject to the Mozilla Public License Version 1.1 (the "License"); 
//you may not use this file except in compliance with the License. You may obtain a copy of the License at 
//http://www.mozilla.org/MPL/ 
//Software distributed under the License is distributed on an "AS IS" basis, WITHOUT WARRANTY OF 
//ANY KIND, either express or implied. See the License for the specificlanguage governing rights and 
//limitations under the License. 
//
//The Original Code is MapWindow Open Source. 
//
//Projection names and WKT definitions are taken from the ESRI projection database.
//
//Contributor(s): (Open source contributors should list themselves and their modifications here). 
//
//** 1/4/2006 Chris Michaelis - Removed some unnecessary stuff.

#include "stdafx.h"
#include "Projections.h"
#include "cpl_string.h"
#include "ogr_spatialref.h"

using namespace std;

	void ProjectionTools::GetProj4FromPRJFile(char * prjfileName, char ** prj4)
	{
		FILE * pFile;
		pFile = fopen (prjfileName,"r");
		if (pFile == NULL)
		{
			// Doesn't exist
			return;
		}
		fclose(pFile);
		pFile = NULL;
		OGRSpatialReference* oSRS = new OGRSpatialReference();

		char **papszPrj = CSLLoad(prjfileName);
		if (papszPrj == NULL)
		{
			return;
		}

		OGRErr eErr = oSRS->importFromESRI(papszPrj);
		if (eErr != OGRERR_NONE)
		{
			OGRErr eErr = oSRS->importFromProj4(*papszPrj);
		}

		CSLDestroy( papszPrj );
		char * pszProj4 = NULL;
		eErr = oSRS->exportToProj4( &pszProj4 );
		delete oSRS;

		*prj4 = new char[_tcslen(pszProj4)+1];
		strcpy(*prj4, pszProj4);
		CPLFree(pszProj4);
	}

	void ProjectionTools::ToESRIWKTFromProj4(char ** wkt, char * prj4)
	{
		OGRSpatialReference oSRS;

		oSRS.importFromProj4(prj4);
		char * wktStr = NULL;
		//10-Aug-09 Rob Cairns dont want to limit .prj information with morph to esri
		//oSRS.morphToESRI();
		oSRS.exportToWkt(&wktStr);

		*wkt = new char[_tcslen(wktStr)+1];

		strcpy(*wkt, wktStr);
		CPLFree(wktStr);

		//delete oSRS; This object keeps track of itself (see deref above) - deleting here will cause "damage after normal block"-style errors.
	}

	// Chris M Oct 15 2005
	double ProjectionTools::round(double doValue, int nPrecision)
	{
		static const double doBase = 10.0;
		double doComplete5, doComplete5i;
	   
		doComplete5 = doValue * pow(doBase, (double) (nPrecision + 1));
	   
		if(doValue < 0.0)
			doComplete5 -= 5.0;
		else
			doComplete5 += 5.0;
	   
		doComplete5 /= doBase;
		modf(doComplete5, &doComplete5i);
	   
		return doComplete5i / pow(doBase, (double) nPrecision);
	}

	bool ProjectionTools::IsSameProjection(LPCTSTR proj4_a, LPCTSTR proj4_b)
	{
		bool retval = false;

		try
		{
			OGRSpatialReference oSRS_1;
			OGRSpatialReference oSRS_2;

			char * proja = (char*)proj4_a;
			char * projb = (char*)proj4_b;
			
			oSRS_1.importFromProj4(proja);
			oSRS_2.importFromProj4(projb);

			if (oSRS_1.IsSame(&oSRS_2))
				retval = true;
			else
				retval = false;

			if (!retval)
			{
				// Dump to proj4 from the oSRS objects rather than
				// using the passed in values, because the oSRS object
				// will have done the favor of cleaning up the strings.

				char * proja_secondarycomp = NULL;
				char * projb_secondarycomp = NULL;

				oSRS_1.exportToProj4(&proja_secondarycomp);
				oSRS_2.exportToProj4(&projb_secondarycomp);

				// Chris M Oct 15 2005 -- If comparison failed,
				// try rounding each parameter to 8 decimal places
				// to return true for "practically equivalent"
				if (compareProj4WithRound5(proja_secondarycomp, projb_secondarycomp))
					retval = true;
			}
		}
		catch(...)
		{
		}
		return retval;
	}

	// Shade1974 Jan 10, 2006
	// Explicit casting to int using rounding
	int ProjectionTools::countSpaces(char * str)
	{
		register int count= 0;
		for (register unsigned int i = 0; i < _tcslen(str); i++)
		{
			if (isspace(str[i]))
				count++;
		}

		return count;
	}

	// Chris M Oct 15 2005
	bool ProjectionTools::compareProj4WithRound5(char * proj1, char * proj2)
	{
		CString * proj1Params = new CString[countSpaces(proj1) + 1];
		CString * proj2Params = new CString[countSpaces(proj2) + 1];

		char * buffer = new char[1024];
		int bufferindex = 0;
		int proj1Count = 0;
		for (register unsigned int i = 0; i < _tcslen(proj1); i++)
		{
			if (isspace(proj1[i]))
			{
				if (bufferindex > 0)
				{
					buffer[bufferindex] = '\0';
					proj1Params[proj1Count] = buffer;
					bufferindex = 0;
					proj1Count++;
				}
			}
			else
			{
				buffer[bufferindex] = proj1[i];
				bufferindex++;
			}
		}
		// Catch final buffer content
		if (bufferindex > 0)
		{
			buffer[bufferindex] = '\0';
			proj1Params[proj1Count] = buffer;
			bufferindex = 0;
			proj1Count++;
		}

		// Now do projection string 2
		bufferindex = 0;
		int proj2Count = 0;
		for (register unsigned int i = 0; i < _tcslen(proj2); i++)
		{
			if (isspace(proj2[i]))
			{
				if (bufferindex > 0)
				{
					buffer[bufferindex] = '\0';
					proj2Params[proj2Count] = buffer;
					bufferindex = 0;
					proj2Count++;
				}
			}
			else
			{
				buffer[bufferindex] = proj2[i];
				bufferindex++;
			}
		}
		// Catch final buffer content
		if (bufferindex > 0)
		{
			buffer[bufferindex] = '\0';
			proj2Params[proj2Count] = buffer;
			bufferindex = 0;
			proj2Count++;
		}

		// Compare the two proj#Params arrays
		if (proj2Count != proj1Count)
			return false;

		bool allMatched = true;
		for (int i = 0; i < proj1Count; i++)
		{
			bool thisMatched = false;
			for (int j = 0; j < proj2Count; j++)
			{
				if (proj1Params[i].CompareNoCase(proj2Params[j]) == 0)
				{
					thisMatched = true;
					break;
				}
				else // Break out the value and examine
				{
					int pos1 = proj1Params[i].Find("=");
					int pos2 = proj2Params[j].Find("=");

					if (pos1 != -1 && pos2 != -1)
					{
						if (proj1Params[i].Left(pos1).CompareNoCase(proj2Params[j].Left(pos2)) == 0)
						{
							CString value1;
							CString value2;

							value1 = proj1Params[i].Mid(pos1 + 1);
							value2 = proj2Params[j].Mid(pos2 + 1);

							double dvalue1 = atof(value1);
							double dvalue2 = atof(value2);

							if (round(dvalue1, 5) == round(dvalue2, 5))
							{
								thisMatched = true;
								break;
							}
						}
					}
				}
			}
			allMatched = allMatched && thisMatched;
		}
		if (allMatched)
			return true;
		else
			return false;
	}