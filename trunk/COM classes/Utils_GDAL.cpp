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
#include "cpl_conv.h"
#include "cpl_multiproc.h"
#include "ogr_spatialref.h"
#include "ogr_api.h"
#include "ogr_srs_api.h"
#include "cpl_vsi.h"
#include "cpl_string.h"
#include "vrtdataset.h"
#include "direct.h"

#pragma warning(disable:4996)

#pragma region common

struct CallbackParams
{
	ICallback *cBack;
	const char *sMsg;
};

static int CPL_STDCALL GDALProgressCallback (double dfComplete, const char* pszMessage, void *pData)
{
	CallbackParams* params = (CallbackParams*)pData;

	if( params != NULL && params->cBack != NULL )
	{
		long percent = long(dfComplete * 100.0);

		if (params->sMsg != NULL)
			params->cBack->Progress(NULL,percent,A2BSTR(params->sMsg));
		else
			params->cBack->Progress(NULL,percent,NULL);
	}
	return TRUE;
}

#pragma endregion

#pragma region gdalinfo

static CString 
GDALInfoReportCorner( GDALDatasetH hDataset, 
                      OGRCoordinateTransformationH hTransform,
                      const char * corner_name,
                      double x, double y );

STDMETHODIMP CUtils::GDALInfo(BSTR bstrSrcFilename, BSTR bstrOptions,
							  ICallback * cBack, BSTR *bstrInfo)
{
	USES_CONVERSION;

	int				opts = 0;
	CString			sOutput = "", sTemp = "";
	GDALDatasetH	hDataset;
	GDALRasterBandH	hBand;
	int				iBand;
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

	Parse(OLE2CA(bstrOptions), &opts);

	for (int i = 1; i < opts; i++)
	{
		if( sArr[i] == "-mm" )
            bComputeMinMax = TRUE;
        else if( sArr[i] == "-hist" )
            bReportHistograms = TRUE;
        else if( sArr[i] == "-proj4" )
            bReportProj4 = TRUE;
        else if( sArr[i] == "-stats" )
        {
            bStats = TRUE;
            bApproxStats = FALSE;
        }
        else if( sArr[i] == "-approx_stats" )
        {
            bStats = TRUE;
            bApproxStats = TRUE;
        }
        else if( sArr[i] == "-sample" )
            bSample = TRUE;
        else if( sArr[i] == "-checksum" )
            bComputeChecksum = TRUE;
        else if( sArr[i] == "-nogcp" )
            bShowGCPs = FALSE;
        else if( sArr[i] == "-nomd" )
            bShowMetadata = FALSE;
        else if( sArr[i] == "-norat" )
            bShowRAT = FALSE;
        else if( sArr[i] == "-noct" )
            bShowColorTable = FALSE;
        else if( sArr[i] == "-mdd" && i < opts-1 )
            papszExtraMDDomains = CSLAddString( papszExtraMDDomains,
                                                sArr[++i].GetBuffer(0) );
        else if( sArr[i] == "-nofl" )
            bShowFileList = FALSE;
        else if( sArr[i] == "-sd" && i < opts-1 )
            nSubdataset = atoi(sArr[++i].GetBuffer (0));
		else if( sArr[i] == "--version" )
		{
			sOutput += GDALVersionInfo( "--version" );

			*bstrInfo = sOutput.AllocSysString();
			return S_OK;
		}
		else if( sArr[i] == "--formats" )
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

				sTemp.Format( "  %s (%s): %s\n",
					GDALGetDriverShortName( hDriver ),
					pszRWFlag,
					GDALGetDriverLongName( hDriver ) );
				sOutput += sTemp;
			}

			*bstrInfo = sOutput.AllocSysString();
			return S_OK;
		}
		else if( sArr[i] == "--format" )
		{
			GDALDriverH hDriver;
			char **papszMD;

			if( i + 1 >= opts )
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError( CE_Failure, CPLE_AppDefined, 
					"--format option given without a format code." );
				return S_OK;
			}

			hDriver = GDALGetDriverByName( sArr[i+1].GetBuffer(0) );
			if( hDriver == NULL )
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError( CE_Failure, CPLE_AppDefined, 
					"--format option given with format '%s', but that format not\n"
					"recognised.  Use the --formats option to get a list of available formats,\n"
					"and use the short code (ie. GTiff or HFA) as the format identifier.\n", 
					sArr[i+1].GetBuffer(0) );
				return S_OK;
			}

			sOutput += "Format Details:\n";
			sTemp.Format( "  Short Name: %s\n", GDALGetDriverShortName( hDriver ) );
			sOutput += sTemp;
			sTemp.Format( "  Long Name: %s\n", GDALGetDriverLongName( hDriver ) );
			sOutput += sTemp;

			papszMD = GDALGetMetadata( hDriver, NULL );

			if( CSLFetchNameValue( papszMD, GDAL_DMD_EXTENSION ) )
			{
				sTemp.Format( "  Extension: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_EXTENSION ) );
				sOutput += sTemp;
			}
			if( CSLFetchNameValue( papszMD, GDAL_DMD_MIMETYPE ) )
			{
				sTemp.Format( "  Mime Type: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_MIMETYPE ) );
				sOutput += sTemp;
			}
			if( CSLFetchNameValue( papszMD, GDAL_DMD_HELPTOPIC ) )
			{
				sTemp.Format( "  Help Topic: %s\n", 
					CSLFetchNameValue( papszMD, GDAL_DMD_HELPTOPIC ) );
				sOutput += sTemp;
			}

			if( CSLFetchNameValue( papszMD, GDAL_DCAP_CREATE ) )
				sOutput += "  Supports: Create() - Create writeable dataset.\n";
			if( CSLFetchNameValue( papszMD, GDAL_DCAP_CREATECOPY ) )
				sOutput += "  Supports: CreateCopy() - Create dataset by copying another.\n";
			if( CSLFetchNameValue( papszMD, GDAL_DMD_CREATIONDATATYPES ) )
			{
				sTemp.Format( "  Creation Datatypes: %s\n",
					CSLFetchNameValue( papszMD, GDAL_DMD_CREATIONDATATYPES ) );
				sOutput += sTemp;
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

				sTemp.Format( "\n%s\n", pszFormattedXML );
				sOutput += sTemp;
				CPLFree( pszFormattedXML );
			}

			*bstrInfo = sOutput.AllocSysString();
			return S_OK;
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Open dataset.                                                   */
	/* -------------------------------------------------------------------- */
    hDataset = GDALOpen( pszFilename, GA_ReadOnly );
    
    if( hDataset == NULL )
    {
		lastErrorCode = tkGDAL_ERROR;
        CPLError(CE_Failure,0,
                 "gdalinfo failed - unable to open '%s'.\n",
                 pszFilename );

        CSLDestroy( papszExtraMDDomains );
    
        GDALDumpOpenDatasets( stderr );

        CPLDumpSharedList( NULL );

        return S_OK;
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
            hDataset = GDALOpen( pszSubdatasetName, GA_ReadOnly );
            CPLFree( pszSubdatasetName );
        }
        else
        {
			lastErrorCode = tkGDAL_ERROR;
            CPLError(CE_Failure,0,
                     "gdalinfo warning: subdataset %d of %d requested. "
                     "Reading the main dataset.\n",
                     nSubdataset, nSubdatasets );
			return S_OK;
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report general info.                                            */
	/* -------------------------------------------------------------------- */
    hDriver = GDALGetDatasetDriver( hDataset );
    sTemp.Format( "Driver: %s/%s\n",
            GDALGetDriverShortName( hDriver ),
            GDALGetDriverLongName( hDriver ) );
	sOutput += sTemp;

    papszFileList = GDALGetFileList( hDataset );
    if( CSLCount(papszFileList) == 0 )
    {
        sOutput += "Files: none associated\n";
    }
    else
    {
        sTemp.Format( "Files: %s\n", papszFileList[0] );
		sOutput += sTemp;
        if( bShowFileList )
        {
            for( int i = 1; papszFileList[i] != NULL; i++ )
			{
                sTemp.Format( "       %s\n", papszFileList[i] );
				sOutput += sTemp;
			}
        }
    }
    CSLDestroy( papszFileList );

    sTemp.Format( "Size is %d, %d\n",
            GDALGetRasterXSize( hDataset ), 
            GDALGetRasterYSize( hDataset ) );
	sOutput += sTemp;

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
            sTemp.Format( "Coordinate System is:\n%s\n", pszPrettyWkt );
			sOutput += sTemp;
            CPLFree( pszPrettyWkt );
        }
        else
		{
            sTemp.Format( "Coordinate System is `%s'\n",
                    GDALGetProjectionRef( hDataset ) );
			sOutput += sTemp;
		}

        if ( bReportProj4 ) 
        {
            char *pszProj4 = NULL;
            OSRExportToProj4( hSRS, &pszProj4 );
            sTemp.Format("PROJ.4 string is:\n\'%s\'\n",pszProj4);
			sOutput += sTemp;
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
            sTemp.Format( "Origin = (%.15f,%.15f)\n",
                    adfGeoTransform[0], adfGeoTransform[3] );
			sOutput += sTemp;

            sTemp.Format( "Pixel Size = (%.15f,%.15f)\n",
                    adfGeoTransform[1], adfGeoTransform[5] );
			sOutput += sTemp;
        }
        else
		{
            sTemp.Format( "GeoTransform =\n"
                    "  %.16g, %.16g, %.16g\n"
                    "  %.16g, %.16g, %.16g\n", 
                    adfGeoTransform[0],
                    adfGeoTransform[1],
                    adfGeoTransform[2],
                    adfGeoTransform[3],
                    adfGeoTransform[4],
                    adfGeoTransform[5] );
			sOutput += sTemp;
		}
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
                sTemp.Format( "GCP Projection = \n%s\n", pszPrettyWkt );
				sOutput += sTemp;
                CPLFree( pszPrettyWkt );
            }
            else
			{
                sTemp.Format( "GCP Projection = %s\n",
                        GDALGetGCPProjection( hDataset ) );
				sOutput += sTemp;
			}

            OSRDestroySpatialReference( hSRS );
        }

        for( int i = 0; i < GDALGetGCPCount(hDataset); i++ )
        {
            const GDAL_GCP	*psGCP;
            
            psGCP = GDALGetGCPs( hDataset ) + i;

            sTemp.Format( "GCP[%3d]: Id=%s, Info=%s\n"
                    "          (%.15g,%.15g) -> (%.15g,%.15g,%.15g)\n", 
                    i, psGCP->pszId, psGCP->pszInfo, 
                    psGCP->dfGCPPixel, psGCP->dfGCPLine, 
                    psGCP->dfGCPX, psGCP->dfGCPY, psGCP->dfGCPZ );
			sOutput += sTemp;
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report metadata.                                                */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, NULL ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Metadata:\n";
        for( int i = 0; papszMetadata[i] != NULL; i++ )
        {
            sTemp.Format( "  %s\n", papszMetadata[i] );
			sOutput += sTemp;
        }
    }

    for( iMDD = 0; bShowMetadata && iMDD < CSLCount(papszExtraMDDomains); iMDD++ )
    {
        papszMetadata = GDALGetMetadata( hDataset, papszExtraMDDomains[iMDD] );
        if( CSLCount(papszMetadata) > 0 )
        {
            sTemp.Format( "Metadata (%s):\n", papszExtraMDDomains[iMDD]);
			sOutput += sTemp;
            for( int i = 0; papszMetadata[i] != NULL; i++ )
            {
                if (EQUALN(papszExtraMDDomains[iMDD], "xml:", 4))
                    sTemp.Format( "%s\n", papszMetadata[i] );
                else
                    sTemp.Format( "  %s\n", papszMetadata[i] );

				sOutput += sTemp;
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
        for( int i = 0; papszMetadata[i] != NULL; i++ )
        {
            sTemp.Format( "  %s\n", papszMetadata[i] );
			sOutput += sTemp;
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report subdatasets.                                             */
	/* -------------------------------------------------------------------- */
    papszMetadata = GDALGetMetadata( hDataset, "SUBDATASETS" );
    if( CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Subdatasets:\n";
        for( int i = 0; papszMetadata[i] != NULL; i++ )
        {
            sTemp.Format( "  %s\n", papszMetadata[i] );
			sOutput += sTemp;
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report geolocation.                                             */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, "GEOLOCATION" ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "Geolocation:\n";
        for( int i = 0; papszMetadata[i] != NULL; i++ )
        {
            sTemp.Format( "  %s\n", papszMetadata[i] );
			sOutput += sTemp;
        }
    }

	/* -------------------------------------------------------------------- */
	/*      Report RPCs                                                     */
	/* -------------------------------------------------------------------- */
    papszMetadata = (bShowMetadata) ? GDALGetMetadata( hDataset, "RPC" ) : NULL;
    if( bShowMetadata && CSLCount(papszMetadata) > 0 )
    {
        sOutput += "RPC Metadata:\n";
        for( int i = 0; papszMetadata[i] != NULL; i++ )
        {
            sTemp.Format( "  %s\n", papszMetadata[i] );
			sOutput += sTemp;
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
            sTemp.Format( "Got %d samples.\n", nCount );
			sOutput + sTemp;
        }
        
        GDALGetBlockSize( hBand, &nBlockXSize, &nBlockYSize );
        sTemp.Format( "Band %d Block=%dx%d Type=%s, ColorInterp=%s\n", iBand+1,
                nBlockXSize, nBlockYSize,
                GDALGetDataTypeName(
                    GDALGetRasterDataType(hBand)),
                GDALGetColorInterpretationName(
                    GDALGetRasterColorInterpretation(hBand)) );
		sOutput += sTemp;

        if( GDALGetDescription( hBand ) != NULL 
            && strlen(GDALGetDescription( hBand )) > 0 )
		{
            sTemp.Format( "  Description = %s\n", GDALGetDescription(hBand) );
			sOutput += sTemp;
		}

        dfMin = GDALGetRasterMinimum( hBand, &bGotMin );
        dfMax = GDALGetRasterMaximum( hBand, &bGotMax );
        if( bGotMin || bGotMax || bComputeMinMax )
        {
            sOutput += "  " ;
            if( bGotMin )
			{
                sTemp.Format( "Min=%.3f ", dfMin );
				sOutput += sTemp;
			}
            if( bGotMax )
			{
                sTemp.Format( "Max=%.3f ", dfMax );
				sOutput += sTemp;
			}
        
            if( bComputeMinMax )
            {
                CPLErrorReset();
                GDALComputeRasterMinMax( hBand, FALSE, adfCMinMax );
                if (CPLGetLastErrorType() == CE_None)
                {
                  sTemp.Format( "  Computed Min/Max=%.3f,%.3f", 
                          adfCMinMax[0], adfCMinMax[1] );
				  sOutput += sTemp;
                }
            }

            sOutput += "\n" ;
        }

        eErr = GDALGetRasterStatistics( hBand, bApproxStats, bStats, 
                                        &dfMin, &dfMax, &dfMean, &dfStdDev );
        if( eErr == CE_None )
        {
            sTemp.Format( "  Minimum=%.3f, Maximum=%.3f, Mean=%.3f, StdDev=%.3f\n",
                    dfMin, dfMax, dfMean, dfStdDev );
			sOutput += sTemp;
        }

        if( bReportHistograms )
        {
            int nBucketCount, *panHistogram = NULL;
			struct CallbackParams params = { cBack, NULL };

            eErr = GDALGetDefaultHistogram( hBand, &dfMin, &dfMax, 
                                            &nBucketCount, &panHistogram, 
                                            TRUE, (GDALProgressFunc) GDALProgressCallback, &params );
            if( eErr == CE_None )
            {
                int iBucket;

                sTemp.Format( "  %d buckets from %g to %g:\n  ",
                        nBucketCount, dfMin, dfMax );
				sOutput += sTemp;

                for( iBucket = 0; iBucket < nBucketCount; iBucket++ )
				{
                    sTemp.Format( "%d ", panHistogram[iBucket] );
					sOutput += sTemp;
				}
                sOutput += "\n" ;
                CPLFree( panHistogram );
            }
        }

        if ( bComputeChecksum)
        {
            sTemp.Format( "  Checksum=%d\n",
                    GDALChecksumImage(hBand, 0, 0,
                                      GDALGetRasterXSize(hDataset),
                                      GDALGetRasterYSize(hDataset)));
			sOutput += sTemp;
        }

        dfNoData = GDALGetRasterNoDataValue( hBand, &bGotNodata );
        if( bGotNodata )
        {
            if (CPLIsNan(dfNoData))
                sTemp.Format( "  NoData Value=nan\n" );
            else
                sTemp.Format( "  NoData Value=%.18g\n", dfNoData );
			sOutput += sTemp;
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
                    sTemp.Format( "%dx%d", 
                            GDALGetRasterBandXSize( hOverview ),
                            GDALGetRasterBandYSize( hOverview ) );
					sOutput += sTemp;

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
					{
                        sTemp.Format( "%d",
                                GDALChecksumImage(hOverview, 0, 0,
                                        GDALGetRasterBandXSize(hOverview),
                                        GDALGetRasterBandYSize(hOverview)));
						sOutput += sTemp;
					}
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
                    sTemp.Format( "%dx%d", 
                            GDALGetRasterBandXSize( hOverview ),
                            GDALGetRasterBandYSize( hOverview ) );
					sOutput += sTemp;
                }
                sOutput += "\n";
            }
        }

        if( strlen(GDALGetRasterUnitType(hBand)) > 0 )
        {
            sTemp.Format( "  Unit Type: %s\n", GDALGetRasterUnitType(hBand) );
			sOutput += sTemp;
        }

        if( GDALGetRasterCategoryNames(hBand) != NULL )
        {
            char **papszCategories = GDALGetRasterCategoryNames(hBand);
            int i;

            sOutput += "  Categories:\n";
            for( i = 0; papszCategories[i] != NULL; i++ )
			{
                sTemp.Format( "    %3d: %s\n", i, papszCategories[i] );
				sOutput += sTemp;
			}
        }

        if( GDALGetRasterScale( hBand, &bSuccess ) != 1.0 
            || GDALGetRasterOffset( hBand, &bSuccess ) != 0.0 )
		{
            sTemp.Format( "  Offset: %.15g,   Scale:%.15g\n",
                    GDALGetRasterOffset( hBand, &bSuccess ),
                    GDALGetRasterScale( hBand, &bSuccess ) );
			sOutput += sTemp;
		}

        papszMetadata = (bShowMetadata) ? GDALGetMetadata( hBand, NULL ) : NULL;
        if( bShowMetadata && CSLCount(papszMetadata) > 0 )
        {
            sOutput += "  Metadata:\n";
            for( int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTemp.Format( "    %s\n", papszMetadata[i] );
				sOutput += sTemp;
            }
        }

        papszMetadata = (bShowMetadata) ? GDALGetMetadata( hBand, "IMAGE_STRUCTURE" ) : NULL;
        if( bShowMetadata && CSLCount(papszMetadata) > 0 )
        {
            sOutput += "  Image Structure Metadata:\n";
            for( int i = 0; papszMetadata[i] != NULL; i++ )
            {
                sTemp.Format( "    %s\n", papszMetadata[i] );
				sOutput += sTemp;
            }
        }

        if( GDALGetRasterColorInterpretation(hBand) == GCI_PaletteIndex 
            && (hTable = GDALGetRasterColorTable( hBand )) != NULL )
        {
            sTemp.Format( "  Color Table (%s with %d entries)\n", 
                    GDALGetPaletteInterpretationName(
                        GDALGetPaletteInterpretation( hTable )), 
                    GDALGetColorEntryCount( hTable ) );
			sOutput += sTemp;

            if (bShowColorTable)
            {
                for( int i = 0; i < GDALGetColorEntryCount( hTable ); i++ )
                {
                    GDALColorEntry	sEntry;
    
                    GDALGetColorEntryAsRGB( hTable, i, &sEntry );
                    sTemp.Format( "  %3d: %d,%d,%d,%d\n", 
                            i, 
                            sEntry.c1,
                            sEntry.c2,
                            sEntry.c3,
                            sEntry.c4 );
					sOutput += sTemp;
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

	return S_OK;
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
 * $Id: gdal_translate.cpp,v 1.32 2005/05/17 19:04:47 fwarmerdam Exp $
 *
 * Project:  GDAL Utilities
 * Purpose:  GDAL Image Translator Program
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 * ****************************************************************************
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
 * ****************************************************************************/

static int ArgIsNumeric( const char * );
static void AttachMetadata( GDALDatasetH, char ** );
static void CopyBandInfo( GDALRasterBand * poSrcBand, GDALRasterBand * poDstBand,
                            int bCanCopyStatsMetadata, int bCopyScale, int bCopyNoData );

/************************************************************************/
/*                             TranslateRaster()                        */
/************************************************************************/

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

	int				opts = 0;
	GDALDatasetH	hDataset, hOutDS;
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

	if (!bSubCall)
		Parse(OLE2CA(bstrOptions), &opts);
	else
		opts = sArr.GetSize();

/* -------------------------------------------------------------------- */
/*      Register standard GDAL drivers, and process generic GDAL        */
/*      command options.                                                */
/* -------------------------------------------------------------------- */
	GDALAllRegister();

	/* -------------------------------------------------------------------- */
	/*		Handle command line arguments.									*/
	/* -------------------------------------------------------------------- */
	for (int i = 0; i < opts; i++)
	{
		if (sArr[i] == "-of" && i < opts-1)
		{
			pszFormat = sArr[++i].GetBuffer (0);
			bFormatExplicitelySet = TRUE;
		}
		else if (sArr[i] == "-ot" && i < opts-1)
		{
			for (int iType = 1; iType < GDT_TypeCount; iType++)
			{
				if (GDALGetDataTypeName((GDALDataType)iType) != NULL &&
					sArr[i+1] == GDALGetDataTypeName((GDALDataType)iType))
				{
					eOutputType = (GDALDataType) iType;
				}
			}

			if (eOutputType == GDT_Unknown)
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError(CE_Failure,0,"Unknown output pixel type: %s", sArr[i+1]);
				return S_OK;
			}
			i++;
		}
		else if (sArr[i] == "-b" && i < opts-1 )
		{
			const char* pszBand = sArr[i+1].GetBuffer (0);
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
				lastErrorCode = tkGDAL_ERROR;
				CPLError(CE_Failure,0,"Unrecognizable band number (%s).", sArr[i+1]);
				// TODO: Usage();
				return S_OK;
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
		else if (sArr[i] == "-mask" && i < opts-1)
		{
			bParsedMaskArgument = TRUE;
			const char* pszBand = sArr[i+1].GetBuffer (0);
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
					lastErrorCode = tkGDAL_ERROR;
					CPLError(CE_Failure, 0, "Unrecognizable band number (%s).", sArr[i+1]);
					// TODO: Usage();
					return S_OK;
				}

				eMaskMode = MASK_USER;
				nMaskBand = nBand;
				if (bMask)
					nMaskBand *= -1;
			}
			i ++;
		}
		else if( sArr[i] == "-not_strict"  )
			bStrict = FALSE;

		else if(sArr[i] == "-strict" )
			bStrict = TRUE;

		else if( sArr[i] == "-sds"  )
			bCopySubDatasets = TRUE;

		else if( sArr[i] == "-gcp" && i < opts - 4 )
		{
			char* endptr = NULL;
			/* -gcp pixel line easting northing [elev] */

			nGCPCount++;
			pasGCPs = (GDAL_GCP *) 
				CPLRealloc( pasGCPs, sizeof(GDAL_GCP) * nGCPCount );
			GDALInitGCPs( 1, pasGCPs + nGCPCount - 1 );

			pasGCPs[nGCPCount-1].dfGCPPixel = CPLAtofM(sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPLine = CPLAtofM(sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPX = CPLAtofM(sArr[++i].GetBuffer (0));
			pasGCPs[nGCPCount-1].dfGCPY = CPLAtofM(sArr[++i].GetBuffer (0));
			if( opts > i+1 && // TODO: Is this valid?
				(CPLStrtod(sArr[i+1].GetBuffer (0), &endptr) != 0.0 || sArr[i+1][0] == '0') )
			{
				/* Check that last argument is really a number and not a filename */
				/* looking like a number (see ticket #863) */
				if (endptr && *endptr == 0)
					pasGCPs[nGCPCount-1].dfGCPZ = CPLAtofM(sArr[++i].GetBuffer (0));
			}

			/* should set id and info? */
		}   

		else if( sArr[i] == "-a_nodata" && i < opts - 1 )
		{
			if (sArr[i+1]== "none")
			{
				bUnsetNoData = TRUE;
			}
			else
			{
				bSetNoData = TRUE;
				dfNoDataReal = CPLAtofM(sArr[i+1].GetBuffer (0));
			}
			i += 1;
		}   

		else if( sArr[i] == "-a_ullr" && i < opts - 4 )
		{
			adfULLR[0] = CPLAtofM(sArr[i+1].GetBuffer (0));
			adfULLR[1] = CPLAtofM(sArr[i+2].GetBuffer (0));
			adfULLR[2] = CPLAtofM(sArr[i+3].GetBuffer (0));
			adfULLR[3] = CPLAtofM(sArr[i+4].GetBuffer (0));

			bGotBounds = TRUE;

			i += 4;
		} 

		else if (sArr[i] == "-co" && i < opts-1)
		{
			papszCreateOptions = CSLAddString(papszCreateOptions, sArr[++i].GetBuffer(0));
		}

		else if( sArr[i] =="-scale" )
		{
			bScale = TRUE;
			if( i < opts-2 && ArgIsNumeric(sArr[i+1].GetBuffer (0)) )
			{
				bHaveScaleSrc = TRUE;
				dfScaleSrcMin = CPLAtofM(sArr[i+1].GetBuffer (0));
				dfScaleSrcMax = CPLAtofM(sArr[i+2].GetBuffer (0));
				i += 2;
			}
			if( i < opts-2 && bHaveScaleSrc && ArgIsNumeric(sArr[i+1].GetBuffer (0)) )
			{
				dfScaleDstMin = CPLAtofM(sArr[i+1].GetBuffer (0));
				dfScaleDstMax = CPLAtofM(sArr[i+2].GetBuffer (0));
				i += 2;
			}
			else
			{
				dfScaleDstMin = 0.0;
				dfScaleDstMax = 255.999;
			}
		}   

		else if( sArr[i] == "-unscale" )
		{
			bUnscale = TRUE;
		}

		else if( sArr[i] =="-mo" && i < opts-1 )
		{
			papszMetadataOptions = CSLAddString( papszMetadataOptions,
				sArr[++i].GetBuffer (0) );
		}

		else if( sArr[i] =="-outsize" && i < opts-2 )
		{
			pszOXSize = sArr[++i].GetBuffer (0);
			pszOYSize = sArr[++i].GetBuffer (0);
		}   

		else if( sArr[i] =="-srcwin" && i < opts-4 )
		{
			anSrcWin[0] = atoi(sArr[++i].GetBuffer (0));
			anSrcWin[1] = atoi(sArr[++i].GetBuffer (0));
			anSrcWin[2] = atoi(sArr[++i].GetBuffer (0));
			anSrcWin[3] = atoi(sArr[++i].GetBuffer (0));
		}   

		else if( sArr[i] =="-projwin" && i < opts-4 )
		{
			dfULX = CPLAtofM(sArr[++i].GetBuffer (0));
			dfULY = CPLAtofM(sArr[++i].GetBuffer (0));
			dfLRX = CPLAtofM(sArr[++i].GetBuffer (0));
			dfLRY = CPLAtofM(sArr[++i].GetBuffer (0));
		}   

		else if( sArr[i] =="-a_srs" && i < opts-1 )
		{
			OGRSpatialReference oOutputSRS;

			if( oOutputSRS.SetFromUserInput( sArr[i+1].GetBuffer (0) ) != OGRERR_NONE )
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError(CE_Failure, 0, "Failed to process SRS definition: %s", sArr[i+1] );
				return S_OK;
			}

			oOutputSRS.exportToWkt( &pszOutputSRS );
			i++;
		}   

		else if( sArr[i] =="-expand" && i < opts-1 )
		{
			if (sArr[i+1] =="gray")
				nRGBExpand = 1;
			else if (sArr[i+1] =="rgb")
				nRGBExpand = 3;
			else if (sArr[i+1] == "rgba")
				nRGBExpand = 4;
			else
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError(CE_Failure, 0, "Value %s unsupported. Only gray, rgb or rgba are supported.", sArr[i+1] );
				// TODO: Usage();
				return S_OK;
			}
			i++;
		}

		else if( sArr[i] == "-stats" )
		{
			bStats = TRUE;
			bApproxStats = FALSE;
		}
		else if( sArr[i] == "-approx_stats" )
		{
			bStats = TRUE;
			bApproxStats = TRUE;
		}

		else if( sArr[i][0] == '-' )
		{
			lastErrorCode = tkGDAL_ERROR;
			CPLError(CE_Failure, 0, "Option %s incomplete, or not recognised.", sArr[i] );
			// TODO: Usage();
			return S_OK;
		}
	}

	if (pszDest == NULL)
	{
		lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure, 0, "No destination dataset specified.");
		return S_OK;
	}

	if (strcmp (pszSource, pszDest) == 0)
	{
		lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure, 0, "Source and destination datasets must be different.");
		return S_OK;
	}

	/* -------------------------------------------------------------------- */
	/*		Attempt to open source file.									*/
	/* -------------------------------------------------------------------- */
	hDataset = GDALOpenShared(pszSource, GA_ReadOnly);

	if (hDataset == NULL)
	{
		lastErrorCode = tkGDAL_ERROR;
		return S_OK;
	}

	/* -------------------------------------------------------------------- */
	/*		Handle subdatasets.												*/
	/* -------------------------------------------------------------------- */
	if (!bCopySubDatasets &&
		CSLCount(GDALGetMetadata(hDataset, "SUBDATASETS")) > 0 &&
		GDALGetRasterCount(hDataset) == 0)
	{
		lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure, 0,
			"Input file contains subdatasets. Please, select one of them for reading." );
		GDALClose(hDataset);
		return S_OK;
	}

	if (CSLCount(GDALGetMetadata(hDataset, "SUBDATASETS")) > 0 &&
		bCopySubDatasets)
	{
		char **papszSubdatasets = GDALGetMetadata(hDataset, "SUBDATASETS");
		char *pszSubDest = (char *) CPLMalloc (strlen (pszDest)+32);
		bool bOldSubCall = bSubCall;

		(*retval) = VARIANT_TRUE;

		bSubCall = true;
		for (int i = 0; papszSubdatasets[i] != NULL; i += 2)
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

		bSubCall = bOldSubCall;

		GDALClose(hDataset);

		if (!bSubCall)
		{
			GDALDumpOpenDatasets(stderr); // TODO: what does this do?
		}
		return S_OK;
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
	/*		Build band list to translate.									*/
	/* -------------------------------------------------------------------- */
	if (nBandCount == 0)
	{
		nBandCount = GDALGetRasterCount (hDataset);
		if (nBandCount == 0)
		{
			lastErrorCode = tkGDAL_ERROR;
			CPLError(CE_Failure, 0, "Input file has no bands, and so cannot be translated." );
			return S_OK;
		}

		panBandList = (int *) CPLMalloc (sizeof(int)*nBandCount);
		for (int i = 0; i < nBandCount; i++)
			panBandList[i] = i+1;
	}
	else
	{
		for (int i = 0; i < nBandCount; i++)
		{
			if (ABS(panBandList[i]) > GDALGetRasterCount (hDataset))
			{
				lastErrorCode = tkGDAL_ERROR;
				CPLError(CE_Failure, 0,
					"Band %d requested, but only bands 1 to %d available.",
					ABS(panBandList[i]), GDALGetRasterCount(hDataset) );
				return S_OK;
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
	if (dfULX != 0.0 || dfULY != 0.0 ||
		dfLRX != 0.0 || dfLRY != 0.0)
	{
		double adfGeoTransform[6] = {0.0};

		GDALGetGeoTransform (hDataset, adfGeoTransform);

		if (adfGeoTransform[2] != 0.0 || adfGeoTransform[4] != 0.0)
		{
			lastErrorCode = tkGDAL_ERROR;
			CPLError(CE_Failure, 0,
				"The -projwin option was used, but the geotransform is "
				"rotated. This configuration is not supported." );
			GDALClose (hDataset);
			CPLFree (panBandList);
			return S_OK;
		}

		anSrcWin[0] = (int)
			((dfULX - adfGeoTransform[0]) / adfGeoTransform[1] + 0.001);
		anSrcWin[1] = (int)
			((dfULY - adfGeoTransform[3]) / adfGeoTransform[5] + 0.001);

		anSrcWin[2] = (int) ((dfLRX - dfULX) / adfGeoTransform[1] + 0.5);
		anSrcWin[3] = (int) ((dfLRY - dfULY) / adfGeoTransform[5] + 0.5);

		if (anSrcWin[0] < 0 || anSrcWin[1] < 0 ||
			anSrcWin[0] + anSrcWin[2] > GDALGetRasterXSize(hDataset) ||
			anSrcWin[1] + anSrcWin[3] > GDALGetRasterYSize(hDataset))
		{
			lastErrorCode = tkGDAL_ERROR;
			CPLError(CE_Failure, 0,
				"Computed -srcwin falls outside raster size of %dx%d.",
				GDALGetRasterXSize(hDataset), 
				GDALGetRasterYSize(hDataset) );
			return S_OK;
		}
	}

	/* -------------------------------------------------------------------- */
	/*		Verify source window.											*/
	/* -------------------------------------------------------------------- */
	if (anSrcWin[0] < 0 || anSrcWin[1] < 0 ||
		anSrcWin[2] <= 0 || anSrcWin[3] <= 0 ||
		anSrcWin[0] + anSrcWin[2] > GDALGetRasterXSize(hDataset) ||
		anSrcWin[1] + anSrcWin[3] > GDALGetRasterYSize(hDataset))
	{
		lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure,0,
			"-srcwin %d %d %d %d falls outside raster size of %dx%d "
			"or is otherwise illegal.",
			anSrcWin[0],
			anSrcWin[1],
			anSrcWin[2],
			anSrcWin[3],
			GDALGetRasterXSize(hDataset), 
			GDALGetRasterYSize(hDataset));
		return S_OK;
	}

	/* -------------------------------------------------------------------- */
	/*		Find the output driver.											*/
	/* -------------------------------------------------------------------- */
	hDriver = GDALGetDriverByName (pszFormat);
	if (hDriver == NULL)
	{
		lastErrorCode = tkGDAL_ERROR;
		CPLError(CE_Failure,0, "Output driver `%s' not recognised.", pszFormat );

		// TODO: List valid drivers?

		GDALClose (hDataset);
		CPLFree (panBandList);
		CSLDestroy (papszCreateOptions);

		return S_OK;
	}

	/* -------------------------------------------------------------------- */
	/*		The short form is to CreateCopy(). We use this if the input		*/
	/*		matches the whole dataset. Eventually we should rewrite			*/
	/*		this entire program to use virtual datasets to construct a		*/
	/*		virtual input source to copy from.								*/
	/* -------------------------------------------------------------------- */
	int bSpatialArrangementPreserved = (
		anSrcWin[0] == 0 && anSrcWin[1] == 0 &&
		anSrcWin[2] == GDALGetRasterXSize (hDataset) &&
		anSrcWin[3] == GDALGetRasterYSize (hDataset) &&
		pszOXSize == NULL && pszOYSize == NULL);

	if (eOutputType == GDT_Unknown &&
		!bScale && !bUnscale &&
		CSLCount (papszMetadataOptions) == 0 && bDefBands &&
		eMaskMode == MASK_AUTO &&
		bSpatialArrangementPreserved &&
		nGCPCount == 0 && !bGotBounds &&
		pszOutputSRS == NULL && !bSetNoData && !bUnsetNoData &&
		nRGBExpand == 0 && !bStats)
	{
		struct CallbackParams params = { cBack, "Translating" };

		hOutDS = GDALCreateCopy (hDriver, pszDest, hDataset,
			bStrict, papszCreateOptions, (GDALProgressFunc)GDALProgressCallback, &params);

		if (hOutDS != NULL)
		{
			(*retval) = VARIANT_TRUE;
			GDALClose (hOutDS);
		}
		else
		{
			lastErrorCode = tkGDAL_ERROR;
		}

		GDALClose (hDataset);

		CPLFree (panBandList);

		if (!bSubCall)
		{
			GDALDumpOpenDatasets (stderr);
		}

		CSLDestroy (papszCreateOptions);

		return S_OK;
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

	/* -------------------------------------------------------------------- */
	/*		Make a virtual clone.											*/
	/* -------------------------------------------------------------------- */
	VRTDataset *poVDS = (VRTDataset *) VRTCreate(nOXSize, nOYSize);

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

	else if (GDALGetGeoTransform (hDataset, adfGeoTransform) == CE_None &&
		nGCPCount == 0)
	{
		adfGeoTransform[0] += anSrcWin[0] * adfGeoTransform[1] +
			anSrcWin[1] * adfGeoTransform[2];
		adfGeoTransform[3] += anSrcWin[0] * adfGeoTransform[4] +
			anSrcWin[1] * adfGeoTransform[5];

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

		for( int i = 0; i < nGCPs; i++ )
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
			CPLError(CE_Failure,0,"Error : band %d has no color table", ABS(panBandList[0]));
			GDALClose( hDataset );
			CPLFree( panBandList );
			CSLDestroy( papszCreateOptions );
			return S_OK;
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
				if (poEntry->c1 != poEntry->c2 || poEntry->c1 != poEntry->c2)
				{
					CPLError(CE_Warning,0,"Warning : color table contains non gray levels colors");
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
			CPLError(CE_Failure,0,"Error : invalid use of -expand option.");
			return S_OK;
		}
	}

	int bFilterOutStatsMetadata =
		(bScale || bUnscale || !bSpatialArrangementPreserved || nRGBExpand != 0);

	/* ==================================================================== */
	/*      Process all bands.                                              */
	/* ==================================================================== */
	for( int i = 0; i < nBandCount; i++ )
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
				printf( "for band %d, nodata value has been clamped "
					"to %.0f, the original value being out of range.",
					i + 1, dfVal);
			}
			else if(bRounded)
			{
				printf("for band %d, nodata value has been rounded "
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
	{
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
	}

	/* -------------------------------------------------------------------- */
	/*      Compute stats if required.                                      */
	/* -------------------------------------------------------------------- */
	if (bStats)
	{
		for( int i = 0; i < poVDS->GetRasterCount(); i++ )
		{
			double dfMin, dfMax, dfMean, dfStdDev;
			poVDS->GetRasterBand(i+1)->ComputeStatistics( bApproxStats,
				&dfMin, &dfMax, &dfMean, &dfStdDev, GDALDummyProgress, NULL );
		}
	}

	/* -------------------------------------------------------------------- */
	/*      Write to the output file using CopyCreate().                    */
	/* -------------------------------------------------------------------- */
	struct CallbackParams params = { cBack, "Translating" };

	hOutDS = GDALCreateCopy( hDriver, pszDest, (GDALDatasetH) poVDS,
		bStrict, papszCreateOptions, (GDALProgressFunc)GDALProgressCallback, &params );

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

	if( !bSubCall )
	{
		GDALDumpOpenDatasets( stderr );
	}

	CSLDestroy( papszCreateOptions );

	return S_OK;
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

STDMETHODIMP CUtils::GDALRasterize(BSTR bstrSrcFilename, BSTR pszDstFilename,
								   BSTR bstrOptions, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	this->lastErrorCode = tkMETHOD_NOT_IMPLEMENTED;
	(*retval) = VARIANT_FALSE;

	return S_OK;
}

#pragma endregion

#pragma region gdalwarp

STDMETHODIMP CUtils::GDALWarp(BSTR bstrSrcFilename, BSTR pszDstFilename, BSTR bstrOptions,
							 ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	this->lastErrorCode = tkMETHOD_NOT_IMPLEMENTED;
	(*retval) = VARIANT_FALSE;

	return S_OK;
}

#pragma endregion

#pragma region gdalbuildvrt

STDMETHODIMP CUtils::GDALBuildVrt(BSTR bstrSrcFilename, BSTR pszDstFilename,
								  BSTR bstrOptions, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	this->lastErrorCode = tkMETHOD_NOT_IMPLEMENTED;
	(*retval) = VARIANT_FALSE;

	return S_OK;
}

#pragma endregion

#pragma region gdaladdo

STDMETHODIMP CUtils::GDALAddOverviews(BSTR bstrSrcFilename, BSTR pszDstFilename,
									  BSTR bstrLevels, ICallback * cBack, VARIANT_BOOL *retval)
{
	USES_CONVERSION;

	this->lastErrorCode = tkMETHOD_NOT_IMPLEMENTED;
	(*retval) = VARIANT_FALSE;

	return S_OK;
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
	GDALDatasetH hSrcDS = GDALOpen( OLE2A(pszSrcFilename), GA_ReadOnly );
	if( hSrcDS == NULL )
	{
		(*retval) = VARIANT_FALSE;
		return S_FALSE;
	}

	GDALRasterBandH hSrcBand = GDALGetRasterBand( hSrcDS, iSrcBand );
	if( hSrcBand == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined,
			      "Band %d does not exist on dataset.", iSrcBand );
		(*retval) = VARIANT_FALSE;
		return S_FALSE;
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
		hMaskDS = GDALOpen( OLE2A(pszMaskFilename), GA_ReadOnly );
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
			return S_FALSE;
		}

		hDstDS = OGR_Dr_CreateDataSource( hDriver, OLE2A(pszDstFilename),
										  NULL );
		if( hDstDS == NULL )
		{
			(*retval) = VARIANT_FALSE;
			return S_FALSE;
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
			return S_FALSE;
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
	struct CallbackParams params = { cBack, "Polygonizing" };

	GDALPolygonize( hSrcBand, hMaskBand, hDstLayer, dst_field, NULL,
					(GDALProgressFunc) GDALProgressCallback, &params );

	OGR_DS_Destroy( hDstDS );
	GDALClose( hMaskDS );
	GDALClose( hSrcDS );

	(*retval) = VARIANT_TRUE;
	return S_OK;
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
        return S_FALSE;
    }

/* -------------------------------------------------------------------- */
/*      Open source raster file.                                        */
/* -------------------------------------------------------------------- */
    GDALRasterBandH hBand;

    hSrcDS = GDALOpen( OLE2A(pszSrcFilename), GA_ReadOnly );
    if( hSrcDS == NULL )
	{
		(*retval) = VARIANT_FALSE;
		return S_FALSE;
	}

    hBand = GDALGetRasterBand( hSrcDS, nBandIn );
    if( hBand == NULL )
    {
        CPLError( CE_Failure, CPLE_AppDefined, 
                  "Band %d does not exist on dataset.", 
                  nBandIn );
		(*retval) = VARIANT_FALSE;
		return S_FALSE;
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
        //fprintf( stderr, "Unable to find format driver named %s.\n", 
        //         pszFormat );
		(*retval) = VARIANT_FALSE;
        return S_FALSE;
    }

    hDS = OGR_Dr_CreateDataSource( hDriver, OLE2A(pszDstFilename), NULL );
    if( hDS == NULL )
	{
		(*retval) = VARIANT_FALSE;
        return S_FALSE;
	}

    hLayer = OGR_DS_CreateLayer( hDS, "contour", hSRS, 
                                 b3D ? wkbLineString25D : wkbLineString,
                                 NULL );
    if( hLayer == NULL )
	{
		(*retval) = VARIANT_FALSE;
        return S_FALSE;
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
                                GDALTermProgress, NULL );

    OGR_DS_Destroy( hDS );
    GDALClose( hSrcDS );

	(*retval) = VARIANT_TRUE;
	return S_OK;
}


#pragma endregion