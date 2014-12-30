#include "stdafx.h"
#include "BingMapProvider.h"

// ******************************************************
//    Initialize()
// ******************************************************
// Runs imagery metadata request: http://msdn.microsoft.com/en-us/library/ff701716.aspx
bool BingBaseProvider::Initialize()
{
	if (UrlFormat.GetLength() > 0) return true;

	if (m_globalSettings.bingApiKey.GetLength() == 0)
	{
		CallbackHelper::ErrorMsg("No Bing Maps API key was provided. See GlobalSettings.BingApiKey.");
		return false;
	}

	_initAttemptCount++;
	if (_initAttemptCount > 3)
	{
		CallbackHelper::ErrorMsg("Number of initialization attempts for Bing Maps provider was exceeded (3).");
		return false;
	}

	MyHttpClient httpClient;
	InitHttpClient(httpClient);

	CAtlNavigateData navData;

	CString url;
	url.Format("http://dev.virtualearth.net/REST/v1/Imagery/Metadata/%s?key=%s&o=xml", _imagerySet, m_globalSettings.bingApiKey);

	bool result = false;

	if (!httpClient.Navigate(url, &navData) || httpClient.GetStatus() != 200)
	{
		CallbackHelper::ErrorMsg(Debug::Format("Failed to perform imagery metadata request. URL: ", url));
		return false;
	}
	
	int bodyLen = httpClient.GetBodyLength();
	if (bodyLen > 0)
	{
		char* body = new char[bodyLen + 1];
		memcpy(body, httpClient.GetBody(), bodyLen);
		CString s = body;
		delete[] body;

		int pos = s.Find("<ImageUrl>");
		int pos2 = s.Find("</ImageUrl>");
		s = s.Mid(pos + 10, pos2 - pos - 10);
		s.Replace("&amp;", "&");

		if (s.GetLength() == 0) 
			return false;

		UrlFormat = s;
		return true;
	}
	return result;
}

// ******************************************************
//    TileXYToQuadKey()
// ******************************************************
// Converts tile XY coordinates into a QuadKey at a specified level of detail.
// LevelOfDetail: Level of detail, from 1 (lowest detail) to 23 (highest detail).
CString BingBaseProvider::TileXYToQuadKey(int tileX, int tileY, int levelOfDetail)
{
	CString s;
	for (int i = levelOfDetail; i > 0; i--)
	{
		char digit = '0';
		int mask = 1 << (i - 1);
		if ((tileX & mask) != 0)
		{
			digit++;
		}
		if ((tileY & mask) != 0)
		{
			digit++;
			digit++;
		}

		s.AppendChar(digit);
	}
	return s;
}