//********************************************************************************************************
//File name: Utils_GDAL.cpp
//Description: Implementation of CUtils.
//********************************************************************************************************
#include "stdafx.h"
#include "Utils.h"
#include "atlsafe.h"
#include <stack>
#include <comdef.h>
#include "gdal.h"
#include "gdal_alg.h"
#include "gdalwarper.h"
#include "gdal_proxy.h"
#include "cpl_conv.h"
#include "cpl_multiproc.h"
#include "ogr_api.h"
#include "ogr_srs_api.h"
#include "cpl_vsi.h"
#include "cpl_string.h"
#include "vrtdataset.h"
#include "direct.h"

#pragma warning(disable:4996)

#pragma region common

/* -------------------------------------------------------------------- */
/*                      CheckExtensionConsistency()                     */
/*                                                                      */
/*      Check that the target file extension is consistent with the     */
/*      requested driver. Actually, we only warn in cases where the     */
/*      inconsistency is blatant (use of an extension declared by one   */
/*      or several drivers, and not by the selected one)                */
/* -------------------------------------------------------------------- */

void CheckExtensionConsistency(const char* pszDestFilename,
                               const char* pszDriverName)
{

    char* pszDestExtension = CPLStrdup(CPLGetExtension(pszDestFilename));
    if (pszDestExtension[0] != '\0')
    {
        int nDriverCount = GDALGetDriverCount();
        CPLString osConflictingDriverList;
        for(int i=0;i<nDriverCount;i++)
        {
            GDALDriverH hDriver = GDALGetDriver(i);
            const char* pszDriverExtension = 
                GDALGetMetadataItem( hDriver, GDAL_DMD_EXTENSION, NULL );   
            if (pszDriverExtension && EQUAL(pszDestExtension, pszDriverExtension))
            {
                if (GDALGetDriverByName(pszDriverName) != hDriver)
                {
                    if (osConflictingDriverList.size())
                        osConflictingDriverList += ", ";
                    osConflictingDriverList += GDALGetDriverShortName(hDriver);
                }
                else
                {
                    /* If the request driver allows the used extension, then */
                    /* just stop iterating now */
                    osConflictingDriverList = "";
                    break;
                }
            }
        }
        if (osConflictingDriverList.size())
        {
            CPLError( CE_Warning, CPLE_AppDefined,
                    "Warning: The target file has a '%s' extension, which is normally used by the %s driver%s,\n"
                    "but the requested output driver is %s. Is it really what you want ?\n",
                    pszDestExtension,
                    osConflictingDriverList.c_str(),
                    strchr(osConflictingDriverList.c_str(), ',') ? "s" : "",
                    pszDriverName);
        }
    }

    CPLFree(pszDestExtension);
}

BOOL CUtils::ProcessGeneralOptions(int * opts)
{
    int  iArg;

/* -------------------------------------------------------------------- */
/*      Clear any existing options.                                     */
/* -------------------------------------------------------------------- */
	ResetConfigOptions();

/* ==================================================================== */
/*      Loop over all arguments.                                        */
/* ==================================================================== */
    for( iArg = 1; iArg < _sArr.GetCount(); iArg++ )
    {
/* -------------------------------------------------------------------- */
/*      --config                                                        */
/* -------------------------------------------------------------------- */
        if( EQUAL(_sArr[iArg],"--config") )
        {
            if( iArg + 2 >= _sArr.GetCount() )
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "--config option given without a key and value argument." );
                return FALSE;
            }

            CPLSetConfigOption( _sArr[iArg+1], _sArr[iArg+2] );

			_sConfig.Add( _sArr[iArg+1] );
			_sArr.RemoveAt( iArg, 3 );
            iArg--;
        }

/* -------------------------------------------------------------------- */
/*      --debug                                                         */
/* -------------------------------------------------------------------- */
        else if( EQUAL(_sArr[iArg],"--debug") )
        {
            if( iArg + 1 >= _sArr.GetCount() )
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "--debug option given without debug level." );
                return FALSE;
            }

            CPLSetConfigOption( "CPL_DEBUG", _sArr[iArg+1] );

			_sConfig.Add( "CPL_DEBUG" );
			_sArr.RemoveAt( iArg, 2 );
            iArg--;
        }

/* -------------------------------------------------------------------- */
/*      --optfile                                                       */
/* -------------------------------------------------------------------- */
        else if( EQUAL(_sArr[iArg],"--optfile") )
        {
            const char *pszLine;
            FILE *fpOptFile;

            if( iArg + 1 >= _sArr.GetCount() )
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "--optfile option given without filename." );
                return FALSE;
            }

			CString sFileName = _sArr[iArg+1];
			_sArr.RemoveAt( iArg, 2 );
            iArg--;

            fpOptFile = VSIFOpen( sFileName, "rb" );

            if( fpOptFile == NULL )
            {
                CPLError( CE_Failure, CPLE_AppDefined, 
                          "Unable to open optfile '%s'.\n%s",
                          sFileName, VSIStrerror( errno ) );
                return FALSE;
            }
            
            while( (pszLine = CPLReadLine( fpOptFile )) != NULL )
            {
                char **papszTokens;
                int i;

                if( pszLine[0] == '#' || strlen(pszLine) == 0 )
                    continue;

                papszTokens = CSLTokenizeString( pszLine );
                for( i = 0; papszTokens != NULL && papszTokens[i] != NULL; i++)
                    _sArr.Add( papszTokens[i] );
                CSLDestroy( papszTokens );
            }

            VSIFClose( fpOptFile );
        }
	}

	*opts = _sArr.GetCount();

	return TRUE;
}

HRESULT CUtils::ResetConfigOptions(long ErrorCode)
{
	for( int i = 0; i < _sConfig.GetCount(); i++ )
		CPLSetConfigOption( _sConfig[i].GetBuffer(0), NULL);

	_sConfig.RemoveAll();

	if (ErrorCode > 0)
		this->_lastErrorCode = ErrorCode;

	return 0L;
}

#pragma endregion

#pragma region gdalinfo

static CString 
GDALInfoReportCorner( GDALDatasetH hDataset, 
                      OGRCoordinateTransformationH hTransform,
                      const char * corner_name,
                      double x, double y );

/************************************************************************/
/*                                GDALInfo()                            */
/************************************************************************/

STDMETHODIMP CUtils::GDALInfo(BSTR bstrSrcFilename, BSTR bstrOptions,
							  ICallback * cBack, BSTR *bstrInfo)
{
	USES_CONVERSION;

	int				argc = 0;
	CString			sOutput = "";
	GDALDatasetH	hDataset;
	GDALRasterBandH	hBand;
	int				i, iBand;
	double			adfGeoTransform[6];
	GDALDriverH		hDriver;
	char			**papszMetadata;
	int				bComputeMinMax = FALSE, bSample = FALSE;
	int				bShowGCPs = TRUE, bShowMetadata = TRUE, bShowRAT = TRUE;
	int				bStats = FALSE, bApproxStats = TRUE, iMDD;
	int				bShowColorTable = TRUE, bComputeChecksum = FALSE;
	int				bReportHistograms = FALSE;
	int				bReportProj4 = FALSE;
	int				nSubdataset = -1;
	const char		*pszFilename = NULL;
	char			**papszExtraMDDomains = NULL, **papszFileList;
	const char		*pszProjection = NULL;
	OGRCoordinateTransformationH hTransform = NULL;
	int				bShowFileList = TRUE;

	pszFilename = OLE2CA(bstrSrcFilename);

	GDALAllRegister();

	Parse(OLE2CA(bstrOptions), &argc);

	if (!ProcessGeneralOptions(&argc))
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*      Parse arguments.                                                */
/* -------------------------------------------------------------------- */
	for( i = 1; i < argc; i++ )
	{
		if( EQUAL(_sArr[i], "-mm") )
            bComputeMinMax = TRUE;
        else if( EQUAL(_sArr[i], "-hist") )
            bReportHistograms = TRUE;
        else if( EQUAL(_sArr[i], "-proj4") )
            bReportProj4 = TRUE;
        else if( EQUAL(_sArr[i], "-stats") )
        {
            bStats = TRUE;
            bApproxStats = FALSE;
        }
        else if( EQUAL(_sArr[i], "-approx_stats") )
        {
            bStats = TRUE;
            bApproxStats = TRUE;
        }
        else if( EQUAL(_sArr[i], "-sample") )
            bSample = TRUE;
        else if( EQUAL(_sArr[i], "-checksum") )
            bComputeChecksum = TRUE;
        else if( EQUAL(_sArr[i], "-nogcp") )
            bShowGCPs = FALSE;
        else if( EQUAL(_sArr[i], "-nomd") )
            bShowMetadata = FALSE;
        else if( EQUAL(_sArr[i], "-norat") )
            bShowRAT = FALSE;
        else if( EQUAL(_sArr[i], "-noct") )
            bShowColorTable = FALSE;
        else if( EQUAL(_sArr[i], "-mdd") && i < argc-1 )
            papszExtraMDDomains = CSLAddString( papszExtraMDDomains,
                                                _sArr[++i].GetBuffer(0) );
        else if( EQUAL(_sArr[i], "-nofl") )
            bShowFileList = FALSE;
        else if( EQUAL(_sArr[i], "-sd") && i < argc-1 )
            nSubdataset = atoi(_sArr[++i].GetBuffer (0));
		else if( EQUAL(_sArr[i], "--version") )
		{
			sOutput += GDALVersionInfo( "--version" );

			*bstrInfo = sOutput.AllocSysString();
			return ResetConfigOptions();
		}
		else if( EQUAL(_sArr[i], "--formats") )
		{
			sOutput += "Supported Formats:\n";
			for( int iDr = 0; iDr < GDALGetDriverCount(); iDr++ )
			{
				GDALDriverH hDriver = GDALGetDriver(iDr);
				const char *pszRWFlag;

				if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) )
					pszRWFlag = "rw+";
				else if( GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATECOPY, 
					NULL ) )
					pszRWFlag = "rw";
				else
					pszRWFlag = "ro";

				sOutput.AppendFormat( "  %s (%s): %s\n",
					GDALGetDriverShortName( hDriver ),
					pszRWFlag,
					GDALGetDriverLongName( hDriver ) );
			}

			*bstrInfo = sOutput.AllocSysString();
			return ResetConfigOptions();
		}
		else if( EQUAL(_sArr[i], "--format") )
		{
			GDALDriverH hDriver;
			char **papszMD;

			if( i + 1 >= argc )
			{
				CPLError( CE_Failure, CPLE_AppDefined, 
					"--format option given without a format code." );
				return ResetConfigOptions(tkGDAL_ERROR);
			}

			hDriver = GDALGetDriverByName( _sArr[i+1].GetBuffer(0) );
			if( hDriver == NULL )
			{
				CPLError( CE_Failure, CPLE_AppDefined, 
					"--format option given with format '%s', but that format not\n"
					"recognised.  Use the --formats option to get a list of available formats,\n"
					"and use the short code (ie. GTiff or HFA) as the format identifier.\n", 
					_sArr[i+1].GetBuffer(0) );
				return ResetConfigOptions(tkGDAL_ERROR);
			}

			sOutput += "Format Details:\n";
			sOutput.AppendFormat( "  Short Name: %s\n", GDALGetDriverShortName( hDriver ) );
			sOutput.AppendFormat( "  Long Name: %s\n", GDALGetDriverLongName( hDriver ) );

			papszMD = GDALGetMetadata( hDriver, NULL );

			if( CSLFetchNameValue( papszMD, GDAL_DMD_EXTENSION ) )
			{
				sOutput.AppendFormat( "  Extension: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_EXTENSION ) );
			}
			if( CSLFetchNameValue( papszMD, GDAL_DMD_MIMETYPE ) )
			{
				sOutput.AppendFormat( "  Mime Type: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_MIMETYPE ) );
			}
			if( CSLFetchNameValue( papszMD, GDAL_DMD_HELPTOPIC ) )
			{
				sOutput.AppendFormat( "  Help Topic: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_HELPTOPIC ) );
			}

			if( CSLFetchNameValue( papszMD, GDAL_DCAP_CREATE ) )
				sOutput += "  Supports: Create() - Create writeable dataset.\n";
			if( CSLFetchNameValue( papszMD, GDAL_DCAP_CREATECOPY ) )
				sOutput += "  Supports: CreateCopy() - Create dataset by copying another.\n";
			if( CSLFetchNameValue( papszMD, GDAL_DMD_CREATIONDATATYPES ) )
			{
				sOutput.AppendFormat( "  Creation Datatypes: %s\n",
					CSLFetchNameValue( papszMD, GDAL_DMD_CREATIONDATATYPES ) );
			}
			if( CSLFetchNameValue( papszMD, GDAL_DMD_CREATIONOPTIONLIST ) )
			{
				CPLXMLNode *psCOL = 
					CPLParseXMLString( 
					CSLFetchNameValue( papszMD, 
					GDAL_DMD_CREATIONOPTIONLIST ) );
				char *pszFormattedXML = 
					CPLSerializeXMLTree( psCOL );

				CPLDestroyXMLNode( psCOL );

				sOutput.AppendFormat( "\n%s\n", pszFormattedXML );
				CPLFree( pszFormattedXML );
			}

			*bstrInfo = sOutput.AllocSysString();
			return ResetConfigOptions();
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Open dataset.                                                   */
	/* -------------------------------------------------------------------- */
	hDataset = GdalHelper::OpenRasterDatasetW(OLE2W(bstrSrcFilename), GA_ReadOnly);
    if( hDataset == NULL )
    {
        CPLError(CE_Failure,0,
                 "gdalinfo failed - unable to open '%s'.\n",
                 pszFilename );

        CSLDestroy( papszExtraMDDomains );
    
        GDALDumpOpenDatasets( stderr );

        CPLDumpSharedList( NULL );

        return ResetConfigOptions(tkGDAL_ERROR);
    }

	/* -------------------------------------------------------------------- */
	/*      Read specified subdataset if requested.                         */
	/* -------------------------------------------------------------------- */
    if ( nSubdataset > 0 )
    {
        char **papszSubdatasets = GDALGetMetadata( hDataset, "SUBDATASETS" );
        int nSubdatasets = CSLCount( papszSubdatasets );

        if ( nSubdatasets > 0 && nSubdataset <= nSubdatasets )
        {
            char szKeyName[1024];
            char *pszSubdatasetName;

            snprintf( szKeyName, sizeof(szKeyName),
                      "SUBDATASET_%d_NAME", nSubdataset );
            szKeyName[sizeof(szKeyName) - 1] = '\0';
            pszSubdatasetName =
                CPLStrdup( CSLFetchNameValue( papszSubdatasets, szKeyName ) );
            GDALClose( hDataset );
			hDataset = GDALOpen( pszSubdatasetName, GA_ReadOnly );		// TODO: use Unicode
            CPLFree( pszSubdatasetName );
        }
        else
        {
            CPLError(CE_Failure,0,
                     "gdalinfo warning: subdataset %d of %d requested. "
                     "Reading the main dataset.\n",
                     nSubdataset, nSubdatasets );
			return ResetConfigOptions(tkGDAL_ERROR);
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report general info.                                            */
	/* -------------------------------------------------------------------- */
    hDriver = GDALGetDatasetDriver( hDataset );
    sOutput.AppendFormat( "Driver: %s/%s\n",
            GDALGetDriverShortName( hDriver ),
            GDALGetDriverLongName( hDriver ) );

    papszFileList = GDALGetFileList( hDataset );
    if( CSLCount(papszFileList) == 0 )
    {
        sOutput += "Files: none associated\n";
    }
    else
    {
        sOutput.AppendFormat( "Files: %s\n", papszFileList[0] );
        if( bShowFileList )
        {
            for( i = 1; papszFileList[i] != NULL; i++ )
                sOutput.AppendFormat( "       %s\n", papszFileList[i] );
        }
    }
    CSLDestroy( papszFileList );

    sOutput.AppendFormat( "Size is %d, %d\n",
            GDALGetRasterXSize( hDataset ), 
            GDALGetRasterYSize( hDataset ) );

	/* -------------------------------------------------------------------- */
	/*      Report projection.                                              */
	/* -------------------------------------------------------------------- */
    if( GDALGetProjectionRef( hDataset ) != NULL )
    {
        OGRSpatialReferenceH  hSRS;
        char		      *pszProjection;

        pszProjection = (char *) GDALGetProjectionRef( hDataset );

        hSRS = OSRNewSpatialReference(NULL);
        if( OSRImportFromWkt( hSRS, &pszProjection ) == CE_None )
        {
            char	*pszPrettyWkt = NULL;

            OSRExportToPrettyWkt( hSRS, &pszPrettyWkt, FALSE );
            sOutput.AppendFormat( "Coordinate System is:\n%s\n", pszPrettyWkt );
            CPLFree( pszPrettyWkt );
        }
        else
            sOutput.AppendFormat( "Coordinate System is `%s'\n",
                    GDALGetProjectionRef( hDataset ) );

        if ( bReportProj4 ) 
        {
            char *pszProj4 = NULL;
            OSRExportToProj4( hSRS, &pszProj4 );
            sOutput.AppendFormat("PROJ.4 string is:\n\'%s\'\n",pszProj4);
            CPLFree( pszProj4 ); 
        }

        OSRDestroySpatialReference( hSRS );
    }

	/* -------------------------------------------------------------------- */
	/*      Report Geotransform.                                            */
	/* -------------------------------------------------------------------- */
    if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None )
    {
        if( adfGeoTransform[2] == 0.0 && adfGeoTransform[4] == 0.0 )
        {
            sOutput.AppendFormat( "Origin = (%.15f,%.15f)\n",
                    adfGeoTransform[0], adfGeoTransform[3] );

            sOutput.AppendFormat( "Pixel Size = (%.15f,%.15f)\n",
                    adfGeoTransform[1], adfGeoTransform[5] );
        }
        else
            sOutput.AppendFormat( "GeoTransform =\n"
                    "  %.16g, %.16g, %.16g\n"
                    "  %.16g, %.16g, %.16g\n", 
                    adfGeoTransform[0],
                    adfGeoTransform[1],
                    adfGeoTransform[2],
                    adfGeoTransform[3],
                    adfGeoTransform[4],
                    adfGeoTransform[5] );
    }

	/* -------------------------------------------------------------------- */
	/*      Report GCPs.                                                    */
	/* -------------------------------------------------------------------- */
    if( bShowGCPs && GDALGetGCPCount( hDataset ) > 0 )
    {
        if (GDALGetGCPProjection(hDataset) != NULL)
        {
            OGRSpatialReferenceH  hSRS;
            char		      *pszProjection;

            pszProjection = (char *) GDALGetGCPProjection( hDataset );

            hSRS = OSRNewSpatialReference(NULL);
            if( OSRImportFromWkt( hSRS, &pszProjection ) == CE_None )
            {
                char	*pszPrettyWkt = NULL;

                OSRExportToPrettyWkt( hSRS, &pszPrettyWkt, FALSE );
                sOutput.AppendFormat( "GCP Projection = \n%s\n", pszPrettyWkt );
                CPLFree( pszPrettyWkt );
            }
            else
                sOutput.AppendFormat( "GCP Projection = %s\n",
                        GDALGetGCPProjection( hDataset ) );

            OSRDestroySpatialReference( hSRS );
        }

        for( i = 0; i < GDALGetGCPCount(hDataset); i++ )
        {
            const GDAL_GCP	*psGCP;
            
            psGCP = GDALGetGCPs( hDataset ) + i;

            sOutput.AppendFormat( "GCP[%3d]: Id=%s, Info=%s\n"
                    "          (%.15g,%.15g) -> (%.15g,%.15g,%.15g)\n", 
                    i, psGCP->pszId, psGCP->pszInfo, 
                    psGCP->dfGCPPixel, psGCP->dfGCPLine, 
                    psGCP->dfGCPX, psGCP->dfGCPY, psGCP->dfGCPZ );
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report metadata.                                                */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, NULL ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Metadata:\n";
        for( i = 0; papszMetadata[i] != NULL; i++ )
        {
            sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
        }
    }

    for( iMDD = 0; bShowMetadata && iMDD < CSLCount(papszExtraMDDomains); iMDD++ )
    {
        papszMetadata = GDALGetMetadata( hDataset, papszExtraMDDomains[iMDD] );
        if( CSLCount(papszMetadata) > 0 )
        {
            sOutput.AppendFormat( "Metadata (%s):\n", papszExtraMDDomains[iMDD]);
            for( i = 0; papszMetadata[i] != NULL; i++ )
            {
                if (EQUALN(papszExtraMDDomains[iMDD], "xml:", 4))
                    sOutput.AppendFormat( "%s\n", papszMetadata[i] );
                else
                    sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
            }
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report "IMAGE_STRUCTURE" metadata.                              */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, "IMAGE_STRUCTURE" ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Image Structure Metadata:\n";
        for( i = 0; papszMetadata[i] != NULL; i++ )
        {
            sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report subdatasets.                                             */
	/* -------------------------------------------------------------------- */
    papszMetadata = GDALGetMetadata( hDataset, "SUBDATASETS" );
    if( CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Subdatasets:\n";
        for( i = 0; papszMetadata[i] != NULL; i++ )
        {
            sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report geolocation.                                             */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, "GEOLOCATION" ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Geolocation:\n";
        for( i = 0; papszMetadata[i] != NULL; i++ )
        {
            sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report RPCs                                                     */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, "RPC" ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "RPC Metadata:\n";
        for( i = 0; papszMetadata[i] != NULL; i++ )
        {
            sOutput.AppendFormat( "  %s\n", papszMetadata[i] );
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Setup projected to lat/long transform if appropriate.           */
	/* -------------------------------------------------------------------- */
    if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None )
        pszProjection = GDALGetProjectionRef(hDataset);

    if( pszProjection != NULL && strlen(pszProjection) > 0 )
    {
        OGRSpatialReferenceH hProj, hLatLong = NULL;

        hProj = OSRNewSpatialReference( pszProjection );
        if( hProj != NULL )
            hLatLong = OSRCloneGeogCS( hProj );

        if( hLatLong != NULL )
        {
            CPLPushErrorHandler( CPLQuietErrorHandler );
            hTransform = OCTNewCoordinateTransformation( hProj, hLatLong );
            CPLPopErrorHandler();
            
            OSRDestroySpatialReference( hLatLong );
        }

        if( hProj != NULL )
            OSRDestroySpatialReference( hProj );
    }

	/* -------------------------------------------------------------------- */
	/*      Report corners.                                                 */
	/* -------------------------------------------------------------------- */
    sOutput += "Corner Coordinates:\n";
    sOutput += GDALInfoReportCorner( hDataset, hTransform, "Upper Left", 
                          0.0, 0.0 );
    sOutput += GDALInfoReportCorner( hDataset, hTransform, "Lower Left", 
                          0.0, GDALGetRasterYSize(hDataset));
    sOutput += GDALInfoReportCorner( hDataset, hTransform, "Upper Right", 
                          GDALGetRasterXSize(hDataset), 0.0 );
    sOutput += GDALInfoReportCorner( hDataset, hTransform, "Lower Right", 
                          GDALGetRasterXSize(hDataset), 
                          GDALGetRasterYSize(hDataset) );
    sOutput += GDALInfoReportCorner( hDataset, hTransform, "Center", 
                          GDALGetRasterXSize(hDataset)/2.0, 
                          GDALGetRasterYSize(hDataset)/2.0 );

    if( hTransform != NULL )
    {
        OCTDestroyCoordinateTransformation( hTransform );
        hTransform = NULL;
    }

	/* ==================================================================== */
	/*      Loop over bands.                                                */
	/* ==================================================================== */
    for( iBand = 0; iBand < GDALGetRasterCount( hDataset ); iBand++ )
    {
        double      dfMin, dfMax, adfCMinMax[2], dfNoData;
        int         bGotMin, bGotMax, bGotNodata, bSuccess;
        int         nBlockXSize, nBlockYSize, nMaskFlags;
        double      dfMean, dfStdDev;
        GDALColorTableH	hTable;
        CPLErr      eErr;

        hBand = GDALGetRasterBand( hDataset, iBand+1 );

        if( bSample )
        {
            float afSample[10000];
            int   nCount;

            nCount = GDALGetRandomRasterSample( hBand, 10000, afSample );
            sOutput.AppendFormat( "Got %d samples.\n", nCount );
        }
        
        GDALGetBlockSize( hBand, &nBlockXSize, &nBlockYSize );
        sOutput.AppendFormat( "Band %d Block=%dx%d Type=%s, ColorInterp=%s\n", iBand+1,
                nBlockXSize, nBlockYSize,
                GDALGetDataTypeName(
                    GDALGetRasterDataType(hBand)),
                GDALGetColorInterpretationName(
                    GDALGetRasterColorInterpretation(hBand)) );

        if( GDALGetDescription( hBand ) != NULL 
            && strlen(GDALGetDescription( hBand )) > 0 )
            sOutput.AppendFormat( "  Description = %s\n", GDALGetDescription(hBand) );

        dfMin = GDALGetRasterMinimum( hBand, &bGotMin );
        dfMax = GDALGetRasterMaximum( hBand, &bGotMax );
        if( bGotMin || bGotMax || bComputeMinMax )
        {
            sOutput += "  " ;
            if( bGotMin )
                sOutput.AppendFormat( "Min=%.3f ", dfMin );
            if( bGotMax )
                sOutput.AppendFormat( "Max=%.3f ", dfMax );
        
            if( bComputeMinMax )
            {
                CPLErrorReset();
                GDALComputeRasterMinMax( hBand, FALSE, adfCMinMax );
                if (CPLGetLastErrorType() == CE_None)
                {
                  sOutput.AppendFormat( "  Computed Min/Max=%.3f,%.3f", 
                          adfCMinMax[0], adfCMinMax[1] );
                }
            }

            sOutput += "\n" ;
        }

        eErr = GDALGetRasterStatistics( hBand, bApproxStats, bStats, 
                                        &dfMin, &dfMax, &dfMean, &dfStdDev );
        if( eErr == CE_None )
        {
            sOutput.AppendFormat( "  Minimum=%.3f, Maximum=%.3f, Mean=%.3f, StdDev=%.3f\n",
                    dfMin, dfMax, dfMean, dfStdDev );
        }

        if( bReportHistograms )
        {
            int nBucketCount, *panHistogram = NULL;
			struct CallbackParams params( GetCallback(), "Analyzing" );

            eErr = GDALGetDefaultHistogram( hBand, &dfMin, &dfMax, 
                                            &nBucketCount, &panHistogram, 
                                            TRUE, (GDALProgressFunc) GDALProgressCallback, &params );
            if( eErr == CE_None )
            {
                int iBucket;

                sOutput.AppendFormat( "  %d buckets from %g to %g:\n  ",
                        nBucketCount, dfMin, dfMax );
                for( iBucket = 0; iBucket < nBucketCount; iBucket++ )
                    sOutput.AppendFormat( "%d ", panHistogram[iBucket] );
                sOutput += "\n" ;
                CPLFree( panHistogram );
            }
        }

        if ( bComputeChecksum)
        {
            sOutput.AppendFormat( "  Checksum=%d\n",
                    GDALChecksumImage(hBand, 0, 0,
                                      GDALGetRasterXSize(hDataset),
                                      GDALGetRasterYSize(hDataset)));
        }

        dfNoData = GDALGetRasterNoDataValue( hBand, &bGotNodata );
        if( bGotNodata )
        {
            if (CPLIsNan(dfNoData))
                sOutput.AppendFormat( "  NoData Value=nan\n" );
            else
                sOutput.AppendFormat( "  NoData Value=%.18g\n", dfNoData );
        }

        if( GDALGetOverviewCount(hBand) > 0 )
        {
            int		iOverview;

            sOutput += "  Overviews: ";
            for( iOverview = 0; 
                 iOverview < GDALGetOverviewCount(hBand);
                 iOverview++ )
            {
                GDALRasterBandH	hOverview;
                const char *pszResampling = NULL;

                if( iOverview != 0 )
                    sOutput += ", " ;

                hOverview = GDALGetOverview( hBand, iOverview );
                if (hOverview != NULL)
                {
                    sOutput.AppendFormat( "%dx%d", 
                            GDALGetRasterBandXSize( hOverview ),
                            GDALGetRasterBandYSize( hOverview ) );

                    pszResampling = 
                        GDALGetMetadataItem( hOverview, "RESAMPLING", "" );

                    if( pszResampling != NULL 
                        && EQUALN(pszResampling,"AVERAGE_BIT2",12) )
                        sOutput += "*" ;
                }
                else
                    sOutput += "(null)";
            }
            sOutput += "\n" ;

            if ( bComputeChecksum)
            {
                sOutput += "  Overviews checksum: ";
                for( iOverview = 0; 
                    iOverview < GDALGetOverviewCount(hBand);
                    iOverview++ )
                {
                    GDALRasterBandH	hOverview;

                    if( iOverview != 0 )
                        sOutput += ", ";

                    hOverview = GDALGetOverview( hBand, iOverview );
                    if (hOverview)
                        sOutput.AppendFormat( "%d",
                                GDALChecksumImage(hOverview, 0, 0,
                                        GDALGetRasterBandXSize(hOverview),
                                        GDALGetRasterBandYSize(hOverview)));
                    else
                        sOutput += "(null)";
                }
                sOutput += "\n";
            }
        }

        if( GDALHasArbitraryOverviews( hBand ) )
        {
            sOutput += "  Overviews: arbitrary\n";
        }
        
        nMaskFlags = GDALGetMaskFlags( hBand );
        if( (nMaskFlags & (GMF_NODATA|GMF_ALL_VALID)) == 0 )
        {
            GDALRasterBandH hMaskBand = GDALGetMaskBand(hBand) ;

            sOutput += "  Mask Flags: ";
            if( nMaskFlags & GMF_PER_DATASET )
                sOutput += "PER_DATASET ";
            if( nMaskFlags & GMF_ALPHA )
                sOutput += "ALPHA ";
            if( nMaskFlags & GMF_NODATA )
                sOutput += "NODATA ";
            if( nMaskFlags & GMF_ALL_VALID )
                sOutput += "ALL_VALID ";
            sOutput += "\n";

            if( hMaskBand != NULL &&
                GDALGetOverviewCount(hMaskBand) > 0 )
            {
                int		iOverview;

                sOutput += "  Overviews of mask band: ";
                for( iOverview = 0; 
                     iOverview < GDALGetOverviewCount(hMaskBand);
                     iOverview++ )
                {
                    GDALRasterBandH	hOverview;

                    if( iOverview != 0 )
                        sOutput += ", ";

                    hOverview = GDALGetOverview( hMaskBand, iOverview );
                    sOutput.AppendFormat( "%dx%d", 
                            GDALGetRasterBandXSize( hOverview ),
                            GDALGetRasterBandYSize( hOverview ) );
                }
                sOutput += "\n";
            }
        }

        if( strlen(GDALGetRasterUnitType(hBand)) > 0 )
        {
            sOutput.AppendFormat( "  Unit Type: %s\n", GDALGetRasterUnitType(hBand) );
        }

        if( GDALGetRasterCategoryNames(hBand) != NULL )
        {
            char **papszCategories = GDALGetRasterCategoryNames(hBand);
            int i;

            sOutput += "  Categories:\n";
            for( i = 0; papszCategories[i] != NULL; i++ )
                sOutput.AppendFormat( "    %3d: %s\n", i, papszCategories[i] );
        }

        if( GDALGetRasterScale( hBand, &bSuccess ) != 1.0 
            || GDALGetRasterOffset( hBand, &bSuccess ) != 0.0 )
            sOutput.AppendFormat( "  Offset: %.15g,   Scale:%.15g\n",
                    GDALGetRasterOffset( hBand, &bSuccess ),
                    GDALGetRasterScale( hBand, &bSuccess ) );

        papszMetadata = (bShowMetadata) ? GDALGetMetadata( hBand, NULL ) : NULL;
        if( bShowMetadata && CSLCount(papszMetadata) > 0 )
        {
            sOutput += "  Metadata:\n";
            for( i = 0; papszMetadata[i] != NULL; i++ )
            {
                sOutput.AppendFormat( "    %s\n", papszMetadata[i] );
            }
        }

        papszMetadata = (bShowMetadata) ? GDALGetMetadata( hBand, "IMAGE_STRUCTURE" ) : NULL;
        if( bShowMetadata && CSLCount(papszMetadata) > 0 )
        {
            sOutput += "  Image Structure Metadata:\n";
            for( i = 0; papszMetadata[i] != NULL; i++ )
            {
                sOutput.AppendFormat( "    %s\n", papszMetadata[i] );
            }
        }

        if( GDALGetRasterColorInterpretation(hBand) == GCI_PaletteIndex 
            && (hTable = GDALGetRasterColorTable( hBand )) != NULL )
        {
			int			i;

            sOutput.AppendFormat( "  Color Table (%s with %d entries)\n", 
                    GDALGetPaletteInterpretationName(
                        GDALGetPaletteInterpretation( hTable )), 
                    GDALGetColorEntryCount( hTable ) );

            if (bShowColorTable)
            {
                for( i = 0; i < GDALGetColorEntryCount( hTable ); i++ )
                {
                    GDALColorEntry	sEntry;
    
                    GDALGetColorEntryAsRGB( hTable, i, &sEntry );
                    sOutput.AppendFormat( "  %3d: %d,%d,%d,%d\n", 
                            i, 
                            sEntry.c1,
                            sEntry.c2,
                            sEntry.c3,
                            sEntry.c4 );
                }
            }
        }

        if( bShowRAT && GDALGetDefaultRAT( hBand ) != NULL )
        {
            GDALRasterAttributeTableH hRAT = GDALGetDefaultRAT( hBand );
            
            GDALRATDumpReadable( hRAT, NULL );
        }
    }

    GDALClose( hDataset );
    
    CSLDestroy( papszExtraMDDomains );
    
    GDALDumpOpenDatasets( stderr );

    CPLDumpSharedList( NULL );
    CPLCleanupTLS();

	*bstrInfo = sOutput.AllocSysString();

	return ResetConfigOptions();
}

/************************************************************************/
/*                        GDALInfoReportCorner()                        */
/************************************************************************/

static CString
GDALInfoReportCorner(GDALDatasetH hDataset,
		OGRCoordinateTransformationH hTransform,
		const char * corner_name, double x, double y)
{
	CString sOutput = "", sTemp = "";
	double	dfGeoX, dfGeoY;
    double	adfGeoTransform[6];
        
    sTemp.Format( "%-11s ", corner_name );
	sOutput += sTemp;
    
/* -------------------------------------------------------------------- */
/*      Transform the point into georeferenced coordinates.             */
/* -------------------------------------------------------------------- */
    if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None )
    {
        dfGeoX = adfGeoTransform[0] + adfGeoTransform[1] * x
            + adfGeoTransform[2] * y;
        dfGeoY = adfGeoTransform[3] + adfGeoTransform[4] * x
            + adfGeoTransform[5] * y;
    }

    else
    {
        sTemp.Format( "(%7.1f,%7.1f)\n", x, y );
		sOutput += sTemp;
        return sOutput;
    }

	/* -------------------------------------------------------------------- */
	/*      Report the georeferenced coordinates.                           */
	/* -------------------------------------------------------------------- */
    if( ABS(dfGeoX) < 181 && ABS(dfGeoY) < 91 )
    {
        sTemp.Format( "(%12.7f,%12.7f) ", dfGeoX, dfGeoY );
    }
    else
    {
        sTemp.Format( "(%12.3f,%12.3f) ", dfGeoX, dfGeoY );
    }
	sOutput += sTemp;

	/* -------------------------------------------------------------------- */
	/*      Transform to latlong and report.                                */
	/* -------------------------------------------------------------------- */
    if( hTransform != NULL 
        && OCTTransform(hTransform,1,&dfGeoX,&dfGeoY,NULL) )
    {
        
        sTemp.Format( "(%s,", GDALDecToDMS( dfGeoX, "Long", 2 ) );
		sOutput += sTemp;
        sTemp.Format( "%s)", GDALDecToDMS( dfGeoY, "Lat", 2 ) );
		sOutput += sTemp;
    }

    sOutput += "\n" ;

    return sOutput;
}

#pragma endregion

#pragma region gdal_translate

/* ****************************************************************************
 * $Id: gdal_translate.cpp 22783 2011-07-23 19:28:16Z rouault $
 *
 * Project:  GDAL Utilities
 * Purpose:  GDAL Image Translator Program
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 1998, 2002, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ****************************************************************************/

static int ArgIsNumeric( const char * );
static void AttachMetadata( GDALDatasetH, char ** );
static void CopyBandInfo( GDALRasterBand * poSrcBand, GDALRasterBand * poDstBand,
                            int bCanCopyStatsMetadata, int bCopyScale, int bCopyNoData );

/************************************************************************/
/*                             TranslateRaster()                        */
/************************************************************************/
__declspec(deprecated("This is a deprecated function, use CGdalUtils::GdalRasterTranslate instead"))
STDMETHODIMP CUtils::TranslateRaster(BSTR bstrSrcFilename, BSTR bstrDstFilename,
									 BSTR bstrOptions, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	enum
	{
		MASK_DISABLED,
		MASK_AUTO,
		MASK_USER
	};

	int				argc = 0;
	GDALDatasetH	hDataset, hOutDS;
    int			i;
	int				nRasterXSize, nRasterYSize;
	const char		*pszSource=NULL, *pszDest=NULL, *pszFormat = "GTiff";
    int bFormatExplicitelySet = FALSE;
	GDALDriverH		hDriver;
	int				*panBandList = NULL; /* negative value of panBandList[i] means mask band of ABS(panBandList[i]) */
	int				nBandCount = 0, bDefBands = TRUE;
	double			adfGeoTransform[6];
	GDALDataType	eOutputType = GDT_Unknown;
	int				nOXSize = 0, nOYSize = 0;
	char			*pszOXSize=NULL, *pszOYSize=NULL;
	char			**papszCreateOptions = NULL;
    int                 anSrcWin[4], bStrict = FALSE;
	const char		*pszProjection;
	int				bScale = FALSE, bHaveScaleSrc = FALSE, bUnscale = FALSE;
	double			dfScaleSrcMin=0.0, dfScaleSrcMax=255.0;
	double			dfScaleDstMin=0.0, dfScaleDstMax=255.0;
    double              dfULX, dfULY, dfLRX, dfLRY;
	char			**papszMetadataOptions = NULL;
	char			*pszOutputSRS = NULL;
	int				bGotBounds = FALSE;
	GDALProgressFunc    pfnProgress = (GDALProgressFunc)GDALProgressCallback;
	int				nGCPCount = 0;
	GDAL_GCP		*pasGCPs = NULL;
	int				bCopySubDatasets = FALSE;
    double              adfULLR[4] = { 0,0,0,0 };
	int				bSetNoData = FALSE;
	int				bUnsetNoData = FALSE;
	double			dfNoDataReal = 0.0;
	int				nRGBExpand = 0;
	int				bParsedMaskArgument = FALSE;
	int				eMaskMode = MASK_AUTO;
	int				nMaskBand = 0; /* negative value means mask band of ABS(nMaskBand) */
	int				bStats = FALSE, bApproxStats = FALSE;


	anSrcWin[0] = 0;
    anSrcWin[1] = 0;
    anSrcWin[2] = 0;
    anSrcWin[3] = 0;

    dfULX = dfULY = dfLRX = dfLRY = 0.0;

	(*retval) = VARIANT_FALSE;

	pszSource = OLE2CA(bstrSrcFilename);
	pszDest = OLE2CA(bstrDstFilename);

/* -------------------------------------------------------------------- */
/*      Register standard GDAL drivers, and process generic GDAL        */
/*      command options.                                                */
/* -------------------------------------------------------------------- */
	GDALAllRegister();

	if (!_bSubCall)
	{
		Parse(OLE2CA(bstrOptions), &argc);

		if (!ProcessGeneralOptions(&argc))
		{
			return ResetConfigOptions(tkGDAL_ERROR);
		}
	}
	else
		argc = _sArr.GetCount();

/* -------------------------------------------------------------------- */
/*		Handle command line arguments.									*/
/* -------------------------------------------------------------------- */
	for( i = 1; i < argc; i++ )
	{
		if( EQUAL(_sArr[i],"-of") && i < argc-1 )
		{
			pszFormat = _sArr[++i].GetBuffer (0);
			bFormatExplicitelySet = TRUE;
		}
		else if( EQUAL(_sArr[i],"-ot") && i < argc-1 )
		{
            int	iType;
            
            for( iType = 1; iType < GDT_TypeCount; iType++ )
			{
				if (GDALGetDataTypeName((GDALDataType)iType) != NULL
                    && EQUAL(GDALGetDataTypeName((GDALDataType)iType),
                             _sArr[i+1]) )
				{
					eOutputType = (GDALDataType) iType;
				}
			}

			if (eOutputType == GDT_Unknown)
			{
				CPLError(CE_Failure,0,"Unknown output pixel type: %s", _sArr[i+1]);
				return ResetConfigOptions(tkGDAL_ERROR);
			}
			i++;
		}
        else if( EQUAL(_sArr[i],"-b") && i < argc-1 )
		{
			const char* pszBand = _sArr[i+1].GetBuffer (0);
			int bMask = FALSE;
			if (EQUAL(pszBand, "mask"))
				pszBand = "mask,1";
			if (EQUALN(pszBand, "mask,", 5))
			{
				bMask = TRUE;
				pszBand += 5;
				/* If we use tha source mask band as a regular band */
				/* don't create a target mask band by default */
				if( !bParsedMaskArgument )
					eMaskMode = MASK_DISABLED;
			}
			int nBand = atoi(pszBand);
			if( nBand < 1 )
			{
				CPLError(CE_Failure,0,"Unrecognizable band number (%s).", _sArr[i+1]);
				return ResetConfigOptions(tkGDAL_ERROR);
			}
			i++;

			nBandCount++;
			panBandList = (int *)
				CPLRealloc(panBandList, sizeof(int) * nBandCount);
			panBandList[nBandCount-1] = nBand;
			if (bMask)
				panBandList[nBandCount-1] *= -1;

			if( panBandList[nBandCount-1] != nBandCount )
				bDefBands = FALSE;
		}
        else if( EQUAL(_sArr[i],"-mask") && i < argc-1 )
		{
			bParsedMaskArgument = TRUE;
			const char* pszBand = _sArr[i+1].GetBuffer (0);
			if (EQUAL(pszBand, "none"))
			{
				eMaskMode = MASK_DISABLED;
			}
			else if (EQUAL(pszBand, "auto"))
			{
				eMaskMode = MASK_AUTO;
			}
			else
			{
				int bMask = FALSE;
				if (EQUAL(pszBand, "mask"))
					pszBand = "mask,1";
				if (EQUALN(pszBand, "mask,", 5))
				{
					bMask = TRUE;
					pszBand += 5;
				}
				int nBand = atoi(pszBand);
				if( nBand < 1 )
				{
					CPLError(CE_Failure, 0, "Unrecognizable band number (%s).", _sArr[i+1]);
					return ResetConfigOptions(tkGDAL_ERROR);
				}

				eMaskMode = MASK_USER;
				nMaskBand = nBand;
				if (bMask)
					nMaskBand *= -1;
			}
			i ++;
		}
        else if( EQUAL(_sArr[i],"-not_strict")  )
			bStrict = FALSE;

        else if( EQUAL(_sArr[i],"-strict")  )
			bStrict = TRUE;

        else if( EQUAL(_sArr[i],"-sds")  )
			bCopySubDatasets = TRUE;

        else if( EQUAL(_sArr[i],"-gcp") && i < argc - 4 )
		{
			char* endptr = NULL;
			/* -gcp pixel line easting northing [elev] */

			nGCPCount++;
			pasGCPs = (GDAL_GCP *) 
				CPLRealloc( pasGCPs, sizeof(GDAL_GCP) * nGCPCount );
			GDALInitGCPs( 1, pasGCPs + nGCPCount - 1 );

			pasGCPs[nGCPCount-1].dfGCPPixel = CPLAtofM(_sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPLine = CPLAtofM(_sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPX = CPLAtofM(_sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPY = CPLAtofM(_sArr[++i].GetBuffer (0));
			if( argc > i+1 && // TODO: Is this valid?
				(CPLStrtod(_sArr[i+1].GetBuffer (0), &endptr) != 0.0 || _sArr[i+1][0] == '0') )
			{
				/* Check that last argument is really a number and not a filename */
				/* looking like a number (see ticket #863) */
				if (endptr && *endptr == 0)
					pasGCPs[nGCPCount-1].dfGCPZ = CPLAtofM(_sArr[++i].GetBuffer (0));
			}

			/* should set id and info? */
		}   

        else if( EQUAL(_sArr[i],"-a_nodata") && i < argc - 1 )
		{
            if (EQUAL(_sArr[i+1], "none"))
			{
				bUnsetNoData = TRUE;
			}
			else
			{
				bSetNoData = TRUE;
				dfNoDataReal = CPLAtofM(_sArr[i+1].GetBuffer (0));
			}
			i += 1;
		}   

        else if( EQUAL(_sArr[i],"-a_ullr") && i < argc - 4 )
		{
			adfULLR[0] = CPLAtofM(_sArr[i+1].GetBuffer (0));
			adfULLR[1] = CPLAtofM(_sArr[i+2].GetBuffer (0));
			adfULLR[2] = CPLAtofM(_sArr[i+3].GetBuffer (0));
			adfULLR[3] = CPLAtofM(_sArr[i+4].GetBuffer (0));

			bGotBounds = TRUE;

			i += 4;
		} 

        else if( EQUAL(_sArr[i],"-co") && i < argc-1 )
		{
			papszCreateOptions = CSLAddString(papszCreateOptions, _sArr[++i].GetBuffer(0));
		}

        else if( EQUAL(_sArr[i],"-scale") )
		{
			bScale = TRUE;
			if( i < argc-2 && ArgIsNumeric(_sArr[i+1].GetBuffer (0)) )
			{
				bHaveScaleSrc = TRUE;
				dfScaleSrcMin = CPLAtofM(_sArr[i+1].GetBuffer (0));
				dfScaleSrcMax = CPLAtofM(_sArr[i+2].GetBuffer (0));
				i += 2;
			}
			if( i < argc-2 && bHaveScaleSrc && ArgIsNumeric(_sArr[i+1].GetBuffer (0)) )
			{
				dfScaleDstMin = CPLAtofM(_sArr[i+1].GetBuffer (0));
				dfScaleDstMax = CPLAtofM(_sArr[i+2].GetBuffer (0));
				i += 2;
			}
			else
			{
				dfScaleDstMin = 0.0;
				dfScaleDstMax = 255.999;
			}
		}   

        else if( EQUAL(_sArr[i], "-unscale") )
		{
			bUnscale = TRUE;
		}

        else if( EQUAL(_sArr[i],"-mo") && i < argc-1 )
		{
			papszMetadataOptions = CSLAddString( papszMetadataOptions,
				_sArr[++i].GetBuffer (0) );
		}

        else if( EQUAL(_sArr[i],"-outsize") && i < argc-2 )
		{
			pszOXSize = _sArr[++i].GetBuffer (0);
			pszOYSize = _sArr[++i].GetBuffer (0);
		}   

        else if( EQUAL(_sArr[i],"-srcwin") && i < argc-4 )
		{
			anSrcWin[0] = atoi(_sArr[++i].GetBuffer (0));
			anSrcWin[1] = atoi(_sArr[++i].GetBuffer (0));
			anSrcWin[2] = atoi(_sArr[++i].GetBuffer (0));
			anSrcWin[3] = atoi(_sArr[++i].GetBuffer (0));
		}   

        else if( EQUAL(_sArr[i],"-projwin") && i < argc-4 )
		{
			dfULX = CPLAtofM(_sArr[++i].GetBuffer (0));
			dfULY = CPLAtofM(_sArr[++i].GetBuffer (0));
			dfLRX = CPLAtofM(_sArr[++i].GetBuffer (0));
			dfLRY = CPLAtofM(_sArr[++i].GetBuffer (0));
		}   

        else if( EQUAL(_sArr[i],"-a_srs") && i < argc-1 )
		{
			OGRSpatialReference oOutputSRS;

			if( oOutputSRS.SetFromUserInput( _sArr[i+1].GetBuffer (0) ) != OGRERR_NONE )
			{
				CPLError(CE_Failure, 0, "Failed to process SRS definition: %s", _sArr[i+1] );
				return ResetConfigOptions(tkGDAL_ERROR);
			}

			oOutputSRS.exportToWkt( &pszOutputSRS );
			i++;
		}   

        else if( EQUAL(_sArr[i],"-expand") && i < argc-1 )
		{
            if (EQUAL(_sArr[i+1], "gray"))
				nRGBExpand = 1;
            else if (EQUAL(_sArr[i+1], "rgb"))
				nRGBExpand = 3;
            else if (EQUAL(_sArr[i+1], "rgba"))
				nRGBExpand = 4;
			else
			{
				CPLError(CE_Failure, 0, "Value %s unsupported. Only gray, rgb or rgba are supported.", _sArr[i+1] );
				return ResetConfigOptions(tkGDAL_ERROR);
			}
			i++;
		}

        else if( EQUAL(_sArr[i], "-stats") )
		{
			bStats = TRUE;
			bApproxStats = FALSE;
		}
        else if( EQUAL(_sArr[i], "-approx_stats") )
		{
			bStats = TRUE;
			bApproxStats = TRUE;
		}

		else if( _sArr[i][0] == '-' )
		{
			CPLError(CE_Failure, 0, "Option %s incomplete, or not recognised.", _sArr[i] );
			return ResetConfigOptions(tkGDAL_ERROR);
		}
	}

	if (pszDest == NULL)
	{
		CPLError(CE_Failure, 0, "No destination dataset specified.");
		return ResetConfigOptions(tkGDAL_ERROR);
	}

	if (strcmp (pszSource, pszDest) == 0)
	{
		CPLError(CE_Failure, 0, "Source and destination datasets must be different.");
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*		Attempt to open source file.									*/
/* -------------------------------------------------------------------- */

	hDataset = GDALOpenShared(pszSource, GA_ReadOnly);

	if (hDataset == NULL)
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*		Handle subdatasets.												*/
/* -------------------------------------------------------------------- */
	if (!bCopySubDatasets
		&& CSLCount(GDALGetMetadata(hDataset, "SUBDATASETS")) > 0
		&& GDALGetRasterCount(hDataset) == 0)
	{
		CPLError(CE_Failure, 0,
			"Input file contains subdatasets. Please, select one of them for reading." );
		GDALClose(hDataset);
		return ResetConfigOptions(tkGDAL_ERROR);
	}

	if (CSLCount(GDALGetMetadata(hDataset, "SUBDATASETS")) > 0
		&& bCopySubDatasets)
	{
		char **papszSubdatasets = GDALGetMetadata(hDataset, "SUBDATASETS");
		char *pszSubDest = (char *) CPLMalloc (strlen (pszDest)+32);
        int i;
		int bOldSubCall = _bSubCall;

		(*retval) = VARIANT_TRUE;

		_bSubCall = TRUE;
		for ( i = 0; papszSubdatasets[i] != NULL; i += 2 )
		{
			sprintf (pszSubDest, "%s%d", pszDest, i/2 + 1);

			BSTR bstrSubDest = SysAllocString(CT2W(pszSubDest));
			BSTR bstrSubSrc = SysAllocString(CT2W(strstr(papszSubdatasets[i],"=")+1));

			VARIANT_BOOL subret = VARIANT_FALSE;
			this->TranslateRaster(bstrSubSrc,bstrSubDest,NULL,cBack,&subret);

			SysFreeString(bstrSubDest);
			SysFreeString(bstrSubSrc);

			if (subret == VARIANT_FALSE)
			{
				(*retval) = VARIANT_FALSE;
				break;
			}
		}

		_bSubCall = bOldSubCall;

		GDALClose(hDataset);

		if (!_bSubCall)
		{
			GDALDumpOpenDatasets(stderr); // TODO: what does this do?
		}
		return ResetConfigOptions();
	}

	/* -------------------------------------------------------------------- */
	/*		Collect some information from the source file.					*/
	/* -------------------------------------------------------------------- */
	nRasterXSize = GDALGetRasterXSize(hDataset);
	nRasterYSize = GDALGetRasterYSize(hDataset);

	if (anSrcWin[2] == 0 && anSrcWin[3] == 0)
	{
		anSrcWin[2] = nRasterXSize;
		anSrcWin[3] = nRasterYSize;
	}

/* -------------------------------------------------------------------- */
/*	Build band list to translate					*/
/* -------------------------------------------------------------------- */
	if (nBandCount == 0)
	{
		nBandCount = GDALGetRasterCount (hDataset);
		if (nBandCount == 0)
		{
			CPLError(CE_Failure, 0, "Input file has no bands, and so cannot be translated." );
			return ResetConfigOptions(tkGDAL_ERROR);
		}

		panBandList = (int *) CPLMalloc (sizeof(int)*nBandCount);
        for( i = 0; i < nBandCount; i++ )
			panBandList[i] = i+1;
	}
	else
	{
        for( i = 0; i < nBandCount; i++ )
		{
			if (ABS(panBandList[i]) > GDALGetRasterCount (hDataset))
			{
				CPLError(CE_Failure, 0,
					"Band %d requested, but only bands 1 to %d available.",
					ABS(panBandList[i]), GDALGetRasterCount(hDataset) );
				return ResetConfigOptions(tkGDAL_ERROR);
			}
		}

		if (nBandCount != GDALGetRasterCount (hDataset))
			bDefBands = FALSE;
	}

	/* -------------------------------------------------------------------- */
	/*		Compute the source window from the projected source window		*/
	/*		if the projected coordinates were provided. Note that the		*/
	/*		projected coordinates are in ulx, uly, lrx, lry format,			*/
	/*		while the anSrcWin is xoff, yoff, xsize, ysize with the			*/
	/*		xoff, yoff being the ulx, uly in pixel/line.					*/
	/* -------------------------------------------------------------------- */
    if( dfULX != 0.0 || dfULY != 0.0 
        || dfLRX != 0.0 || dfLRY != 0.0 )
	{
        double	adfGeoTransform[6];

		GDALGetGeoTransform (hDataset, adfGeoTransform);

		if (adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0)
		{
			CPLError(CE_Failure, 0,
				"The -projwin option was used, but the geotransform is "
				"rotated. This configuration is not supported." );
			GDALClose (hDataset);
			CPLFree (panBandList);
			return ResetConfigOptions(tkGDAL_ERROR);
		}

		anSrcWin[0] = (int)
			((dfULX - adfGeoTransform[0]) / adfGeoTransform[1] + 0.001);
		anSrcWin[1] = (int)
			((dfULY - adfGeoTransform[3]) / adfGeoTransform[5] + 0.001);

		anSrcWin[2] = (int) ((dfLRX - dfULX) / adfGeoTransform[1] + 0.5);
		anSrcWin[3] = (int) ((dfLRY - dfULY) / adfGeoTransform[5] + 0.5);

		if (anSrcWin[0] < 0 || anSrcWin[1] < 0
			||anSrcWin[0] + anSrcWin[2] > GDALGetRasterXSize(hDataset)
			|| anSrcWin[1] + anSrcWin[3] > GDALGetRasterYSize(hDataset))
		{
			CPLError(CE_Failure, 0,
				"Computed -srcwin falls outside raster size of %dx%d.",
				GDALGetRasterXSize(hDataset), 
				GDALGetRasterYSize(hDataset) );
			return ResetConfigOptions(tkGDAL_ERROR);
		}
	}

/* -------------------------------------------------------------------- */
/*		Verify source window.											*/
/* -------------------------------------------------------------------- */
	if (anSrcWin[0] < 0 || anSrcWin[1] < 0
		|| anSrcWin[2] <= 0 || anSrcWin[3] <= 0
		|| anSrcWin[0] + anSrcWin[2] > GDALGetRasterXSize(hDataset)
		|| anSrcWin[1] + anSrcWin[3] > GDALGetRasterYSize(hDataset))
	{
		CPLError(CE_Failure,0,
			"-srcwin %d %d %d %d falls outside raster size of %dx%d "
			"or is otherwise illegal.",
			anSrcWin[0],
			anSrcWin[1],
			anSrcWin[2],
			anSrcWin[3],
			GDALGetRasterXSize(hDataset), 
			GDALGetRasterYSize(hDataset));
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*		Find the output driver.											*/
/* -------------------------------------------------------------------- */
	hDriver = GDALGetDriverByName (pszFormat);
	if (hDriver == NULL)
	{
		CPLError(CE_Failure,0, "Output driver `%s' not recognized.", pszFormat );

		// TODO: List valid drivers?

		GDALClose (hDataset);
		CPLFree (panBandList);
		CSLDestroy (papszCreateOptions);

		return ResetConfigOptions(tkGDAL_ERROR);
	}

	/* -------------------------------------------------------------------- */
	/*		The short form is to CreateCopy(). We use this if the input		*/
	/*		matches the whole dataset. Eventually we should rewrite			*/
	/*		this entire program to use virtual datasets to construct a		*/
	/*		virtual input source to copy from.								*/
	/* -------------------------------------------------------------------- */


	int bSpatialArrangementPreserved = (
           anSrcWin[0] == 0 && anSrcWin[1] == 0
        && anSrcWin[2] == GDALGetRasterXSize(hDataset)
        && anSrcWin[3] == GDALGetRasterYSize(hDataset)
        && pszOXSize == NULL && pszOYSize == NULL );

	if( eOutputType == GDT_Unknown
		&& !bScale && !bUnscale
		&& CSLCount (papszMetadataOptions) == 0 && bDefBands
		&& eMaskMode == MASK_AUTO
		&& bSpatialArrangementPreserved
		&& nGCPCount == 0 && !bGotBounds
		&& pszOutputSRS == NULL && !bSetNoData && !bUnsetNoData
		&& nRGBExpand == 0 && !bStats)
	{
		struct CallbackParams params(GetCallback(), "Translating");

		hOutDS = GDALCreateCopy( hDriver, pszDest, hDataset,
                                 bStrict, papszCreateOptions,
								 pfnProgress, &params);

		if (hOutDS != NULL)
		{
			(*retval) = VARIANT_TRUE;
			GDALClose (hOutDS);
		}
		else
		{
			_lastErrorCode = tkGDAL_ERROR;
		}

		GDALClose (hDataset);

		CPLFree (panBandList);

		if (!_bSubCall)
		{
			GDALDumpOpenDatasets (stderr);
		}

		CSLDestroy (papszCreateOptions);

		return ResetConfigOptions();
	}

/* -------------------------------------------------------------------- */
/*		Establish some parameters.										*/
/* -------------------------------------------------------------------- */
	if (pszOXSize == NULL)
	{
		nOXSize = anSrcWin[2];
		nOYSize = anSrcWin[3];
	}
	else
	{
		nOXSize = (int) ((pszOXSize[strlen(pszOXSize)-1]=='%'
			? CPLAtofM(pszOXSize)/100*anSrcWin[2] : atoi(pszOXSize)));
		nOYSize = (int) ((pszOYSize[strlen(pszOYSize)-1]=='%'
			? CPLAtofM(pszOYSize)/100*anSrcWin[3] : atoi(pszOYSize)));
	}

/* ==================================================================== */
/*      Create a virtual dataset.                                       */
/* ==================================================================== */
    VRTDataset *poVDS;
        
/* -------------------------------------------------------------------- */
/*      Make a virtual clone.                                           */
/* -------------------------------------------------------------------- */
    poVDS = (VRTDataset *) VRTCreate( nOXSize, nOYSize );

	if (nGCPCount == 0)
	{
		if (pszOutputSRS != NULL)
		{
			poVDS->SetProjection (pszOutputSRS);
		}
		else
		{
			pszProjection = GDALGetProjectionRef (hDataset);
			if (pszProjection != NULL && strlen (pszProjection) > 0)
				poVDS->SetProjection (pszProjection);
		}
	}

	if (bGotBounds)
	{
		adfGeoTransform[0] = adfULLR[0];
		adfGeoTransform[1] = (adfULLR[2] - adfULLR[0]) / nOXSize;
		adfGeoTransform[2] = 0.0;
		adfGeoTransform[3] = adfULLR[1];
		adfGeoTransform[4] = 0.0;
		adfGeoTransform[5] = (adfULLR[3] - adfULLR[1]) / nOYSize;

		poVDS->SetGeoTransform (adfGeoTransform);
	}

    else if( GDALGetGeoTransform( hDataset, adfGeoTransform ) == CE_None 
        && nGCPCount == 0 )
	{
        adfGeoTransform[0] += anSrcWin[0] * adfGeoTransform[1]
            + anSrcWin[1] * adfGeoTransform[2];
        adfGeoTransform[3] += anSrcWin[0] * adfGeoTransform[4]
            + anSrcWin[1] * adfGeoTransform[5];

		adfGeoTransform[1] *= anSrcWin[2] / (double) nOXSize;
		adfGeoTransform[2] *= anSrcWin[3] / (double) nOYSize;
		adfGeoTransform[4] *= anSrcWin[2] / (double) nOXSize;
		adfGeoTransform[5] *= anSrcWin[3] / (double) nOYSize;

		poVDS->SetGeoTransform( adfGeoTransform );
	}

	if( nGCPCount != 0 )
	{
		const char *pszGCPProjection = pszOutputSRS;

		if( pszGCPProjection == NULL )
			pszGCPProjection = GDALGetGCPProjection( hDataset );
		if( pszGCPProjection == NULL )
			pszGCPProjection = "";

		poVDS->SetGCPs( nGCPCount, pasGCPs, pszGCPProjection );

		GDALDeinitGCPs( nGCPCount, pasGCPs );
		CPLFree( pasGCPs );
	}

	else if( GDALGetGCPCount( hDataset ) > 0 )
	{
		GDAL_GCP *pasGCPs;
		int       nGCPs = GDALGetGCPCount( hDataset );

		pasGCPs = GDALDuplicateGCPs( nGCPs, GDALGetGCPs( hDataset ) );

        for( i = 0; i < nGCPs; i++ )
		{
			pasGCPs[i].dfGCPPixel -= anSrcWin[0];
			pasGCPs[i].dfGCPLine  -= anSrcWin[1];
			pasGCPs[i].dfGCPPixel *= (nOXSize / (double) anSrcWin[2] );
			pasGCPs[i].dfGCPLine  *= (nOYSize / (double) anSrcWin[3] );
		}

		poVDS->SetGCPs( nGCPs, pasGCPs,
			GDALGetGCPProjection( hDataset ) );

		GDALDeinitGCPs( nGCPs, pasGCPs );
		CPLFree( pasGCPs );
	}

	/* -------------------------------------------------------------------- */
	/*      Transfer generally applicable metadata.                         */
	/* -------------------------------------------------------------------- */
	char** papszMetadata = CSLDuplicate(((GDALDataset*)hDataset)->GetMetadata());
	if ( bScale || bUnscale || eOutputType != GDT_Unknown )
	{
		/* Remove TIFFTAG_MINSAMPLEVALUE and TIFFTAG_MAXSAMPLEVALUE */
		/* if the data range may change because of options */
		char** papszIter = papszMetadata;
		while(papszIter && *papszIter)
		{
			if (EQUALN(*papszIter, "TIFFTAG_MINSAMPLEVALUE=", 23) ||
				EQUALN(*papszIter, "TIFFTAG_MAXSAMPLEVALUE=", 23))
			{
				CPLFree(*papszIter);
				memmove(papszIter, papszIter+1, sizeof(char*) * (CSLCount(papszIter+1)+1));
			}
			else
				papszIter++;
		}
	}
	poVDS->SetMetadata( papszMetadata );
	CSLDestroy( papszMetadata );
	AttachMetadata( (GDALDatasetH) poVDS, papszMetadataOptions );

	const char* pszInterleave = GDALGetMetadataItem(hDataset, "INTERLEAVE", "IMAGE_STRUCTURE");
	if (pszInterleave)
		poVDS->SetMetadataItem("INTERLEAVE", pszInterleave, "IMAGE_STRUCTURE");

	/* -------------------------------------------------------------------- */
	/*      Transfer metadata that remains valid if the spatial             */
	/*      arrangement of the data is unaltered.                           */
	/* -------------------------------------------------------------------- */
	if( bSpatialArrangementPreserved )
	{
		char **papszMD;

		papszMD = ((GDALDataset*)hDataset)->GetMetadata("RPC");
		if( papszMD != NULL )
			poVDS->SetMetadata( papszMD, "RPC" );

		papszMD = ((GDALDataset*)hDataset)->GetMetadata("GEOLOCATION");
		if( papszMD != NULL )
			poVDS->SetMetadata( papszMD, "GEOLOCATION" );
	}

	int nSrcBandCount = nBandCount;

	if (nRGBExpand != 0)
	{
		GDALRasterBand  *poSrcBand;
		poSrcBand = ((GDALDataset *) 
			hDataset)->GetRasterBand(ABS(panBandList[0]));
		if (panBandList[0] < 0)
			poSrcBand = poSrcBand->GetMaskBand();
		GDALColorTable* poColorTable = poSrcBand->GetColorTable();
		if (poColorTable == NULL)
		{
			CPLError(CE_Failure, 0, "Error : band %d has no color table", ABS(panBandList[0]));
			GDALClose( hDataset );
			CPLFree( panBandList );
			CSLDestroy( papszCreateOptions );
			return ResetConfigOptions(tkGDAL_ERROR);
		}

		/* Check that the color table only contains gray levels */
		/* when using -expand gray */
		if (nRGBExpand == 1)
		{
			int nColorCount = poColorTable->GetColorEntryCount();
			int nColor;
			for( nColor = 0; nColor < nColorCount; nColor++ )
			{
				const GDALColorEntry* poEntry = poColorTable->GetColorEntry(nColor);
				if (poEntry->c1 != poEntry->c2 || poEntry->c1 != poEntry->c2)	// TODO: perhaps c3
				{
					CPLError(CE_Warning, 0, "Warning : color table contains non gray levels colors");
					break;
				}
			}
		}

		if (nBandCount == 1)
			nBandCount = nRGBExpand;
		else if (nBandCount == 2 && (nRGBExpand == 3 || nRGBExpand == 4))
			nBandCount = nRGBExpand;
		else
		{
			CPLError(CE_Failure, 0, "Error : invalid use of -expand option.");
			return ResetConfigOptions(tkGDAL_ERROR);
		}
	}

	int bFilterOutStatsMetadata =
		(bScale || bUnscale || !bSpatialArrangementPreserved || nRGBExpand != 0);

	/* ==================================================================== */
	/*      Process all bands.                                              */
	/* ==================================================================== */
    for( i = 0; i < nBandCount; i++ )
	{
		VRTSourcedRasterBand   *poVRTBand;
		GDALRasterBand  *poSrcBand;
		GDALDataType    eBandType;
		int             nComponent = 0;

		int nSrcBand;
		if (nRGBExpand != 0)
		{
			if (nSrcBandCount == 2 && nRGBExpand == 4 && i == 3)
				nSrcBand = panBandList[1];
			else
			{
				nSrcBand = panBandList[0];
				nComponent = i + 1;
			}
		}
		else
			nSrcBand = panBandList[i];

		poSrcBand = ((GDALDataset *) hDataset)->GetRasterBand(ABS(nSrcBand));

		/* -------------------------------------------------------------------- */
		/*      Select output data type to match source.                        */
		/* -------------------------------------------------------------------- */
		if( eOutputType == GDT_Unknown )
			eBandType = poSrcBand->GetRasterDataType();
		else
			eBandType = eOutputType;

		/* -------------------------------------------------------------------- */
		/*      Create this band.                                               */
		/* -------------------------------------------------------------------- */
		poVDS->AddBand( eBandType, NULL );
		poVRTBand = (VRTSourcedRasterBand *) poVDS->GetRasterBand( i+1 );
		if (nSrcBand < 0)
		{
			poVRTBand->AddMaskBandSource(poSrcBand);
			continue;
		}

		/* -------------------------------------------------------------------- */
		/*      Do we need to collect scaling information?                      */
		/* -------------------------------------------------------------------- */
		double dfScale=1.0, dfOffset=0.0;

		if( bScale && !bHaveScaleSrc )
		{
			double	adfCMinMax[2];
			GDALComputeRasterMinMax( poSrcBand, TRUE, adfCMinMax );
			dfScaleSrcMin = adfCMinMax[0];
			dfScaleSrcMax = adfCMinMax[1];
		}

		if( bScale )
		{
			if( dfScaleSrcMax == dfScaleSrcMin )
				dfScaleSrcMax += 0.1;
			if( dfScaleDstMax == dfScaleDstMin )
				dfScaleDstMax += 0.1;

			dfScale = (dfScaleDstMax - dfScaleDstMin) 
				/ (dfScaleSrcMax - dfScaleSrcMin);
			dfOffset = -1 * dfScaleSrcMin * dfScale + dfScaleDstMin;
		}

		if( bUnscale )
		{
			dfScale = poSrcBand->GetScale();
			dfOffset = poSrcBand->GetOffset();
		}

		/* -------------------------------------------------------------------- */
		/*      Create a simple or complex data source depending on the         */
		/*      translation type required.                                      */
		/* -------------------------------------------------------------------- */
		if( bUnscale || bScale || (nRGBExpand != 0 && i < nRGBExpand) )
		{
			poVRTBand->AddComplexSource( poSrcBand,
				anSrcWin[0], anSrcWin[1], 
				anSrcWin[2], anSrcWin[3], 
				0, 0, nOXSize, nOYSize,
				dfOffset, dfScale,
				VRT_NODATA_UNSET,
				nComponent );
		}
		else
			poVRTBand->AddSimpleSource( poSrcBand,
			anSrcWin[0], anSrcWin[1], 
			anSrcWin[2], anSrcWin[3], 
			0, 0, nOXSize, nOYSize );

		/* -------------------------------------------------------------------- */
		/*      In case of color table translate, we only set the color         */
		/*      interpretation other info copied by CopyBandInfo are            */
		/*      not relevant in RGB expansion.                                  */
		/* -------------------------------------------------------------------- */
		if (nRGBExpand == 1)
		{
			poVRTBand->SetColorInterpretation( GCI_GrayIndex );
		}
		else if (nRGBExpand != 0 && i < nRGBExpand)
		{
			poVRTBand->SetColorInterpretation( (GDALColorInterp) (GCI_RedBand + i) );
		}

		/* -------------------------------------------------------------------- */
		/*      copy over some other information of interest.                   */
		/* -------------------------------------------------------------------- */
		else
		{
			CopyBandInfo( poSrcBand, poVRTBand,
				!bStats && !bFilterOutStatsMetadata,
				!bUnscale,
				!bSetNoData && !bUnsetNoData );
		}

		/* -------------------------------------------------------------------- */
		/*      Set a forcable nodata value?                                    */
		/* -------------------------------------------------------------------- */
		if( bSetNoData )
		{
			double dfVal = dfNoDataReal;
			int bClamped = FALSE, bRounded = FALSE;

#define CLAMP(val,type,minval,maxval) \
	do { if (val < minval) { bClamped = TRUE; val = minval; } \
		else if (val > maxval) { bClamped = TRUE; val = maxval; } \
		else if (val != (type)val) { bRounded = TRUE; val = (type)(val + 0.5); } } \
		while(0)

			switch(eBandType)
			{
			case GDT_Byte:
				CLAMP(dfVal, GByte, 0.0, 255.0);
				break;
			case GDT_Int16:
				CLAMP(dfVal, GInt16, -32768.0, 32767.0);
				break;
			case GDT_UInt16:
				CLAMP(dfVal, GUInt16, 0.0, 65535.0);
				break;
			case GDT_Int32:
				CLAMP(dfVal, GInt32, -2147483648.0, 2147483647.0);
				break;
			case GDT_UInt32:
				CLAMP(dfVal, GUInt32, 0.0, 4294967295.0);
				break;
			default:
				break;
			}

			if (bClamped)
			{
				CPLError(CE_Warning, CPLE_AppDefined, "for band %d, nodata value has been clamped "
                       "to %.0f, the original value being out of range.",
					i + 1, dfVal);
			}
			else if(bRounded)
			{
				CPLError(CE_Warning, CPLE_AppDefined, "for band %d, nodata value has been rounded "
                       "to %.0f, %s being an integer datatype.",
					i + 1, dfVal,
					GDALGetDataTypeName(eBandType));
			}

			poVRTBand->SetNoDataValue( dfVal );
		}

		if (eMaskMode == MASK_AUTO &&
			(GDALGetMaskFlags(GDALGetRasterBand(hDataset, 1)) & GMF_PER_DATASET) == 0 &&
			(poSrcBand->GetMaskFlags() & (GMF_ALL_VALID | GMF_NODATA)) == 0)
		{
			if (poVRTBand->CreateMaskBand(poSrcBand->GetMaskFlags()) == CE_None)
			{
				VRTSourcedRasterBand* hMaskVRTBand =
					(VRTSourcedRasterBand*)poVRTBand->GetMaskBand();
				hMaskVRTBand->AddMaskBandSource(poSrcBand,
					anSrcWin[0], anSrcWin[1],
					anSrcWin[2], anSrcWin[3],
					0, 0, nOXSize, nOYSize );
			}
		}
	}

	if (eMaskMode == MASK_USER)
	{
		GDALRasterBand *poSrcBand =
			(GDALRasterBand*)GDALGetRasterBand(hDataset, ABS(nMaskBand));
		if (poSrcBand && poVDS->CreateMaskBand(GMF_PER_DATASET) == CE_None)
		{
			VRTSourcedRasterBand* hMaskVRTBand = (VRTSourcedRasterBand*)
				GDALGetMaskBand(GDALGetRasterBand((GDALDatasetH)poVDS, 1));
			if (nMaskBand > 0)
				hMaskVRTBand->AddSimpleSource(poSrcBand,
				anSrcWin[0], anSrcWin[1],
				anSrcWin[2], anSrcWin[3],
				0, 0, nOXSize, nOYSize );
			else
				hMaskVRTBand->AddMaskBandSource(poSrcBand,
				anSrcWin[0], anSrcWin[1],
				anSrcWin[2], anSrcWin[3],
				0, 0, nOXSize, nOYSize );
		}
	}
	else
	if (eMaskMode == MASK_AUTO && nSrcBandCount > 0 &&
		GDALGetMaskFlags(GDALGetRasterBand(hDataset, 1)) == GMF_PER_DATASET)
	{
		if (poVDS->CreateMaskBand(GMF_PER_DATASET) == CE_None)
		{
			VRTSourcedRasterBand* hMaskVRTBand = (VRTSourcedRasterBand*)
				GDALGetMaskBand(GDALGetRasterBand((GDALDatasetH)poVDS, 1));
			hMaskVRTBand->AddMaskBandSource((GDALRasterBand*)GDALGetRasterBand(hDataset, 1),
				anSrcWin[0], anSrcWin[1],
				anSrcWin[2], anSrcWin[3],
				0, 0, nOXSize, nOYSize );
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Compute stats if required.                                      */
	/* -------------------------------------------------------------------- */
	if (bStats)
	{
        for( i = 0; i < poVDS->GetRasterCount(); i++ )
		{
			double dfMin, dfMax, dfMean, dfStdDev;
			poVDS->GetRasterBand(i+1)->ComputeStatistics( bApproxStats,
				&dfMin, &dfMax, &dfMean, &dfStdDev, GDALDummyProgress, NULL );
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Write to the output file using CopyCreate().                    */
	/* -------------------------------------------------------------------- */
	struct CallbackParams params(GetCallback(), "Translating");

	hOutDS = GDALCreateCopy( hDriver, pszDest, (GDALDatasetH) poVDS,
                             bStrict, papszCreateOptions,
							 pfnProgress, &params );
	if( hOutDS != NULL )
	{
		int bHasGotErr = FALSE;
		CPLErrorReset();
		GDALFlushCache( hOutDS );
		if (CPLGetLastErrorType() != CE_None)
			bHasGotErr = TRUE;
		else
			(*retval) = VARIANT_TRUE;

		GDALClose( hOutDS );
		if (bHasGotErr)
			hOutDS = NULL;
	}

	GDALClose( (GDALDatasetH) poVDS );

	GDALClose(hDataset);

	CPLFree( panBandList );

	CPLFree( pszOutputSRS );

	if( !_bSubCall )
	{
		GDALDumpOpenDatasets( stderr );
	}

	CSLDestroy( papszCreateOptions );

	return ResetConfigOptions();
}

/************************************************************************/
/*                            ArgIsNumeric()                            */
/************************************************************************/

static int ArgIsNumeric( const char *pszArg )

{
	if( pszArg[0] == '-' )
		pszArg++;

	if( *pszArg == '\0' )
		return FALSE;

	while( *pszArg != '\0' )
	{
		if( (*pszArg < '0' || *pszArg > '9') && *pszArg != '.' )
			return FALSE;
		pszArg++;
	}

	return TRUE;
}

/************************************************************************/
/*                           AttachMetadata()                           */
/************************************************************************/

static void AttachMetadata( GDALDatasetH hDS, char **papszMetadataOptions )

{
	int nCount = CSLCount(papszMetadataOptions);
	int i;

	for( i = 0; i < nCount; i++ )
	{
		char    *pszKey = NULL;
		const char *pszValue;

		pszValue = CPLParseNameValue( papszMetadataOptions[i], &pszKey );
		GDALSetMetadataItem(hDS,pszKey,pszValue,NULL);
		CPLFree( pszKey );
	}

	CSLDestroy( papszMetadataOptions );
}

/************************************************************************/
/*                           CopyBandInfo()                            */
/************************************************************************/

/* A bit of a clone of VRTRasterBand::CopyCommonInfoFrom(), but we need */
/* more and more custom behaviour in the context of gdal_translate ... */

static void CopyBandInfo(GDALRasterBand * poSrcBand, GDALRasterBand * poDstBand,
						  int bCanCopyStatsMetadata, int bCopyScale, int bCopyNoData)

{
	int bSuccess;
	double dfNoData;

	if (bCanCopyStatsMetadata)
	{
		poDstBand->SetMetadata( poSrcBand->GetMetadata() );
	}
	else
	{
		char** papszMetadata = poSrcBand->GetMetadata();
		char** papszMetadataNew = NULL;
		for( int i = 0; papszMetadata != NULL && papszMetadata[i] != NULL; i++ )
		{
			if (strncmp(papszMetadata[i], "STATISTICS_", 11) != 0)
				papszMetadataNew = CSLAddString(papszMetadataNew, papszMetadata[i]);
		}
		poDstBand->SetMetadata( papszMetadataNew );
		CSLDestroy(papszMetadataNew);
	}

	poDstBand->SetColorTable( poSrcBand->GetColorTable() );
	poDstBand->SetColorInterpretation(poSrcBand->GetColorInterpretation());
	if( strlen(poSrcBand->GetDescription()) > 0 )
		poDstBand->SetDescription( poSrcBand->GetDescription() );

	if (bCopyNoData)
	{
		dfNoData = poSrcBand->GetNoDataValue( &bSuccess );
		if( bSuccess )
			poDstBand->SetNoDataValue( dfNoData );
	}

	if (bCopyScale)
	{
		poDstBand->SetOffset( poSrcBand->GetOffset() );
		poDstBand->SetScale( poSrcBand->GetScale() );
	}

	poDstBand->SetCategoryNames( poSrcBand->GetCategoryNames() );
	if( !EQUAL(poSrcBand->GetUnitType(),"") )
		poDstBand->SetUnitType( poSrcBand->GetUnitType() );
}

#pragma endregion

#pragma region gdal_rasterize

/************************************************************************/
/*                          InvertGeometries()                          */
/************************************************************************/

static void InvertGeometries( GDALDatasetH hDstDS, 
                              std::vector<OGRGeometryH> &ahGeometries )

{
    OGRGeometryH hCollection = 
        OGR_G_CreateGeometry( wkbGeometryCollection );

/* -------------------------------------------------------------------- */
/*      Create a ring that is a bit outside the raster dataset.         */
/* -------------------------------------------------------------------- */
    OGRGeometryH hUniversePoly, hUniverseRing;
    double adfGeoTransform[6];
    int brx = GDALGetRasterXSize( hDstDS ) + 2;
    int bry = GDALGetRasterYSize( hDstDS ) + 2;

    GDALGetGeoTransform( hDstDS, adfGeoTransform );

    hUniverseRing = OGR_G_CreateGeometry( wkbLinearRing );
    
    OGR_G_AddPoint_2D( 
        hUniverseRing, 
        adfGeoTransform[0] + -2*adfGeoTransform[1] + -2*adfGeoTransform[2],
        adfGeoTransform[3] + -2*adfGeoTransform[4] + -2*adfGeoTransform[5] );
                       
    OGR_G_AddPoint_2D( 
        hUniverseRing, 
        adfGeoTransform[0] + brx*adfGeoTransform[1] + -2*adfGeoTransform[2],
        adfGeoTransform[3] + brx*adfGeoTransform[4] + -2*adfGeoTransform[5] );
                       
    OGR_G_AddPoint_2D( 
        hUniverseRing, 
        adfGeoTransform[0] + brx*adfGeoTransform[1] + bry*adfGeoTransform[2],
        adfGeoTransform[3] + brx*adfGeoTransform[4] + bry*adfGeoTransform[5] );
                       
    OGR_G_AddPoint_2D( 
        hUniverseRing, 
        adfGeoTransform[0] + -2*adfGeoTransform[1] + bry*adfGeoTransform[2],
        adfGeoTransform[3] + -2*adfGeoTransform[4] + bry*adfGeoTransform[5] );
                       
    OGR_G_AddPoint_2D( 
        hUniverseRing, 
        adfGeoTransform[0] + -2*adfGeoTransform[1] + -2*adfGeoTransform[2],
        adfGeoTransform[3] + -2*adfGeoTransform[4] + -2*adfGeoTransform[5] );
                       
    hUniversePoly = OGR_G_CreateGeometry( wkbPolygon );
    OGR_G_AddGeometryDirectly( hUniversePoly, hUniverseRing );

    OGR_G_AddGeometryDirectly( hCollection, hUniversePoly );
    
/* -------------------------------------------------------------------- */
/*      Add the rest of the geometries into our collection.             */
/* -------------------------------------------------------------------- */
    unsigned int iGeom;

    for( iGeom = 0; iGeom < ahGeometries.size(); iGeom++ )
        OGR_G_AddGeometryDirectly( hCollection, ahGeometries[iGeom] );

    ahGeometries.resize(1);
    ahGeometries[0] = hCollection;
}

/************************************************************************/
/*                            ProcessLayer()                            */
/*                                                                      */
/*      Process all the features in a layer selection, collecting       */
/*      geometries and burn values.                                     */
/************************************************************************/

static void ProcessLayer( 
    OGRLayerH hSrcLayer, int bSRSIsSet, 
    GDALDatasetH hDstDS, std::vector<int> anBandList,
    std::vector<double> &adfBurnValues, int b3D, int bInverse,
    const char *pszBurnAttribute, char **papszRasterizeOptions,
    GDALProgressFunc pfnProgress, void* pProgressData )

{
/* -------------------------------------------------------------------- */
/*      Checkout that SRS are the same.                                 */
/*      If -a_srs is specified, skip the test                           */
/* -------------------------------------------------------------------- */
    if (!bSRSIsSet)
    {
        OGRSpatialReferenceH  hDstSRS = NULL;
        if( GDALGetProjectionRef( hDstDS ) != NULL )
        {
            char *pszProjection;
    
            pszProjection = (char *) GDALGetProjectionRef( hDstDS );
    
            hDstSRS = OSRNewSpatialReference(NULL);
            if( OSRImportFromWkt( hDstSRS, &pszProjection ) != CE_None )
            {
                OSRDestroySpatialReference(hDstSRS);
                hDstSRS = NULL;
            }
        }
    
        OGRSpatialReferenceH hSrcSRS = OGR_L_GetSpatialRef(hSrcLayer);
        if( hDstSRS != NULL && hSrcSRS != NULL )
        {
            if( OSRIsSame(hSrcSRS, hDstSRS) == FALSE )
            {
                CPLError( CE_Warning, CPLE_AppDefined,
                        "Warning : the output raster dataset and the input vector layer do not have the same SRS.\n"
                        "Results might be incorrect (no on-the-fly reprojection of input data).\n");
            }
        }
        else if( hDstSRS != NULL && hSrcSRS == NULL )
        {
            CPLError( CE_Warning, CPLE_AppDefined,
                    "Warning : the output raster dataset has a SRS, but the input vector layer SRS is unknown.\n"
                    "Ensure input vector has the same SRS, otherwise results might be incorrect.\n");
        }
        else if( hDstSRS == NULL && hSrcSRS != NULL )
        {
            CPLError( CE_Warning, CPLE_AppDefined,
                    "Warning : the input vector layer has a SRS, but the output raster dataset SRS is unknown.\n"
                    "Ensure output raster dataset has the same SRS, otherwise results might be incorrect.\n");
        }
    
        if( hDstSRS != NULL )
        {
            OSRDestroySpatialReference(hDstSRS);
        }
    }

/* -------------------------------------------------------------------- */
/*      Get field index, and check.                                     */
/* -------------------------------------------------------------------- */
    int iBurnField = -1;

    if( pszBurnAttribute )
    {
        iBurnField = OGR_FD_GetFieldIndex( OGR_L_GetLayerDefn( hSrcLayer ),
                                           pszBurnAttribute );
        if( iBurnField == -1 )
        {
            CPLError( CE_Failure, CPLE_AppDefined,
				"Failed to find field %s on layer %s, skipping.\n",
                    pszBurnAttribute, 
                    OGR_FD_GetName( OGR_L_GetLayerDefn( hSrcLayer ) ) );
            return;
        }
    }

/* -------------------------------------------------------------------- */
/*      Collect the geometries from this layer, and build list of       */
/*      burn values.                                                    */
/* -------------------------------------------------------------------- */
    OGRFeatureH hFeat;
    std::vector<OGRGeometryH> ahGeometries;
    std::vector<double> adfFullBurnValues;

    OGR_L_ResetReading( hSrcLayer );
    
    while( (hFeat = OGR_L_GetNextFeature( hSrcLayer )) != NULL )
    {
        OGRGeometryH hGeom;

        if( OGR_F_GetGeometryRef( hFeat ) == NULL )
        {
            OGR_F_Destroy( hFeat );
            continue;
        }

        hGeom = OGR_G_Clone( OGR_F_GetGeometryRef( hFeat ) );
        ahGeometries.push_back( hGeom );

        for( unsigned int iBand = 0; iBand < anBandList.size(); iBand++ )
        {
            if( adfBurnValues.size() > 0 )
                adfFullBurnValues.push_back( 
                    adfBurnValues[MIN(iBand,adfBurnValues.size()-1)] );
            else if( pszBurnAttribute )
            {
                adfFullBurnValues.push_back( OGR_F_GetFieldAsDouble( hFeat, iBurnField ) );
            }
            /* I have made the 3D option exclusive to other options since it
               can be used to modify the value from "-burn value" or
               "-a attribute_name" */
            if( b3D )
            {
                // TODO: get geometry "z" value
                /* Points and Lines will have their "z" values collected at the
                   point and line levels respectively. However filled polygons
                   (GDALdllImageFilledPolygon) can use some help by getting
                   their "z" values here. */
                adfFullBurnValues.push_back( 0.0 );
            }
        }
        
        OGR_F_Destroy( hFeat );
    }

/* -------------------------------------------------------------------- */
/*      If we are in inverse mode, we add one extra ring around the     */
/*      whole dataset to invert the concept of insideness and then      */
/*      merge everything into one geometry collection.                  */
/* -------------------------------------------------------------------- */
    if( bInverse )
    {
        InvertGeometries( hDstDS, ahGeometries );
    }

/* -------------------------------------------------------------------- */
/*      Perform the burn.                                               */
/* -------------------------------------------------------------------- */
    GDALRasterizeGeometries( hDstDS, anBandList.size(), &(anBandList[0]), 
                             ahGeometries.size(), &(ahGeometries[0]), 
                             NULL, NULL, &(adfFullBurnValues[0]), 
                             papszRasterizeOptions,
                             pfnProgress, pProgressData );

/* -------------------------------------------------------------------- */
/*      Cleanup geometries.                                             */
/* -------------------------------------------------------------------- */
    int iGeom;

    for( iGeom = ahGeometries.size()-1; iGeom >= 0; iGeom-- )
        OGR_G_DestroyGeometry( ahGeometries[iGeom] );
}

/************************************************************************/
/*                  CreateOutputDataset()                               */
/************************************************************************/

static
GDALDatasetH CreateOutputDataset(std::vector<OGRLayerH> ahLayers,
                                 OGRSpatialReferenceH hSRS,
                                 int bGotBounds, OGREnvelope sEnvelop,
                                 GDALDriverH hDriver, const char* pszDstFilename,
                                 int nXSize, int nYSize, double dfXRes, double dfYRes,
                                 int bTargetAlignedPixels,
                                 int nBandCount, GDALDataType eOutputType,
                                 char** papszCreateOptions, std::vector<double> adfInitVals,
                                 int bNoDataSet, double dfNoData)
{
    int bFirstLayer = TRUE;
    char* pszWKT = NULL;
    GDALDatasetH hDstDS = NULL;
    unsigned int i;

    for( i = 0; i < ahLayers.size(); i++ )
    {
        OGRLayerH hLayer = ahLayers[i];

        if (!bGotBounds)
        {
            OGREnvelope sLayerEnvelop;

            if (OGR_L_GetExtent(hLayer, &sLayerEnvelop, TRUE) != OGRERR_NONE)
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Cannot get layer extent\n");
                return NULL;
            }

            /* When rasterizing point layers and that the bounds have */
            /* not been explicitely set, voluntary increase the extent by */
            /* a half-pixel size to avoid missing points on the border */
            if (wkbFlatten(OGR_L_GetGeomType(hLayer)) == wkbPoint &&
                !bTargetAlignedPixels && dfXRes != 0 && dfYRes != 0)
            {
                sLayerEnvelop.MinX -= dfXRes / 2;
                sLayerEnvelop.MaxX += dfXRes / 2;
                sLayerEnvelop.MinY -= dfYRes / 2;
                sLayerEnvelop.MaxY += dfYRes / 2;
            }

            if (bFirstLayer)
            {
                sEnvelop.MinX = sLayerEnvelop.MinX;
                sEnvelop.MinY = sLayerEnvelop.MinY;
                sEnvelop.MaxX = sLayerEnvelop.MaxX;
                sEnvelop.MaxY = sLayerEnvelop.MaxY;

                if (hSRS == NULL)
                    hSRS = OGR_L_GetSpatialRef(hLayer);

                bFirstLayer = FALSE;
            }
            else
            {
                sEnvelop.MinX = MIN(sEnvelop.MinX, sLayerEnvelop.MinX);
                sEnvelop.MinY = MIN(sEnvelop.MinY, sLayerEnvelop.MinY);
                sEnvelop.MaxX = MAX(sEnvelop.MaxX, sLayerEnvelop.MaxX);
                sEnvelop.MaxY = MAX(sEnvelop.MaxY, sLayerEnvelop.MaxY);
            }
        }
        else
        {
            if (bFirstLayer)
            {
                if (hSRS == NULL)
                    hSRS = OGR_L_GetSpatialRef(hLayer);

                bFirstLayer = FALSE;
            }
        }
    }

    if (dfXRes == 0 && dfYRes == 0)
    {
        dfXRes = (sEnvelop.MaxX - sEnvelop.MinX) / nXSize;
        dfYRes = (sEnvelop.MaxY - sEnvelop.MinY) / nYSize;
    }
    else if (bTargetAlignedPixels && dfXRes != 0 && dfYRes != 0)
    {
        sEnvelop.MinX = floor(sEnvelop.MinX / dfXRes) * dfXRes;
        sEnvelop.MaxX = ceil(sEnvelop.MaxX / dfXRes) * dfXRes;
        sEnvelop.MinY = floor(sEnvelop.MinY / dfYRes) * dfYRes;
        sEnvelop.MaxY = ceil(sEnvelop.MaxY / dfYRes) * dfYRes;
    }

    double adfProjection[6];
    adfProjection[0] = sEnvelop.MinX;
    adfProjection[1] = dfXRes;
    adfProjection[2] = 0;
    adfProjection[3] = sEnvelop.MaxY;
    adfProjection[4] = 0;
    adfProjection[5] = -dfYRes;

    if (nXSize == 0 && nYSize == 0)
    {
        nXSize = (int)(0.5 + (sEnvelop.MaxX - sEnvelop.MinX) / dfXRes);
        nYSize = (int)(0.5 + (sEnvelop.MaxY - sEnvelop.MinY) / dfYRes);
    }

    hDstDS = GDALCreate(hDriver, pszDstFilename, nXSize, nYSize,
                        nBandCount, eOutputType, papszCreateOptions);
    if (hDstDS == NULL)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Cannot create %s\n", pszDstFilename);
        return NULL;
    }

    GDALSetGeoTransform(hDstDS, adfProjection);

    if (hSRS)
        OSRExportToWkt(hSRS, &pszWKT);
    if (pszWKT)
        GDALSetProjection(hDstDS, pszWKT);
    CPLFree(pszWKT);

    int iBand;
    /*if( nBandCount == 3 || nBandCount == 4 )
    {
        for(iBand = 0; iBand < nBandCount; iBand++)
        {
            GDALRasterBandH hBand = GDALGetRasterBand(hDstDS, iBand + 1);
            GDALSetRasterColorInterpretation(hBand, (GDALColorInterp)(GCI_RedBand + iBand));
        }
    }*/

    if (bNoDataSet)
    {
        for(iBand = 0; iBand < nBandCount; iBand++)
        {
            GDALRasterBandH hBand = GDALGetRasterBand(hDstDS, iBand + 1);
            GDALSetRasterNoDataValue(hBand, dfNoData);
        }
    }

    if (adfInitVals.size() != 0)
    {
        for(iBand = 0; iBand < MIN(nBandCount,(int)adfInitVals.size()); iBand++)
        {
            GDALRasterBandH hBand = GDALGetRasterBand(hDstDS, iBand + 1);
            GDALFillRaster(hBand, adfInitVals[iBand], 0);
        }
    }

    return hDstDS;
}

/************************************************************************/
/*                                GDALRasterize()                       */
/************************************************************************/

STDMETHODIMP CUtils::GDALRasterize(BSTR bstrSrcFilename, BSTR bstrDstFilename,
								   BSTR bstrOptions, ICallback * cBack, VARIANT_BOOL *retval)

{
	USES_CONVERSION;

	int argc = 0;
    int i, b3D = FALSE;
    int bInverse = FALSE;
    const char *pszSrcFilename = NULL;
    const char *pszDstFilename = NULL;
    char **papszLayers = NULL;
    const char *pszSQL = NULL;
    const char *pszBurnAttribute = NULL;
    const char *pszWHERE = NULL;
    std::vector<int> anBandList;
    std::vector<double> adfBurnValues;
    char **papszRasterizeOptions = NULL;
    double dfXRes = 0, dfYRes = 0;
    int bCreateOutput = FALSE;
    const char* pszFormat = "GTiff";
    int bFormatExplicitelySet = FALSE;
    char **papszCreateOptions = NULL;
    GDALDriverH hDriver = NULL;
    GDALDataType eOutputType = GDT_Float64;
    std::vector<double> adfInitVals;
    int bNoDataSet = FALSE;
    double dfNoData = 0;
    OGREnvelope sEnvelop;
    int bGotBounds = FALSE;
    int nXSize = 0, nYSize = 0;
    int bQuiet = FALSE;
    GDALProgressFunc pfnProgress = GDALProgressCallback;
    OGRSpatialReferenceH hSRS = NULL;
    int bTargetAlignedPixels = FALSE;
	struct CallbackParams params(GetCallback(), "Rasterizing");
    
	(*retval) = VARIANT_FALSE;

    GDALAllRegister();
    OGRRegisterAll();

	Parse(OLE2CA(bstrOptions), &argc);

	if (!ProcessGeneralOptions(&argc))
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*      Parse arguments.                                                */
/* -------------------------------------------------------------------- */
    for( i = 1; i < argc; i++ )
    {
		if( EQUAL(_sArr[i],"-a") && i < argc-1 )
        {
            pszBurnAttribute = _sArr[++i];
        }
        else if( EQUAL(_sArr[i],"-b") && i < argc-1 )
        {
            if (strchr(_sArr[i+1], ' '))
            {
                char** papszTokens = CSLTokenizeString( _sArr[i+1] );
                char** papszIter = papszTokens;
                while(papszIter && *papszIter)
                {
                    anBandList.push_back(atoi(*papszIter));
                    papszIter ++;
                }
                CSLDestroy(papszTokens);
                i += 1;
            }
            else
            {
                while(i < argc-1 && ArgIsNumeric(_sArr[i+1]))
                {
                    anBandList.push_back(atoi(_sArr[i+1]));
                    i += 1;
                }
            }
        }
        else if( EQUAL(_sArr[i],"-3d")  )
        {
            b3D = TRUE;
            papszRasterizeOptions = 
                CSLSetNameValue( papszRasterizeOptions, "BURN_VALUE_FROM", "Z");
        }
        else if( EQUAL(_sArr[i],"-i")  )
        {
            bInverse = TRUE;
        }
        else if( EQUAL(_sArr[i],"-at")  )
        {
            papszRasterizeOptions = 
                CSLSetNameValue( papszRasterizeOptions, "ALL_TOUCHED", "TRUE" );
        }
        else if( EQUAL(_sArr[i],"-burn") && i < argc-1 )
        {
            if (strchr(_sArr[i+1], ' '))
            {
                char** papszTokens = CSLTokenizeString( _sArr[i+1] );
                char** papszIter = papszTokens;
                while(papszIter && *papszIter)
                {
                    adfBurnValues.push_back(atof(*papszIter));
                    papszIter ++;
                }
                CSLDestroy(papszTokens);
                i += 1;
            }
            else
            {
                while(i < argc-1 && ArgIsNumeric(_sArr[i+1]))
                {
                    adfBurnValues.push_back(atof(_sArr[i+1]));
                    i += 1;
                }
            }
        }
        else if( EQUAL(_sArr[i],"-where") && i < argc-1 )
        {
            pszWHERE = _sArr[++i];
        }
        else if( EQUAL(_sArr[i],"-l") && i < argc-1 )
        {
            papszLayers = CSLAddString( papszLayers, _sArr[++i] );
        }
        else if( EQUAL(_sArr[i],"-sql") && i < argc-1 )
        {
            pszSQL = _sArr[++i];
        }
        else if( EQUAL(_sArr[i],"-of") && i < argc-1 )
        {
            pszFormat = _sArr[++i];
            bFormatExplicitelySet = TRUE;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-init") && i < argc - 1 )
        {
            if (strchr(_sArr[i+1], ' '))
            {
                char** papszTokens = CSLTokenizeString( _sArr[i+1] );
                char** papszIter = papszTokens;
                while(papszIter && *papszIter)
                {
                    adfInitVals.push_back(atof(*papszIter));
                    papszIter ++;
                }
                CSLDestroy(papszTokens);
                i += 1;
            }
            else
            {
                while(i < argc-1 && ArgIsNumeric(_sArr[i+1]))
                {
                    adfInitVals.push_back(atof(_sArr[i+1]));
                    i += 1;
                }
            }
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-a_nodata") && i < argc - 1 )
        {
            dfNoData = atof(_sArr[i+1]);
            bNoDataSet = TRUE;
            i += 1;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-a_srs") && i < argc-1 )
        {
            hSRS = OSRNewSpatialReference( NULL );

            if( OSRSetFromUserInput(hSRS, _sArr[i+1]) != OGRERR_NONE )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Failed to process SRS definition: %s\n", 
                         _sArr[i+1] );
                return ResetConfigOptions(tkGDAL_ERROR);
            }

            i++;
            bCreateOutput = TRUE;
        }   

        else if( EQUAL(_sArr[i],"-te") && i < argc - 4 )
        {
            sEnvelop.MinX = atof(_sArr[++i]);
            sEnvelop.MinY = atof(_sArr[++i]);
            sEnvelop.MaxX = atof(_sArr[++i]);
            sEnvelop.MaxY = atof(_sArr[++i]);
            bGotBounds = TRUE;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-a_ullr") && i < argc - 4 )
        {
            sEnvelop.MinX = atof(_sArr[++i]);
            sEnvelop.MaxY = atof(_sArr[++i]);
            sEnvelop.MaxX = atof(_sArr[++i]);
            sEnvelop.MinY = atof(_sArr[++i]);
            bGotBounds = TRUE;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-co") && i < argc-1 )
        {
            papszCreateOptions = CSLAddString( papszCreateOptions, _sArr[++i] );
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-ot") && i < argc-1 )
        {
            int	iType;
            
            for( iType = 1; iType < GDT_TypeCount; iType++ )
            {
                if( GDALGetDataTypeName((GDALDataType)iType) != NULL
                    && EQUAL(GDALGetDataTypeName((GDALDataType)iType),
                             _sArr[i+1]) )
                {
                    eOutputType = (GDALDataType) iType;
                }
            }

            if( eOutputType == GDT_Unknown )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Unknown output pixel type: %s\n", _sArr[i+1] );
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            i++;
            bCreateOutput = TRUE;
        }
        else if( (EQUAL(_sArr[i],"-ts") || EQUAL(_sArr[i],"-outsize")) && i < argc-2 )
        {
            nXSize = atoi(_sArr[++i]);
            nYSize = atoi(_sArr[++i]);
            if (nXSize <= 0 || nYSize <= 0)
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Wrong value for -outsize parameters\n");
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-tr") && i < argc-2 )
        {
            dfXRes = atof(_sArr[++i]);
            dfYRes = fabs(atof(_sArr[++i]));
            if( dfXRes == 0 || dfYRes == 0 )
            {
                CPLError( CE_Failure, CPLE_AppDefined, "Wrong value for -tr parameters\n");
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-tap") )
        {
            bTargetAlignedPixels = TRUE;
            bCreateOutput = TRUE;
        }
    }

	pszSrcFilename = OLE2CA(bstrSrcFilename);
	pszDstFilename = OLE2CA(bstrDstFilename);

    if( pszSrcFilename == NULL || pszDstFilename == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Missing source or destination.\n\n" );
		return ResetConfigOptions(tkGDAL_ERROR);
    }

    if( adfBurnValues.size() == 0 && pszBurnAttribute == NULL && !b3D )
    {
        CPLError( CE_Failure, CPLE_AppDefined, "At least one of -3d, -burn or -a required.\n\n" );
		return ResetConfigOptions(tkGDAL_ERROR);
    }

    if( bCreateOutput )
    {
        if( dfXRes == 0 && dfYRes == 0 && nXSize == 0 && nYSize == 0 )
        {
            CPLError( CE_Failure, CPLE_AppDefined, "'-tr xres yes' or '-ts xsize ysize' is required.\n\n" );
			return ResetConfigOptions(tkGDAL_ERROR);
        }
    
        if (bTargetAlignedPixels && dfXRes == 0 && dfYRes == 0)
        {
            CPLError( CE_Failure, CPLE_AppDefined, "-tap option cannot be used without using -tr\n");
			return ResetConfigOptions(tkGDAL_ERROR);
        }

        if( anBandList.size() != 0 )
        {
            CPLError( CE_Failure, CPLE_AppDefined, "-b option cannot be used when creating a GDAL dataset.\n\n" );
			return ResetConfigOptions(tkGDAL_ERROR);
        }

        int nBandCount = 1;

        if (adfBurnValues.size() != 0)
            nBandCount = adfBurnValues.size();

        if ((int)adfInitVals.size() > nBandCount)
            nBandCount = adfInitVals.size();

        if (adfInitVals.size() == 1)
        {
            for(i=1;i<=nBandCount - 1;i++)
                adfInitVals.push_back( adfInitVals[0] );
        }

        int i;
        for(i=1;i<=nBandCount;i++)
            anBandList.push_back( i );
    }
    else
    {
        if( anBandList.size() == 0 )
            anBandList.push_back( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Open source vector dataset.                                     */
/* -------------------------------------------------------------------- */
    OGRDataSourceH hSrcDS;

    hSrcDS = OGROpen( pszSrcFilename, FALSE, NULL );
    if( hSrcDS == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Failed to open feature source: %s\n", 
                 pszSrcFilename);
		return ResetConfigOptions(tkGDAL_ERROR);
    }

    if( pszSQL == NULL && papszLayers == NULL )
    {
        if( OGR_DS_GetLayerCount(hSrcDS) == 1 )
        {
            papszLayers = CSLAddString(NULL, OGR_L_GetName(OGR_DS_GetLayer(hSrcDS, 0)));
        }
        else
        {
            CPLError( CE_Failure, CPLE_AppDefined, "At least one of -l or -sql required.\n\n" );
			return ResetConfigOptions(tkGDAL_ERROR);
         }
    }

/* -------------------------------------------------------------------- */
/*      Open target raster file.  Eventually we will add optional       */
/*      creation.                                                       */
/* -------------------------------------------------------------------- */
    GDALDatasetH hDstDS = NULL;

    if (bCreateOutput)
    {
/* -------------------------------------------------------------------- */
/*      Find the output driver.                                         */
/* -------------------------------------------------------------------- */
        hDriver = GDALGetDriverByName( pszFormat );
        if( hDriver == NULL 
            || GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) == NULL )
        {

			CPLError(CE_Failure, CPLE_AppDefined, "Output driver `%s' not recognised or does not support direct output file creation", pszFormat);
			return ResetConfigOptions();
        }

        if (!bQuiet && !bFormatExplicitelySet)
            CheckExtensionConsistency(pszDstFilename, pszFormat);
    }
    else
    {
		hDstDS = GdalHelper::OpenRasterDatasetW(OLE2W(bstrDstFilename), GA_Update);
        if( hDstDS == NULL )
            return ResetConfigOptions();
    }

/* -------------------------------------------------------------------- */
/*      Process SQL request.                                            */
/* -------------------------------------------------------------------- */
    if( pszSQL != NULL )
    {
        OGRLayerH hLayer;

        hLayer = OGR_DS_ExecuteSQL( hSrcDS, pszSQL, NULL, NULL ); 
        if( hLayer != NULL )
        {
            if (bCreateOutput)
            {
                std::vector<OGRLayerH> ahLayers;
                ahLayers.push_back(hLayer);

                hDstDS = CreateOutputDataset(ahLayers, hSRS,
                                 bGotBounds, sEnvelop,
                                 hDriver, pszDstFilename,
                                 nXSize, nYSize, dfXRes, dfYRes,
                                 bTargetAlignedPixels,
                                 anBandList.size(), eOutputType,
                                 papszCreateOptions, adfInitVals,
                                 bNoDataSet, dfNoData);
            }

            ProcessLayer( hLayer, hSRS != NULL, hDstDS, anBandList, 
                          adfBurnValues, b3D, bInverse, pszBurnAttribute,
                          papszRasterizeOptions, pfnProgress, &params );

            OGR_DS_ReleaseResultSet( hSrcDS, hLayer );
        }
    }

/* -------------------------------------------------------------------- */
/*      Create output file if necessary.                                */
/* -------------------------------------------------------------------- */
    int nLayerCount = CSLCount(papszLayers);

    if (bCreateOutput && hDstDS == NULL)
    {
        std::vector<OGRLayerH> ahLayers;

        for( i = 0; i < nLayerCount; i++ )
        {
            OGRLayerH hLayer = OGR_DS_GetLayerByName( hSrcDS, papszLayers[i] );
            if( hLayer == NULL )
            {
                continue;
            }
            ahLayers.push_back(hLayer);
        }

        hDstDS = CreateOutputDataset(ahLayers, hSRS,
                                bGotBounds, sEnvelop,
                                hDriver, pszDstFilename,
                                nXSize, nYSize, dfXRes, dfYRes,
                                bTargetAlignedPixels,
                                anBandList.size(), eOutputType,
                                papszCreateOptions, adfInitVals,
                                bNoDataSet, dfNoData);
    }

/* -------------------------------------------------------------------- */
/*      Process each layer.                                             */
/* -------------------------------------------------------------------- */

    for( i = 0; i < nLayerCount; i++ )
    {
        OGRLayerH hLayer = OGR_DS_GetLayerByName( hSrcDS, papszLayers[i] );
        if( hLayer == NULL )
        {
            CPLError( CE_Warning, CPLE_AppDefined, "Unable to find layer %s, skipping.\n", 
                      papszLayers[i] );
            continue;
        }

        if( pszWHERE )
        {
            if( OGR_L_SetAttributeFilter( hLayer, pszWHERE ) != OGRERR_NONE )
                break;
        }

        void *pScaledProgress;
        pScaledProgress =
            GDALCreateScaledProgress( 0.0, 1.0 * (i + 1) / nLayerCount,
                                      pfnProgress, &params );

        ProcessLayer( hLayer, hSRS != NULL, hDstDS, anBandList, 
                      adfBurnValues, b3D, bInverse, pszBurnAttribute,
                      papszRasterizeOptions, GDALScaledProgress, pScaledProgress );

        GDALDestroyScaledProgress( pScaledProgress );
    }

	(*retval) = VARIANT_TRUE;

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */

    OGR_DS_Destroy( hSrcDS );
    GDALClose( hDstDS );

    OSRDestroySpatialReference(hSRS);

    CSLDestroy( papszRasterizeOptions );
    CSLDestroy( papszLayers );
    CSLDestroy( papszCreateOptions );
    
    return ResetConfigOptions();
}

#pragma endregion

#pragma region gdalwarp

static void
LoadCutline( const char *pszCutlineDSName, const char *pszCLayer, 
             const char *pszCWHERE, const char *pszCSQL, 
             void **phCutlineRet );
static void
TransformCutlineToSource( GDALDatasetH hSrcDS, void *hCutline,
                          char ***ppapszWarpOptions, char **papszTO );

static GDALDatasetH 
GDALWarpCreateOutput( char **papszSrcFiles, const char *pszFilename, 
                      const char *pszFormat, char **papszTO, 
                      char ***ppapszCreateOptions, GDALDataType eDT,
                      void ** phTransformArg,
                      GDALDatasetH* phSrcDS,
					  int bVRT, int bQuiet, int bTargetAlignedPixels,
					  double dfXRes, double dfYRes,
					  double dfMinX, double dfMaxX,
					  double dfMinY, double dfMaxY,
					  int nForcePixels, int nForceLines,
					  int bEnableSrcAlpha, int bEnableDstAlpha );

/************************************************************************/
/*                             SanitizeSRS                              */
/************************************************************************/

char *SanitizeSRS( const char *pszUserInput )

{
    OGRSpatialReferenceH hSRS;
    char *pszResult = NULL;

    CPLErrorReset();
    
    hSRS = OSRNewSpatialReference( NULL );
    if( OSRSetFromUserInput( hSRS, pszUserInput ) == OGRERR_NONE )
        OSRExportToWkt( hSRS, &pszResult );
    else
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "Translating source or target SRS failed:\n%s",
                  pszUserInput );
        return NULL;
    }
    
    OSRDestroySpatialReference( hSRS );

    return pszResult;
}

/************************************************************************/
/*                                GDALWarp()                            */
/************************************************************************/
__declspec(deprecated("This is a deprecated function, use CGdalUtils::GdalRasterWarp instead"))
STDMETHODIMP CUtils::GDALWarp(BSTR bstrSrcFilename, BSTR bstrDstFilename,
							  BSTR bstrOptions, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	int argc = 0;
	double	       dfMinX=0.0, dfMinY=0.0, dfMaxX=0.0, dfMaxY=0.0;
	double	       dfXRes=0.0, dfYRes=0.0;
	int             bTargetAlignedPixels = FALSE;
	int             nForcePixels=0, nForceLines=0;
	int             bEnableDstAlpha = FALSE, bEnableSrcAlpha = FALSE;
	int             bVRT = FALSE;
    GDALDatasetH	hDstDS;
    const char         *pszFormat = "GTiff";
    int bFormatExplicitelySet = FALSE;
    char              **papszSrcFiles = NULL;
    char               *pszDstFilename = NULL;
    int                 bCreateOutput = FALSE, i;
    void               *hTransformArg, *hGenImgProjArg=NULL, *hApproxArg=NULL;
    char               **papszWarpOptions = NULL;
    double             dfErrorThreshold = 0.125;
    double             dfWarpMemoryLimit = 0.0;
    GDALTransformerFunc pfnTransformer = NULL;
    char                **papszCreateOptions = NULL;
    GDALDataType        eOutputType = GDT_Unknown, eWorkingType = GDT_Unknown; 
    GDALResampleAlg     eResampleAlg = GRA_NearestNeighbour;
    const char          *pszSrcNodata = NULL;
    const char          *pszDstNodata = NULL;
    int                 bMulti = FALSE;
    char                **papszTO = NULL;
    char                *pszCutlineDSName = NULL;
    char                *pszCLayer = NULL, *pszCWHERE = NULL, *pszCSQL = NULL;
    void                *hCutline = NULL;
    int                  bHasGotErr = FALSE;
    int                  bCropToCutline = FALSE;
    int                  bOverwrite = FALSE;
	struct CallbackParams params(GetCallback(), "Warping");	

/* -------------------------------------------------------------------- */
/*      Register standard GDAL drivers, and process generic GDAL        */
/*      command options.                                                */
/* -------------------------------------------------------------------- */
    GDALAllRegister();

	(*retval) = VARIANT_FALSE;

	CString opt = OLE2A(bstrOptions);
	Parse(opt, &argc);

	if (!ProcessGeneralOptions(&argc))
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

	papszSrcFiles = CSLAddString(papszSrcFiles, OLE2A(bstrSrcFilename));
	papszSrcFiles = CSLAddString(papszSrcFiles, OLE2A(bstrDstFilename));

/* -------------------------------------------------------------------- */
/*      Parse arguments.                                                */
/* -------------------------------------------------------------------- */
    for( i = 1; i < argc; i++ )
    {
        if( EQUAL(_sArr[i],"-tps") || EQUAL(_sArr[i],"-rpc") || EQUAL(_sArr[i],"-geoloc")  )
        {
            const char* pszMethod = CSLFetchNameValue(papszTO, "METHOD");
            if (pszMethod)
                CPLError( CE_Warning, CPLE_AppDefined, "Warning: only one METHOD can be used. Method %s is already defined.\n",
                        pszMethod);
            const char* pszMAX_GCP_ORDER = CSLFetchNameValue(papszTO, "MAX_GCP_ORDER");
            if (pszMAX_GCP_ORDER)
                CPLError( CE_Warning, CPLE_AppDefined, "Warning: only one METHOD can be used. -order %s option was specified, so it is likely that GCP_POLYNOMIAL was implied.\n",
                        pszMAX_GCP_ORDER);
        }

        if( EQUAL(_sArr[i],"-co") && i < argc-1 )
        {
            papszCreateOptions = CSLAddString( papszCreateOptions, _sArr[++i] );
            bCreateOutput = TRUE;
        }   
        else if( EQUAL(_sArr[i],"-wo") && i < argc-1 )
        {
            papszWarpOptions = CSLAddString( papszWarpOptions, _sArr[++i] );
        }   
        else if( EQUAL(_sArr[i],"-multi") )
        {
            bMulti = TRUE;
        }   
        else if( EQUAL(_sArr[i],"-dstalpha") )
        {
            bEnableDstAlpha = TRUE;
        }
        else if( EQUAL(_sArr[i],"-srcalpha") )
        {
            bEnableSrcAlpha = TRUE;
        }
        else if( EQUAL(_sArr[i],"-of") && i < argc-1 )
        {
            pszFormat = _sArr[++i];
            bFormatExplicitelySet = TRUE;
            bCreateOutput = TRUE;
            if( EQUAL(pszFormat,"VRT") )
                bVRT = TRUE;
        }
        else if( EQUAL(_sArr[i],"-t_srs") && i < argc-1 )
        {
            char *pszSRS = SanitizeSRS(_sArr[++i]);

			if (pszSRS == NULL)
			{
				CPLError(CE_Warning, CPLE_AppDefined, "GdalWarp: failed to read target projection argument (-t_srs).");
				return ResetConfigOptions();
			}

            papszTO = CSLSetNameValue( papszTO, "DST_SRS", pszSRS );
            CPLFree( pszSRS );
        }
        else if( EQUAL(_sArr[i],"-s_srs") && i < argc-1 )
        {
            char *pszSRS = SanitizeSRS(_sArr[++i]);

			if (pszSRS == NULL)
			{
				CPLError(CE_Warning, CPLE_AppDefined, "GdalWarp: failed to read source projection argument (-s_srs).");
				return ResetConfigOptions();
			}

            papszTO = CSLSetNameValue( papszTO, "SRC_SRS", pszSRS );
            CPLFree( pszSRS );
        }
        else if( EQUAL(_sArr[i],"-order") && i < argc-1 )
        {
            const char* pszMethod = CSLFetchNameValue(papszTO, "METHOD");
            if (pszMethod)
                CPLError( CE_Warning, CPLE_AppDefined, "Warning: only one METHOD can be used. Method %s is already defined\n",
                        pszMethod);
            papszTO = CSLSetNameValue( papszTO, "MAX_GCP_ORDER", _sArr[++i] );
        }
        else if( EQUAL(_sArr[i],"-refine_gcps") && i < argc-1 )
        {
            papszTO = CSLSetNameValue( papszTO, "REFINE_TOLERANCE", _sArr[++i] );
            if(atof(_sArr[i]) < 0)
            {
                CPLError(CE_Failure, CPLE_AppDefined, "The tolerance for -refine_gcps may not be negative\n");
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            if (i < argc-1 && atoi(_sArr[i+1]) >= 0 && isdigit(_sArr[i+1][0]))
            {
                papszTO = CSLSetNameValue( papszTO, "REFINE_MINIMUM_GCPS", _sArr[++i] );
            }
            else
            {
                papszTO = CSLSetNameValue( papszTO, "REFINE_MINIMUM_GCPS", "-1" );
            }
        }
        else if( EQUAL(_sArr[i],"-tps") )
        {
            papszTO = CSLSetNameValue( papszTO, "METHOD", "GCP_TPS" );
        }
        else if( EQUAL(_sArr[i],"-rpc") )
        {
            papszTO = CSLSetNameValue( papszTO, "METHOD", "RPC" );
        }
        else if( EQUAL(_sArr[i],"-geoloc") )
        {
            papszTO = CSLSetNameValue( papszTO, "METHOD", "GEOLOC_ARRAY" );
        }
        else if( EQUAL(_sArr[i],"-to") && i < argc-1 )
        {
            papszTO = CSLAddString( papszTO, _sArr[++i] );
        }
        else if( EQUAL(_sArr[i],"-et") && i < argc-1 )
        {
            dfErrorThreshold = CPLAtofM(_sArr[++i]);
        }
        else if( EQUAL(_sArr[i],"-wm") && i < argc-1 )
        {
            if( CPLAtofM(_sArr[i+1]) < 10000 )
                dfWarpMemoryLimit = CPLAtofM(_sArr[i+1]) * 1024 * 1024;
            else
                dfWarpMemoryLimit = CPLAtofM(_sArr[i+1]);
            i++;
        }
        else if( EQUAL(_sArr[i],"-srcnodata") && i < argc-1 )
        {
            pszSrcNodata = _sArr[++i];
        }
        else if( EQUAL(_sArr[i],"-dstnodata") && i < argc-1 )
        {
            pszDstNodata = _sArr[++i];
        }
        else if( EQUAL(_sArr[i],"-tr") && i < argc-2 )
        {
            dfXRes = CPLAtofM(_sArr[++i]);
            dfYRes = fabs(CPLAtofM(_sArr[++i]));
            if( dfXRes == 0 || dfYRes == 0 )
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Wrong value for -tr parameters\n");
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-tap") )
        {
            bTargetAlignedPixels = TRUE;
        }
        else if( EQUAL(_sArr[i],"-ot") && i < argc-1 )
        {
            int	iType;
            
            for( iType = 1; iType < GDT_TypeCount; iType++ )
            {
                if( GDALGetDataTypeName((GDALDataType)iType) != NULL
                    && EQUAL(GDALGetDataTypeName((GDALDataType)iType),
                             _sArr[i+1]) )
                {
                    eOutputType = (GDALDataType) iType;
                }
            }

            if( eOutputType == GDT_Unknown )
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Unknown output pixel type: %s\n", _sArr[i+1] );
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            i++;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-wt") && i < argc-1 )
        {
            int	iType;
            
            for( iType = 1; iType < GDT_TypeCount; iType++ )
            {
                if( GDALGetDataTypeName((GDALDataType)iType) != NULL
                    && EQUAL(GDALGetDataTypeName((GDALDataType)iType),
                             _sArr[i+1]) )
                {
                    eWorkingType = (GDALDataType) iType;
                }
            }

            if( eWorkingType == GDT_Unknown )
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Unknown output pixel type: %s\n", _sArr[i+1] );
				return ResetConfigOptions(tkGDAL_ERROR);
            }
            i++;
        }
        else if( EQUAL(_sArr[i],"-ts") && i < argc-2 )
        {
            nForcePixels = atoi(_sArr[++i]);
            nForceLines = atoi(_sArr[++i]);
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-te") && i < argc-4 )
        {
            dfMinX = CPLAtofM(_sArr[++i]);
            dfMinY = CPLAtofM(_sArr[++i]);
            dfMaxX = CPLAtofM(_sArr[++i]);
            dfMaxY = CPLAtofM(_sArr[++i]);
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-rn") )
            eResampleAlg = GRA_NearestNeighbour;

        else if( EQUAL(_sArr[i],"-rb") )
            eResampleAlg = GRA_Bilinear;

        else if( EQUAL(_sArr[i],"-rc") )
            eResampleAlg = GRA_Cubic;

        else if( EQUAL(_sArr[i],"-rcs") )
            eResampleAlg = GRA_CubicSpline;

        else if( EQUAL(_sArr[i],"-r") && i < argc - 1 )
        {
            if ( EQUAL(_sArr[++i], "near") )
                eResampleAlg = GRA_NearestNeighbour;
            else if ( EQUAL(_sArr[i], "bilinear") )
                eResampleAlg = GRA_Bilinear;
            else if ( EQUAL(_sArr[i], "cubic") )
                eResampleAlg = GRA_Cubic;
            else if ( EQUAL(_sArr[i], "cubicspline") )
                eResampleAlg = GRA_CubicSpline;
            else if ( EQUAL(_sArr[i], "lanczos") )
                eResampleAlg = GRA_Lanczos;
            else
            {
                CPLError(CE_Failure, CPLE_AppDefined, "Unknown resampling method: \"%s\".\n", _sArr[i] );
				return ResetConfigOptions(tkGDAL_ERROR);
            }
        }

        else if( EQUAL(_sArr[i],"-cutline") && i < argc-1 )
        {
            pszCutlineDSName = _sArr[++i].GetBuffer(0);
        }
        else if( EQUAL(_sArr[i],"-cwhere") && i < argc-1 )
        {
            pszCWHERE = _sArr[++i].GetBuffer(0);
        }
        else if( EQUAL(_sArr[i],"-cl") && i < argc-1 )
        {
            pszCLayer = _sArr[++i].GetBuffer(0);
        }
        else if( EQUAL(_sArr[i],"-csql") && i < argc-1 )
        {
            pszCSQL = _sArr[++i].GetBuffer(0);
        }
        else if( EQUAL(_sArr[i],"-cblend") && i < argc-1 )
        {
            papszWarpOptions = 
                CSLSetNameValue( papszWarpOptions, 
                                 "CUTLINE_BLEND_DIST", _sArr[++i] );
        }
        else if( EQUAL(_sArr[i],"-crop_to_cutline")  )
        {
            bCropToCutline = TRUE;
            bCreateOutput = TRUE;
        }
        else if( EQUAL(_sArr[i],"-overwrite") )
            bOverwrite = TRUE;

        else if( _sArr[i][0] == '-' )
		{
            // Skip it.
		}

        else 
            papszSrcFiles = CSLAddString( papszSrcFiles, _sArr[i] );
    }
/* -------------------------------------------------------------------- */
/*      Check that incompatible options are not used                    */
/* -------------------------------------------------------------------- */

    if ((nForcePixels != 0 || nForceLines != 0) && 
        (dfXRes != 0 && dfYRes != 0))
    {
        CPLError(CE_Failure, CPLE_AppDefined, "-tr and -ts options cannot be used at the same time\n");
		return ResetConfigOptions(tkGDAL_ERROR);
    }
    
    if (bTargetAlignedPixels && dfXRes == 0 && dfYRes == 0)
    {
        CPLError(CE_Failure, CPLE_AppDefined, "-tap option cannot be used without using -tr\n");
		return ResetConfigOptions(tkGDAL_ERROR);
    }

/* -------------------------------------------------------------------- */
/*      The last filename in the file list is really our destination    */
/*      file.                                                           */
/* -------------------------------------------------------------------- */
    if( CSLCount(papszSrcFiles) > 1 )
    {
        pszDstFilename = papszSrcFiles[CSLCount(papszSrcFiles)-1];
        papszSrcFiles[CSLCount(papszSrcFiles)-1] = NULL;
    }

    if( pszDstFilename == NULL )
	{
		CPLError(CE_Warning, CPLE_AppDefined, "GdalWarp: failed to find destination file name.");
		return ResetConfigOptions();
	}
        
    if( bVRT && CSLCount(papszSrcFiles) > 1 )
    {
        CPLError( CE_Warning, CPLE_AppDefined, "Warning: gdalwarp -of VRT just takes into account "
                        "the first source dataset.\nIf all source datasets "
                        "are in the same projection, try making a mosaic of\n"
                        "them with gdalbuildvrt, and use the resulting "
                        "VRT file as the input of\ngdalwarp -of VRT.\n");
    }

/* -------------------------------------------------------------------- */
/*      Does the output dataset already exist?                          */
/* -------------------------------------------------------------------- */

    /* FIXME ? source filename=target filename and -overwrite is definitely */
    /* an error. But I can't imagine of a valid case (without -overwrite), */
    /* where it would make sense. In doubt, let's keep that dubious possibility... */
    if ( CSLCount(papszSrcFiles) == 1 &&
         strcmp(papszSrcFiles[0], pszDstFilename) == 0 && bOverwrite)
    {
        CPLError( CE_Failure, CPLE_AppDefined, "Source and destination datasets must be different.\n");
		return ResetConfigOptions(tkGDAL_ERROR);
    }

    CPLPushErrorHandler( CPLQuietErrorHandler );
	hDstDS = GdalHelper::OpenRasterDatasetW(OLE2W(bstrDstFilename), GA_Update);
    CPLPopErrorHandler();

    if( hDstDS != NULL && bOverwrite )
    {
        GDALClose(hDstDS);
        hDstDS = NULL;
    }

    if( hDstDS != NULL && bCreateOutput )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                 "Output dataset %s exists,\n"
                 "but some commandline options were provided indicating a new dataset\n"
                 "should be created.  Please delete existing dataset and run again.\n",
                 pszDstFilename );
        // TODO: clean up memory?
		return ResetConfigOptions(tkGDAL_ERROR);
    }

    /* Avoid overwriting an existing destination file that cannot be opened in */
    /* update mode with a new GTiff file */
    if ( hDstDS == NULL && !bOverwrite )
    {
        CPLPushErrorHandler( CPLQuietErrorHandler );
        hDstDS = GdalHelper::OpenRasterDatasetW(OLE2W(bstrDstFilename), GA_ReadOnly);
        CPLPopErrorHandler();
        
        if (hDstDS)
        {
            CPLError( CE_Failure, CPLE_AppDefined, 
                     "Output dataset %s exists, but cannot be opened in update mode\n",
                     pszDstFilename );
            GDALClose(hDstDS);
            // TODO: clean up memory?
			return ResetConfigOptions(tkGDAL_ERROR);
        }
    }

/* -------------------------------------------------------------------- */
/*      If we have a cutline datasource read it and attach it in the    */
/*      warp options.                                                   */
/* -------------------------------------------------------------------- */
    if( pszCutlineDSName != NULL )
    {
        LoadCutline( pszCutlineDSName, pszCLayer, pszCWHERE, pszCSQL,
                     &hCutline );
    }

#ifdef OGR_ENABLED
    if ( bCropToCutline && hCutline != NULL )
    {
        OGRGeometryH hCutlineGeom = OGR_G_Clone( (OGRGeometryH) hCutline );
        OGRSpatialReferenceH hCutlineSRS = OGR_G_GetSpatialReference( hCutlineGeom );
        const char *pszThisTargetSRS = CSLFetchNameValue( papszTO, "DST_SRS" );
        OGRCoordinateTransformationH hCT = NULL;
        if (hCutlineSRS == NULL)
        {
            /* We suppose it is in target coordinates */
        }
        else if (pszThisTargetSRS != NULL)
        {
            OGRSpatialReferenceH hTargetSRS = OSRNewSpatialReference(NULL);
            if( OSRImportFromWkt( hTargetSRS, (char **)&pszThisTargetSRS ) != CE_None )
            {
                fprintf(stderr, "Cannot compute bounding box of cutline.\n");
                GDALExit(1);
            }

            hCT = OCTNewCoordinateTransformation(hCutlineSRS, hTargetSRS);

            OSRDestroySpatialReference(hTargetSRS);
        }
        else if (pszThisTargetSRS == NULL)
        {
            if (papszSrcFiles[0] != NULL)
            {
				GDALDatasetH hSrcDS = GDALOpen(papszSrcFiles[0], GA_ReadOnly);		// TODO: use Unicode
                if (hSrcDS == NULL)
                {
                    fprintf(stderr, "Cannot compute bounding box of cutline.\n");
                    GDALExit(1);
                }

                OGRSpatialReferenceH  hRasterSRS = NULL;
                const char *pszProjection = NULL;

                if( GDALGetProjectionRef( hSrcDS ) != NULL
                    && strlen(GDALGetProjectionRef( hSrcDS )) > 0 )
                    pszProjection = GDALGetProjectionRef( hSrcDS );
                else if( GDALGetGCPProjection( hSrcDS ) != NULL )
                    pszProjection = GDALGetGCPProjection( hSrcDS );

                if( pszProjection == NULL )
                {
                    fprintf(stderr, "Cannot compute bounding box of cutline.\n");
                    GDALExit(1);
                }

                hRasterSRS = OSRNewSpatialReference(NULL);
                if( OSRImportFromWkt( hRasterSRS, (char **)&pszProjection ) != CE_None )
                {
                    fprintf(stderr, "Cannot compute bounding box of cutline.\n");
                    GDALExit(1);
                }

                hCT = OCTNewCoordinateTransformation(hCutlineSRS, hRasterSRS);

                OSRDestroySpatialReference(hRasterSRS);

                GDALClose(hSrcDS);
            }
            else
            {
                fprintf(stderr, "Cannot compute bounding box of cutline.\n");
                GDALExit(1);
            }
        }

        if (hCT)
        {
            OGR_G_Transform( hCutlineGeom, hCT );

            OCTDestroyCoordinateTransformation(hCT);
        }

        OGREnvelope sEnvelope;
        OGR_G_GetEnvelope(hCutlineGeom, &sEnvelope);

        dfMinX = sEnvelope.MinX;
        dfMinY = sEnvelope.MinY;
        dfMaxX = sEnvelope.MaxX;
        dfMaxY = sEnvelope.MaxY;
        
        OGR_G_DestroyGeometry(hCutlineGeom);
    }
#endif
    
/* -------------------------------------------------------------------- */
/*      If not, we need to create it.                                   */
/* -------------------------------------------------------------------- */
    int   bInitDestSetForFirst = FALSE;

    void* hUniqueTransformArg = NULL;
    GDALDatasetH hUniqueSrcDS = NULL;

    if( hDstDS == NULL )
    {
        if (!bFormatExplicitelySet)
            CheckExtensionConsistency(pszDstFilename, pszFormat);

        hDstDS = GDALWarpCreateOutput( papszSrcFiles, pszDstFilename,pszFormat,
                                       papszTO, &papszCreateOptions, 
                                       eOutputType, &hUniqueTransformArg,
                                       &hUniqueSrcDS,
									   bVRT, FALSE, bTargetAlignedPixels,
									   dfXRes, dfYRes,
									   dfMinX, dfMaxX,
									   dfMinY, dfMaxY,
									   nForcePixels, nForceLines,
									   bEnableSrcAlpha, bEnableDstAlpha);
        bCreateOutput = TRUE;

        if( CSLFetchNameValue( papszWarpOptions, "INIT_DEST" ) == NULL 
            && pszDstNodata == NULL )
        {
            papszWarpOptions = CSLSetNameValue(papszWarpOptions,
                                               "INIT_DEST", "0");
            bInitDestSetForFirst = TRUE;
        }
        else if( CSLFetchNameValue( papszWarpOptions, "INIT_DEST" ) == NULL )
        {
            papszWarpOptions = CSLSetNameValue(papszWarpOptions,
                                               "INIT_DEST", "NO_DATA" );
            bInitDestSetForFirst = TRUE;
        }

        CSLDestroy( papszCreateOptions );
        papszCreateOptions = NULL;
    }
 
    if( hDstDS == NULL )
        // TODO: clean up memory...set error code?
		return ResetConfigOptions();

/* -------------------------------------------------------------------- */
/*      Loop over all source files, processing each in turn.            */
/* -------------------------------------------------------------------- */
    int iSrc;

    for( iSrc = 0; papszSrcFiles[iSrc] != NULL; iSrc++ )
    {
        GDALDatasetH hSrcDS;
       
/* -------------------------------------------------------------------- */
/*      Open this file.                                                 */
/* -------------------------------------------------------------------- */
        if (hUniqueSrcDS)
            hSrcDS = hUniqueSrcDS;
        else
            hSrcDS = GDALOpen( papszSrcFiles[iSrc], GA_ReadOnly );			// TODO: use Unicode
    
        if( hSrcDS == NULL )
            // TODO: clean up memory?
			return ResetConfigOptions();

/* -------------------------------------------------------------------- */
/*      Check that there's at least one raster band                     */
/* -------------------------------------------------------------------- */
        if ( GDALGetRasterCount(hSrcDS) == 0 )
        {
            CPLError( CE_Failure, CPLE_AppDefined, "Input file %s has no raster bands.\n", papszSrcFiles[iSrc] );
            // TODO: clean up memory?
			return ResetConfigOptions(tkGDAL_ERROR);
        }

/* -------------------------------------------------------------------- */
/*      Warns if the file has a color table and something more          */
/*      complicated than nearest neighbour resampling is asked          */
/* -------------------------------------------------------------------- */
 
        if ( eResampleAlg != GRA_NearestNeighbour &&
             GDALGetRasterColorTable(GDALGetRasterBand(hSrcDS, 1)) != NULL)
        {
            CPLError( CE_Warning, CPLE_AppDefined, "Warning: Input file %s has a color table, which will likely lead to "
                    "bad results when using a resampling method other than "
                    "nearest neighbour. Converting the dataset prior to 24/32 bit "
                    "is advised.\n", papszSrcFiles[iSrc] );
        }

/* -------------------------------------------------------------------- */
/*      Do we have a source alpha band?                                 */
/* -------------------------------------------------------------------- */
        if( GDALGetRasterColorInterpretation( 
                GDALGetRasterBand(hSrcDS,GDALGetRasterCount(hSrcDS)) ) 
            == GCI_AlphaBand 
            && !bEnableSrcAlpha )
        {
            bEnableSrcAlpha = TRUE;
        }

/* -------------------------------------------------------------------- */
/*      Create a transformation object from the source to               */
/*      destination coordinate system.                                  */
/* -------------------------------------------------------------------- */
        if (hUniqueTransformArg)
            hTransformArg = hGenImgProjArg = hUniqueTransformArg;
        else
            hTransformArg = hGenImgProjArg =
                GDALCreateGenImgProjTransformer2( hSrcDS, hDstDS, papszTO );
        
        if( hTransformArg == NULL )
            // TODO: clean up memory?
			return ResetConfigOptions();
        
        pfnTransformer = GDALGenImgProjTransform;

/* -------------------------------------------------------------------- */
/*      Warp the transformer with a linear approximator unless the      */
/*      acceptable error is zero.                                       */
/* -------------------------------------------------------------------- */
        if( dfErrorThreshold != 0.0 )
        {
            hTransformArg = hApproxArg = 
                GDALCreateApproxTransformer( GDALGenImgProjTransform, 
                                             hGenImgProjArg, dfErrorThreshold);
            pfnTransformer = GDALApproxTransform;
        }

/* -------------------------------------------------------------------- */
/*      Clear temporary INIT_DEST settings after the first image.       */
/* -------------------------------------------------------------------- */
        if( bInitDestSetForFirst && iSrc == 1 )
            papszWarpOptions = CSLSetNameValue( papszWarpOptions, 
                                                "INIT_DEST", NULL );

/* -------------------------------------------------------------------- */
/*      Setup warp options.                                             */
/* -------------------------------------------------------------------- */
        GDALWarpOptions *psWO = GDALCreateWarpOptions();

        psWO->papszWarpOptions = CSLDuplicate(papszWarpOptions);
        psWO->eWorkingDataType = eWorkingType;
        psWO->eResampleAlg = eResampleAlg;

        psWO->hSrcDS = hSrcDS;
        psWO->hDstDS = hDstDS;

        psWO->pfnTransformer = pfnTransformer;
        psWO->pTransformerArg = hTransformArg;
        psWO->pfnProgress = GDALProgressCallback;
		psWO->pProgressArg = &params;

        if( dfWarpMemoryLimit != 0.0 )
            psWO->dfWarpMemoryLimit = dfWarpMemoryLimit;

/* -------------------------------------------------------------------- */
/*      Setup band mapping.                                             */
/* -------------------------------------------------------------------- */
        if( bEnableSrcAlpha )
            psWO->nBandCount = GDALGetRasterCount(hSrcDS) - 1;
        else
            psWO->nBandCount = GDALGetRasterCount(hSrcDS);

        psWO->panSrcBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));
        psWO->panDstBands = (int *) CPLMalloc(psWO->nBandCount*sizeof(int));

        for( i = 0; i < psWO->nBandCount; i++ )
        {
            psWO->panSrcBands[i] = i+1;
            psWO->panDstBands[i] = i+1;
        }

/* -------------------------------------------------------------------- */
/*      Setup alpha bands used if any.                                  */
/* -------------------------------------------------------------------- */
        if( bEnableSrcAlpha )
            psWO->nSrcAlphaBand = GDALGetRasterCount(hSrcDS);

        if( !bEnableDstAlpha 
            && GDALGetRasterCount(hDstDS) == psWO->nBandCount+1 
            && GDALGetRasterColorInterpretation( 
                GDALGetRasterBand(hDstDS,GDALGetRasterCount(hDstDS))) 
            == GCI_AlphaBand )
        {
            bEnableDstAlpha = TRUE;
        }

        if( bEnableDstAlpha )
            psWO->nDstAlphaBand = GDALGetRasterCount(hDstDS);

/* -------------------------------------------------------------------- */
/*      Setup NODATA options.                                           */
/* -------------------------------------------------------------------- */
        if( pszSrcNodata != NULL && !EQUALN(pszSrcNodata,"n",1) )
        {
            char **papszTokens = CSLTokenizeString( pszSrcNodata );
            int  nTokenCount = CSLCount(papszTokens);

            psWO->padfSrcNoDataReal = (double *) 
                CPLMalloc(psWO->nBandCount*sizeof(double));
            psWO->padfSrcNoDataImag = (double *) 
                CPLMalloc(psWO->nBandCount*sizeof(double));

            for( i = 0; i < psWO->nBandCount; i++ )
            {
                if( i < nTokenCount )
                {
                    CPLStringToComplex( papszTokens[i], 
                                        psWO->padfSrcNoDataReal + i,
                                        psWO->padfSrcNoDataImag + i );
                }
                else
                {
                    psWO->padfSrcNoDataReal[i] = psWO->padfSrcNoDataReal[i-1];
                    psWO->padfSrcNoDataImag[i] = psWO->padfSrcNoDataImag[i-1];
                }
            }

            CSLDestroy( papszTokens );

            psWO->papszWarpOptions = CSLSetNameValue(psWO->papszWarpOptions,
                                               "UNIFIED_SRC_NODATA", "YES" );
        }

/* -------------------------------------------------------------------- */
/*      If -srcnodata was not specified, but the data has nodata        */
/*      values, use them.                                               */
/* -------------------------------------------------------------------- */
        if( pszSrcNodata == NULL )
        {
            int bHaveNodata = FALSE;
            double dfReal = 0.0;

            for( i = 0; !bHaveNodata && i < psWO->nBandCount; i++ )
            {
                GDALRasterBandH hBand = GDALGetRasterBand( hSrcDS, i+1 );
                dfReal = GDALGetRasterNoDataValue( hBand, &bHaveNodata );
            }

            if( bHaveNodata )
            {
                psWO->padfSrcNoDataReal = (double *) 
                    CPLMalloc(psWO->nBandCount*sizeof(double));
                psWO->padfSrcNoDataImag = (double *) 
                    CPLMalloc(psWO->nBandCount*sizeof(double));
                
                for( i = 0; i < psWO->nBandCount; i++ )
                {
                    GDALRasterBandH hBand = GDALGetRasterBand( hSrcDS, i+1 );

                    dfReal = GDALGetRasterNoDataValue( hBand, &bHaveNodata );

                    if( bHaveNodata )
                    {
                        psWO->padfSrcNoDataReal[i] = dfReal;
                        psWO->padfSrcNoDataImag[i] = 0.0;
                    }
                    else
                    {
                        psWO->padfSrcNoDataReal[i] = -123456.789;
                        psWO->padfSrcNoDataImag[i] = 0.0;
                    }
                }
            }
        }

/* -------------------------------------------------------------------- */
/*      If the output dataset was created, and we have a destination    */
/*      nodata value, go through marking the bands with the information.*/
/* -------------------------------------------------------------------- */
        if( pszDstNodata != NULL )
        {
            char **papszTokens = CSLTokenizeString( pszDstNodata );
            int  nTokenCount = CSLCount(papszTokens);

            psWO->padfDstNoDataReal = (double *) 
                CPLMalloc(psWO->nBandCount*sizeof(double));
            psWO->padfDstNoDataImag = (double *) 
                CPLMalloc(psWO->nBandCount*sizeof(double));

            for( i = 0; i < psWO->nBandCount; i++ )
            {
                if( i < nTokenCount )
                {
                    CPLStringToComplex( papszTokens[i], 
                                        psWO->padfDstNoDataReal + i,
                                        psWO->padfDstNoDataImag + i );
                }
                else
                {
                    psWO->padfDstNoDataReal[i] = psWO->padfDstNoDataReal[i-1];
                    psWO->padfDstNoDataImag[i] = psWO->padfDstNoDataImag[i-1];
                }
                
                GDALRasterBandH hBand = GDALGetRasterBand( hDstDS, i+1 );
                int bClamped = FALSE, bRounded = FALSE;

#define CLAMP(val,type,minval,maxval) \
    do { if (val < minval) { bClamped = TRUE; val = minval; } \
    else if (val > maxval) { bClamped = TRUE; val = maxval; } \
    else if (val != (type)val) { bRounded = TRUE; val = (type)(val + 0.5); } } \
    while(0)

                switch(GDALGetRasterDataType(hBand))
                {
                    case GDT_Byte:
                        CLAMP(psWO->padfDstNoDataReal[i], GByte,
                              0.0, 255.0);
                        break;
                    case GDT_Int16:
                        CLAMP(psWO->padfDstNoDataReal[i], GInt16,
                              -32768.0, 32767.0);
                        break;
                    case GDT_UInt16:
                        CLAMP(psWO->padfDstNoDataReal[i], GUInt16,
                              0.0, 65535.0);
                        break;
                    case GDT_Int32:
                        CLAMP(psWO->padfDstNoDataReal[i], GInt32,
                              -2147483648.0, 2147483647.0);
                        break;
                    case GDT_UInt32:
                        CLAMP(psWO->padfDstNoDataReal[i], GUInt32,
                              0.0, 4294967295.0);
                        break;
                    default:
                        break;
                }
                    
                if (bClamped)
                {
					CPLError(CE_Warning, CPLE_AppDefined, "for band %d, destination nodata value has been clamped "
                           "to %.0f, the original value being out of range.\n",
                           i + 1, psWO->padfDstNoDataReal[i]);
                }
                else if(bRounded)
                {
					CPLError(CE_Warning, CPLE_AppDefined, "for band %d, destination nodata value has been rounded "
                           "to %.0f, %s being an integer datatype.\n",
                           i + 1, psWO->padfDstNoDataReal[i],
                           GDALGetDataTypeName(GDALGetRasterDataType(hBand)));
                }

                if( bCreateOutput )
                {
                    GDALSetRasterNoDataValue( 
                        GDALGetRasterBand( hDstDS, psWO->panDstBands[i] ), 
                        psWO->padfDstNoDataReal[i] );
                }
            }

            CSLDestroy( papszTokens );
        }

/* -------------------------------------------------------------------- */
/*      If we have a cutline, transform it into the source              */
/*      pixel/line coordinate system and insert into warp options.      */
/* -------------------------------------------------------------------- */
        if( hCutline != NULL )
        {
            TransformCutlineToSource( hSrcDS, hCutline, 
                                      &(psWO->papszWarpOptions), 
                                      papszTO );
        }

/* -------------------------------------------------------------------- */
/*      If we are producing VRT output, then just initialize it with    */
/*      the warp options and write out now rather than proceeding       */
/*      with the operations.                                            */
/* -------------------------------------------------------------------- */
        if( bVRT )
        {
            if( GDALInitializeWarpedVRT( hDstDS, psWO ) != CE_None )
                // TODO: clean up memory...set error code?
				return ResetConfigOptions();

            GDALClose( hDstDS );
            GDALClose( hSrcDS );

            /* The warped VRT will clean itself the transformer used */
            /* So we have only to destroy the hGenImgProjArg if we */
            /* have wrapped it inside the hApproxArg */
            if (pfnTransformer == GDALApproxTransform)
            {
                if( hGenImgProjArg != NULL )
                    GDALDestroyGenImgProjTransformer( hGenImgProjArg );
            }

            GDALDestroyWarpOptions( psWO );

            CPLFree( pszDstFilename );
            CSLDestroy( papszSrcFiles );
            CSLDestroy( papszWarpOptions );
            CSLDestroy( papszTO );
    
            GDALDumpOpenDatasets( stderr );
        
            return 0;
        }

/* -------------------------------------------------------------------- */
/*      Initialize and execute the warp.                                */
/* -------------------------------------------------------------------- */
        GDALWarpOperation oWO;

        if( oWO.Initialize( psWO ) == CE_None )
        {
            CPLErr eErr;
            if( bMulti )
                eErr = oWO.ChunkAndWarpMulti( 0, 0, 
                                       GDALGetRasterXSize( hDstDS ),
                                       GDALGetRasterYSize( hDstDS ) );
            else
                eErr = oWO.ChunkAndWarpImage( 0, 0, 
                                       GDALGetRasterXSize( hDstDS ),
                                       GDALGetRasterYSize( hDstDS ) );
            if (eErr != CE_None)
                bHasGotErr = TRUE;
        }

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
        if( hApproxArg != NULL )
            GDALDestroyApproxTransformer( hApproxArg );
        
        if( hGenImgProjArg != NULL )
            GDALDestroyGenImgProjTransformer( hGenImgProjArg );
        
        GDALDestroyWarpOptions( psWO );

        GDALClose( hSrcDS );
    }

/* -------------------------------------------------------------------- */
/*      Final Cleanup.                                                  */
/* -------------------------------------------------------------------- */
    CPLErrorReset();
    GDALFlushCache( hDstDS );
    if( CPLGetLastErrorType() != CE_None )
        bHasGotErr = TRUE;
    GDALClose( hDstDS );
    
    CPLFree( pszDstFilename );
    CSLDestroy( papszSrcFiles );
    CSLDestroy( papszWarpOptions );
    CSLDestroy( papszTO );

    GDALDumpOpenDatasets( stderr );

#ifdef OGR_ENABLED
    if( hCutline != NULL )
        OGR_G_DestroyGeometry( (OGRGeometryH) hCutline );
#endif

	(*retval) = (bHasGotErr) ? VARIANT_FALSE : VARIANT_TRUE;

    return ResetConfigOptions();
}

/************************************************************************/
/*                        GDALWarpCreateOutput()                        */
/*                                                                      */
/*      Create the output file based on various commandline options,    */
/*      and the input file.                                             */
/*      If there's just one source file, then *phTransformArg and       */
/*      *phSrcDS will be set, in order them to be reused by main        */
/*      function. This saves dataset re-opening, and above all transform*/
/*      recomputation, which can be expensive in the -tps case          */
/************************************************************************/

static GDALDatasetH 
GDALWarpCreateOutput( char **papszSrcFiles, const char *pszFilename, 
                      const char *pszFormat, char **papszTO, 
                      char ***ppapszCreateOptions, GDALDataType eDT,
                      void ** phTransformArg,
                      GDALDatasetH* phSrcDS,
					  int bVRT, int bQuiet, int bTargetAlignedPixels,
					  double dfXRes, double dfYRes,
					  double dfMinX, double dfMaxX,
					  double dfMinY, double dfMaxY,
					  int nForcePixels, int nForceLines,
					  int bEnableSrcAlpha, int bEnableDstAlpha )
{
    GDALDriverH hDriver;
    GDALDatasetH hDstDS;
    void *hTransformArg;
    GDALColorTableH hCT = NULL;
    double dfWrkMinX=0, dfWrkMaxX=0, dfWrkMinY=0, dfWrkMaxY=0;
    double dfWrkResX=0, dfWrkResY=0;
    int nDstBandCount = 0;
    std::vector<GDALColorInterp> apeColorInterpretations;

    *phTransformArg = NULL;
    *phSrcDS = NULL;

/* -------------------------------------------------------------------- */
/*      Find the output driver.                                         */
/* -------------------------------------------------------------------- */
    hDriver = GDALGetDriverByName( pszFormat );
    if( hDriver == NULL 
        || GDALGetMetadataItem( hDriver, GDAL_DCAP_CREATE, NULL ) == NULL )
    {
		CPLError(CE_Failure, CPLE_AppDefined, "Output driver `%s' not recognised or does not support direct output file creation.", pszFormat);
        
        // TODO: free up memory...set error code?
		return NULL;
    }

/* -------------------------------------------------------------------- */
/*      For virtual output files, we have to set a special subclass     */
/*      of dataset to create.                                           */
/* -------------------------------------------------------------------- */
    if( bVRT )
        *ppapszCreateOptions = 
            CSLSetNameValue( *ppapszCreateOptions, "SUBCLASS", 
                             "VRTWarpedDataset" );

/* -------------------------------------------------------------------- */
/*      Loop over all input files to collect extents.                   */
/* -------------------------------------------------------------------- */
    int     iSrc;
    char    *pszThisTargetSRS = (char*)CSLFetchNameValue( papszTO, "DST_SRS" );
    if( pszThisTargetSRS != NULL )
        pszThisTargetSRS = CPLStrdup( pszThisTargetSRS );

    for( iSrc = 0; papszSrcFiles[iSrc] != NULL; iSrc++ )
    {
        GDALDatasetH hSrcDS;
        const char *pszThisSourceSRS = CSLFetchNameValue(papszTO,"SRC_SRS");

        hSrcDS = GDALOpen( papszSrcFiles[iSrc], GA_ReadOnly );			// TODO: use Unicode
        if( hSrcDS == NULL )
			// TODO: free up memory...set error code?
			return NULL;

/* -------------------------------------------------------------------- */
/*      Check that there's at least one raster band                     */
/* -------------------------------------------------------------------- */
        if ( GDALGetRasterCount(hSrcDS) == 0 )
        {
			CPLError(CE_Failure, CPLE_AppDefined, "Input file %s has no raster bands.\n", papszSrcFiles[iSrc]);
            // TODO: free up memory...set error code?
			return NULL;
        }

        if( eDT == GDT_Unknown )
            eDT = GDALGetRasterDataType(GDALGetRasterBand(hSrcDS,1));

/* -------------------------------------------------------------------- */
/*      If we are processing the first file, and it has a color         */
/*      table, then we will copy it to the destination file.            */
/* -------------------------------------------------------------------- */
        if( iSrc == 0 )
        {
            nDstBandCount = GDALGetRasterCount(hSrcDS);
            hCT = GDALGetRasterColorTable( GDALGetRasterBand(hSrcDS,1) );
            if( hCT != NULL )
            {
                hCT = GDALCloneColorTable( hCT );
                if( !bQuiet )
                    printf( "Copying color table from %s to new file.\n", 
                            papszSrcFiles[iSrc] );
            }

            for(int iBand = 0; iBand < nDstBandCount; iBand++)
            {
                apeColorInterpretations.push_back(
                    GDALGetRasterColorInterpretation(GDALGetRasterBand(hSrcDS,iBand+1)) );
            }
        }

/* -------------------------------------------------------------------- */
/*      Get the sourcesrs from the dataset, if not set already.         */
/* -------------------------------------------------------------------- */
        if( pszThisSourceSRS == NULL )
        {
            const char *pszMethod = CSLFetchNameValue( papszTO, "METHOD" );

            if( GDALGetProjectionRef( hSrcDS ) != NULL 
                && strlen(GDALGetProjectionRef( hSrcDS )) > 0
                && (pszMethod == NULL || EQUAL(pszMethod,"GEOTRANSFORM")) )
                pszThisSourceSRS = GDALGetProjectionRef( hSrcDS );
            
            else if( GDALGetGCPProjection( hSrcDS ) != NULL
                     && strlen(GDALGetGCPProjection(hSrcDS)) > 0 
                     && GDALGetGCPCount( hSrcDS ) > 1 
                     && (pszMethod == NULL || EQUALN(pszMethod,"GCP_",4)) )
                pszThisSourceSRS = GDALGetGCPProjection( hSrcDS );
            else if( pszMethod != NULL && EQUAL(pszMethod,"RPC") )
#if GDAL_VERSION_MAJOR >= 3
                pszThisSourceSRS = SRS_WKT_WGS84_LAT_LONG; //  SRS_WKT_WGS84 macro is no longer declared by default since WKT without AXIS is too ambiguous. Preferred remediation: use SRS_WKT_WGS84_LAT_LONG
#else
				pszThisSourceSRS = SRS_WKT_WGS84;
#endif
            else
                pszThisSourceSRS = "";
        }

        if( pszThisTargetSRS == NULL )
            pszThisTargetSRS = CPLStrdup( pszThisSourceSRS );
        
/* -------------------------------------------------------------------- */
/*      Create a transformation object from the source to               */
/*      destination coordinate system.                                  */
/* -------------------------------------------------------------------- */
        hTransformArg = 
            GDALCreateGenImgProjTransformer2( hSrcDS, NULL, papszTO );
        
        if( hTransformArg == NULL )
        {
            CPLFree( pszThisTargetSRS );
            GDALClose( hSrcDS );
            return NULL;
        }
        
        GDALTransformerInfo* psInfo = (GDALTransformerInfo*)hTransformArg;

/* -------------------------------------------------------------------- */
/*      Get approximate output definition.                              */
/* -------------------------------------------------------------------- */
        double adfThisGeoTransform[6];
        double adfExtent[4];
        int    nThisPixels, nThisLines;

        if( GDALSuggestedWarpOutput2( hSrcDS, 
                                      psInfo->pfnTransform, hTransformArg, 
                                      adfThisGeoTransform, 
                                      &nThisPixels, &nThisLines, 
                                      adfExtent, 0 ) != CE_None )
        {
            CPLFree( pszThisTargetSRS );
            GDALClose( hSrcDS );
            return NULL;
        }
        
        if (CPLGetConfigOption( "CHECK_WITH_INVERT_PROJ", NULL ) == NULL)
        {
            double MinX = adfExtent[0];
            double MaxX = adfExtent[2];
            double MaxY = adfExtent[3];
            double MinY = adfExtent[1];
            int bSuccess = TRUE;
            
            /* Check that the the edges of the target image are in the validity area */
            /* of the target projection */
#define N_STEPS 20
            int i,j;
            for(i=0;i<=N_STEPS && bSuccess;i++)
            {
                for(j=0;j<=N_STEPS && bSuccess;j++)
                {
                    double dfRatioI = i * 1.0 / N_STEPS;
                    double dfRatioJ = j * 1.0 / N_STEPS;
                    double expected_x = (1 - dfRatioI) * MinX + dfRatioI * MaxX;
                    double expected_y = (1 - dfRatioJ) * MinY + dfRatioJ * MaxY;
                    double x = expected_x;
                    double y = expected_y;
                    double z = 0;
                    /* Target SRS coordinates to source image pixel coordinates */
                    if (!psInfo->pfnTransform(hTransformArg, TRUE, 1, &x, &y, &z, &bSuccess) || !bSuccess)
                        bSuccess = FALSE;
                    /* Source image pixel coordinates to target SRS coordinates */
                    if (!psInfo->pfnTransform(hTransformArg, FALSE, 1, &x, &y, &z, &bSuccess) || !bSuccess)
                        bSuccess = FALSE;
                    if (fabs(x - expected_x) > (MaxX - MinX) / nThisPixels ||
                        fabs(y - expected_y) > (MaxY - MinY) / nThisLines)
                        bSuccess = FALSE;
                }
            }
            
            /* If not, retry with CHECK_WITH_INVERT_PROJ=TRUE that forces ogrct.cpp */
            /* to check the consistency of each requested projection result with the */
            /* invert projection */
            if (!bSuccess)
            {
                CPLSetConfigOption( "CHECK_WITH_INVERT_PROJ", "TRUE" );
                CPLDebug("WARP", "Recompute out extent with CHECK_WITH_INVERT_PROJ=TRUE");

                if( GDALSuggestedWarpOutput2( hSrcDS, 
                                      psInfo->pfnTransform, hTransformArg, 
                                      adfThisGeoTransform, 
                                      &nThisPixels, &nThisLines, 
                                      adfExtent, 0 ) != CE_None )
                {
                    CPLFree( pszThisTargetSRS );
                    GDALClose( hSrcDS );
                    return NULL;
                }
            }
        }

/* -------------------------------------------------------------------- */
/*      Expand the working bounds to include this region, ensure the    */
/*      working resolution is no more than this resolution.             */
/* -------------------------------------------------------------------- */
        if( dfWrkMaxX == 0.0 && dfWrkMinX == 0.0 )
        {
            dfWrkMinX = adfExtent[0];
            dfWrkMaxX = adfExtent[2];
            dfWrkMaxY = adfExtent[3];
            dfWrkMinY = adfExtent[1];
            dfWrkResX = adfThisGeoTransform[1];
            dfWrkResY = ABS(adfThisGeoTransform[5]);
        }
        else
        {
            dfWrkMinX = MIN(dfWrkMinX,adfExtent[0]);
            dfWrkMaxX = MAX(dfWrkMaxX,adfExtent[2]);
            dfWrkMaxY = MAX(dfWrkMaxY,adfExtent[3]);
            dfWrkMinY = MIN(dfWrkMinY,adfExtent[1]);
            dfWrkResX = MIN(dfWrkResX,adfThisGeoTransform[1]);
            dfWrkResY = MIN(dfWrkResY,ABS(adfThisGeoTransform[5]));
        }

        if (iSrc == 0 && papszSrcFiles[1] == NULL)
        {
            *phTransformArg = hTransformArg;
            *phSrcDS = hSrcDS;
        }
        else
        {
            GDALDestroyGenImgProjTransformer( hTransformArg );
            GDALClose( hSrcDS );
        }
    }

/* -------------------------------------------------------------------- */
/*      Did we have any usable sources?                                 */
/* -------------------------------------------------------------------- */
    if( nDstBandCount == 0 )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
                  "No usable source images." );
        CPLFree( pszThisTargetSRS );
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Turn the suggested region into a geotransform and suggested     */
/*      number of pixels and lines.                                     */
/* -------------------------------------------------------------------- */
    double adfDstGeoTransform[6];
    int nPixels, nLines;

    adfDstGeoTransform[0] = dfWrkMinX;
    adfDstGeoTransform[1] = dfWrkResX;
    adfDstGeoTransform[2] = 0.0;
    adfDstGeoTransform[3] = dfWrkMaxY;
    adfDstGeoTransform[4] = 0.0;
    adfDstGeoTransform[5] = -1 * dfWrkResY;

    nPixels = (int) ((dfWrkMaxX - dfWrkMinX) / dfWrkResX + 0.5);
    nLines = (int) ((dfWrkMaxY - dfWrkMinY) / dfWrkResY + 0.5);

/* -------------------------------------------------------------------- */
/*      Did the user override some parameters?                          */
/* -------------------------------------------------------------------- */
    if( dfXRes != 0.0 && dfYRes != 0.0 )
    {
        if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
        {
            dfMinX = adfDstGeoTransform[0];
            dfMaxX = adfDstGeoTransform[0] + adfDstGeoTransform[1] * nPixels;
            dfMaxY = adfDstGeoTransform[3];
            dfMinY = adfDstGeoTransform[3] + adfDstGeoTransform[5] * nLines;
        }
        
        if ( bTargetAlignedPixels )
        {
            dfMinX = floor(dfMinX / dfXRes) * dfXRes;
            dfMaxX = ceil(dfMaxX / dfXRes) * dfXRes;
            dfMinY = floor(dfMinY / dfYRes) * dfYRes;
            dfMaxY = ceil(dfMaxY / dfYRes) * dfYRes;
        }

        nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
        nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);
        adfDstGeoTransform[0] = dfMinX;
        adfDstGeoTransform[3] = dfMaxY;
        adfDstGeoTransform[1] = dfXRes;
        adfDstGeoTransform[5] = -dfYRes;
    }

    else if( nForcePixels != 0 && nForceLines != 0 )
    {
        if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
        {
            dfMinX = dfWrkMinX;
            dfMaxX = dfWrkMaxX;
            dfMaxY = dfWrkMaxY;
            dfMinY = dfWrkMinY;
        }

        dfXRes = (dfMaxX - dfMinX) / nForcePixels;
        dfYRes = (dfMaxY - dfMinY) / nForceLines;

        adfDstGeoTransform[0] = dfMinX;
        adfDstGeoTransform[3] = dfMaxY;
        adfDstGeoTransform[1] = dfXRes;
        adfDstGeoTransform[5] = -dfYRes;

        nPixels = nForcePixels;
        nLines = nForceLines;
    }

    else if( nForcePixels != 0 )
    {
        if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
        {
            dfMinX = dfWrkMinX;
            dfMaxX = dfWrkMaxX;
            dfMaxY = dfWrkMaxY;
            dfMinY = dfWrkMinY;
        }

        dfXRes = (dfMaxX - dfMinX) / nForcePixels;
        dfYRes = dfXRes;

        adfDstGeoTransform[0] = dfMinX;
        adfDstGeoTransform[3] = dfMaxY;
        adfDstGeoTransform[1] = dfXRes;
        adfDstGeoTransform[5] = -dfYRes;

        nPixels = nForcePixels;
        nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);
    }

    else if( nForceLines != 0 )
    {
        if( dfMinX == 0.0 && dfMinY == 0.0 && dfMaxX == 0.0 && dfMaxY == 0.0 )
        {
            dfMinX = dfWrkMinX;
            dfMaxX = dfWrkMaxX;
            dfMaxY = dfWrkMaxY;
            dfMinY = dfWrkMinY;
        }

        dfYRes = (dfMaxY - dfMinY) / nForceLines;
        dfXRes = dfYRes;

        adfDstGeoTransform[0] = dfMinX;
        adfDstGeoTransform[3] = dfMaxY;
        adfDstGeoTransform[1] = dfXRes;
        adfDstGeoTransform[5] = -dfYRes;

        nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
        nLines = nForceLines;
    }

    else if( dfMinX != 0.0 || dfMinY != 0.0 || dfMaxX != 0.0 || dfMaxY != 0.0 )
    {
        dfXRes = adfDstGeoTransform[1];
        dfYRes = fabs(adfDstGeoTransform[5]);

        nPixels = (int) ((dfMaxX - dfMinX + (dfXRes/2.0)) / dfXRes);
        nLines = (int) ((dfMaxY - dfMinY + (dfYRes/2.0)) / dfYRes);

        dfXRes = (dfMaxX - dfMinX) / nPixels;
        dfYRes = (dfMaxY - dfMinY) / nLines;

        adfDstGeoTransform[0] = dfMinX;
        adfDstGeoTransform[3] = dfMaxY;
        adfDstGeoTransform[1] = dfXRes;
        adfDstGeoTransform[5] = -dfYRes;
    }

/* -------------------------------------------------------------------- */
/*      Do we want to generate an alpha band in the output file?        */
/* -------------------------------------------------------------------- */
    if( bEnableSrcAlpha )
        nDstBandCount--;

    if( bEnableDstAlpha )
        nDstBandCount++;

/* -------------------------------------------------------------------- */
/*      Create the output file.                                         */
/* -------------------------------------------------------------------- */
    if( !bQuiet )
        printf( "Creating output file that is %dP x %dL.\n", nPixels, nLines );

    hDstDS = GDALCreate( hDriver, pszFilename, nPixels, nLines, 
                         nDstBandCount, eDT, *ppapszCreateOptions );
    
    if( hDstDS == NULL )
    {
        CPLFree( pszThisTargetSRS );
        return NULL;
    }

/* -------------------------------------------------------------------- */
/*      Write out the projection definition.                            */
/* -------------------------------------------------------------------- */
    GDALSetProjection( hDstDS, pszThisTargetSRS );
    GDALSetGeoTransform( hDstDS, adfDstGeoTransform );

    if (*phTransformArg != NULL)
        GDALSetGenImgProjTransformerDstGeoTransform( *phTransformArg, adfDstGeoTransform);

/* -------------------------------------------------------------------- */
/*      Try to set color interpretation of source bands to target       */
/*      dataset.                                                        */
/*      FIXME? We should likely do that for other drivers than VRT      */
/*      but it might create spurious .aux.xml files (at least with HFA, */
/*      and netCDF)                                                     */
/* -------------------------------------------------------------------- */
    if( bVRT )
    {
        int nBandsToCopy = (int)apeColorInterpretations.size();
        if ( bEnableSrcAlpha )
            nBandsToCopy --;
        for(int iBand = 0; iBand < nBandsToCopy; iBand++)
        {
            GDALSetRasterColorInterpretation(
                GDALGetRasterBand( hDstDS, iBand + 1 ),
                apeColorInterpretations[iBand] );
        }
    }
    
/* -------------------------------------------------------------------- */
/*      Try to set color interpretation of output file alpha band.      */
/* -------------------------------------------------------------------- */
    if( bEnableDstAlpha )
    {
        GDALSetRasterColorInterpretation( 
            GDALGetRasterBand( hDstDS, nDstBandCount ), 
            GCI_AlphaBand );
    }

/* -------------------------------------------------------------------- */
/*      Copy the color table, if required.                              */
/* -------------------------------------------------------------------- */
    if( hCT != NULL )
    {
        GDALSetRasterColorTable( GDALGetRasterBand(hDstDS,1), hCT );
        GDALDestroyColorTable( hCT );
    }

    CPLFree( pszThisTargetSRS );
    return hDstDS;
}

/************************************************************************/
/*                      GeoTransform_Transformer()                      */
/*                                                                      */
/*      Convert points from georef coordinates to pixel/line based      */
/*      on a geotransform.                                              */
/************************************************************************/

class CutlineTransformer : public OGRCoordinateTransformation
{
public:

    void         *hSrcImageTransformer;

    virtual OGRSpatialReference *GetSourceCS() { return NULL; }
    virtual OGRSpatialReference *GetTargetCS() { return NULL; }

    virtual int Transform( int nCount, 
                           double *x, double *y, double *z = NULL ) {
        int nResult;

        int *pabSuccess = (int *) CPLCalloc(sizeof(int),nCount);
        nResult = TransformEx( nCount, x, y, z, pabSuccess );
        CPLFree( pabSuccess );

        return nResult;
    }

    virtual int TransformEx( int nCount, 
                             double *x, double *y, double *z = NULL,
                             int *pabSuccess = NULL ) {
        return GDALGenImgProjTransform( hSrcImageTransformer, TRUE, 
                                        nCount, x, y, z, pabSuccess );
    }
};


/************************************************************************/
/*                            LoadCutline()                             */
/*                                                                      */
/*      Load blend cutline from OGR datasource.                         */
/************************************************************************/

static void
LoadCutline( const char *pszCutlineDSName, const char *pszCLayer, 
             const char *pszCWHERE, const char *pszCSQL, 
             void **phCutlineRet )

{
#ifndef OGR_ENABLED
	CPLError( CE_Failure, CPLE_AppDefined, 
              "Request to load a cutline failed, this build does not support OGR features.\n" );
	return;
#else // def OGR_ENABLED
    OGRRegisterAll();

/* -------------------------------------------------------------------- */
/*      Open source vector dataset.                                     */
/* -------------------------------------------------------------------- */
    OGRDataSourceH hSrcDS;

    hSrcDS = OGROpen( pszCutlineDSName, FALSE, NULL );
    if( hSrcDS == NULL )
        GDALExit( 1 );

/* -------------------------------------------------------------------- */
/*      Get the source layer                                            */
/* -------------------------------------------------------------------- */
    OGRLayerH hLayer = NULL;

    if( pszCSQL != NULL )
        hLayer = OGR_DS_ExecuteSQL( hSrcDS, pszCSQL, NULL, NULL ); 
    else if( pszCLayer != NULL )
        hLayer = OGR_DS_GetLayerByName( hSrcDS, pszCLayer );
    else
        hLayer = OGR_DS_GetLayer( hSrcDS, 0 );

    if( hLayer == NULL )
    {
        fprintf( stderr, "Failed to identify source layer from datasource.\n" );
        GDALExit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Apply WHERE clause if there is one.                             */
/* -------------------------------------------------------------------- */
    if( pszCWHERE != NULL )
        OGR_L_SetAttributeFilter( hLayer, pszCWHERE );

/* -------------------------------------------------------------------- */
/*      Collect the geometries from this layer, and build list of       */
/*      burn values.                                                    */
/* -------------------------------------------------------------------- */
    OGRFeatureH hFeat;
    OGRGeometryH hMultiPolygon = OGR_G_CreateGeometry( wkbMultiPolygon );

    OGR_L_ResetReading( hLayer );
    
    while( (hFeat = OGR_L_GetNextFeature( hLayer )) != NULL )
    {
        OGRGeometryH hGeom = OGR_F_GetGeometryRef(hFeat);

        if( hGeom == NULL )
        {
            fprintf( stderr, "ERROR: Cutline feature without a geometry.\n" );
            GDALExit( 1 );
        }
        
        OGRwkbGeometryType eType = wkbFlatten(OGR_G_GetGeometryType( hGeom ));

        if( eType == wkbPolygon )
            OGR_G_AddGeometry( hMultiPolygon, hGeom );
        else if( eType == wkbMultiPolygon )
        {
            int iGeom;

            for( iGeom = 0; iGeom < OGR_G_GetGeometryCount( hGeom ); iGeom++ )
            {
                OGR_G_AddGeometry( hMultiPolygon, 
                                   OGR_G_GetGeometryRef(hGeom,iGeom) );
            }
        }
        else
        {
            fprintf( stderr, "ERROR: Cutline not of polygon type.\n" );
            GDALExit( 1 );
        }

        OGR_F_Destroy( hFeat );
    }

    if( OGR_G_GetGeometryCount( hMultiPolygon ) == 0 )
    {
        fprintf( stderr, "ERROR: Did not get any cutline features.\n" );
        GDALExit( 1 );
    }

/* -------------------------------------------------------------------- */
/*      Ensure the coordinate system gets set on the geometry.          */
/* -------------------------------------------------------------------- */
    OGR_G_AssignSpatialReference(
        hMultiPolygon, OGR_L_GetSpatialRef(hLayer) );

    *phCutlineRet = (void *) hMultiPolygon;

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
    if( pszCSQL != NULL )
        OGR_DS_ReleaseResultSet( hSrcDS, hLayer );

    OGR_DS_Destroy( hSrcDS );
#endif
}

/************************************************************************/
/*                      TransformCutlineToSource()                      */
/*                                                                      */
/*      Transform cutline from its SRS to source pixel/line coordinates.*/
/************************************************************************/
static void
TransformCutlineToSource( GDALDatasetH hSrcDS, void *hCutline,
                          char ***ppapszWarpOptions, char **papszTO_In )

{
#ifdef OGR_ENABLED
    OGRGeometryH hMultiPolygon = OGR_G_Clone( (OGRGeometryH) hCutline );
    char **papszTO = CSLDuplicate( papszTO_In );

/* -------------------------------------------------------------------- */
/*      Checkout that SRS are the same.                                 */
/* -------------------------------------------------------------------- */
    OGRSpatialReferenceH  hRasterSRS = NULL;
    const char *pszProjection = NULL;

    if( GDALGetProjectionRef( hSrcDS ) != NULL 
        && strlen(GDALGetProjectionRef( hSrcDS )) > 0 )
        pszProjection = GDALGetProjectionRef( hSrcDS );
    else if( GDALGetGCPProjection( hSrcDS ) != NULL )
        pszProjection = GDALGetGCPProjection( hSrcDS );

    if( pszProjection != NULL )
    {
        hRasterSRS = OSRNewSpatialReference(NULL);
        if( OSRImportFromWkt( hRasterSRS, (char **)&pszProjection ) != CE_None )
        {
            OSRDestroySpatialReference(hRasterSRS);
            hRasterSRS = NULL;
        }
    }

    OGRSpatialReferenceH hCutlineSRS = OGR_G_GetSpatialReference( hMultiPolygon );
    if( hRasterSRS != NULL && hCutlineSRS != NULL )
    {
        /* ok, we will reproject */
    }
    else if( hRasterSRS != NULL && hCutlineSRS == NULL )
    {
        fprintf(stderr,
                "Warning : the source raster dataset has a SRS, but the cutline features\n"
                "not.  We assume that the cutline coordinates are expressed in the destination SRS.\n"
                "If not, cutline results may be incorrect.\n");
    }
    else if( hRasterSRS == NULL && hCutlineSRS != NULL )
    {
        fprintf(stderr,
                "Warning : the input vector layer has a SRS, but the source raster dataset does not.\n"
                "Cutline results may be incorrect.\n");
    }

    if( hRasterSRS != NULL )
        OSRDestroySpatialReference(hRasterSRS);

/* -------------------------------------------------------------------- */
/*      Extract the cutline SRS WKT.                                    */
/* -------------------------------------------------------------------- */
    if( hCutlineSRS != NULL )
    {
        char *pszCutlineSRS_WKT = NULL;

        OSRExportToWkt( hCutlineSRS, &pszCutlineSRS_WKT );
        papszTO = CSLSetNameValue( papszTO, "DST_SRS", pszCutlineSRS_WKT );
        CPLFree( pszCutlineSRS_WKT );
    }

/* -------------------------------------------------------------------- */
/*      It may be unwise to let the mask geometry be re-wrapped by      */
/*      the CENTER_LONG machinery as this can easily screw up world     */
/*      spanning masks and invert the mask topology.                    */
/* -------------------------------------------------------------------- */
    papszTO = CSLSetNameValue( papszTO, "INSERT_CENTER_LONG", "FALSE" );

/* -------------------------------------------------------------------- */
/*      Transform the geometry to pixel/line coordinates.               */
/* -------------------------------------------------------------------- */
    CutlineTransformer oTransformer;

    /* The cutline transformer will *invert* the hSrcImageTransformer */
    /* so it will convert from the cutline SRS to the source pixel/line */
    /* coordinates */
    oTransformer.hSrcImageTransformer = 
        GDALCreateGenImgProjTransformer2( hSrcDS, NULL, papszTO );

    CSLDestroy( papszTO );

    if( oTransformer.hSrcImageTransformer == NULL )
        GDALExit( 1 );

    OGR_G_Transform( hMultiPolygon, 
                     (OGRCoordinateTransformationH) &oTransformer );

    GDALDestroyGenImgProjTransformer( oTransformer.hSrcImageTransformer );

/* -------------------------------------------------------------------- */
/*      Convert aggregate geometry into WKT.                            */
/* -------------------------------------------------------------------- */
    char *pszWKT = NULL;

    OGR_G_ExportToWkt( hMultiPolygon, &pszWKT );
    OGR_G_DestroyGeometry( hMultiPolygon );

    *ppapszWarpOptions = CSLSetNameValue( *ppapszWarpOptions, 
                                          "CUTLINE", pszWKT );
    CPLFree( pszWKT );
#endif
}

#pragma endregion

#pragma region gdalbuildvrt



typedef enum
{
    LOWEST_RESOLUTION,
    HIGHEST_RESOLUTION,
    AVERAGE_RESOLUTION,
    USER_RESOLUTION
} ResolutionStrategy;

typedef struct
{
    int    isFileOK;
    int    nRasterXSize;
    int    nRasterYSize;
    double adfGeoTransform[6];
    int    nBlockXSize;
    int    nBlockYSize;
    GDALDataType firstBandType;
    int*         panHasNoData;
    double*      padfNoDataValues;
    int    bHasDatasetMask;
    int    nMaskBlockXSize;
    int    nMaskBlockYSize;
} DatasetProperty;

typedef struct
{
    GDALColorInterp        colorInterpretation;
    GDALDataType           dataType;
    GDALColorTableH        colorTable;
    int                    bHasNoData;
    double                 noDataValue;
} BandProperty;

/************************************************************************/
/*                         GetSrcDstWin()                               */
/************************************************************************/

int  GetSrcDstWin(DatasetProperty* psDP,
                  double we_res, double ns_res,
                  double minX, double minY, double maxX, double maxY,
                  int* pnSrcXOff, int* pnSrcYOff, int* pnSrcXSize, int* pnSrcYSize,
                  int* pnDstXOff, int* pnDstYOff, int* pnDstXSize, int* pnDstYSize)
{
    /* Check that the destination bounding box intersects the source bounding box */
    if ( psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_X] +
         psDP->nRasterXSize *
         psDP->adfGeoTransform[GEOTRSFRM_WE_RES] < minX )
         return FALSE;
    if ( psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_X] > maxX )
         return FALSE;
    if ( psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_Y] +
         psDP->nRasterYSize *
         psDP->adfGeoTransform[GEOTRSFRM_NS_RES] > maxY )
         return FALSE;
    if ( psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_Y] < minY )
         return FALSE;

    *pnSrcXSize = psDP->nRasterXSize;
    *pnSrcYSize = psDP->nRasterYSize;
    if ( psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_X] < minX )
    {
        *pnSrcXOff = (int)((minX - psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_X]) /
            psDP->adfGeoTransform[GEOTRSFRM_WE_RES] + 0.5);
        *pnDstXOff = 0;
    }
    else
    {
        *pnSrcXOff = 0;
        *pnDstXOff = (int)
            (0.5 + (psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_X] - minX) / we_res);
    }
    if ( maxY < psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_Y])
    {
        *pnSrcYOff = (int)((psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_Y] - maxY) /
            -psDP->adfGeoTransform[GEOTRSFRM_NS_RES] + 0.5);
        *pnDstYOff = 0;
    }
    else
    {
        *pnSrcYOff = 0;
        *pnDstYOff = (int)
            (0.5 + (maxY - psDP->adfGeoTransform[GEOTRSFRM_TOPLEFT_Y]) / -ns_res);
    }
    *pnDstXSize = (int)
        (0.5 + psDP->nRasterXSize *
         psDP->adfGeoTransform[GEOTRSFRM_WE_RES] / we_res);
    *pnDstYSize = (int)
        (0.5 + psDP->nRasterYSize *
         psDP->adfGeoTransform[GEOTRSFRM_NS_RES] / ns_res);
         
    return TRUE;
}

/************************************************************************/
/*                            VRTBuilder                                */
/************************************************************************/

class VRTBuilder
{
    /* Input parameters */
    char               *pszOutputFilename;
    int                 nInputFiles;
    char              **ppszInputFilenames;
    ResolutionStrategy  resolutionStrategy;
    double              we_res;
    double              ns_res;
    int                 bTargetAlignedPixels;
    double              minX;
    double              minY;
    double              maxX;
    double              maxY;
    int                 bSeparate;
    int                 bAllowProjectionDifference;
    int                 bAddAlpha;
    int                 bHideNoData;
    char               *pszSrcNoData;
    char               *pszVRTNoData;

    /* Internal variables */
    char               *pszProjectionRef;
    int                 nBands;
    BandProperty       *pasBandProperties;
    int                 bFirst;
    int                 bHasGeoTransform;
    int                 nRasterXSize;
    int                 nRasterYSize;
    DatasetProperty    *pasDatasetProperties;
    int                 bUserExtent;
    int                 bAllowSrcNoData;
    double             *padfSrcNoData;
    int                 nSrcNoDataCount;
    int                 bAllowVRTNoData;
    double             *padfVRTNoData;
    int                 nVRTNoDataCount;
    int                 bHasRunBuild;
    int                 bHasDatasetMask;

    int         AnalyseRaster(GDALDatasetH hDS, const char* dsFileName,
                              DatasetProperty* psDatasetProperties);

    void        CreateVRTSeparate(VRTDatasetH hVRTDS);
    void        CreateVRTNonSeparate(VRTDatasetH hVRTDS);

    public:
                VRTBuilder(const char* pszOutputFilename,
                           int nInputFiles, const char* const * ppszInputFilenames,
                           ResolutionStrategy resolutionStrategy,
                           double we_res, double ns_res,
                           int bTargetAlignedPixels,
                           double minX, double minY, double maxX, double maxY,
                           int bSeparate, int bAllowProjectionDifference,
                           int bAddAlpha, int bHideNoData,
                           const char* pszSrcNoData, const char* pszVRTNoData);

               ~VRTBuilder();

        int     Build(GDALProgressFunc pfnProgress, void * pProgressData);
};


/************************************************************************/
/*                          VRTBuilder()                                */
/************************************************************************/

VRTBuilder::VRTBuilder(const char* pszOutputFilename,
                       int nInputFiles, const char* const * ppszInputFilenames,
                       ResolutionStrategy resolutionStrategy,
                       double we_res, double ns_res,
                       int bTargetAlignedPixels,
                       double minX, double minY, double maxX, double maxY,
                       int bSeparate, int bAllowProjectionDifference,
                       int bAddAlpha, int bHideNoData,
                       const char* pszSrcNoData, const char* pszVRTNoData)
{
    this->pszOutputFilename = CPLStrdup(pszOutputFilename);
    this->nInputFiles = nInputFiles;

    this->ppszInputFilenames = (char**) CPLMalloc(nInputFiles * sizeof(char*));
    int i;
    for(i=0;i<nInputFiles;i++)
    {
        this->ppszInputFilenames[i] = CPLStrdup(ppszInputFilenames[i]);
    }

    this->resolutionStrategy = resolutionStrategy;
    this->we_res = we_res;
    this->ns_res = ns_res;
    this->bTargetAlignedPixels = bTargetAlignedPixels;
    this->minX = minX;
    this->minY = minY;
    this->maxX = maxX;
    this->maxY = maxY;
    this->bSeparate = bSeparate;
    this->bAllowProjectionDifference = bAllowProjectionDifference;
    this->bAddAlpha = bAddAlpha;
    this->bHideNoData = bHideNoData;
    this->pszSrcNoData = (pszSrcNoData) ? CPLStrdup(pszSrcNoData) : NULL;
    this->pszVRTNoData = (pszVRTNoData) ? CPLStrdup(pszVRTNoData) : NULL;

    bUserExtent = FALSE;
    pszProjectionRef = NULL;
    nBands = 0;
    pasBandProperties = NULL;
    bFirst = TRUE;
    bHasGeoTransform = FALSE;
    nRasterXSize = 0;
    nRasterYSize = 0;
    pasDatasetProperties = NULL;
    bAllowSrcNoData = TRUE;
    padfSrcNoData = NULL;
    nSrcNoDataCount = 0;
    bAllowVRTNoData = TRUE;
    padfVRTNoData = NULL;
    nVRTNoDataCount = 0;
    bHasRunBuild = FALSE;
    bHasDatasetMask = FALSE;
}

/************************************************************************/
/*                         ~VRTBuilder()                                */
/************************************************************************/

VRTBuilder::~VRTBuilder()
{
    CPLFree(pszOutputFilename);
    CPLFree(pszSrcNoData);
    CPLFree(pszVRTNoData);

    int i;
    for(i=0;i<nInputFiles;i++)
    {
        CPLFree(ppszInputFilenames[i]);
    }
    CPLFree(ppszInputFilenames);

    if (pasDatasetProperties != NULL)
    {
        for(i=0;i<nInputFiles;i++)
        {
            CPLFree(pasDatasetProperties[i].padfNoDataValues);
            CPLFree(pasDatasetProperties[i].panHasNoData);
        }
    }
    CPLFree(pasDatasetProperties);

    if (!bSeparate && pasBandProperties != NULL)
    {
        int j;
        for(j=0;j<nBands;j++)
        {
            GDALDestroyColorTable(pasBandProperties[j].colorTable);
        }
    }
    CPLFree(pasBandProperties);

    CPLFree(pszProjectionRef);
    CPLFree(padfSrcNoData);
    CPLFree(padfVRTNoData);
}

/************************************************************************/
/*                           ProjAreEqual()                             */
/************************************************************************/

static int ProjAreEqual(const char* pszWKT1, const char* pszWKT2)
{
    int bRet;
    OGRSpatialReferenceH hSRS1, hSRS2;

    if (EQUAL(pszWKT1, pszWKT2))
        return TRUE;

    hSRS1 = OSRNewSpatialReference(pszWKT1);
    hSRS2 = OSRNewSpatialReference(pszWKT2);
    bRet = hSRS1 != NULL && hSRS2 != NULL && OSRIsSame(hSRS1,hSRS2);
    if (hSRS1)
        OSRDestroySpatialReference(hSRS1);
    if (hSRS2)
        OSRDestroySpatialReference(hSRS2);
    return bRet;
}

/************************************************************************/
/*                           AnalyseRaster()                            */
/************************************************************************/

int VRTBuilder::AnalyseRaster( GDALDatasetH hDS, const char* dsFileName,
                                  DatasetProperty* psDatasetProperties)
{
    char** papszMetadata = GDALGetMetadata( hDS, "SUBDATASETS" );
    if( CSLCount(papszMetadata) > 0 && GDALGetRasterCount(hDS) == 0 )
    {
        pasDatasetProperties =
            (DatasetProperty*) CPLRealloc(pasDatasetProperties,
                            (nInputFiles+CSLCount(papszMetadata))*sizeof(DatasetProperty));

        ppszInputFilenames = (char**)CPLRealloc(ppszInputFilenames,
                                sizeof(char*) * (nInputFiles+CSLCount(papszMetadata)));
        int count = 1;
        char subdatasetNameKey[256];
        sprintf(subdatasetNameKey, "SUBDATASET_%d_NAME", count);
        while(*papszMetadata != NULL)
        {
            if (EQUALN(*papszMetadata, subdatasetNameKey, strlen(subdatasetNameKey)))
            {
                memset(&pasDatasetProperties[nInputFiles], 0, sizeof(DatasetProperty));
                ppszInputFilenames[nInputFiles++] =
                        CPLStrdup(*papszMetadata+strlen(subdatasetNameKey)+1);
                count++;
                sprintf(subdatasetNameKey, "SUBDATASET_%d_NAME", count);
            }
            papszMetadata++;
        }
        return FALSE;
    }

    const char* proj = GDALGetProjectionRef(hDS);
    double* padfGeoTransform = psDatasetProperties->adfGeoTransform;
    int bGotGeoTransform = GDALGetGeoTransform(hDS, padfGeoTransform) == CE_None;
    if (bSeparate)
    {
        if (bFirst)
        {
            bHasGeoTransform = bGotGeoTransform;
            if (!bHasGeoTransform)
            {
                if (bUserExtent)
                {
                    CPLError(CE_Warning, CPLE_NotSupported,
                        "User extent ignored by gdalbuildvrt -separate with ungeoreferenced images.");
                }
                if (resolutionStrategy == USER_RESOLUTION)
                {
                    CPLError(CE_Warning, CPLE_NotSupported,
                        "User resolution ignored by gdalbuildvrt -separate with ungeoreferenced images.");
                }
            }
        }
        else if (bHasGeoTransform != bGotGeoTransform)
        {
            CPLError(CE_Warning, CPLE_NotSupported,
                    "gdalbuildvrt -separate cannot stack ungeoreferenced and georeferenced images. Skipping %s",
                    dsFileName);
            return FALSE;
        }
        else if (!bHasGeoTransform &&
                    (nRasterXSize != GDALGetRasterXSize(hDS) ||
                    nRasterYSize != GDALGetRasterYSize(hDS)))
        {
            CPLError(CE_Warning, CPLE_NotSupported,
                    "gdalbuildvrt -separate cannot stack ungeoreferenced images that have not the same dimensions. Skipping %s",
                    dsFileName);
            return FALSE;
        }
    }
    else
    {
        if (!bGotGeoTransform)
        {
            CPLError(CE_Warning, CPLE_NotSupported,
                    "gdalbuildvrt does not support ungeoreferenced image. Skipping %s",
                    dsFileName);
            return FALSE;
        }
        bHasGeoTransform = TRUE;
    }

    if (bGotGeoTransform)
    {
        if (padfGeoTransform[GEOTRSFRM_ROTATION_PARAM1] != 0 ||
            padfGeoTransform[GEOTRSFRM_ROTATION_PARAM2] != 0)
        {
            CPLError(CE_Warning, CPLE_NotSupported,
                    "gdalbuildvrt does not support rotated geo transforms. Skipping %s",
                    dsFileName);
            return FALSE;
        }
        if (padfGeoTransform[GEOTRSFRM_NS_RES] >= 0)
        {
            CPLError(CE_Warning, CPLE_NotSupported,
                    "gdalbuildvrt does not support positive NS resolution. Skipping %s",
                    dsFileName);
            return FALSE;
        }
    }

    psDatasetProperties->nRasterXSize = GDALGetRasterXSize(hDS);
    psDatasetProperties->nRasterYSize = GDALGetRasterYSize(hDS);
    if (bFirst && bSeparate && !bGotGeoTransform)
    {
        nRasterXSize = GDALGetRasterXSize(hDS);
        nRasterYSize = GDALGetRasterYSize(hDS);
    }

    double ds_minX = padfGeoTransform[GEOTRSFRM_TOPLEFT_X];
    double ds_maxY = padfGeoTransform[GEOTRSFRM_TOPLEFT_Y];
    double ds_maxX = ds_minX +
                GDALGetRasterXSize(hDS) *
                padfGeoTransform[GEOTRSFRM_WE_RES];
    double ds_minY = ds_maxY +
                GDALGetRasterYSize(hDS) *
                padfGeoTransform[GEOTRSFRM_NS_RES];

    GDALGetBlockSize(GDALGetRasterBand( hDS, 1 ),
                        &psDatasetProperties->nBlockXSize,
                        &psDatasetProperties->nBlockYSize);

    int _nBands = GDALGetRasterCount(hDS);
    if (_nBands == 0)
    {
        CPLError(CE_Warning, CPLE_AppDefined,
                    "Skipping %s as it has no bands", dsFileName);
        return FALSE;
    }
    else if (_nBands > 1 && bSeparate)
    {
        CPLError(CE_Warning, CPLE_AppDefined, "%s has %d bands. Only the first one will "
                    "be taken into account in the -separate case",
                    dsFileName, _nBands);
        _nBands = 1;
    }

    /* For the -separate case */
    psDatasetProperties->firstBandType = GDALGetRasterDataType(GDALGetRasterBand(hDS, 1));

    psDatasetProperties->padfNoDataValues = (double*)CPLCalloc(sizeof(double), _nBands);
    psDatasetProperties->panHasNoData = (int*)CPLCalloc(sizeof(int), _nBands);

    psDatasetProperties->bHasDatasetMask = GDALGetMaskFlags(GDALGetRasterBand(hDS, 1)) == GMF_PER_DATASET;
    if (psDatasetProperties->bHasDatasetMask)
        bHasDatasetMask = TRUE;
    GDALGetBlockSize(GDALGetMaskBand(GDALGetRasterBand( hDS, 1 )),
                        &psDatasetProperties->nMaskBlockXSize,
                        &psDatasetProperties->nMaskBlockYSize);

    int j;
    for(j=0;j<_nBands;j++)
    {
        if (nSrcNoDataCount > 0)
        {
            psDatasetProperties->panHasNoData[j] = TRUE;
            if (j < nSrcNoDataCount)
                psDatasetProperties->padfNoDataValues[j] = padfSrcNoData[j];
            else
                psDatasetProperties->padfNoDataValues[j] = padfSrcNoData[nSrcNoDataCount - 1];
        }
        else
        {
            psDatasetProperties->padfNoDataValues[j]  =
                GDALGetRasterNoDataValue(GDALGetRasterBand(hDS, j+1),
                                        &psDatasetProperties->panHasNoData[j]);
        }
    }

    if (bFirst)
    {
        if (proj)
            pszProjectionRef = CPLStrdup(proj);
        if (!bUserExtent)
        {
            minX = ds_minX;
            minY = ds_minY;
            maxX = ds_maxX;
            maxY = ds_maxY;
        }
        nBands = _nBands;

        if (!bSeparate)
        {
            pasBandProperties = (BandProperty*)CPLMalloc(nBands*sizeof(BandProperty));
            for(j=0;j<nBands;j++)
            {
                GDALRasterBandH hRasterBand = GDALGetRasterBand( hDS, j+1 );
                pasBandProperties[j].colorInterpretation =
                        GDALGetRasterColorInterpretation(hRasterBand);
                pasBandProperties[j].dataType = GDALGetRasterDataType(hRasterBand);
                if (pasBandProperties[j].colorInterpretation == GCI_PaletteIndex)
                {
                    pasBandProperties[j].colorTable =
                            GDALGetRasterColorTable( hRasterBand );
                    if (pasBandProperties[j].colorTable)
                    {
                        pasBandProperties[j].colorTable =
                                GDALCloneColorTable(pasBandProperties[j].colorTable);
                    }
                }
                else
                    pasBandProperties[j].colorTable = 0;

                if (nVRTNoDataCount > 0)
                {
                    pasBandProperties[j].bHasNoData = TRUE;
                    if (j < nVRTNoDataCount)
                        pasBandProperties[j].noDataValue = padfVRTNoData[j];
                    else
                        pasBandProperties[j].noDataValue = padfVRTNoData[nVRTNoDataCount - 1];
                }
                else
                {
                    pasBandProperties[j].noDataValue =
                            GDALGetRasterNoDataValue(hRasterBand, &pasBandProperties[j].bHasNoData);
                }
            }
        }
    }
    else
    {
        if ((proj != NULL && pszProjectionRef == NULL) ||
            (proj == NULL && pszProjectionRef != NULL) ||
            (proj != NULL && pszProjectionRef != NULL && ProjAreEqual(proj, pszProjectionRef) == FALSE))
        {
            if (!bAllowProjectionDifference)
            {
                CPLError(CE_Warning, CPLE_NotSupported,
                            "gdalbuildvrt does not support heterogenous projection. Skipping %s",
                            dsFileName);
                return FALSE;
            }
        }
        if (!bSeparate)
        {
            if (nBands != _nBands)
            {
                CPLError(CE_Warning, CPLE_NotSupported,
                            "gdalbuildvrt does not support heterogenous band numbers. Skipping %s",
                        dsFileName);
                return FALSE;
            }
            for(j=0;j<nBands;j++)
            {
                GDALRasterBandH hRasterBand = GDALGetRasterBand( hDS, j+1 );
                if (pasBandProperties[j].colorInterpretation != GDALGetRasterColorInterpretation(hRasterBand) ||
                    pasBandProperties[j].dataType != GDALGetRasterDataType(hRasterBand))
                {
                    CPLError(CE_Warning, CPLE_NotSupported,
                                "gdalbuildvrt does not support heterogenous band characteristics. Skipping %s",
                                dsFileName);
                    return FALSE;
                }
                if (pasBandProperties[j].colorTable)
                {
                    GDALColorTableH colorTable = GDALGetRasterColorTable( hRasterBand );
                    int nRefColorEntryCount = GDALGetColorEntryCount(pasBandProperties[j].colorTable);
                    int i;
                    if (colorTable == NULL ||
                        GDALGetColorEntryCount(colorTable) != nRefColorEntryCount)
                    {
                        CPLError(CE_Warning, CPLE_NotSupported,
                                    "gdalbuildvrt does not support rasters with different color tables (different number of color table entries). Skipping %s",
                                dsFileName);
                        return FALSE;
                    }

                    /* Check that the palette are the same too */
                    /* We just warn and still process the file. It is not a technical no-go, but the user */
                    /* should check that the end result is OK for him. */
                    for(i=0;i<nRefColorEntryCount;i++)
                    {
                        const GDALColorEntry* psEntry = GDALGetColorEntry(colorTable, i);
                        const GDALColorEntry* psEntryRef = GDALGetColorEntry(pasBandProperties[j].colorTable, i);
                        if (psEntry->c1 != psEntryRef->c1 || psEntry->c2 != psEntryRef->c2 ||
                            psEntry->c3 != psEntryRef->c3 || psEntry->c4 != psEntryRef->c4)
                        {
                            static int bFirstWarningPCT = TRUE;
                            if (bFirstWarningPCT)
                                CPLError(CE_Warning, CPLE_NotSupported,
                                        "%s has different values than the first raster for some entries in the color table.\n"
                                        "The end result might produce weird colors.\n"
                                        "You're advised to preprocess your rasters with other tools, such as pct2rgb.py or gdal_translate -expand RGB\n"
                                        "to operate gdalbuildvrt on RGB rasters instead", dsFileName);
                            else
                                CPLError(CE_Warning, CPLE_NotSupported,
                                            "%s has different values than the first raster for some entries in the color table.",
                                            dsFileName);
                            bFirstWarningPCT = FALSE;
                            break;
                        }
                    }
                }
            }

        }
        if (!bUserExtent)
        {
            if (ds_minX < minX) minX = ds_minX;
            if (ds_minY < minY) minY = ds_minY;
            if (ds_maxX > maxX) maxX = ds_maxX;
            if (ds_maxY > maxY) maxY = ds_maxY;
        }
    }

    if (resolutionStrategy == AVERAGE_RESOLUTION)
    {
        we_res += padfGeoTransform[GEOTRSFRM_WE_RES];
        ns_res += padfGeoTransform[GEOTRSFRM_NS_RES];
    }
    else if (resolutionStrategy != USER_RESOLUTION)
    {
        if (bFirst)
        {
            we_res = padfGeoTransform[GEOTRSFRM_WE_RES];
            ns_res = padfGeoTransform[GEOTRSFRM_NS_RES];
        }
        else if (resolutionStrategy == HIGHEST_RESOLUTION)
        {
            we_res = MIN(we_res, padfGeoTransform[GEOTRSFRM_WE_RES]);
            /* Yes : as ns_res is negative, the highest resolution is the max value */
            ns_res = MAX(ns_res, padfGeoTransform[GEOTRSFRM_NS_RES]);
        }
        else
        {
            we_res = MAX(we_res, padfGeoTransform[GEOTRSFRM_WE_RES]);
            /* Yes : as ns_res is negative, the lowest resolution is the min value */
            ns_res = MIN(ns_res, padfGeoTransform[GEOTRSFRM_NS_RES]);
        }
    }

    return TRUE;
}

/************************************************************************/
/*                         CreateVRTSeparate()                          */
/************************************************************************/

void VRTBuilder::CreateVRTSeparate(VRTDatasetH hVRTDS)
{
    int i;
    int iBand = 1;
    for(i=0;i<nInputFiles;i++)
    {
        DatasetProperty* psDatasetProperties = &pasDatasetProperties[i];

        if (psDatasetProperties->isFileOK == FALSE)
            continue;

        int nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
            nDstXOff, nDstYOff, nDstXSize, nDstYSize;
        if (bHasGeoTransform)
        {
            if ( ! GetSrcDstWin(psDatasetProperties,
                        we_res, ns_res, minX, minY, maxX, maxY,
                        &nSrcXOff, &nSrcYOff, &nSrcXSize, &nSrcYSize,
                        &nDstXOff, &nDstYOff, &nDstXSize, &nDstYSize) )
                continue;
        }
        else
        {
            nSrcXOff = nSrcYOff = nDstXOff = nDstYOff = 0;
            nSrcXSize = nDstXSize = nRasterXSize;
            nSrcYSize = nDstYSize = nRasterYSize;
        }

        const char* dsFileName = ppszInputFilenames[i];

        GDALAddBand(hVRTDS, psDatasetProperties->firstBandType, NULL);

        GDALProxyPoolDatasetH hProxyDS =
            GDALProxyPoolDatasetCreate(dsFileName,
                                        psDatasetProperties->nRasterXSize,
                                        psDatasetProperties->nRasterYSize,
                                        GA_ReadOnly, TRUE, pszProjectionRef,
                                        psDatasetProperties->adfGeoTransform);
        GDALProxyPoolDatasetAddSrcBandDescription(hProxyDS,
                                            psDatasetProperties->firstBandType,
                                            psDatasetProperties->nBlockXSize,
                                            psDatasetProperties->nBlockYSize);

        VRTSourcedRasterBandH hVRTBand =
                (VRTSourcedRasterBandH)GDALGetRasterBand(hVRTDS, iBand);

        if (bHideNoData)
            GDALSetMetadataItem(hVRTBand,"HideNoDataValue","1",NULL);

        if (bAllowSrcNoData && psDatasetProperties->panHasNoData[0])
        {
            GDALSetRasterNoDataValue(hVRTBand, psDatasetProperties->padfNoDataValues[0]);
            VRTAddComplexSource(hVRTBand, GDALGetRasterBand((GDALDatasetH)hProxyDS, 1),
                            nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                            nDstXOff, nDstYOff, nDstXSize, nDstYSize,
                            0, 1, psDatasetProperties->padfNoDataValues[0]);
        }
        else
            /* Place the raster band at the right position in the VRT */
            VRTAddSimpleSource(hVRTBand, GDALGetRasterBand((GDALDatasetH)hProxyDS, 1),
                            nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                            nDstXOff, nDstYOff, nDstXSize, nDstYSize,
                            "near", VRT_NODATA_UNSET);

        GDALDereferenceDataset(hProxyDS);

        iBand ++;
    }
}

/************************************************************************/
/*                       CreateVRTNonSeparate()                         */
/************************************************************************/

void VRTBuilder::CreateVRTNonSeparate(VRTDatasetH hVRTDS)
{
    int i, j;

    for(j=0;j<nBands;j++)
    {
        GDALRasterBandH hBand;
        GDALAddBand(hVRTDS, pasBandProperties[j].dataType, NULL);
        hBand = GDALGetRasterBand(hVRTDS, j+1);
        GDALSetRasterColorInterpretation(hBand, pasBandProperties[j].colorInterpretation);
        if (pasBandProperties[j].colorInterpretation == GCI_PaletteIndex)
        {
            GDALSetRasterColorTable(hBand, pasBandProperties[j].colorTable);
        }
        if (bAllowVRTNoData && pasBandProperties[j].bHasNoData)
            GDALSetRasterNoDataValue(hBand, pasBandProperties[j].noDataValue);
        if ( bHideNoData )
            GDALSetMetadataItem(hBand,"HideNoDataValue","1",NULL);
    }

    VRTSourcedRasterBand* hMaskVRTBand = NULL;
    if (bAddAlpha)
    {
        GDALRasterBandH hBand;
        GDALAddBand(hVRTDS, GDT_Byte, NULL);
        hBand = GDALGetRasterBand(hVRTDS, nBands + 1);
        GDALSetRasterColorInterpretation(hBand, GCI_AlphaBand);
    }
    else if (bHasDatasetMask)
    {
        GDALCreateDatasetMaskBand(hVRTDS, GMF_PER_DATASET);
        hMaskVRTBand = (VRTSourcedRasterBand*)GDALGetMaskBand(GDALGetRasterBand(hVRTDS, 1));
    }


    for(i=0;i<nInputFiles;i++)
    {
        DatasetProperty* psDatasetProperties = &pasDatasetProperties[i];

        if (psDatasetProperties->isFileOK == FALSE)
            continue;

        int nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
            nDstXOff, nDstYOff, nDstXSize, nDstYSize;
        if ( ! GetSrcDstWin(psDatasetProperties,
                        we_res, ns_res, minX, minY, maxX, maxY,
                        &nSrcXOff, &nSrcYOff, &nSrcXSize, &nSrcYSize,
                        &nDstXOff, &nDstYOff, &nDstXSize, &nDstYSize) )
            continue;

        const char* dsFileName = ppszInputFilenames[i];

        GDALProxyPoolDatasetH hProxyDS =
            GDALProxyPoolDatasetCreate(dsFileName,
                                        psDatasetProperties->nRasterXSize,
                                        psDatasetProperties->nRasterYSize,
                                        GA_ReadOnly, TRUE, pszProjectionRef,
                                        psDatasetProperties->adfGeoTransform);

        for(j=0;j<nBands;j++)
        {
            GDALProxyPoolDatasetAddSrcBandDescription(hProxyDS,
                                            pasBandProperties[j].dataType,
                                            psDatasetProperties->nBlockXSize,
                                            psDatasetProperties->nBlockYSize);
        }
        if (bHasDatasetMask && !bAddAlpha)
        {
            ((GDALProxyPoolRasterBand*)((GDALProxyPoolDataset*)hProxyDS)->GetRasterBand(1))->
                    AddSrcMaskBandDescription  (GDT_Byte,
                                                psDatasetProperties->nMaskBlockXSize,
                                                psDatasetProperties->nMaskBlockYSize);
        }

        for(j=0;j<nBands;j++)
        {
            VRTSourcedRasterBandH hVRTBand =
                    (VRTSourcedRasterBandH)GDALGetRasterBand(hVRTDS, j + 1);

            /* Place the raster band at the right position in the VRT */
            if (bAllowSrcNoData && psDatasetProperties->panHasNoData[j])
                VRTAddComplexSource(hVRTBand, GDALGetRasterBand((GDALDatasetH)hProxyDS, j + 1),
                                nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                                nDstXOff, nDstYOff, nDstXSize, nDstYSize,
                                0, 1, psDatasetProperties->padfNoDataValues[j]);
            else
                VRTAddSimpleSource(hVRTBand, GDALGetRasterBand((GDALDatasetH)hProxyDS, j + 1),
                                nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                                nDstXOff, nDstYOff, nDstXSize, nDstYSize,
                                "near", VRT_NODATA_UNSET);
        }

        if (bAddAlpha)
        {
            VRTSourcedRasterBandH hVRTBand =
                    (VRTSourcedRasterBandH)GDALGetRasterBand(hVRTDS, nBands + 1);
            /* Little trick : we use an offset of 255 and a scaling of 0, so that in areas covered */
            /* by the source, the value of the alpha band will be 255, otherwise it will be 0 */
            VRTAddComplexSource(hVRTBand, GDALGetRasterBand((GDALDatasetH)hProxyDS, 1),
                                nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                                nDstXOff, nDstYOff, nDstXSize, nDstYSize,
                                255, 0, VRT_NODATA_UNSET);
        }
        else if (bHasDatasetMask)
        {
            hMaskVRTBand->AddMaskBandSource((GDALRasterBand*)GDALGetRasterBand((GDALDatasetH)hProxyDS, 1),
                                            nSrcXOff, nSrcYOff, nSrcXSize, nSrcYSize,
                                            nDstXOff, nDstYOff, nDstXSize, nDstYSize);
        }

        GDALDereferenceDataset(hProxyDS);
    }
}

/************************************************************************/
/*                             Build()                                  */
/************************************************************************/

int VRTBuilder::Build(GDALProgressFunc pfnProgress, void * pProgressData)
{
    int i;

    if (bHasRunBuild)
        return CE_Failure;
    bHasRunBuild = TRUE;

    if( pfnProgress == NULL )
        pfnProgress = GDALDummyProgress;

    bUserExtent = (minX != 0 || minY != 0 || maxX != 0 || maxY != 0);
    if (bUserExtent)
    {
        if (minX >= maxX || minY >= maxY )
        {
            CPLError(CE_Failure, CPLE_IllegalArg, "Invalid user extent");
            return CE_Failure;
        }
    }

    if (resolutionStrategy == USER_RESOLUTION)
    {
        if (we_res <= 0 || ns_res <= 0)
        {
            CPLError(CE_Failure, CPLE_IllegalArg, "Invalid user resolution");
            return CE_Failure;
        }

        /* We work with negative north-south resolution in all the following code */
        ns_res = -ns_res;
    }
    else
    {
        we_res = ns_res = 0;
    }

    pasDatasetProperties =
            (DatasetProperty*) CPLCalloc(nInputFiles, sizeof(DatasetProperty));

    if (pszSrcNoData != NULL)
    {
        if (EQUAL(pszSrcNoData, "none"))
        {
            bAllowSrcNoData = FALSE;
        }
        else
        {
            char **papszTokens = CSLTokenizeString( pszSrcNoData );
            nSrcNoDataCount = CSLCount(papszTokens);
            padfSrcNoData = (double *) CPLMalloc(sizeof(double) * nSrcNoDataCount);
            for(i=0;i<nSrcNoDataCount;i++)
                padfSrcNoData[i] = CPLAtofM(papszTokens[i]);
            CSLDestroy(papszTokens);
        }
    }

    if (pszVRTNoData != NULL)
    {
        if (EQUAL(pszVRTNoData, "none"))
        {
            bAllowVRTNoData = FALSE;
        }
        else
        {
            char **papszTokens = CSLTokenizeString( pszVRTNoData );
            nVRTNoDataCount = CSLCount(papszTokens);
            padfVRTNoData = (double *) CPLMalloc(sizeof(double) * nVRTNoDataCount);
            for(i=0;i<nVRTNoDataCount;i++)
                padfVRTNoData[i] = CPLAtofM(papszTokens[i]);
            CSLDestroy(papszTokens);
        }
    }

    int nCountValid = 0;
    for(i=0;i<nInputFiles;i++)
    {
        const char* dsFileName = ppszInputFilenames[i];

        if (!pfnProgress( 1.0 * (i+1) / nInputFiles, NULL, pProgressData))
        {
            return CE_Failure;
        }

        GDALDatasetH hDS = GDALOpen(ppszInputFilenames[i], GA_ReadOnly );		// TODO: use Unicode
        pasDatasetProperties[i].isFileOK = FALSE;

        if (hDS)
        {
            if (AnalyseRaster( hDS, dsFileName, &pasDatasetProperties[i] ))
            {
                pasDatasetProperties[i].isFileOK = TRUE;
                nCountValid ++;
                bFirst = FALSE;
            }
            GDALClose(hDS);
        }
        else
        {
            CPLError(CE_Warning, CPLE_AppDefined, 
                     "Can't open %s. Skipping it", dsFileName);
        }
    }

    if (nCountValid == 0)
        return CE_Failure;

    if (bHasGeoTransform)
    {
        if (resolutionStrategy == AVERAGE_RESOLUTION)
        {
            we_res /= nCountValid;
            ns_res /= nCountValid;
        }
        
        if ( bTargetAlignedPixels )
        {
            minX = floor(minX / we_res) * we_res;
            maxX = ceil(maxX / we_res) * we_res;
            minY = floor(minY / -ns_res) * -ns_res;
            maxY = ceil(maxY / -ns_res) * -ns_res;
        }

        nRasterXSize = (int)(0.5 + (maxX - minX) / we_res);
        nRasterYSize = (int)(0.5 + (maxY - minY) / -ns_res);
    }

    if (nRasterXSize == 0 || nRasterYSize == 0)
    {
        CPLError(CE_Failure, CPLE_AppDefined, 
                  "Computed VRT dimension is invalid. You've probably specified unappropriate resolution.");
        return CE_Failure;
    }

    VRTDatasetH hVRTDS = VRTCreate(nRasterXSize, nRasterYSize);
    GDALSetDescription(hVRTDS, pszOutputFilename);

    if (pszProjectionRef)
    {
        GDALSetProjection(hVRTDS, pszProjectionRef);
    }

    if (bHasGeoTransform)
    {
        double adfGeoTransform[6];
        adfGeoTransform[GEOTRSFRM_TOPLEFT_X] = minX;
        adfGeoTransform[GEOTRSFRM_WE_RES] = we_res;
        adfGeoTransform[GEOTRSFRM_ROTATION_PARAM1] = 0;
        adfGeoTransform[GEOTRSFRM_TOPLEFT_Y] = maxY;
        adfGeoTransform[GEOTRSFRM_ROTATION_PARAM2] = 0;
        adfGeoTransform[GEOTRSFRM_NS_RES] = ns_res;
        GDALSetGeoTransform(hVRTDS, adfGeoTransform);
    }

    if (bSeparate)
    {
        CreateVRTSeparate(hVRTDS);
    }
    else
    {
        CreateVRTNonSeparate(hVRTDS);
    }

    GDALClose(hVRTDS);

    return CE_None;
}

/************************************************************************/
/*                        add_file_to_list()                            */
/************************************************************************/

static void add_file_to_list(const char* filename, const char* tile_index,
                             int* pnInputFiles, char*** pppszInputFilenames)
{
   
    int nInputFiles = *pnInputFiles;
    char** ppszInputFilenames = *pppszInputFilenames;
    
    if (EQUAL(CPLGetExtension(filename), "SHP"))
    {
#ifndef OGR_ENABLED
        CPLError(CE_Failure, CPLE_AppDefined, "OGR support needed to read tileindex");
        *pnInputFiles = 0;
        *pppszInputFilenames = NULL;
#else
        OGRDataSourceH hDS;
        OGRLayerH      hLayer;
        OGRFeatureDefnH hFDefn;
        int j, ti_field;

        OGRRegisterAll();
        
        /* Handle GDALTIndex Shapefile as a special case */
        hDS = OGROpen( filename, FALSE, NULL );
        if( hDS  == NULL )
        {
            fprintf( stderr, "Unable to open shapefile `%s'.\n", 
                    filename );
            exit(2);
        }
        
        hLayer = OGR_DS_GetLayer(hDS, 0);

        hFDefn = OGR_L_GetLayerDefn(hLayer);

        for( ti_field = 0; ti_field < OGR_FD_GetFieldCount(hFDefn); ti_field++ )
        {
            OGRFieldDefnH hFieldDefn = OGR_FD_GetFieldDefn( hFDefn, ti_field );
            const char* pszName = OGR_Fld_GetNameRef(hFieldDefn);

            if (strcmp(pszName, "LOCATION") == 0 && strcmp("LOCATION", tile_index) != 0 )
            {
                fprintf( stderr, "This shapefile seems to be a tile index of "
                                "OGR features and not GDAL products.\n");
            }
            if( strcmp(pszName, tile_index) == 0 )
                break;
        }
    
        if( ti_field == OGR_FD_GetFieldCount(hFDefn) )
        {
            fprintf( stderr, "Unable to find field `%s' in DBF file `%s'.\n", 
                    tile_index, filename );
            return;
        }
    
        /* Load in memory existing file names in SHP */
        int nTileIndexFiles = OGR_L_GetFeatureCount(hLayer, TRUE);
        if (nTileIndexFiles == 0)
        {
            fprintf( stderr, "Tile index %s is empty. Skipping it.\n", filename);
            return;
        }
        
        ppszInputFilenames = (char**)CPLRealloc(ppszInputFilenames,
                              sizeof(char*) * (nInputFiles+nTileIndexFiles));
        for(j=0;j<nTileIndexFiles;j++)
        {
            OGRFeatureH hFeat = OGR_L_GetNextFeature(hLayer);
            ppszInputFilenames[nInputFiles++] =
                    CPLStrdup(OGR_F_GetFieldAsString(hFeat, ti_field ));
            OGR_F_Destroy(hFeat);
        }

        OGR_DS_Destroy( hDS );
#endif
    }
    else
    {
        ppszInputFilenames = (char**)CPLRealloc(ppszInputFilenames,
                                                 sizeof(char*) * (nInputFiles+1));
        ppszInputFilenames[nInputFiles++] = CPLStrdup(filename);
    }

    *pnInputFiles = nInputFiles;
    *pppszInputFilenames = ppszInputFilenames;
}

/************************************************************************/
/*                                GDALBuildVrt()                        */
/************************************************************************/

STDMETHODIMP CUtils::GDALBuildVrt(BSTR bstrDstFilename, BSTR bstrOptions,
								  ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	int nArgc = 0;
    const char *tile_index = "location";
    const char *resolution = NULL;
    int nInputFiles = 0;
    char ** ppszInputFilenames = NULL;
    const char * pszOutputFilename = NULL;
    int i, iArg;
    int bSeparate = FALSE;
    int bAllowProjectionDifference = FALSE;
    int bQuiet = FALSE;
    GDALProgressFunc pfnProgress = GDALProgressCallback;
    double we_res = 0, ns_res = 0;
    int bTargetAlignedPixels = FALSE;
    double xmin = 0, ymin = 0, xmax = 0, ymax = 0;
    int bAddAlpha = FALSE;
    int bForceOverwrite = FALSE;
    int bHideNoData = FALSE;
    const char* pszSrcNoData = NULL;
    const char* pszVRTNoData = NULL;
	CallbackParams params(GetCallback(), "Building Virtual Dataset");

    GDALAllRegister();

	(*retval) = VARIANT_FALSE;

    Parse(bstrOptions, &nArgc);

	if (!ProcessGeneralOptions(&nArgc))
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*      Parse commandline.                                              */
/* -------------------------------------------------------------------- */
    for( iArg = 1; iArg < nArgc; iArg++ )
    {
        if( EQUAL(_sArr[iArg],"-tileindex") &&
                 iArg + 1 < nArgc)
        {
            tile_index = _sArr[++iArg];
        }
        else if( EQUAL(_sArr[iArg],"-resolution") &&
                 iArg + 1 < nArgc)
        {
            resolution = _sArr[++iArg];
        }
        else if( EQUAL(_sArr[iArg],"-input_file_list") &&
                 iArg + 1 < nArgc)
        {
            const char* input_file_list = _sArr[++iArg];
            FILE* f = VSIFOpen(input_file_list, "r");
            if (f)
            {
                while(1)
                {
                    const char* filename = CPLReadLine(f);
                    if (filename == NULL)
                        break;
                    add_file_to_list(filename, tile_index,
                                     &nInputFiles, &ppszInputFilenames);
                }
                VSIFClose(f);
            }
        }
        else if ( EQUAL(_sArr[iArg],"-separate") )
        {
            bSeparate = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-allow_projection_difference") )
        {
            bAllowProjectionDifference = TRUE;
        }
        /* Alternate syntax for output file */
        else if( EQUAL(_sArr[iArg],"-o")  &&
                 iArg + 1 < nArgc)
        {
            pszOutputFilename = _sArr[++iArg];
        }
        else if ( EQUAL(_sArr[iArg],"-q") || EQUAL(_sArr[iArg],"-quiet") )
        {
            bQuiet = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-tr") && iArg + 2 < nArgc)
        {
            we_res = CPLAtofM(_sArr[++iArg]);
            ns_res = CPLAtofM(_sArr[++iArg]);
        }
        else if( EQUAL(_sArr[iArg],"-tap") )
        {
            bTargetAlignedPixels = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-te") && iArg + 4 < nArgc)
        {
            xmin = CPLAtofM(_sArr[++iArg]);
            ymin = CPLAtofM(_sArr[++iArg]);
            xmax = CPLAtofM(_sArr[++iArg]);
            ymax = CPLAtofM(_sArr[++iArg]);
        }
        else if ( EQUAL(_sArr[iArg],"-addalpha") )
        {
            bAddAlpha = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-hidenodata") )
        {
            bHideNoData = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-overwrite") )
        {
            bForceOverwrite = TRUE;
        }
        else if ( EQUAL(_sArr[iArg],"-srcnodata") && iArg + 1 < nArgc)
        {
            pszSrcNoData = _sArr[++iArg];
        }
        else if ( EQUAL(_sArr[iArg],"-vrtnodata") && iArg + 1 < nArgc)
        {
            pszVRTNoData = _sArr[++iArg];
        }
        else if ( _sArr[iArg][0] == '-' )
        {
            CPLError(CE_Warning, CPLE_AppDefined,"Unrecognized option : %s\n", _sArr[iArg]);
        }
        else
        {
            add_file_to_list(_sArr[iArg], tile_index,
                             &nInputFiles, &ppszInputFilenames);
        }
    }

	pszOutputFilename = OLE2CA(bstrDstFilename);

    if( pszOutputFilename == NULL )
	{
		this->_lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure, CPLE_AppDefined, "Invalid output file specified");
		return ResetConfigOptions();
	}
		
	if( nInputFiles == 0 )
	{
		this->_lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure, CPLE_AppDefined, "No inputs files specified");
        return ResetConfigOptions();
	}

    /* Avoid overwriting a non VRT dataset if the user did not put the */
    /* filenames in the right order */
    VSIStatBuf sBuf;
    if (!bForceOverwrite)
    {
        int bExists = (VSIStat(pszOutputFilename, &sBuf) == 0);
        if (bExists)
        {
            GDALDriverH hDriver = GDALIdentifyDriver( pszOutputFilename, NULL );
            if (hDriver && !EQUAL(GDALGetDriverShortName(hDriver), "VRT"))
            {
                CPLError(CE_Failure, CPLE_AppDefined,
                        "'%s' is an existing GDAL dataset managed by %s driver.\n"
                        "There is an high chance you did not put filenames in the right order.\n"
                        "If you want to overwrite %s, add -overwrite option to the command line.\n\n",
                        pszOutputFilename, GDALGetDriverShortName(hDriver), pszOutputFilename);
				return ResetConfigOptions(tkGDAL_ERROR);
            }
        }
    }
    
    if (we_res != 0 && ns_res != 0 &&
        resolution != NULL && !EQUAL(resolution, "user"))
    {
        CPLError(CE_Failure, CPLE_AppDefined, "-tr option is not compatible with -resolution %s\n", resolution);
		return ResetConfigOptions(tkGDAL_ERROR);
    }
    
    if (bTargetAlignedPixels && we_res == 0 && ns_res == 0)
    {
        CPLError(CE_Failure, CPLE_AppDefined, "-tap option cannot be used without using -tr\n");
		return ResetConfigOptions(tkGDAL_ERROR);
    }
    
    if (bAddAlpha && bSeparate)
    {
        CPLError(CE_Failure, CPLE_AppDefined, "-addalpha option is not compatible with -separate\n");
		return ResetConfigOptions(tkGDAL_ERROR);
    }
        
    ResolutionStrategy eStrategy = AVERAGE_RESOLUTION;
    if ( resolution == NULL || EQUAL(resolution, "user") )
    {
        if ( we_res != 0 || ns_res != 0)
            eStrategy = USER_RESOLUTION;
        else if ( resolution != NULL && EQUAL(resolution, "user") )
        {
            CPLError(CE_Failure, CPLE_AppDefined, "-tr option must be used with -resolution user\n");
			return ResetConfigOptions(tkGDAL_ERROR);
        }
    }
    else if ( EQUAL(resolution, "average") )
        eStrategy = AVERAGE_RESOLUTION;
    else if ( EQUAL(resolution, "highest") )
        eStrategy = HIGHEST_RESOLUTION;
    else if ( EQUAL(resolution, "lowest") )
        eStrategy = LOWEST_RESOLUTION;
    else
    {
        CPLError(CE_Failure, CPLE_AppDefined, "invalid value (%s) for -resolution\n", resolution);
		return ResetConfigOptions(tkGDAL_ERROR);
    }
    
    /* If -srcnodata is specified, use it as the -vrtnodata if the latter is not */
    /* specified */
    if (pszSrcNoData != NULL && pszVRTNoData == NULL)
        pszVRTNoData = pszSrcNoData;

    VRTBuilder oBuilder(pszOutputFilename, nInputFiles, ppszInputFilenames,
                        eStrategy, we_res, ns_res, bTargetAlignedPixels, xmin, ymin, xmax, ymax,
                        bSeparate, bAllowProjectionDifference, bAddAlpha, bHideNoData,
                        pszSrcNoData, pszVRTNoData);

    if (CE_None == oBuilder.Build(pfnProgress, &params))
		(*retval) = VARIANT_TRUE;
	else
		this->_lastErrorCode = tkGDAL_ERROR;

    for(i=0;i<nInputFiles;i++)
    {
        CPLFree(ppszInputFilenames[i]);
    }
    CPLFree(ppszInputFilenames);

    GDALDumpOpenDatasets( stderr );

    return ResetConfigOptions();
}

#pragma endregion

#pragma region gdaladdo

/************************************************************************/
/*                                GDALAddOverviews()                    */
/************************************************************************/
__declspec(deprecated("This is a deprecated function, use CGdalUtils::GdalBuildOverviews instead"))
STDMETHODIMP CUtils::GDALAddOverviews(BSTR bstrSrcFilename, BSTR bstrOptions,
									  BSTR bstrLevels, ICallback * cBack, VARIANT_BOOL *retval)

{
	USES_CONVERSION;

	int nArgc = 0;
    GDALDatasetH     hDataset;
    const char      *pszResampling = "nearest";
    const char      *pszFilename = NULL;
    int              anLevels[1024];
    int              nLevelCount = 0;
    int              nResultStatus = 0;
    int              bReadOnly = FALSE;
    int              bClean = FALSE;
    GDALProgressFunc pfnProgress = (GDALProgressFunc) GDALProgressCallback;
	struct CallbackParams params(GetCallback(), "Adding Overviews");

    GDALAllRegister();

	(*retval) = VARIANT_FALSE;

	Parse(bstrOptions, &nArgc);

	if (!ProcessGeneralOptions(&nArgc))
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*      Parse commandline.                                              */
/* -------------------------------------------------------------------- */
    for( int iArg = 1; iArg < nArgc; iArg++ )
    {
        if( EQUAL(_sArr[iArg],"-r") && iArg < nArgc-1 )
            pszResampling = _sArr[++iArg];
        else if( EQUAL(_sArr[iArg],"-ro"))
            bReadOnly = TRUE;
        else if( EQUAL(_sArr[iArg],"-clean"))
		{
            bClean = TRUE;
			params.sMsg = "Cleaning overviews";
		}
    }

	pszFilename = OLE2CA(bstrSrcFilename);

	int curPos = 0;
	CString sLevels = OLE2CA(bstrLevels);
	CString sLevelToken = sLevels.Tokenize(" ", curPos);
	

	while( !sLevelToken.IsEmpty() )
	{
		anLevels[nLevelCount++] = atoi( sLevelToken );
		sLevelToken = sLevels.Tokenize(" ", curPos);
	}

    if( pszFilename == NULL || (nLevelCount == 0 && !bClean) )
	{
		return ResetConfigOptions();
	}

/* -------------------------------------------------------------------- */
/*      Open data file.                                                 */
/* -------------------------------------------------------------------- */
    if (bReadOnly)
        hDataset = NULL;
    else
    {
        CPLPushErrorHandler( CPLQuietErrorHandler );
		hDataset = GdalHelper::OpenRasterDatasetW(OLE2W(bstrSrcFilename), GA_Update);
        CPLPopErrorHandler();
    }

    if( hDataset == NULL )
	{
		hDataset = GdalHelper::OpenRasterDatasetW(OLE2W(bstrSrcFilename), GA_ReadOnly);
	}

    if( hDataset == NULL )
	{
		return ResetConfigOptions(tkGDAL_ERROR);
	}

/* -------------------------------------------------------------------- */
/*      Clean overviews.                                                */
/* -------------------------------------------------------------------- */
    if ( bClean &&
        GDALBuildOverviews( hDataset,pszResampling, 0, 0, 
                             0, NULL, pfnProgress, &params ) != CE_None )
    {
		this->_lastErrorCode = tkGDAL_ERROR;
        CPLError(CE_Failure,0,"Cleaning overviews failed.");
        nResultStatus = 200;
    }

/* -------------------------------------------------------------------- */
/*      Generate overviews.                                             */
/* -------------------------------------------------------------------- */
    if (nLevelCount > 0 && nResultStatus == 0 &&
        GDALBuildOverviews( hDataset,pszResampling, nLevelCount, anLevels,
                             0, NULL, pfnProgress, &params ) != CE_None )
    {
		this->_lastErrorCode = tkGDAL_ERROR;
        CPLError(CE_Failure,0,"Overview building failed.");
        nResultStatus = 100;
    }

/* -------------------------------------------------------------------- */
/*      Cleanup                                                         */
/* -------------------------------------------------------------------- */
    GDALClose(hDataset);

	if (nResultStatus == 0)
		(*retval) = VARIANT_TRUE;

    return ResetConfigOptions();
}

#pragma endregion

#pragma region gdal_polygonize

/******************************************************************************
 * $Id: gdal_polygonize.py 15700 2008-11-10 15:29:13Z warmerdam $
 *
 * Project:  GDAL Python Interface
 * Purpose:  Application for converting raster data to a vector polygon layer.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2008, Frank Warmerdam
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/
STDMETHODIMP CUtils::Polygonize(BSTR pszSrcFilename, BSTR pszDstFilename,
								int iSrcBand, VARIANT_BOOL NoMask,
								BSTR pszMaskFilename, BSTR pszOGRFormat,
								BSTR pszDstLayerName, BSTR pszPixValFieldName,
								ICallback * cBack, VARIANT_BOOL * retval)
{
	USES_CONVERSION;

	GDALAllRegister();
	OGRRegisterAll();

/* -------------------------------------------------------------------- */
/*      Open source file.                                               */
/* -------------------------------------------------------------------- */
	GDALDatasetH hSrcDS = GdalHelper::OpenRasterDatasetW(OLE2W(pszSrcFilename), GA_ReadOnly);
	if( hSrcDS == NULL )
	{
		(*retval) = VARIANT_FALSE;
		return S_OK;
	}

	GDALRasterBandH hSrcBand = GDALGetRasterBand( hSrcDS, iSrcBand );
	if( hSrcBand == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
			      "Band %d does not exist on dataset.", iSrcBand );
		(*retval) = VARIANT_FALSE;
		return S_OK;
    }

	GDALRasterBandH hMaskBand;
	GDALDatasetH hMaskDS = NULL;
	if( NoMask )
	{
		hMaskBand = NULL;
	}
	else if( pszMaskFilename == NULL) // default mask
	{
		hMaskBand = GDALGetMaskBand( hSrcBand );
	}
	else
	{
		hMaskDS = GdalHelper::OpenRasterDatasetW(OLE2W(pszMaskFilename), GA_ReadOnly);
		hMaskBand = GDALGetRasterBand( hMaskDS, 1 );
	}

/* -------------------------------------------------------------------- */
/*      Try opening the destination file as an existing file.           */
/* -------------------------------------------------------------------- */
	CPLPushErrorHandler(CPLQuietErrorHandler);
	OGRDataSourceH hDstDS =  OGROpen( OLE2A(pszDstFilename), TRUE, NULL );
	CPLPopErrorHandler();

/* -------------------------------------------------------------------- */
/*      Create output file.                                             */
/* -------------------------------------------------------------------- */
	if( hDstDS == NULL )
	{
		OGRSFDriverH hDriver = OGRGetDriverByName( OLE2A(pszOGRFormat) );
		if( hDriver == NULL )
		{
			(*retval) = VARIANT_FALSE;
			return S_OK;
		}

		hDstDS = OGR_Dr_CreateDataSource( hDriver, OLE2A(pszDstFilename),
										  NULL );
		if( hDstDS == NULL )
		{
			(*retval) = VARIANT_FALSE;
			return S_OK;
		}
	}

/* -------------------------------------------------------------------- */
/*      Find or create destination layer.                               */
/* -------------------------------------------------------------------- */
	__int8 dst_field = -1;

	OGRLayerH hDstLayer = OGR_DS_GetLayerByName( hDstDS,
												 OLE2A(pszDstLayerName) );
	if( hDstLayer == NULL )
	{
		OGRSpatialReferenceH hSRS = NULL;
		const char *pszWkt = GDALGetProjectionRef(hSrcDS);
		if( pszWkt != NULL && _tcslen(pszWkt) != 0 )
		{
			hSRS = OSRNewSpatialReference( pszWkt );
		}

		hDstLayer = OGR_DS_CreateLayer( hDstDS, OLE2A(pszDstLayerName), hSRS,
										wkbUnknown, NULL );
		if( hDstLayer == NULL )
		{
			(*retval) = VARIANT_FALSE;
			return S_OK;
		}

		OGRFieldDefnH hFld = OGR_Fld_Create( OLE2A(pszPixValFieldName),
											 OFTInteger );
		OGR_Fld_SetWidth( hFld, 8 );
		OGR_L_CreateField( hDstLayer, hFld, TRUE );
		OGR_Fld_Destroy( hFld );
		dst_field = 0;
	}

/* -------------------------------------------------------------------- */
/*      Invoke algorithm.                                               */
/* -------------------------------------------------------------------- */
	struct CallbackParams params(GetCallback(), "Polygonizing");

	GDALPolygonize( hSrcBand, hMaskBand, hDstLayer, dst_field, NULL,
					(GDALProgressFunc) GDALProgressCallback, &params );

	OGR_DS_Destroy( hDstDS );
	GDALClose( hMaskDS );
	GDALClose( hSrcDS );

	(*retval) = VARIANT_TRUE;
	return ResetConfigOptions();
}

#pragma endregion

#pragma region gdal_contour

/******************************************************************************
 * $Id: gdal_contour.cpp,v 1.10 2005/02/03 17:28:37 fwarmerdam Exp $
 *
 * Project:  Contour Generator
 * Purpose:  Contour Generator mainline.
 * Author:   Frank Warmerdam <warmerdam@pobox.com>
 *
 ******************************************************************************
 * Copyright (c) 2003, Applied Coherent Technology (www.actgate.com). 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 ******************************************************************************/

STDMETHODIMP CUtils::GenerateContour(BSTR pszSrcFilename, BSTR pszDstFilename, double dfInterval, double dfNoData, VARIANT_BOOL Is3D, VARIANT dblFLArray, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	GDALDatasetH hSrcDS;
    int b3D = FALSE;
    int nBandIn = 1;
    double dfOffset = 0.0;
    const char *pszElevAttrib = "Grid_value";
    const char *pszFormat = "ESRI Shapefile";
    double adfFixedLevels[1000];
    int    nFixedLevelCount = 0;
	GDALProgressFunc pfnProgress = (GDALProgressFunc) GDALProgressCallback;
	struct CallbackParams params(GetCallback(), "Generating Contour");

	if (Is3D == VARIANT_TRUE)
		b3D = TRUE;

    GDALAllRegister();
    OGRRegisterAll();

	//Added by Rob Cairns 4-Mar-06
	if ( dblFLArray.vt == (VT_ARRAY | VT_R8) )
	{	
		long Dims = SafeArrayGetDim(dblFLArray.parray);
		long UpperBounds;
		long LowerBounds;
		SafeArrayGetLBound(dblFLArray.parray, 1, &LowerBounds);
		SafeArrayGetUBound(dblFLArray.parray, 1, &UpperBounds);
		if (Dims ==1 && UpperBounds > 0) 
		{
			double *arrVal;
			SafeArrayAccessData (dblFLArray.parray, (void**)&arrVal);
			for (int i = 0; i < UpperBounds; i++) 
			{
				adfFixedLevels[i] = arrVal[i];
				nFixedLevelCount++;
				arrVal[i] = 0;
			}
			SafeArrayUnaccessData (dblFLArray.parray);
		}
	}


    if( dfInterval == 0.0 && nFixedLevelCount == 0 )
    {
		(*retval) = VARIANT_FALSE;
        return S_OK;
    }

/* -------------------------------------------------------------------- */
/*      Open source raster file.                                        */
/* -------------------------------------------------------------------- */
    GDALRasterBandH hBand;

	hSrcDS = GdalHelper::OpenRasterDatasetW(OLE2W(pszSrcFilename), GA_ReadOnly);
    if( hSrcDS == NULL )
	{
		(*retval) = VARIANT_FALSE;
		return S_OK;
	}

    hBand = GDALGetRasterBand( hSrcDS, nBandIn );
    if( hBand == NULL )
    {
		this->_lastErrorCode = tkGDAL_ERROR;
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "Band %d does not exist on dataset.", 
                  nBandIn );
		(*retval) = VARIANT_FALSE;
		return S_OK;
    }

    //if( !bNoDataSet && !bIgnoreNoData )
	/*int bNoDataSet = FALSE;
    double val = GDALGetRasterNoDataValue( hBand, &bNoDataSet );
	if (bNoDataSet)	
		dfNoData = val;*/

/* -------------------------------------------------------------------- */
/*      Try to get a coordinate system from the raster.                 */
/* -------------------------------------------------------------------- */
    OGRSpatialReferenceH hSRS = NULL;

    const char *pszWKT = GDALGetProjectionRef( hBand );

    if( pszWKT != NULL && _tcslen(pszWKT) != 0 )
        hSRS = OSRNewSpatialReference( pszWKT );

/* -------------------------------------------------------------------- */
/*      Create the outputfile.                                          */
/* -------------------------------------------------------------------- */
    OGRDataSourceH hDS;
    OGRSFDriverH hDriver = OGRGetDriverByName( pszFormat );
    OGRFieldDefnH hFld;
    OGRLayerH hLayer;
    int nElevField = -1;

    if( hDriver == NULL )
    {
		this->_lastErrorCode = tkGDAL_ERROR;
        CPLError( CE_Failure, CPLE_AppDefined, "Unable to find format driver named %s.\n", 
                 pszFormat );
		(*retval) = VARIANT_FALSE;
        return S_OK;
    }

    hDS = OGR_Dr_CreateDataSource( hDriver, OLE2A(pszDstFilename), NULL );
    if( hDS == NULL )
	{
		this->_lastErrorCode = tkGDAL_ERROR;
		(*retval) = VARIANT_FALSE;
        return S_OK;
	}

    hLayer = OGR_DS_CreateLayer( hDS, "contour", hSRS, 
                                 b3D ? wkbLineString25D : wkbLineString,
                                 NULL );
    if( hLayer == NULL )
	{
		this->_lastErrorCode = tkGDAL_ERROR;
		(*retval) = VARIANT_FALSE;
        return S_OK;
	}

    hFld = OGR_Fld_Create( "ID", OFTInteger );
    OGR_Fld_SetWidth( hFld, 8 );
    OGR_L_CreateField( hLayer, hFld, FALSE );
    OGR_Fld_Destroy( hFld );

    if( pszElevAttrib )
    {
        hFld = OGR_Fld_Create( pszElevAttrib, OFTReal );
        OGR_Fld_SetWidth( hFld, 12 );
        OGR_Fld_SetPrecision( hFld, 3 );
        OGR_L_CreateField( hLayer, hFld, FALSE );
        OGR_Fld_Destroy( hFld );
        nElevField = 1;
    }

/* -------------------------------------------------------------------- */
/*      Invoke.                                                         */
/* -------------------------------------------------------------------- */
    CPLErr eErr;
    
	bool bNoDataSet = true;
    eErr = GDALContourGenerate( hBand, dfInterval, dfOffset, 
                                nFixedLevelCount, adfFixedLevels,
                                bNoDataSet, dfNoData, 
                                hLayer, 0, nElevField,
                                pfnProgress, &params );

    OGR_DS_Destroy( hDS );
    GDALClose( hSrcDS );

	(*retval) = VARIANT_TRUE;
	return ResetConfigOptions();
}

#pragma endregion