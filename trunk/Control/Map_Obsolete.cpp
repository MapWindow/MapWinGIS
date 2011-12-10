// The old procedures are stored in this file. To access them Map.ShapeDrawingMethod should be set
// to dmStandard or dmNewWithSelection

#pragma once
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"

#include "xtiffio.h"  /* for TIFF */
#include "geotiffio.h" /* for GeoTIFF */
#include "tiff.h"
#include "geotiff.h"
#include "geo_normalize.h"
#include "geovalues.h"
#include "tiffio.h"
#include "tiffiop.h"
#include <fstream>
#include <vector>
#include <atlsafe.h>
#include "IndexSearching.h"

#include "MapPpg.h"
#include "Enumerations.h"

#include "LabelCategory.h"
#include "Labels.h"
#include "Image.h"

#include "ShapefileDrawing.h"
#include "ImageDrawing.h"
#include "LabelDrawing.h"
#include "ChartDrawing.h"

//#include "UtilityFunctions.h"
#include "Projections.h"

using namespace std;

/***********************************************************************/
/*							DrawShapefileAlt()				           */
/***********************************************************************/
/*  Alternative procedure for shapefile drawing. Written to unite drawing
 *	in regular and edit mode and to use quad tree in both modes by default. 
 *  The aim is to reduce size of the function for easier support
 */
void CMapView::DrawShapefileAlt(const CRect & rcBounds, CDC * dc, Layer * layer)
{
	try
	{
		/* -------------------------------------------------------------------- */
		/*		Acquiring reference to the shapefile							*/
		/* -------------------------------------------------------------------- */
		double scale = GetCurrentScale();
		if (!layer->IsVisible(scale))
		//if((layer->flags & Visible) == 0)
			return;

		if( layer->object == NULL )
			return;

		//CComPtr<IShapefile> ishp = NULL;
		IShapefile* ishp = NULL;
		layer->object->QueryInterface(IID_IShapefile,(void**)&ishp);
		if( ishp == NULL )
		{	
			m_lastErrorCode = tkINTERFACE_NOT_SUPPORTED;
			if( m_globalCallback != NULL )
					m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
			return;
		}

		/* -------------------------------------------------------------------- */
		/*		Check drawing bounds											*/
		/* -------------------------------------------------------------------- */
		double xm,ym,zm,xM,yM,zM;
		IExtents * box=NULL;
		ishp->get_Extents(&box);
		box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
		box->Release();
		box=NULL;

		if( OUT_OF_VIEW(xm,ym,xM,yM,extents) )
			return;

		/* -------------------------------------------------------------------- */
		/*		Reading shapefile properties									*/
		/* -------------------------------------------------------------------- */
		AlignShapeLayerAndShapes(layer);
		ShapeLayerInfo * sli = (ShapeLayerInfo*)layer->addInfo;
		
		long numShapes = 0;
		ishp->get_NumShapes(&numShapes);

		ShpfileType shpfiletype = SHP_NULLSHAPE;
		ishp->get_ShapefileType(&shpfiletype);

		VARIANT_BOOL isEditing = FALSE;
		ishp->get_EditingShapes(&isEditing);
	
		VARIANT_BOOL _useQTree = VARIANT_FALSE;
		ishp->get_UseQTree(&_useQTree);
		
		VARIANT_BOOL UseSpatialIndex = VARIANT_FALSE;
		VARIANT_BOOL hasSpatilaIndex = VARIANT_FALSE;
		ishp->get_UseSpatialIndex(&UseSpatialIndex);
		ishp->get_HasSpatialIndex(&hasSpatilaIndex);
		UseSpatialIndex = (UseSpatialIndex && hasSpatilaIndex);

		/* -------------------------------------------------------------------- */
		/*		Preparing pens and brushes for layer drawing options			*/
		/* -------------------------------------------------------------------- */
		// layer drawing options 
		ShapeInfo siLyr;
		sli->AlignShapeInfo(siLyr);

		CPen * slPointPen = new CPen();
		slPointPen->CreatePen(PS_SOLID,1,sli->pointClr);
		CPen * slLinePen = new CPen();
		CreateCustomPen(&siLyr, sli->udLineStipple, slLinePen);

		CBrush * slPointBrush = new CBrush();
		slPointBrush->CreateSolidBrush(sli->pointClr);
		CBrush * slFillBrush = new CBrush();
		CreateCustomBrush(dc, &siLyr, slFillBrush);

		// single shapes drawing options
		CBrush * oldBrush = NULL, * newBrush = NULL;
		CPen * oldPen = NULL, * newPen = NULL;
		
		// saving of the current pen/brush
		CBrush * slOldBrush=NULL;
		CPen * slOldPen=NULL;
		slOldBrush = dc->SelectObject(slPointBrush);
		slOldPen = dc->SelectObject(slPointPen);

		// GDI+ instruments
		dc->SetPolyFillMode(WINDING);
		Gdiplus::Graphics graphics(dc->m_hDC);
		Gdiplus::Color penColor(0, 0, 0, 0);
		Gdiplus::Pen pen(penColor);
		Gdiplus::Color brushColor(0, 0, 0, 0);
		Gdiplus::SolidBrush brush(brushColor);
		
		/* -------------------------------------------------------------------- */
		/*		Pens and brushes for selected objects							*/
		/* -------------------------------------------------------------------- */
		IShapeDrawingOptions* selOpt =NULL;
		ishp->get_SelectionDrawingOptions(&selOpt);
		VARIANT_BOOL drawFill, drawLines, drawPoints;
	
		ShapeInfo siSel;
		selOpt->get_FillVisible(&drawFill);
		selOpt->get_LineVisible(&drawLines);
		
		if (shpfiletype == SHP_POINT || shpfiletype == SHP_POINTM || shpfiletype == SHP_POINTZ ||
			shpfiletype == SHP_MULTIPOINT || shpfiletype == SHP_MULTIPOINTM || shpfiletype == SHP_MULTIPOINTZ)
			drawPoints = true;
		else
			drawPoints = false;
		
		if (drawFill) siSel.flags = siSel.flags | slfDrawFill;
		if (drawLines) siSel.flags = siSel.flags | slfDrawLine;
		if (drawPoints) siSel.flags = siSel.flags | slfDrawPoint;

		selOpt->get_LineColor(&siSel.lineClr);
		selOpt->get_FillColor(&siSel.fillClr);
		selOpt->get_LineWidth(&siSel.lineWidth);
		selOpt->get_PointSize(&siSel.pointSize);
		selOpt->get_FillTransparency(&siSel.trans);
		
		//selOpt->get_LineStipple(&siSel.lineStipple);
		//selOpt->get_PointColor(&(siSel.pointClr));
		//selOpt->get_FillStipple(&siSel.fillStipple);
		//selOpt->get_PointType(&siSel.pointType);
		//selOpt->get_StippleColor(&siSel.stippleLineClr);
		
		VARIANT_BOOL temp;
		selOpt->get_FillBgTransparent(&temp);
		siSel.transparentStipple = (temp == VARIANT_FALSE);
		
		CPen * selPointPen = new CPen();
		selPointPen->CreatePen(PS_SOLID,1,siSel.pointClr);
		CPen * selLinePen = new CPen();
		CreateCustomPen(&siSel, sli->udLineStipple, selLinePen);

		CBrush * selPointBrush = new CBrush();
		selPointBrush->CreateSolidBrush(siSel.pointClr);
		CBrush * selFillBrush = new CBrush();
		CreateCustomBrush(dc, &siSel, selFillBrush);
		
		/* -------------------------------------------------------------------- */
		/*		Pattern brush based on IImage									*/
		/* -------------------------------------------------------------------- */
		bool bCanDrawRaster = false;
		CBrush* brushRaster = new CBrush();
		if (siSel.fillStipple == fsRaster)
		{
			IImage* rasterFill = NULL;
			//selOpt->get_RasterFill(&rasterFill);
			if (rasterFill != NULL)
				bCanDrawRaster = Image2PatternBrush(dc, rasterFill, brushRaster);
		}
		
		/* -------------------------------------------------------------------- */
		/*		Commonly used variables											*/
		/* -------------------------------------------------------------------- */
		long numPoints = 0;			// number of shape points
		long numParts = 0;			// number of shape parts
		long firstPoint = 0;		// first point of a part
		long lastPoint = 0;			// last point of a part

		double prX = 0, prY = 0;	// projected coordinates
		int	   pxX = 0, pxY = 0;	// screen coordinates
		
		VARIANT_BOOL vbretval;
		
		int* qtreeResult;			// results of quad tree selection
		vector<long> selectResult;	// results of spatial index selection
		
		int offset;					// position (number) of a shape in the shapefile
		char * indexData;			// content of shx file
		
		FILE * shpfile;				//
		
		ShapeInfo* siCur = NULL;	// layer or selection drawing options

		/* ----------------------------------------------------------------- */
		/*		PREPARING TO READ DISC-BASED VERSION OF A SHAPEFILE			 */
		/* ----------------------------------------------------------------- */
		if( isEditing == FALSE)
		{		
			if (((CShapefile*)ishp)->get_File() == NULL)  //layer->file == NULL )
			{
				perror("Shapefile doesn't exist");
				return;
			}
			else
			{
				BSTR fname;
				ishp->get_Filename(&fname);
				int b_strlen = wcslen(fname);

				//Neio mofified following http://www.mapwindow.org/phorum/read.php?7,12162 by gischai
				//Multilanguage support problem
				char * sFilename = new char[(b_strlen+1)<<1];
				int sFilenamelast = WideCharToMultiByte(CP_ACP,0,fname,b_strlen,sFilename,b_strlen<<1,0,0);
				::SysFreeString(fname);

				sFilename[sFilenamelast] = 0;

				/* ----------------------------------------------------------------- */
				/*		Reading shape index (SHX)									 */
				/* ----------------------------------------------------------------- */
				FILE * shpIndex = NULL;
				sFilename[sFilenamelast - 1] = 'x'; 
				shpIndex = fopen(sFilename, "rb");
				
				if (shpIndex == NULL)
				{
					perror("ERROR OPENING *.shx");
					return;
				}
				else
				{
					fseek (shpIndex, 0, SEEK_END);
					int indexFileSize = ftell(shpIndex);
					rewind(shpIndex);
					
					// 100 is for header
					fseek(shpIndex, 100, SEEK_SET);
					indexData = new char[indexFileSize - 100];
					long result = fread(indexData, sizeof(char), indexFileSize - 100, shpIndex);
					fclose(shpIndex);

					shpfile = ((CShapefile*)ishp)->get_File(); //layer->file;
					rewind(shpfile);
				}

				/* ----------------------------------------------------------------- */
				/*		Extracting shapes from spatial index						 */
				/* ----------------------------------------------------------------- */
				if (UseSpatialIndex)
				{
					double lowVals[2], highVals[2];

					lowVals[0] = extents.left;
					lowVals[1] = extents.bottom;
					highVals[0] = extents.right;
					highVals[1] = extents.top;
					if (!selectResult.empty())
						selectResult.clear();
					
					string baseName;
					baseName = sFilename;
					baseName = baseName.substr(0,baseName.find_last_of("."));

					IndexSearching::CIndexSearching *res = new IndexSearching::CIndexSearching();

					int ret = IndexSearching::selectShapesFromIndex((char *)baseName.c_str(), lowVals, highVals, IndexSearching::intersection, 100, res);
					if (ret == 0)
					{
						selectResult.reserve(res->getLength());
						for (int i = 0 ;i < res->getLength(); i++)
							selectResult.push_back((long)res->getValue(i));
					}
					else
						UseSpatialIndex = false;
					
					numShapes = selectResult.size();
					delete res;
				}
				delete [] sFilename;
				sFilename = NULL;
			}	// (file != NULL);
		}		//(isEditing == FALSE);

		/* ----------------------------------------------------------------- */
		/*		Extracting shapes using quad tree							 */
		/* ----------------------------------------------------------------- */
		if(_useQTree)
		{
			int shapesCount;
			IExtents * bBox = NULL;

			CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&bBox );
			bBox->SetBounds(extents.left,extents.bottom,0,extents.right, extents.top,0);
			((CShapefile*)ishp)->QuickQueryInEditMode(bBox,&qtreeResult,&shapesCount);
			if (qtreeResult == NULL) return;  // temporary
			bBox->Release();
			bBox=NULL;

			numShapes = (long)shapesCount;
		}

		/* ----------------------------------------------------------------- */
		/*		BEGINNING OF DRAWING SHAPES
		/* ----------------------------------------------------------------- */
		for(int i = 0; i < numShapes; i++ )
		{
			
			if(_useQTree)	
				offset = qtreeResult[i];
			else
			{
				if (!isEditing && UseSpatialIndex)
				{
					offset = selectResult.at(i);
					offset--;	
				}
				else
					offset = i;
			}

			ShapeInfo * si = sli->shapeInfo[offset];
			if( (si->flags & sfVisible) == 0 )	
				continue;
			
			// dynamically allocated
			int* parts = NULL;
			int * pntcnt = NULL;
			CPoint * pnts = NULL;			
			// dynamically allocated

			double* points;
			ShpfileType shptype;
			VARIANT_BOOL shpSelected;
			ishp->get_ShapeSelected(offset, &shpSelected);
			siCur = shpSelected?(&siSel):si;

			/* ----------------------------------------------------------------- */
			/*		Reading shape data for disk version
			/* ----------------------------------------------------------------- */
			if (!isEditing)
			{
				int iShapeOffset, readOffset;
				char * cShapeOffset;
				int contentLength;
				

				// index records are 8 bytes;
				iShapeOffset=((int*)&indexData[offset*8])[0];	
				cShapeOffset = (char*) &iShapeOffset;
				SWAP_ENDIAN_INT(cShapeOffset);
				readOffset = (iShapeOffset) * 2;

				iShapeOffset = ((int*)&indexData[offset*8])[1];
				cShapeOffset = (char*)&iShapeOffset;
				SWAP_ENDIAN_INT(cShapeOffset);
				contentLength = (iShapeOffset)*2;
				
				fseek(shpfile, (long)readOffset, SEEK_SET);
				
				// *2 for conversion from 16-bit words to 8-bit words
				char * shapeData;
				int * intdata;
				shapeData = new char[contentLength * 2];
				long result = fread(shapeData, sizeof(char), contentLength * 2, shpfile);
				
				intdata = (int*) shapeData;
				shptype = (ShpfileType)intdata[2];
				
				if( shptype != shpfiletype )
				{
					if( shptype != SHP_NULLSHAPE )
					{	
						m_lastErrorCode = tkINVALID_SHP_FILE;
						if( m_globalCallback != NULL )
							m_globalCallback->Error( m_key.AllocSysString(),  A2BSTR(ErrorMsg(m_lastErrorCode) ) );
					}
				}
				
				else if( shptype == SHP_POINT || shptype == SHP_POINTZ || shptype == SHP_POINTM )
				{
					// Char is 1 byte, need to skip 12 bytes past
					numPoints = 1;
					points = (double *)&(shapeData[12]); 
					PROJECTION_TO_PIXEL_INT(points[0],points[1],pxX,pxY);
					
					// dynamic allocation
					pnts = new CPoint[1];
					// dynamic allocation

					pnts[0] = CPoint(pxX,pxY);
				}
				
				else if( shptype == SHP_MULTIPOINT || shptype == SHP_MULTIPOINTZ || shptype == SHP_MULTIPOINTM )
				{	
					double* bounds = (double*)(&(shapeData[12]));
					if(! OUT_OF_VIEW(bounds[0],bounds[1],bounds[2],bounds[3],extents ) )
					{
						numPoints = (long)((int*)shapeData)[11];
						points = (double*)&(shapeData[48]);
						
						// dynamic allocation
						pnts = new CPoint[numPoints];
						// dynamic allocation
						
						for(int j = 0; j < numPoints; j++ )
						{	
							PROJECTION_TO_PIXEL_INT(points[2*j],points[2*j+1],pxX,pxY);
							pnts[j] = CPoint(pxX,pxY);
						}
					}
				}
				
				else if( shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM ||
						 shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM )
				{	
					double* bounds = (double*)(&(shapeData[12]));
					if(! OUT_OF_VIEW(bounds[0],bounds[1],bounds[2],bounds[3],extents ))
					{					
						numParts = (long)intdata[11];
						numPoints = (long)intdata[12];
						int* prts = (int*)&(intdata[13]);

						int * begOfPts = &(intdata[13+numParts]);
						points = (double*)begOfPts;
						
						// dynamic allocation
						pnts = new CPoint[numPoints];
						pntcnt = new int[numParts];
						parts = new int[numParts];
						// dynamic allocation

						for(int j = 0; j < numPoints; j++ )
						{	
							PROJECTION_TO_PIXEL_INT(points[2*j],points[2*j+1],pxX,pxY);
							pnts[j] = CPoint(pxX,pxY);
						}
					
						for(int p = 0; p < numParts; p++ )
						{
							firstPoint=prts[p];
							parts[p] = prts[p];
							lastPoint= p+1<numParts?prts[p+1]:numPoints;
							pntcnt[p] = lastPoint - firstPoint;
							if (pntcnt[p] <= 0)
							{
								delete[] pnts;
								delete[] pntcnt;
							}
						}
					}
				}
				delete [] shapeData;
				shapeData = NULL;
				
				// nothing was read; we need not free or draw anything
				if (pnts == NULL)
					continue;
			}

			/* ----------------------------------------------------------------- */
			/*		Reading shape data for in-memory version
			/* ----------------------------------------------------------------- */
			else //if(isEditing)
			{
				CComPtr<IShape> shp = NULL;
				ishp->get_Shape(offset, &shp);

				ishp->QuickExtents(offset, &box);
				
				box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
				box->Release();

				if( OUT_OF_VIEW(xm,ym,xM,yM,extents))
				{				
					continue;
				}
				{
					shp->get_NumPoints(&numPoints);
					if (numPoints <= 0) continue;
					
					shp->get_ShapeType(&shptype);

					if( shptype == SHP_POINT || shptype == SHP_POINTZ || shptype == SHP_POINTM ||
						shptype == SHP_MULTIPOINT || shptype == SHP_MULTIPOINTZ || shptype == SHP_MULTIPOINTM )
					{
						/* dynamic allocation */
						pnts = new CPoint[numPoints];
						/* dynamic allocation */

						for(int j = 0; j < numPoints; j++)
						{	
							shp->get_XY(j, &prX, &prY, &vbretval);
							PROJECTION_TO_PIXEL_INT(prX,prY,pxX,pxY);
							pnts[j]=CPoint(pxX,pxY);
						}
					}
					else if( shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM ||
							 shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM )
					{
						shp->get_NumParts(&numParts);
						if (numParts == 0)
							continue;

						/* dynamical allocation */
						pnts = new CPoint[numPoints];
						parts = new int[numParts];
						pntcnt = new int[numParts];
						/* dynamical allocation */

						for(int p = 0; p < numParts; p++ )
						{
							shp->get_Part( p, &firstPoint);
							if( p + 1 < numParts )
								shp->get_Part( p + 1, &lastPoint);
							else
								lastPoint = numPoints;
							pntcnt[p] = lastPoint - firstPoint;
							parts[p] = firstPoint;

							if (pntcnt[p] <= 0)
							{
								delete[] pnts;
								delete[] parts;
								delete[] pntcnt;
								continue;
							}
						}

						for(int j = 0; j < numPoints; j++ )
						{	
							shp->get_XY(j, &prX, &prY, &vbretval);
							PROJECTION_TO_PIXEL_INT(prX, prY, pxX, pxY);
							pnts[j] = CPoint(pxX,pxY);
						}
					}
				}
			}

			/* ----------------------------------------------------------------- */
			/*		Drawing of polygons
			/* ----------------------------------------------------------------- */
			if (shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM)
			{			
				/* we'll draw poly borders in this section as well;		*/
				/* if there is no fill we'll draw them in line section	*/
				if(siCur->flags &slfDrawFill)
				{
					bool useGDI = (siCur->trans == 255 || siCur->fillStipple != fsNone);
					
					if (siCur->fillStipple!=fsNone && !siCur->transparentStipple)
					{
						// we draw stipple and fill simultaneously
						dc->SetBkColor(siCur->fillClr);	
						dc->SetBkMode(OPAQUE);
					}
					else
						dc->SetBkMode(TRANSPARENT);

					/* ----------------------------------------------------------------- */
					/*		Drawing polygon w/o transparency. Uses GDI
					/* ----------------------------------------------------------------- */
					if ( useGDI )
					{					
						if (! shpSelected)
						{						
							if (sli->fillClr != si->fillClr || sli->fillStipple != si->fillStipple || si->fillStipple != fsNone) 
							{						
								newBrush = new CBrush();
								CreateCustomBrush(dc, si, newBrush);
								oldBrush = dc->SelectObject(newBrush);
							}
							else
								dc->SelectObject(slFillBrush);
						}
						else
						{
							if (siSel.fillStipple == fsRaster && bCanDrawRaster)
							{	
								dc->SelectObject(brushRaster);
							}
							else
								dc->SelectObject(selFillBrush);									
						}

						if (!shpSelected)
						{
							if( si->lineClr != sli->lineClr || si->lineStipple != lsNone || si->lineWidth != sli->lineWidth)
							{
								newPen = new CPen();
								CreateCustomPen(si,sli->udLineStipple,newPen);
								oldPen=dc->SelectObject(newPen);
							}
							else
								dc->SelectObject(slLinePen);
						}
						else
							dc->SelectObject(selLinePen);
						
						dc->PolyPolygon(pnts,pntcnt,numParts);
						if(newBrush)	RELEASE_BRUSH(dc, newBrush, oldBrush);
						if(newPen)  	RELEASE_PEN(dc, newPen, oldPen);
					}
					else
					{
						/* ----------------------------------------------------------------- */
						/*		Drawing polygon with transparency. Uses GDI+
						/*		We don't use fill stipples with GDI+
						/* ----------------------------------------------------------------- */
						long alpha = ((long)siCur->trans)<<24;
						brushColor.SetValue(alpha | BGR_TO_RGB(siCur->fillClr));
						brush.SetColor(brushColor);						
						

						// numPoints is obvious; +(numParts*2) is due to occasionally needing to close a hole's polygon, 
						// and needing to loop back to original polygon if doing transparent fill on a multipart (holes) polygon.
						Gdiplus::Point * allpnts = new Gdiplus::Point[numPoints + (numParts*2)];

						Gdiplus::Point returnPoint;
						int acnt = 0;
						
						int cnt = 0;
						for(int p = 0; p < numParts; p++)
						{
							Gdiplus::Point * gdipnts = new Gdiplus::Point[pntcnt[p]];
							
							for(int j = 0; j < pntcnt[p]; j++ )
							{
								gdipnts[j]= Gdiplus::Point(pnts[cnt].x, pnts[cnt].y);
								allpnts[acnt++] = Gdiplus::Point(pnts[cnt].x, pnts[cnt].y);
								cnt++;
							}

							// Close the polygon for filling?
							firstPoint = parts[p];
							lastPoint = p+1<numParts?parts[p+1] - 1:numPoints - 1;

							if (pnts[firstPoint].x != pnts[lastPoint].x || 
								pnts[firstPoint].y != pnts[lastPoint].y)
							{
								allpnts[acnt++]=Gdiplus::Point(pnts[0].x, pnts[0].y);
							}

							// Add the return point - in a hole now
							if (p != 0)
								allpnts[acnt++] = Gdiplus::Point(pnts[0].x, pnts[0].y);

							// Inverse (fill holes only - interesting effect):
							// graphics.FillPolygon(&brush, gdipnts, subNumPoints, Gdiplus::FillModeWinding);
							// (comment out FillPolygon below to see this in action though)
							if((siCur->flags & slfDrawLine) && siCur->lineWidth != 0) 
							{
								penColor.SetValue(0xFF000000 | BGR_TO_RGB(siCur->lineClr));
								pen.SetColor(penColor);
								pen.SetWidth(siCur->lineWidth);

								if		(siCur->lineStipple == lsNone)			pen.SetDashStyle((Gdiplus::DashStyle)0); // DashStyleSolid;
								else if (siCur->lineStipple == lsDashed)		pen.SetDashStyle((Gdiplus::DashStyle)1); // DashStyleDash
								else if (siCur->lineStipple == lsDotted)		pen.SetDashStyle((Gdiplus::DashStyle)2); // DashStyleDot
								else if (siCur->lineStipple == lsDashDotDash)	pen.SetDashStyle((Gdiplus::DashStyle)3); // DashStyleDashDot
								else if (siCur->lineStipple == lsDashDotDot)	pen.SetDashStyle((Gdiplus::DashStyle)4); // DashStyleDashDotDot
								else											pen.SetDashStyle((Gdiplus::DashStyle)0);

								graphics.DrawPolygon(&pen, gdipnts, pntcnt[p]);
							}
							
							delete [] gdipnts;
						}
						graphics.FillPolygon(&brush, allpnts, acnt, Gdiplus::FillModeAlternate);
						delete [] allpnts;
					}
				}
			}

			/* ----------------------------------------------------------------- */
			/*		Drawing of lines											 */
			/* ----------------------------------------------------------------- */
			/* We draw polygon borders in this section as well if there is no fill */			
			if(shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM ||
			  (shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM ) && !(si->flags & slfDrawFill))
			{
				dc->SetBkMode(TRANSPARENT);
				if( (siCur->flags & slfDrawLine) && (siCur->lineWidth > 0) )
				{	
					if (!shpSelected)
					{
						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone || si->lineWidth != sli->lineWidth )
						{
							newPen = new CPen();
							CreateCustomPen(si, sli->udLineStipple, newPen);
							oldPen = dc->SelectObject(newPen);
						}
						else
							dc->SelectObject(slLinePen);
					}
					else
						dc->SelectObject(selLinePen);
					
					int numRepeats = 1;
					if (siCur->lineStipple == lsDoubleSolid) numRepeats = 2;
					if (siCur->lineStipple== lsDoubleSolidPlusDash) numRepeats = 3;
					
					for (int completedRepeats = 0; completedRepeats < numRepeats; completedRepeats++)
					{
						// If necessary, do adjustments, based on the line stipple type.
						if (si->lineStipple == lsTrainTracks)
						{
							// Draw train track blocks according to the chunks
							// of the polyline about to be drawn
							// (technically the blocks will be drawn prior to the line itself)
							LPPOINT trackLine = new POINT[3];
							if (!trackLine == NULL)
							{
								newPen->DeleteObject();
								newPen->CreatePen(PS_SOLID, (int)siCur->lineWidth,siCur->lineClr);
								dc->SelectObject(newPen);
								for (int i = 1; i < numPoints - 1; i++)
								{
									findPerpPoints(10, pnts[i].x, pnts[i].y, pnts[i+1].x, pnts[i+1].y, trackLine[0].x, trackLine[0].y, trackLine[1].x, trackLine[1].y);
									trackLine[2].x = (trackLine[0].x + trackLine[1].x) / 2;
									trackLine[2].y = (trackLine[0].y + trackLine[1].y) / 2;
									dc->Polyline(trackLine, 2);
								}
								delete [] trackLine;
							}
							newPen->DeleteObject();
							newPen->CreatePen(PS_SOLID, (int)siCur->lineWidth,siCur->lineClr);
							dc->SelectObject(newPen);

							dc->PolyPolyline(pnts, (DWORD*)pntcnt, numParts);
						}
						else if (siCur->lineStipple == lsDoubleSolid || siCur->lineStipple == lsDoubleSolidPlusDash)
						{
							int style = (completedRepeats==2)?PS_DOT:PS_SOLID;
							
							newPen->DeleteObject();
							newPen->CreatePen(style, (int)siCur->lineWidth,siCur->lineClr);
							dc->SelectObject(newPen);
							
							if (completedRepeats != 2)
							{	
								int cnt = 0;
								for (int p = 0; p < numParts; p++)
								{
									CPoint* shiftedPnts = new CPoint[pntcnt[p]];
									for (int j = 0; j < pntcnt[p]; j++)
									{
										shiftedPnts[j] = pnts[cnt];
										cnt++;
									}
									adjustLine("DOUBLESOLID", shiftedPnts, pntcnt[p], completedRepeats);  // 0 or 1
									dc->Polyline(shiftedPnts, pntcnt[p]);
									delete[] shiftedPnts;
								}
							}
							else
							{
								dc->PolyPolyline(pnts, (DWORD*)pntcnt, numParts);
							}
						}
						else
						{
							dc->PolyPolyline(pnts, (DWORD*)pntcnt, numParts);
						}
					}
					if(newPen)		
						RELEASE_PEN(dc, newPen, oldPen);
				}
			}

			/* ----------------------------------------------------------------- */
			/*		Drawing of points
			/* ----------------------------------------------------------------- */
			if( siCur->flags & slfDrawPoint)
			{
				dc->SetBkMode(TRANSPARENT);
				double hsize=siCur->pointSize*.5;
				
				if (!shpSelected)
					if( si->pointClr != sli->pointClr || si->pointSize != sli->pointSize)
					{	
						newPen = new CPen();
						newPen->CreatePen(PS_SOLID,1,si->pointClr);
						oldPen=dc->SelectObject(newPen);
						
						newBrush = new CBrush();
						newBrush->CreateSolidBrush( si->pointClr);
						oldBrush=dc->SelectObject(newBrush);
					}
					else
					{	
						dc->SelectObject(slPointBrush);
						dc->SelectObject(slPointPen);
					}
				else
				{
					dc->SelectObject(selPointBrush);
					dc->SelectObject(selPointPen);
				}

				//PROJECTION_TO_PIXEL_INT(points[0],points[1],pxX,pxY);
				for (int j = 0; j < numPoints; j++)
				{
					//DRAW_POINT(dc,sli,siCur,pnts[j].x,pnts[j].y,hsize);
					DrawShapePoint(dc,sli,siCur,pnts[j].x,pnts[j].y, (int)hsize);	// lsu: 29-jan-2011
				}
				if(newBrush) RELEASE_BRUSH(dc, newBrush, oldBrush);
				if(newPen)	 RELEASE_PEN(dc, newPen, oldPen);
			}

			/* ----------------------------------------------------------------- */
			/*		Cleaning of shape data
			/* ----------------------------------------------------------------- */
			delete [] pnts;
			pnts=NULL;

			if( shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM ||
			    shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM )
			{
				
				delete[] parts;
				parts = NULL;

				delete[] pntcnt;
				pntcnt = NULL;
			}
		} // end of shape cycle

		/* ----------------------------------------------------------------- */
		/*		Final cleaning
		/* ----------------------------------------------------------------- */
		if (!isEditing)
			delete [] indexData;
		
		if(_useQTree) 
			delete[] qtreeResult;
		else
		{
			if (UseSpatialIndex)
				selectResult.clear();
		}

		dc->SelectObject(slOldBrush);
		dc->SelectObject(slOldPen);
		
		// layer drawing options
		slPointBrush->DeleteObject();
		slPointPen->DeleteObject();
		slLinePen->DeleteObject();
		slFillBrush->DeleteObject();
		
		delete slPointBrush;
		delete slPointPen;
		delete slLinePen;
		delete slFillBrush;
		
		// selection drawing options
		selPointBrush->DeleteObject();
		selPointPen->DeleteObject();
		selLinePen->DeleteObject();
		selFillBrush->DeleteObject();
		
		delete selPointBrush;
		delete selPointPen;
		delete selLinePen;
		delete selFillBrush;
		delete brushRaster;
		
		// TODO: remoce after testing

		/* ----------------------------------------------------------------- */
		/*		Drawing of labels
		/* ----------------------------------------------------------------- */
		//if (m_ShapeDrawingMethod == dmStandard || m_ShapeDrawingMethod == dmNewWithSelection)
		//{
		//	// old method for labels drawing
		//	m_labelsToDraw.push(layer);
		//}
		//else if (m_ShapeDrawingMethod = dmNewWithLabels)
		//{
		//	// drawing of labels immediately after the layer 
		//	ILabels* labels;
		//	ishp->get_Labels(&labels);
		//	if(labels != NULL)
		//	{
		//		tkVerticalPosition vertPos;
		//		labels->get_VerticalPosition(&vertPos);
		//		//if (vertPos == vpAboveParentLayer)
		//		//{
		//			//DrawLabelsAlt(dc, labels);
		//		//}
		//	}
		//}
	}
	catch(...)
	{
	}
}


// *************************************************************
//		DrawShapePoint()
// *************************************************************
// there are problems when we use DRAW_POINT macro from DrawShapefileAlt function.
// so I changed to inline function. There must not be decrease in speed if I understand it right,
// and it's nearly impossible to debug such a big macro.
void CMapView::DrawShapePoint(CDC* dc,ShapeLayerInfo* sli,ShapeInfo* si,int pixX, int pixY, int hsize)
{
	if( si->pointType == ptSquare )
	{	
		if( si->pointSize <= 1 )
		{
			dc->SetPixelV(pixX,pixY,si->pointClr);
		}
		else
		{
			dc->FillSolidRect(pixX-hsize,pixY-hsize, (int)si->pointSize, (int)si->pointSize,si->pointClr);
		}
	}
	else if( si->pointType == ptCircle )
	{
		if( si->pointSize <= 1 )
		{
			dc->SetPixel(pixX,pixY,si->pointClr);
		}
		else
		{
			dc->Ellipse(pixX-hsize,pixY-hsize,pixX+hsize,pixY+hsize);
		}
	}
	else if( si->pointType == ptDiamond )
	{	
		CPoint pnts[4];
		pnts[0]=CPoint(pixX,pixY-hsize);
		pnts[1]=CPoint(pixX-hsize,pixY);
		pnts[2]=CPoint(pixX,pixY+hsize);
		pnts[3]=CPoint(pixX+hsize,pixY);
		dc->Polygon(pnts,4);
	}
	else if( si->pointType == ptTriangleUp )
	{	
		CPoint pnts[3];
		pnts[0]=CPoint(pixX,pixY-hsize);
		pnts[1]=CPoint(pixX-hsize,pixY+hsize);
		pnts[2]=CPoint(pixX+hsize,pixY+hsize);
		dc->Polygon(pnts,3);
	}
	else if( si->pointType == ptTriangleDown )
	{	
		CPoint pnts[3];
		pnts[0]=CPoint(pixX,pixY+hsize);
		pnts[1]=CPoint(pixX+hsize,pixY-hsize);
		pnts[2]=CPoint(pixX-hsize,pixY-hsize);
		dc->Polygon(pnts,3);
	}
	else if( si->pointType == ptTriangleLeft )
	{	CPoint pnts[3];
		pnts[0]=CPoint(pixX-hsize,pixY);
		pnts[1]=CPoint(pixX+hsize,pixY+hsize);
		pnts[2]=CPoint(pixX+hsize,pixY-hsize);
		dc->Polygon(pnts,3);
	}
	else if( si->pointType == ptTriangleRight )
	{	CPoint pnts[3];
		pnts[0]=CPoint(pixX+hsize,pixY);
		pnts[1]=CPoint(pixX-hsize,pixY-hsize);
		pnts[2]=CPoint(pixX-hsize,pixY+hsize);
		dc->Polygon(pnts,3);
	}
	else if( si->pointType == ptUserDefined )
	{
		long width = sli->udPointTypeWidth;
		long height = sli->udPointTypeHeight;
		
		if( width<=0 || height<=0 || sli->udPointType==NULL)
		{
			dc->SetPixelV(pixX,pixY,si->pointClr);
		}
		else
		{	
			double left=pixX-width*.5;
			double right=pixX+width*.5;
			double top=pixY+height*.5;
			double bottom=pixY-height*.5;
			
			if( left> m_viewWidth || right < 0 || bottom > m_viewHeight || top < 0 )
				return; //continue;
			
			if( si->pointSize != 1.0 )
			{
				if(si->pointClr == sli->pointClr)
				{
					CDC * stretchDC = new CDC();
					stretchDC->CreateCompatibleDC(dc);
					CBitmap * bmp = new CBitmap();
					CBitmap * oldbmp = NULL;
					long stretchWidth = (long)(sli->udPointTypeWidth * si->pointSize);
					long stretchHeight= (long)(sli->udPointTypeHeight * si->pointSize);
					bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);
					oldbmp = stretchDC->SelectObject(bmp);
					stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,sli->udDC,0,0,width,height,SRCCOPY);
					TransparentBlt(dc->m_hDC, pixX - (int)(stretchWidth * 0.5), pixY-(int)(stretchHeight * 0.5), stretchWidth, stretchHeight, 
								  stretchDC->m_hDC, 0, 0, stretchWidth, stretchHeight, sli->udTransColor);
					stretchDC->SelectObject(oldbmp);
					bmp->DeleteObject();
					stretchDC->DeleteDC();
					delete stretchDC;
					delete bmp;
				}
				else
				{
					long r, g, b;
					CDC * stretchDC = new CDC();
					stretchDC->CreateCompatibleDC(dc);
					CBitmap * bmp = new CBitmap();
					CBitmap * oldbmp = NULL;
					long stretchWidth = (long)(sli->udPointTypeWidth * si->pointSize);
					long stretchHeight = (long)(sli->udPointTypeHeight * si->pointSize);
					COLORREF color;
					COLORREF transparent_color = (COLORREF)sli->udTransColor;
					bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);
					oldbmp = stretchDC->SelectObject(bmp);
					for(int i = 0; i < height; i++)
					{
						for(int j = 0; j < width; j++)
						{
							color = sli->udDC->GetPixel(j,i);
							color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));
							stretchDC->SetPixel(j,i,color);
						}
					}
					r = 255-GetRValue(transparent_color);
					g = 255-GetGValue(transparent_color);
					b = 255-GetBValue(transparent_color);
					stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,stretchDC,0,0,width,height,SRCCOPY);
					TransparentBlt(dc->m_hDC, pixX- int(stretchWidth * 0.5),pixY - int(stretchHeight * 0.5), stretchWidth, stretchHeight,
								   stretchDC->m_hDC, 0, 0, stretchWidth, stretchHeight, RGB(r,g,b));
					stretchDC->SelectObject(oldbmp);
					bmp->DeleteObject();
					stretchDC->DeleteDC();
					delete stretchDC;
					delete bmp;
				}
			}
			else
			{
				if(si->pointClr == sli->pointClr)
				{
					TransparentBlt(dc->m_hDC, pixX - int(width * 0.5),pixY - int(height * 0.5), width, height, sli->udDC->m_hDC, 0, 0, width, height, sli->udTransColor);
				}
				else
				{
					long r, g, b;
					CDC * selectedDC = new CDC();
					selectedDC->CreateCompatibleDC(dc);
					CBitmap * bmp = new CBitmap();
					CBitmap * oldbmp = NULL;
					COLORREF color;
					COLORREF transparent_color = (COLORREF)sli->udTransColor;
					bmp->CreateDiscardableBitmap(dc,width,height);
					oldbmp = selectedDC->SelectObject(bmp);
					for(int i = 0; i < height; i++)
					{
						for(int j = 0; j < width; j++)
						{
							color = sli->udDC->GetPixel(j,i);
							color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));
							selectedDC->SetPixel(j,i,color);
						}
					}
					r = 255-GetRValue(transparent_color);
					g = 255-GetGValue(transparent_color);
					b = 255-GetBValue(transparent_color);
					TransparentBlt(dc->m_hDC, pixX - int(width * 0.5),pixY- int(height * 0.5), width, height,
								   selectedDC->m_hDC, 0, 0, width, height, RGB(r,g,b));
					selectedDC->SelectObject(oldbmp);
					bmp->DeleteObject();
					selectedDC->DeleteDC();
					delete selectedDC;
					delete bmp;
				}
			}
		}
	}
	else if( si->pointType == ptImageList )
	{
		if( si->ImageListIndex == -1)
			dc->SetPixelV(pixX,pixY,si->pointClr);
		else
		{	
			udPointListItem * iconItem = sli->PointImageList[si->ImageListIndex];
			long width = iconItem->udPointTypeWidth;
			long height = iconItem->udPointTypeHeight;
			if( width<=0 || height<=0 || iconItem->udPointType==NULL)
				dc->SetPixelV(pixX,pixY,si->pointClr);
			else
			{	
				double left=pixX-width*.5;
				double right=pixX+width*.5;
				double top=pixY+height*.5;
				double bottom=pixY-height*.5;
				if( left> m_viewWidth || right < 0 || bottom > m_viewHeight || top < 0 )
					return; //continue;
				
				if( si->pointSize != 1.0 )
				{
					if(si->pointClr == sli->pointClr)
					{
						CDC * stretchDC = new CDC();
						stretchDC->CreateCompatibleDC(dc);
						CBitmap * bmp = new CBitmap();
						CBitmap * oldbmp = NULL;
						long stretchWidth = long(iconItem->udPointTypeWidth * si->pointSize);
						long stretchHeight = long(iconItem->udPointTypeHeight * si->pointSize);
						bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);
						oldbmp = stretchDC->SelectObject(bmp);
						stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,iconItem->udDC,0,0,width,height,SRCCOPY);
						TransparentBlt(dc->m_hDC, pixX - int(stretchWidth * 0.5), pixY - int(stretchHeight * 0.5), stretchWidth, stretchHeight, 
									   stretchDC->m_hDC, 0, 0, stretchWidth, stretchHeight, iconItem->udTransColor);
						stretchDC->SelectObject(oldbmp);
						bmp->DeleteObject();
						stretchDC->DeleteDC();
						delete stretchDC;
						delete bmp;
					}else
					{
						long r, g, b;
						CDC * stretchDC = new CDC();
						stretchDC->CreateCompatibleDC(dc);
						CBitmap * bmp = new CBitmap();
						CBitmap * oldbmp = NULL;
						long stretchWidth = long(iconItem->udPointTypeWidth*si->pointSize);
						long stretchHeight = long(iconItem->udPointTypeHeight*si->pointSize);
						COLORREF color;
						COLORREF transparent_color = (COLORREF)iconItem->udTransColor;
						bmp->CreateDiscardableBitmap(dc,stretchWidth,stretchHeight);
						oldbmp = stretchDC->SelectObject(bmp);
						for(int i = 0; i < height; i++)
						{
							for(int j = 0; j < width; j++)
							{
								color = iconItem->udDC->GetPixel(j,i);
								color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));
								stretchDC->SetPixel(j,i,color);
							}
						}
						r = 255-GetRValue(transparent_color);
						g = 255-GetGValue(transparent_color);
						b = 255-GetBValue(transparent_color);
						stretchDC->StretchBlt(0,0,stretchWidth,stretchHeight,stretchDC,0,0,width,height,SRCCOPY);
						TransparentBlt(dc->m_hDC, pixX - int(stretchWidth * 0.5), pixY - int(stretchHeight * 0.5), stretchWidth, stretchHeight,
									   stretchDC->m_hDC, 0, 0, stretchWidth, stretchHeight, RGB(r,g,b));
						stretchDC->SelectObject(oldbmp);
						bmp->DeleteObject();
						stretchDC->DeleteDC();
						delete stretchDC;
						delete bmp;
					}
				}
				else
				{
					if(si->pointClr == sli->pointClr)
					{
						TransparentBlt(dc->m_hDC,pixX-int(width * 0.5),pixY-int(height * 0.5),width,height,
									   iconItem->udDC->m_hDC,0,0,width,height,iconItem->udTransColor);
					}
					else
					{
						long r, g, b;
						CDC * selectedDC = new CDC();
						selectedDC->CreateCompatibleDC(dc);
						CBitmap * bmp = new CBitmap();
						CBitmap * oldbmp = NULL;
						COLORREF color;
						COLORREF transparent_color = (COLORREF)iconItem->udTransColor;
						bmp->CreateDiscardableBitmap(dc,width,height);
						oldbmp = selectedDC->SelectObject(bmp);
						for(int i = 0; i < height; i++)
						{
							for(int j = 0; j < width; j++)
							{
								color = iconItem->udDC->GetPixel(j,i);
								color = RGB(255-GetRValue(color),255-GetGValue(color),255-GetBValue(color));
								selectedDC->SetPixel(j,i,color);
							}
						}
						r = 255-GetRValue(transparent_color);
						g = 255-GetGValue(transparent_color);
						b = 255-GetBValue(transparent_color);
						TransparentBlt(dc->m_hDC, pixX - int(width * 0.5),pixY - int(height * 0.5), width, height, 
										selectedDC->m_hDC, 0, 0, width, height, RGB(r,g,b));
						selectedDC->SelectObject(oldbmp);
						bmp->DeleteObject();
						selectedDC->DeleteDC();
						delete selectedDC;
						delete bmp;
					}
				}
			}
		}
	}
	else if( si->pointType == ptFontChar )
	{
		if( si->FontCharListIndex == -1)
			dc->SetPixelV(pixX,pixY,si->pointClr);
		else
		{
			CFont * charFont = new CFont();
			CFont * oldFont = NULL;
			COLORREF oldColor;
			char cVal[2];
			LOGFONT lf;
			RECT myRect;
			CSize mySize;
			udPointListFontCharItem *fontCharItem = sli->PointFontCharList[si->FontCharListIndex];
			cVal[0] = (char)fontCharItem->udPointFontCharIdx;
			cVal[1] = '\0';
			sli->GetFont()->GetLogFont(&lf);
			lf.lfHeight = (LONG)(sli->fontSize * 10);
			charFont->CreatePointFontIndirect(&lf, dc);
			oldFont = dc->SelectObject(charFont);
			oldColor = dc->SetTextColor(fontCharItem->udPointFontCharColor);
			mySize = dc->GetTextExtent(cVal, 1);
			myRect.top = pixY - (mySize.cy / 2);
			myRect.bottom =myRect.top + mySize.cy;
			myRect.left = pixX - (mySize.cx / 2);
			myRect.right = myRect.left  + mySize.cx;
			dc->DrawTextEx(cVal, 1, &myRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE , NULL);
			dc->SelectObject(oldFont);
			dc->SetTextColor(oldColor);
			charFont->DeleteObject();
		}
	}
}

// ************************************************************
//		DrawFontChar()
// ************************************************************
void CMapView::DrawFontChar(CDC * dc, ShapeLayerInfo * sli, ShapeInfo * si, int pixX, int pixY, double hsize)
{

	if( si->FontCharListIndex == -1)
		dc->SetPixelV(pixX,pixY,si->pointClr);
	else
	{
		CFont * charFont = new CFont();
		CFont * oldFont = NULL;
		COLORREF oldColor;
		char cVal[2];
		LOGFONT lf;
		RECT myRect;
		CSize mySize;
		udPointListFontCharItem *fontCharItem = sli->PointFontCharList[si->FontCharListIndex];
		cVal[0] = (char)fontCharItem->udPointFontCharIdx;
		cVal[1] = '\0';
		sli->GetFont()->GetLogFont(&lf);
		lf.lfHeight = (long)(sli->fontSize * 10.0);
		charFont->CreatePointFontIndirect(&lf, dc);
		oldFont = dc->SelectObject(charFont);
		oldColor = dc->SetTextColor(fontCharItem->udPointFontCharColor);
		mySize = dc->GetTextExtent(cVal, 1);
		myRect.top = pixY - (mySize.cy / 2);
		myRect.bottom =myRect.top + mySize.cy;
		myRect.left = pixX - (mySize.cx / 2);
		myRect.right = myRect.left  + mySize.cx;
		dc->DrawTextEx(cVal, 1, &myRect, DT_CENTER | DT_VCENTER | DT_SINGLELINE , NULL);
		dc->SelectObject(oldFont);
		dc->SetTextColor(oldColor);
		charFont->DeleteObject();
	}
}


// ***************************************************************
//	  DrawShapefile
// ***************************************************************
void CMapView::DrawShapefile(const CRect & rcBounds, CDC * dc, Layer * layer)
{
# ifdef _DEBUG //Code added by Lailin Chen to profile the time consumption of this function. --- Lailin Chen 11/7/2005
DWORD startTick = ::GetTickCount();
#endif
FILE * shpIndex;	// lsu 07/03/09 - to be able to close file in catch block
try
{
	if( layer->object == NULL )
		return;

	//if( ( layer->flags & Visible ) == 0 )
	double scale = GetCurrentScale();
	if( ! layer->IsVisible(scale) )
		return;

	//CComPtr<IShapefile> ishp = NULL;
	IShapefile* ishp = NULL;
	layer->object->QueryInterface(IID_IShapefile,(void**)&ishp);
	if( ishp == NULL )
	{	m_lastErrorCode = tkINTERFACE_NOT_SUPPORTED;
		if( m_globalCallback != NULL )
				m_globalCallback->Error(m_key.AllocSysString(),A2BSTR(ErrorMsg(m_lastErrorCode)));
		return;
	}

	//Check Drawing Bounds
	double xm,ym,zm,xM,yM,zM;
	IExtents * box=NULL;
	ishp->get_Extents(&box);
	box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
	box->Release();
	box=NULL;

	if( OUT_OF_VIEW(xm,ym,xM,yM,extents) )
		return;
	
	AlignShapeLayerAndShapes(layer);
	ShapeLayerInfo * sli = (ShapeLayerInfo*)layer->addInfo;

	ShpfileType shpfiletype = SHP_NULLSHAPE;
	ishp->get_ShapefileType(&shpfiletype);

	long numberShapes = 0;
	ishp->get_NumShapes(&numberShapes);

	VARIANT_BOOL isEditing = FALSE;
	ishp->get_EditingShapes(&isEditing);

	CBrush * oldBrush=NULL, * newBrush=NULL;
	CPen * oldPen=NULL, * newPen=NULL;

	CBrush * slOldBrush=NULL;
	CPen * slOldPen=NULL;
	CBrush * slPointBrush = new CBrush();
	CPen * slPointPen = new CPen();
	CPen * slLinePen = new CPen();
	CBrush * slLineBrush = new CBrush();
	CBrush * slFillBrush = new CBrush();
	CPen * slFillPen = new CPen();

	slPointBrush->CreateSolidBrush(sli->pointClr);
	slPointPen->CreatePen(PS_SOLID,1,sli->pointClr);
	slLinePen->CreatePen(PS_SOLID,(int)sli->lineWidth,sli->lineClr);
	slLineBrush->CreateSolidBrush(sli->lineClr);
	slFillBrush->CreateSolidBrush(sli->fillClr);
	slFillPen->CreatePen(PS_SOLID,1,sli->fillClr);

	slOldBrush = dc->SelectObject(slPointBrush);
	slOldPen = dc->SelectObject(slPointPen);

	OLE_COLOR LastBrushColor;

	// lsu 07/03/2009 - if points are drawn it's necessary to use 2 pens to draw each polyline/polygon and 2 brushes to draw each polygon;
	// attemts to use only one pen for all shapes lead to lines and fill drawn with point color;
	// so these 2 variables may be used for point shapefiles only; it's possible to use them for other types when points aren't drawn,
	// but I think it doesn't make sense to check conditions like (sliPenInDC == false || si->flags & slfDrawPoint) for each shape
	// as there is no noticeable gain in speed but evidently more comlicated code
	bool sliBrushInDC = false;
	bool sliPenInDC = false;

	double pixX, pixY;

	dc->SetPolyFillMode(WINDING);
	
	Gdiplus::Graphics graphics(dc->m_hDC);
	Gdiplus::Color penColor(0, 0, 0, 0);
	Gdiplus::Pen pen(penColor);
	Gdiplus::Color brushColor(0, 0, 0, 0);
	Gdiplus::SolidBrush brush(brushColor);

	if( isEditing == FALSE && ((CShapefile*)ishp)->get_File() != NULL) //layer->file != NULL )
	{
		//(sm) 08-24-2009 move spatial index functionaltiy to shapefile
		CComSafeArray<long> selectResults;			

		//get visible shapes from shapefile select using spatial indexes (if available/efficient)
		VARIANT_BOOL useSpatialIndex;
		IExtents* pExtents = NULL;
		CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&pExtents );
		pExtents->SetBounds(extents.left, extents.bottom, 0, extents.right, extents.top, 0);
		ishp->get_CanUseSpatialIndex(pExtents, &useSpatialIndex);
		if (useSpatialIndex)
		{
			VARIANT_BOOL selectSucceeded;
			VARIANT	vVarSel;
			HRESULT retCode = ishp->SelectShapes(pExtents, 0.0, INTERSECTION, &vVarSel, &selectSucceeded); //shape file uses index to select shapes

			if (retCode != S_OK || selectSucceeded != VARIANT_TRUE)  // an error
			{
				useSpatialIndex = VARIANT_FALSE;
			}
			else
			{
				selectResults = vVarSel.parray;
				numberShapes = selectResults.GetCount();
			}
		}
		pExtents->Release();
		pExtents=NULL;


		BSTR fname;
		ishp->get_Filename(&fname);
		int b_strlen = wcslen(fname);

		//Neio mofified following http://www.mapwindow.org/phorum/read.php?7,12162 by gischai
		//Multilanguage support problem
		char * sFilename = new char[(b_strlen+1)<<1];
		int sFilenamelast = WideCharToMultiByte(CP_ACP,0,fname,b_strlen,sFilename,b_strlen<<1,0,0);
		sFilename[sFilenamelast] = 0;

		// Open the SHX
		int outCnt = 0;
		//ofstream out;
		int shapeIdx = 0;

		//// Optimize
		//ifstream getsize1(sFilename);
		//getsize1.seekg (0, ios::end);
		//int shpFileSize = getsize1.tellg();
		//getsize1.close();
		//ofstream out("c:\\profile-newmethod.txt",ios::app);
		//Init_Timer();
		//Start_Timer();
		//int optimizeSFType;
		//-Optimize
		sFilename[sFilenamelast - 1] = 'x'; // Replace ...shp with ...shx
		::SysFreeString(fname);

		//FILE * shpIndex = fopen(sFilename, "rb"); // Open shape index for reading
		shpIndex = fopen(sFilename, "rb");
		
		if (shpIndex != NULL)
		{
			//rewind(shpIndex);			// original version;
			//ifstream getsize(sFilename);		// stream doesn't open if we have cyrillic characters
			//getsize.seekg (0, ios::end);
			//int indexFileSize = getsize.tellg();
			//getsize.close();

			fseek (shpIndex, 0, SEEK_END);		// Fix from http://www.mapwindow.org/phorum/read.php?5,13054,13054#msg-13054
			int indexFileSize = ftell(shpIndex);
			rewind(shpIndex);

			fseek(shpIndex, 100, SEEK_SET); 	// Skip the header
			char * indexData = new char[indexFileSize - 100]; 	// here is an exception in original version(length of array is negative)
			long result = fread(indexData, sizeof(char), indexFileSize - 100, shpIndex);
			
			// Prepare to read shp data
			FILE * shpfile = ((CShapefile*)ishp)->get_File(); //layer->file;
			rewind(shpfile);

			int iShapeOffset;
			char * cShapeOffset;

			int readOffset;
			int contentLength;

			int * intdata;

			long percentOfShapesDrawn = 0;
			long newPercentOfShapesDrawn = 0;

			register int i;
			for( i = 0; i < numberShapes; i++ )
			{
				if( m_globalCallback != NULL )
				{
					newPercentOfShapesDrawn = (i+1)/numberShapes*100;
					if(newPercentOfShapesDrawn > percentOfShapesDrawn)
					{	
						percentOfShapesDrawn = newPercentOfShapesDrawn;
						m_globalCallback->Progress(m_key.AllocSysString(),percentOfShapesDrawn,A2BSTR("Drawing Shape File"));
					}
				}

				if (useSpatialIndex)
				{
					register int offset = selectResults.GetAt(i);
					shapeIdx = offset;
					iShapeOffset=((int*)&indexData[offset*8])[0]; // Index Records are 8 bytes; skip to proper index record.
				}
				else
					iShapeOffset=((int*)&indexData[i*8])[0]; // Index Records are 8 bytes; skip to proper index record.
				cShapeOffset = (char*)&iShapeOffset;
				SWAP_ENDIAN_INT(cShapeOffset);
				readOffset = (iShapeOffset)*2;

				if (useSpatialIndex)
				{
					int offset = selectResults.GetAt(i);
					iShapeOffset=((int*)&indexData[offset*8])[1]; // Index Records are 8 bytes; skip to proper index record.
				}
				else					
					iShapeOffset=((int*)&indexData[i*8])[1]; // Index Records are 8 bytes; skip to proper index record.
				cShapeOffset = (char*)&iShapeOffset;
				SWAP_ENDIAN_INT(cShapeOffset);
				contentLength = (iShapeOffset)*2;

				fseek(shpfile, (long)readOffset, SEEK_SET);

				char * shapeData = new char[contentLength * 2];  // (todo) Does contentLength include the header? Assuming yes here.
				result = fread(shapeData, sizeof(char), contentLength * 2, shpfile); // Load up the data. *2 for conversion from 16-bit words to 8-bit words

				intdata = (int*) shapeData;
				ShapeInfo * si;
				if (useSpatialIndex)    // ajp 18-02-09, added hasIndex code, was selecting wrong shapeInfo
				{
					si = sli->shapeInfo[shapeIdx];
				}
				else
				{
					si = sli->shapeInfo[i];
				}
				ShpfileType shptype = (ShpfileType)intdata[2];
				//optimizeSFType = shptype;
				//SHP_NULLSHAPES
				// cdm 11/22/2006 what's with the two IF's comparing the same thing? 
				// if( shptype != shpfiletype )
				// {
					if( shptype != shpfiletype )
					{
						if( shptype != SHP_NULLSHAPE )
						{	m_lastErrorCode = tkINVALID_SHP_FILE;
							if( m_globalCallback != NULL )
								m_globalCallback->Error( m_key.AllocSysString(),  A2BSTR(ErrorMsg(m_lastErrorCode) ) );
							// CDM -- continue anyway, and draw the rest of the shapes.
							// Note that the error callback will have fired.
							// MessageBox("Warning: Bad Shape Found");
							// break;

							delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
							continue;
						}
						else
						{
							delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
							continue;
						}
					}
				// }

				//Shape specific record contents
				if( shptype == SHP_POINT || shptype == SHP_POINTZ || shptype == SHP_POINTM )
				{
					//Skip the Shape it's not visible;
					if( (si->flags & sfVisible) == 0 )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//Beginning of Points
					double * points = (double *)&(shapeData[12]); // Char is 1 byte, need to skip 12 bytes past
					if( si->flags & slfDrawPoint )
					{
						if( OUT_OF_VIEW(points[0],points[1],points[0],points[1],extents) )
						{
							delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
							continue;
						}

						double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	if( sliBrushInDC == false || sliPenInDC == false )
							{
								dc->SelectObject(slPointBrush);
								dc->SelectObject(slPointPen);
								sliBrushInDC = true;
								sliPenInDC = true;
							}
						}

						PROJECTION_TO_PIXEL(points[0],points[1],pixX,pixY);
						//Rob Cairns 10 Jan 06
						int pxX = Utility::Rint(pixX);
						int pxY = Utility::Rint(pixY);
						
						DrawShapePoint(dc, sli, si, pxX, pxY, (int)hsize);
						//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);			// lsu: 29-jan-2011

						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
				}
				else if( shptype == SHP_POLYLINE || shptype == SHP_POLYLINEZ || shptype == SHP_POLYLINEM )
				{	int numParts;
					int numPoints;
					int *parts;

					double * bounds;
					//Bounds
					bounds = (double*)(&(shapeData[12]));
					if( OUT_OF_VIEW(bounds[0],bounds[1],bounds[2],bounds[3],extents ) )
					{
						outCnt++;
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}


					//#Parts
					numParts=intdata[11];
					//#Points
					numPoints=intdata[12];
					//Parts
					parts=(int*)&(intdata[13]);

					//Skip the Shape it's not visible;
					if( (si->flags & sfVisible ) == 0 )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//Beginning of Points
					int * begOfPts = &(intdata[13+numParts]);
					double * points = (double*)begOfPts;

					if( (si->flags & slfDrawLine) && (si->lineWidth > 0) )
					{	CPoint * pnts = new CPoint[numPoints];

						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone || si->lineWidth != sli->lineWidth )
						{
							newPen = new CPen();
							if(si->lineStipple != lsNone)
							{
								get_LineStipplePen(sli, si, newPen);
								//LINE_STIPPLE(sli,si,newPen);		// lsu 29 jan 2011
							}
							else
							{
								newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
							}
							oldPen = dc->SelectObject(newPen);
						}
						else
						{
							//if( sliPenInDC == false)	// lsu 07/03/2009
							//{
								dc->SelectObject(slLinePen);
							//	sliPenInDC = true;
							//}
						}

						if( numParts == 0 )  // Does this ever get hit?
						{
							int cnt=0, pti2;
							register int j;
							for( j = 0; j < numPoints; j++ )
							{	pti2=2*j;
								PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pixX);
								int pxY = Utility::Rint(pixY);
								pnts[cnt++]=CPoint(pxX,pxY);
							}
							dc->Polyline(pnts,numPoints);
						}
						else
						{
							int numRepeats = 1;
							if (si->lineStipple == lsDoubleSolid) numRepeats = 2; // Repeat twice for double line
							if (si->lineStipple == lsDoubleSolidPlusDash) numRepeats = 3; // Twice for double line, third for the dashes.
							
							for (int completedRepeats = 0; completedRepeats < numRepeats; completedRepeats++)
							{
								register int p, j;
								long pointIndex, lastPolygonPoint, pti2;
								for( p = 0; p < numParts; p++ )
								{
									pointIndex=parts[p];
									lastPolygonPoint=p+1<numParts?parts[p+1]:numPoints;

									if(pointIndex<0)break;
									if(pointIndex>=numPoints)break;
									if(lastPolygonPoint<0)break;
									if(lastPolygonPoint>numPoints)break;

									int cnt=0;

								for( j = pointIndex; j < lastPolygonPoint; j++ )
									{	pti2=2*j;
										PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
										//Rob Cairns 10 Jan 06
										int pxX = Utility::Rint(pixX);
										int pxY = Utility::Rint(pixY);
										pnts[cnt++]=CPoint(pxX,pxY);
									}
									// If necessary, do adjustments, based on the line stipple type.
									switch(si->lineStipple)
									{
										case lsTrainTracks:
										{
											// Draw train track blocks according to the chunks
											// of the polyline about to be drawn
											// (technically the blocks will be drawn prior to the line itself)
											newPen->DeleteObject();
											newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
											dc->SelectObject(newPen);

											LPPOINT trackLine = new POINT[3];
											if (trackLine == NULL)
											{
												delete [] trackLine;
												break;
											}
											for (int i = 1; i < numPoints - 1; i++)
											{
												findPerpPoints(4, pnts[i].x, pnts[i].y, pnts[i+1].x, pnts[i+1].y, trackLine[0].x, trackLine[0].y, trackLine[1].x, trackLine[1].y);
												trackLine[2].x = (trackLine[0].x + trackLine[1].x) / 2;
												trackLine[2].y = (trackLine[0].y + trackLine[1].y) / 2;
												dc->Polyline(trackLine, 2);
											}

											delete [] trackLine;
											break;
										}
										case lsDoubleSolid:
										{
											newPen->DeleteObject();
											newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
											dc->SelectObject(newPen);
											adjustLine("DOUBLESOLID", pnts, numPoints, completedRepeats);
											break;
										}
										case lsDoubleSolidPlusDash:
										{
											if (completedRepeats == 0)
											{
												newPen->DeleteObject();
												newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
												dc->SelectObject(newPen);
												adjustLine("DOUBLESOLID", pnts, numPoints, completedRepeats);
											}
											else if (completedRepeats == 2)
											{
												newPen->DeleteObject();
												newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
												dc->SelectObject(newPen);
												adjustLine("DOUBLESOLID", pnts, numPoints, 1);
											}
											else
											{
												newPen->DeleteObject();
												newPen->CreatePen(PS_DOT, (int)si->lineWidth,si->lineClr);
												dc->SelectObject(newPen);
											}
											break;
										}
									}
									// Draw the array of points:
									dc->Polyline(pnts,lastPolygonPoint-pointIndex);
								}
							}

							if( newPen )
							{
								dc->SelectObject(oldPen);
								newPen->DeleteObject();
								delete newPen;
								newPen = NULL;
							}

							delete [] pnts;
							pnts=NULL;
						}
					}

					if( si->flags & slfDrawPoint )
					{
						double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	//if( sliBrushInDC == false || sliPenInDC == false)		// lsu 07/03/2009
							//{
								dc->SelectObject(slPointBrush);
								dc->SelectObject(slPointPen);
							//	sliBrushInDC = true;
							//	sliPenInDC = true;
							//}
						}

						register int jjj;
						for( jjj = 0; jjj < numPoints; jjj++ )
						{	PROJECTION_TO_PIXEL(points[2*jjj],points[2*jjj+1],pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);
							DrawShapePoint(dc,sli,si,pxX,pxY,(int)hsize);
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);			// lsu 29 - jan - 2011
						}
						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
				}
				else if( shptype == SHP_POLYGON || shptype == SHP_POLYGONZ || shptype == SHP_POLYGONM )
				{
					int numParts;
					int numPoints;
					int *parts;

					double * bounds;
					//Bounds
					bounds = (double*)(&(shapeData[12]));  // This is wrong!
					if( OUT_OF_VIEW(bounds[0],bounds[1],bounds[2],bounds[3],extents ) )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//#Parts
					numParts=intdata[11];
					//#Points
					numPoints=intdata[12];
					//Parts
					parts=&(intdata[13]);

					//Skip the Shape it's not visible;
					if( (si->flags & sfVisible) == 0 )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//Beginning of Points
					int * begOfPts = &(intdata[13+numParts]);
					double * points = (double*)begOfPts;

					if( si->flags & slfDrawFill )
					{	CPoint * pnts = new CPoint[numPoints];

						//cdm 10/31/2006 -- not always the case; fill color may change for one shape only midway through.
						//if( si->fillClr != sli->fillClr || si->fillStipple != fsNone )
						if(si->fillStipple != fsNone)
						{
							if (newBrush)
							{
								dc->SelectObject(oldBrush);
								newBrush->DeleteObject();
								delete newBrush;
							}
							newBrush = new CBrush();
							FILL_STIPPLE(dc,si,newBrush);
							dc->SetBkColor(si->fillClr);
							if (si->transparentStipple)
								dc->SetBkMode(TRANSPARENT);
							else
								dc->SetBkMode(OPAQUE);
							LastBrushColor = si->stippleLineClr;
							oldBrush = dc->SelectObject(newBrush);
						}
						else
						{	
							if (newBrush)
							{
								dc->SelectObject(oldBrush);
								newBrush->DeleteObject();
								delete newBrush;
							}

							newBrush = new CBrush();
							FILL_STIPPLE(dc,si,newBrush); // Just creates solid brush since fstype==none
							dc->SetBkMode(TRANSPARENT);
							dc->SetBkColor(RGB(255,255,255));
							LastBrushColor = si->fillClr;
							oldBrush = dc->SelectObject(newBrush);

							long alpha = ((long)si->trans)<<24;
							brushColor.SetValue(alpha | BGR_TO_RGB(si->fillClr));
						}

						if( (si->lineWidth == 0) || !(si->flags & slfDrawLine) )
						{
							newPen = new CPen();
							newPen->CreatePen(PS_SOLID,1,si->lineClr);
							oldPen = dc->SelectObject(newPen);
							penColor.SetValue(0xFF000000 | BGR_TO_RGB(si->fillClr));
						}
						else if( si->lineClr != sli->lineClr || si->lineStipple != lsNone || si->lineWidth != sli->lineWidth)
						{
							newPen = new CPen();
							if(si->lineStipple != lsNone)
							{
								get_LineStipplePen(sli,si,newPen);		// lsu: 29-jan-2011
								//LINE_STIPPLE(sli,si,newPen);
							}
							else
							{
								newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
							}
							oldPen = dc->SelectObject(newPen);
							//sliPenInDC = false;
							penColor.SetValue(0xFF000000 | BGR_TO_RGB(si->lineClr));
						}
						else
						{	
							//if( sliPenInDC == false )			// lsu 07/03/2009
							//{
								dc->SelectObject(slLinePen);
							//	sliPenInDC = true;
								penColor.SetValue(0xFF000000 | BGR_TO_RGB(sli->lineClr));
							//}
						}

						if( numParts < 2 )
						{
							int cnt=0, pti2;
							register int j;
							Gdiplus::Point * gdipnts = new Gdiplus::Point[numPoints];
							for( j = 0; j < numPoints; j++ )
							{	pti2=2*j;
								PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pixX);
								int pxY = Utility::Rint(pixY);
								//cdm 10/31/2006 if (sli->trans == 255)
								if (si->trans == 255 || si->fillStipple != fsNone)
								{
									// No partial transparency. Enable fill stipples
									// by using old drawing method
									pnts[cnt++]=CPoint(pxX,pxY);
								}
								else
								{
									// Do it the new way to allow transparency
									gdipnts[cnt++]=Gdiplus::Point(pxX,pxY);
								}
							}
							//cdm 10/31/2006 if (sli->trans == 255)
							if (si->trans == 255 || si->fillStipple != fsNone)
							{
								// No partial transparency.	Enable fill stipples
								// by using old drawing method
								dc->Polygon(pnts,numPoints);
							}
							else
							{
								// The new way
								brush.SetColor(brushColor);
								graphics.FillPolygon(&brush, gdipnts, numPoints, Gdiplus::FillModeWinding);
							
								if(si->flags & slfDrawLine) {
									pen.SetColor(penColor);
									pen.SetWidth(si->lineWidth);
									graphics.DrawPolygon(&pen, gdipnts, numPoints);
								}
							}
							delete [] gdipnts;
						}
						else if (si->trans != 255 && si->fillStipple == fsNone) // Chris M 10/31/2006 - Multipart -- with transparency. Use GDI+ (slower)
						{
							register int p = 0, j;
							register int pxX, pxY;
							int fpx, fpy;
							long pointIndex, lastPolygonPoint, pti2;
							
							brush.SetColor(brushColor);

							// numPoints is obvious; +(numParts*2) is due to occasionally needing to close a hole's polygon, and needing to loop back to original polygon if doing transparent fill on a multipart (holes) polygon.
							Gdiplus::Point * allpnts = new Gdiplus::Point[numPoints + (numParts*2)];

							Gdiplus::Point returnPoint;
							int acnt = 0;

							for( p = 0; p < numParts; p++)
							{
								int cnt = 0;
								pointIndex=parts[p];
								lastPolygonPoint=p+1<numParts?parts[p+1]:numPoints;

								if(pointIndex<0)break;
								if(pointIndex>=numPoints)break;
								if(lastPolygonPoint<0)break;
								if(lastPolygonPoint>numPoints)break;

								int subNumPoints = lastPolygonPoint - pointIndex - 1;
								Gdiplus::Point * gdipnts = new Gdiplus::Point[subNumPoints];

								for( j = pointIndex; j < lastPolygonPoint - 1; j++ )
								{	pti2=2*j;
									PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
									//Rob Cairns 10 Jan 06
									pxX = Utility::Rint(pixX);
									pxY = Utility::Rint(pixY);
									gdipnts[cnt++]=Gdiplus::Point(pxX,pxY);
									allpnts[acnt++]=Gdiplus::Point(pxX,pxY);

									// Save first point for potential completion
									if (j == pointIndex)
									{
										fpx = pxX;
										fpy = pxY;
									}
								}

								// Close the polygon for filling?
								if (pxX != fpx || pxY != fpy)
								{
									allpnts[acnt++]=Gdiplus::Point(fpx, fpy);
								}

								if (p == 0)
								{
									// Save the first part's first point to return to, when filling holes
									returnPoint = Gdiplus::Point(fpx,fpy);
								}
								else if (p != 0)
								{
									// Add the return point - in a hole now
									allpnts[acnt++] = Gdiplus::Point(returnPoint.X,returnPoint.Y);
								}

								// Inverse (fill holes only - interesting effect):
								// graphics.FillPolygon(&brush, gdipnts, subNumPoints, Gdiplus::FillModeWinding);
								// (comment out FillPolygon below to see this in action though)

								if(si->flags & slfDrawLine) {
									pen.SetColor(penColor);
									pen.SetWidth(si->lineWidth);
									graphics.DrawPolygon(&pen, gdipnts, subNumPoints);
								}
								
								delete [] gdipnts;
							}

							graphics.FillPolygon(&brush, allpnts, acnt, Gdiplus::FillModeAlternate);

							delete [] allpnts;
						}
						else // Chris M 10/31/2006 - Multipart -- no partial transparency. Do quickly with polypolygon.
						{
							register int p, j;
							long pointIndex, lastPolygonPoint, pti2;
							int * pntcnt = new int[numParts];
							int cnt=0;
							for( p = 0; p < numParts; p++ )
							{
								pointIndex=parts[p];
								lastPolygonPoint=p+1<numParts?parts[p+1]:numPoints;

								if(pointIndex<0)break;
								if(pointIndex>=numPoints)break;
								if(lastPolygonPoint<0)break;
								if(lastPolygonPoint>numPoints)break;

								for( j = pointIndex; j < lastPolygonPoint - 1; j++ )
								{	pti2=2*j;
									PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
									//Rob Cairns 10 Jan 06
									int pxX = Utility::Rint(pixX);
									int pxY = Utility::Rint(pixY);
									pnts[cnt++]=CPoint(pxX,pxY);
								}
								pntcnt[p] = lastPolygonPoint - pointIndex - 1;
							}
							dc->PolyPolygon(pnts,pntcnt,numParts);

							delete [] pntcnt;
							pntcnt = NULL;
						}

						delete [] pnts;
						pnts=NULL;

						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
					else if( (si->flags & slfDrawLine) && (si->lineWidth > 0) )
					{	CPoint * pnts = new CPoint[numPoints];

						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone )
						{
							newPen = new CPen();
							//LINE_STIPPLE(sli,si,newPen);	// lsu: 29-jan-2011
							get_LineStipplePen(sli,si,newPen);
							oldPen = dc->SelectObject(newPen);
						}
						else
						{
							dc->SelectObject(slLinePen);
						}

						register int p, j;
						long pointIndex, lastPolygonPoint, pti2;
						for( p = 0; p < numParts; p++ )
						{
							pointIndex=parts[p];
							lastPolygonPoint=p+1<numParts?parts[p+1]:numPoints;

							if(pointIndex<0)break;
							if(pointIndex>=numPoints)break;
							if(lastPolygonPoint<0)break;
							if(lastPolygonPoint>numPoints)break;

							int cnt=0;

							for( j = pointIndex; j < lastPolygonPoint; j++ )
							{	pti2=2*j;
								PROJECTION_TO_PIXEL(points[pti2],points[pti2+1],pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pxX);
								int pxY = Utility::Rint(pxY);
								pnts[cnt++]=CPoint((int)pixX,(int)pixY);
							}
							dc->Polyline(pnts,lastPolygonPoint-pointIndex);
						}

						delete [] pnts;
						pnts=NULL;

						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}

					if( si->flags & slfDrawPoint )
					{	double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	
							dc->SelectObject(slPointBrush);
							dc->SelectObject(slPointPen);
						}

						register int jjj;
						for( jjj = 0; jjj < numPoints; jjj++ )
						{	PROJECTION_TO_PIXEL(points[2*jjj],points[2*jjj+1],pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);	
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);		// lsu: 29-jan-2011
							DrawShapePoint(dc,sli,si,pxX,pxY, (int)hsize);
						}
						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
				}
				else if( shptype == SHP_MULTIPOINT || shptype == SHP_MULTIPOINTZ || shptype == SHP_MULTIPOINTM )
				{	int numPoints;

					double * bounds;
					//Bounds
					bounds = (double*)(&(shapeData[12]));
					if( OUT_OF_VIEW(bounds[0],bounds[1],bounds[2],bounds[3],extents ) )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//Skip the Shape it's not visible;
					if( (si->flags & sfVisible) == 0 )
					{
						delete [] shapeData;//Delete the shapeData array since the delete at the end of the loop won't be called once we continue.
						continue;
					}

					//NumPoints
					// Chris Michaelis May 4 2006 -- this doesn't seem to be accurate. 
					// Apparently the comment below was on the nail....

					// OLD: This index number, and the # below, may be wrong - multipointM untested
					// OLD: numPoints = intdata[6];  

					// This works better:
					numPoints = ((int*)shapeData)[11];

					//Beginning of Points
					// Chris Michaelis May 2006 -- Beginning of points is also wrong.
					//int * begOfPts = &(intdata[7]);
					//double * points = (double *)begOfPts;
					// Correct:
					double * points = (double*)&(shapeData[48]);

					if( si->flags & slfDrawPoint )
					{	double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	if( sliBrushInDC == false || sliPenInDC == false )
							{
								dc->SelectObject(slPointBrush);
								dc->SelectObject(slPointPen);
								sliBrushInDC = true;
								sliPenInDC = true;
							}
						}

						register int jjj;
						for( jjj = 0; jjj < numPoints; jjj++ )
						{	PROJECTION_TO_PIXEL(points[2*jjj],points[2*jjj+1],pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);	
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);	lsu: 29-jan-2011
							DrawShapePoint(dc,sli,si,pxX,pxY,(int)hsize);
						}
						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
				}
				delete [] shapeData;
			}//for
				//}
			/*delete [] data;
			data = NULL;*/
			fclose(shpIndex);
			delete [] indexData;
			indexData = NULL;
		}
		else
		{
			perror("ERROR OPENING *.shx");
		}
		delete [] sFilename;
	}
	//Memory Based
	else
	{
		//Neio 20090721 Add Query QuadTree
		int* qtreeResult;
		int qtreeResultCount = numberShapes;
		IExtents * bBox = NULL;
		VARIANT_BOOL _useQTree = VARIANT_FALSE;
		ishp->get_UseQTree(&_useQTree);
		if(_useQTree == VARIANT_TRUE)
		{
			CoCreateInstance( CLSID_Extents, NULL, CLSCTX_INPROC_SERVER, IID_IExtents, (void**)&bBox );
			bBox->SetBounds(extents.left,extents.bottom,0,extents.right, extents.top,0);
			((CShapefile*)ishp)->QuickQueryInEditMode(bBox,&qtreeResult,&qtreeResultCount);
			bBox->Release();
			bBox=NULL;
		}

		if( shpfiletype == SHP_POINT || shpfiletype == SHP_POINTZ || shpfiletype == SHP_POINTM ||
			shpfiletype == SHP_MULTIPOINT || shpfiletype == SHP_MULTIPOINTZ || shpfiletype == SHP_MULTIPOINTM )
		{
			register int i;
			//Neio 2009/07/21 Add QuadTree Support
			for(register int j = 0; j < qtreeResultCount ; j++)
			//for( i = 0; i < numberShapes; i++ )
			{
				//Neio 2009/07/21
				if(_useQTree == VARIANT_TRUE)
				{
					i = qtreeResult[j];
				}
				else
					i = j;

				ShapeInfo * si = sli->shapeInfo[i];

				if( si->flags & sfVisible )
				{
					if( si->flags & slfDrawPoint )
					{
						CComPtr<IShape> shp = NULL;
						ishp->get_Shape( i, &shp );

						ishp->QuickExtents(i, &box);
						box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
						box->Release();

						if( OUT_OF_VIEW(xm,ym,xM,yM,extents) )
							continue;

						long numberPoints = 0;
						shp->get_NumPoints( &numberPoints );
						register int j;
						for( j=0;j<numberPoints;j++)
						{	double hsize=si->pointSize*.5;
							if( si->pointClr != sli->pointClr )
							{	newBrush=new CBrush();
								newBrush->CreateSolidBrush(si->pointClr);
								oldBrush=dc->SelectObject(newBrush);
								newPen=new CPen();
								newPen->CreatePen(PS_SOLID,1,si->pointClr);
								oldPen=dc->SelectObject(newPen);
							}
							else
							{	if( sliBrushInDC == false || sliPenInDC == false )
								{
									dc->SelectObject(slPointBrush);
									dc->SelectObject(slPointPen);
									sliBrushInDC = true;
									sliPenInDC = true;
								}
							}

							double px, py;
							IPoint * pnt=NULL;
							shp->get_Point(j,&pnt);
							pnt->get_X(&px);
							pnt->get_Y(&py);
							pnt->Release();
							pnt=NULL;
							PROJECTION_TO_PIXEL(px,py,pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);			// lsu: 29-jan-2011
							DrawShapePoint(dc,sli,si,pxX,pxY,(int)hsize);

							if( newBrush )
							{
								dc->SelectObject(oldBrush);
								newBrush->DeleteObject();
								delete newBrush;
								newBrush = NULL;
							}
							if( newPen )
							{
								dc->SelectObject(oldPen);
								newPen->DeleteObject();
								delete newPen;
								newPen = NULL;
							}
						}
					}
				}
			}
		}
		else if( shpfiletype == SHP_POLYLINE || shpfiletype == SHP_POLYLINEZ || shpfiletype == SHP_POLYLINEM )
		{
			register int i;
			//Neio 2009/07/21 Add QuadTree Support
			for(register int j = 0; j < qtreeResultCount ; j++)
			//for( i = 0; i < numberShapes; i++ )
			{
				//Neio 2009/07/21
				if(_useQTree == VARIANT_TRUE){
					i = qtreeResult[j];
				}
				else
					i = j;

				ShapeInfo * si = sli->shapeInfo[i];

				if( si->flags & sfVisible )
				{
					CComPtr<IShape> shp = NULL;
					ishp->get_Shape( i, &shp );

					ishp->QuickExtents(i, &box);
					box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
					box->Release();

					if( OUT_OF_VIEW(xm,ym,xM,yM,extents) )
						continue;

					long numberPoints = 0;
					shp->get_NumPoints( &numberPoints );
					long numberParts = 0;
					shp->get_NumParts( &numberParts );
					IPoint * pnt = NULL;

					if( si->flags & slfDrawLine )
					{
						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone || si->lineWidth != sli->lineWidth )
						{
							newPen = new CPen();
							if(si->lineStipple != lsNone)
							{
								//LINE_STIPPLE(sli,si,newPen);
								get_LineStipplePen(sli,si,newPen);		// lsu: 29-01-2011
							}
							else
							{
								newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
							}
							
							oldPen = dc->SelectObject(newPen);

							/*newPen = new CPen();
							LINE_STIPPLE(sli,si,newPen);
							oldPen = dc->SelectObject(newPen);*/
						}
						else
						{
							dc->SelectObject(slLinePen);
						}

						CPoint * pnts = new CPoint[numberPoints];
						register int p, j;
						long pointIndex, lastPolygonPoint;
						if( numberParts == 0 )
						{
							int cnt=0;
							double prx, pry;
							for( j = 0; j < numberPoints; j++ )
							{	shp->get_Point( j, &pnt );
								pnt->get_X( &prx );
								pnt->get_Y( &pry );

								PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pixX);
								int pxY = Utility::Rint(pixY);
								pnts[cnt++]=CPoint(pxX,pxY);

								pnt->Release();
								pnt=NULL;
							}
							dc->Polyline(pnts,numberPoints);
						}
						else
						{
							for( p = 0; p < numberParts; p++ )
							{
								shp->get_Part( p, &pointIndex );
								if( p + 1 < numberParts )
									shp->get_Part( p + 1, &lastPolygonPoint );
								else
									lastPolygonPoint = numberPoints;

								if(pointIndex<0)break;
								if(pointIndex>=numberPoints)break;
								if(lastPolygonPoint<0)break;
								if(lastPolygonPoint>numberPoints)break;

								int cnt=0;
								double prx, pry;
								for( j = pointIndex; j < lastPolygonPoint; j++ )
								{	shp->get_Point( j, &pnt );
									pnt->get_X( &prx );
									pnt->get_Y( &pry );

									PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
									//Rob Cairns 10 Jan 06
									int pxX = Utility::Rint(pixX);
									int pxY = Utility::Rint(pixY);
									pnts[cnt++]=CPoint(pxX,pxY);

									pnt->Release();
									pnt=NULL;
								}
								dc->Polyline(pnts,lastPolygonPoint-pointIndex);
							}
						}

						delete [] pnts;
						pnts=NULL;

						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}

					if( si->flags & slfDrawPoint )
					{
						IShape * shp2 = NULL;
						ishp->get_Shape( i, &shp2 );
						long numberPoints = 0;
						shp2->get_NumPoints( &numberPoints );

						double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	
							//if( sliBrushInDC == false || sliPenInDC == false )	// lsu 07/03/09
							//{
								dc->SelectObject(slPointBrush);
								dc->SelectObject(slPointPen);
							//	sliBrushInDC = true;
							//	sliPenInDC = true;
							//}
						}

						double px, py;
						register int j;
						for( j=0;j<numberPoints;j++)
						{	IPoint * pnt=NULL;
							shp2->get_Point(j,&pnt);
							pnt->get_X(&px);
							pnt->get_Y(&py);
							pnt->Release();
							pnt=NULL;
							PROJECTION_TO_PIXEL(px,py,pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);		// lsu: 29-jan-2011
							DrawShapePoint(dc,sli,si,pxX,pxY,(int)hsize);
						}

						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
						//Rob Cairns 21 December 2005
						shp2->Release(); //Also renamed shp to shp2 to avoid confusion with CComPtr
					}
				}
			}
		}
		else if( shpfiletype == SHP_POLYGON || shpfiletype == SHP_POLYGONZ || shpfiletype == SHP_POLYGONM )
		{
			register int i;
			//Neio 2009/07/21 Add QuadTree Support
			for(register int j = 0; j < qtreeResultCount ; j++)
			//for( i = 0; i < numberShapes; i++ )
			{
				//Neio 2009/07/21
				if(_useQTree == VARIANT_TRUE){
					i = qtreeResult[j];
				}
				else
					i = j;

				ShapeInfo * si = sli->shapeInfo[i];

				if( si->flags & sfVisible )
				{
					CComPtr<IShape> shp = NULL;
					ishp->get_Shape( i, &shp );
					
					ishp->QuickExtents(i, &box);
					box->GetBounds(&xm,&ym,&zm,&xM,&yM,&zM);
					box->Release();

					if( OUT_OF_VIEW(xm,ym,xM,yM,extents) )
						continue;

					long numberPoints = 0;
					shp->get_NumPoints( &numberPoints );
					long numberParts = 0;
					shp->get_NumParts( &numberParts );
					IPoint * pnt = NULL;
					long lastPolygonPoint = numberPoints;

					if( si->flags & slfDrawFill )
					{
						CPoint * pnts = new CPoint[numberPoints];
						
						// lsu 07/03/09 - changed to the same implementation as in regular mode
						// line stipple for other shapefiles was drawn in the fill color of the current layer

						//if( si->fillClr != sli->fillClr || si->fillStipple != fsNone )
						//{
						//	// Chris Michaelis - Feb 28 2009 - For better hatching support
						//	// Also made changes to FILL_STIPPLE and added a bunch of interface properties
						//	newBrush = new CBrush();
						//	FILL_STIPPLE(dc,si,newBrush);
						//	dc->SetBkColor(si->fillClr);
						//	if (si->transparentStipple)
						//		dc->SetBkMode(TRANSPARENT);
						//	else
						//		dc->SetBkMode(OPAQUE);
						//	LastBrushColor = si->stippleLineClr;
						//	oldBrush = dc->SelectObject(newBrush);
						//}
						//else
						//{	//if( sliBrushInDC == false )		// lsu 07/03/09
						//	//{	
						//	dc->SelectObject(slFillBrush);
						//	//	sliBrushInDC = true;
						//	//}
						//}

						if(si->fillStipple != fsNone)
						{
							if (newBrush)
							{
								dc->SelectObject(oldBrush);
								newBrush->DeleteObject();
								delete newBrush;
							}
							newBrush = new CBrush();
							FILL_STIPPLE(dc,si,newBrush);
							dc->SetBkColor(si->fillClr);
							if (si->transparentStipple)
								dc->SetBkMode(TRANSPARENT);
							else
								dc->SetBkMode(OPAQUE);
							LastBrushColor = si->stippleLineClr;
							oldBrush = dc->SelectObject(newBrush);
						}
						else
						{	
							if (newBrush)
							{
								dc->SelectObject(oldBrush);
								newBrush->DeleteObject();
								delete newBrush;
							}

							newBrush = new CBrush();
							FILL_STIPPLE(dc,si,newBrush); // Just creates solid brush since fstype==none
							dc->SetBkMode(TRANSPARENT);
							dc->SetBkColor(RGB(255,255,255));
							LastBrushColor = si->fillClr;
							oldBrush = dc->SelectObject(newBrush);

							long alpha = ((long)si->trans)<<24;
							brushColor.SetValue(alpha | BGR_TO_RGB(si->fillClr));
						}

						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone )
						{
							newPen = new CPen();
							//LINE_STIPPLE(sli,si,newPen);		// lsu: 29-jan-2011
							get_LineStipplePen(sli,si,newPen);
							oldPen = dc->SelectObject(newPen);
						}
						else
						{	
							//if( sliPenInDC == false )		// lsu 07/03/09
							//{
								dc->SelectObject(slLinePen);
							//	sliPenInDC = true;
							//}
						}

						if( numberParts < 2 )
						{
							int cnt=0;
							register int j;
							double prx, pry;
							for( j = 0; j < numberPoints; j++ )
							{
								shp->get_Point( j, &pnt );
								pnt->get_X( &prx );
								pnt->get_Y( &pry );
								pnt->Release();
								pnt = NULL;

								PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pixX);
								int pxY = Utility::Rint(pixY);
								pnts[cnt++]=CPoint(pxX,pxY);
							}
							dc->Polygon(pnts,numberPoints);
						}
						else
						{
							register int p, j;
							long pointIndex, lastPolygonPoint;
							int * pntcnt = new int[numberParts];
							int cnt=0;
							double prx, pry;
							for( p = 0; p < numberParts; p++ )
							{
								shp->get_Part( p, &pointIndex );
								if( p + 1 < numberParts )
									shp->get_Part( p + 1, &lastPolygonPoint );
								else
									lastPolygonPoint = numberPoints;

								if(pointIndex<0)break;
								if(pointIndex>=numberPoints)break;
								if(lastPolygonPoint<0)break;
								if(lastPolygonPoint>numberPoints)break;

								for( j = pointIndex; j < lastPolygonPoint - 1; j++ )
								{	shp->get_Point( j, &pnt );
									pnt->get_X( &prx );
									pnt->get_Y( &pry );
									pnt->Release();
									pnt = NULL;

									PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
									//Rob Cairns 10 Jan 06
									int pxX = Utility::Rint(pixX);
									int pxY = Utility::Rint(pixY);
									pnts[cnt++]=CPoint(pxX,pxY);
								}
								pntcnt[p] = lastPolygonPoint - pointIndex - 1;
							}
							dc->PolyPolygon(pnts,pntcnt,numberParts);

							delete [] pntcnt;
							pntcnt = NULL;
						}

						delete [] pnts;
						pnts=NULL;

						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}
					else if( si->flags & slfDrawLine )
					{	CPoint * pnts = new CPoint[numberPoints];

						if( si->lineClr != sli->lineClr || si->lineStipple != lsNone )
						{
							newPen = new CPen();
							//LINE_STIPPLE(sli,si,newPen);		lsu: 29-jan-2011
							get_LineStipplePen(sli,si,newPen);
							oldPen = dc->SelectObject(newPen);
						}
						else
						{	//if( sliPenInDC == false )			// lsu 07/03/09
							//{
								dc->SelectObject(slLinePen);
							//	sliPenInDC = true;
							//}
						}

						register int p, j;
						long pointIndex, lastPolygonPoint;
						if( numberParts == 0 )
						{
							int cnt=0;
							double prx, pry;
							for( j = 0; j < numberPoints; j++ )
							{	shp->get_Point( j, &pnt );
								pnt->get_X( &prx );
								pnt->get_Y( &pry );

								PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
								//Rob Cairns 10 Jan 06
								int pxX = Utility::Rint(pixX);
								int pxY = Utility::Rint(pixY);
								pnts[cnt++]=CPoint(pxX,pxY);

								pnt->Release();
								pnt=NULL;
							}
							dc->Polyline(pnts,numberPoints);
						}
						else
						{
							for( p = 0; p < numberParts; p++ )
							{
								shp->get_Part( p, &pointIndex );
								if( p + 1 < numberParts )
									shp->get_Part( p + 1, &lastPolygonPoint );
								else
									lastPolygonPoint = numberPoints;

								if(pointIndex<0)break;
								if(pointIndex>=numberPoints)break;
								if(lastPolygonPoint<0)break;
								if(lastPolygonPoint>numberPoints)break;

								int cnt=0;
								double prx, pry;
								for( j = pointIndex; j < lastPolygonPoint; j++ )
								{	shp->get_Point( j, &pnt );
									pnt->get_X( &prx );
									pnt->get_Y( &pry );
									pnt->Release();
									pnt = NULL;

									PROJECTION_TO_PIXEL(prx,pry,pixX,pixY);
									//Rob Cairns 10 Jan 06
									int pxX = Utility::Rint(pixX);
									int pxY = Utility::Rint(pixY);
									pnts[cnt++]=CPoint(pxX,pxY);
								}
								dc->Polyline(pnts,lastPolygonPoint-pointIndex);
							}
						}

						delete [] pnts;
						pnts=NULL;

						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
					}

					if( si->flags & slfDrawPoint )
					{
						IShape * shp2 = NULL;
						ishp->get_Shape( i, &shp2 );
						long numberPoints = 0;
						shp2->get_NumPoints( &numberPoints );

						double hsize=si->pointSize*.5;
						if( si->pointClr != sli->pointClr )
						{	newBrush=new CBrush();
							newBrush->CreateSolidBrush(si->pointClr);
							oldBrush=dc->SelectObject(newBrush);
							newPen=new CPen();
							newPen->CreatePen(PS_SOLID,1,si->pointClr);
							oldPen=dc->SelectObject(newPen);
						}
						else
						{	
							//if( sliBrushInDC == false || sliPenInDC == false )	// lsu 07/03/09
							//{
								dc->SelectObject(slPointBrush);
								dc->SelectObject(slPointPen);
							//	sliBrushInDC = true;
							//	sliPenInDC = true;
							//}
						}

						double px, py;
						register int j;
						for( j=0;j<numberPoints;j++)
						{	IPoint * pnt=NULL;
							shp2->get_Point(j,&pnt);
							pnt->get_X(&px);
							pnt->get_Y(&py);
							pnt->Release();
							pnt=NULL;
							PROJECTION_TO_PIXEL(px,py,pixX,pixY);
							//Rob Cairns 10 Jan 06
							int pxX = Utility::Rint(pixX);
							int pxY = Utility::Rint(pixY);
							//DRAW_POINT(dc,sli,si,pxX,pxY,hsize);			// lsu: 29-jan-2011
							DrawShapePoint(dc,sli,si,pxX,pxY, (int)hsize);
						}

						if( newBrush )
						{
							dc->SelectObject(oldBrush);
							newBrush->DeleteObject();
							delete newBrush;
							newBrush = NULL;
						}
						if( newPen )
						{
							dc->SelectObject(oldPen);
							newPen->DeleteObject();
							delete newPen;
							newPen = NULL;
						}
						//Rob Cairns 21 December 2005
						shp2->Release(); //Also renamed shp to shp2 to avoid confusion with CComPtr
					}
				}
			}
		}

		//Neio 2009/07/21 clearing
		if(_useQTree == VARIANT_TRUE)
		{
			delete[] qtreeResult;
		}

	}
	dc->SelectObject(slOldBrush);
	dc->SelectObject(slOldPen);
	slPointBrush->DeleteObject();
	delete slPointBrush;
	slPointPen->DeleteObject();
	delete slPointPen;
	slLineBrush->DeleteObject();
	delete slLineBrush;
	slLinePen->DeleteObject();
	delete slLinePen;
	slFillBrush->DeleteObject();
	delete slFillBrush;
	slFillPen->DeleteObject();
	delete slFillPen;

	//m_labelsToDraw.push(layer);
	//DrawLabels( dc, layer );
}
catch(...)
{
	if (shpIndex != NULL) fclose(shpIndex);		// lsu 07/03/09 - if an exception took place file must be released
	// AfxMessageBox("Draw Shapefile ERROR!!!!!");
}
}

/***********************************************************************/
/*							CreateCustomBrush()				           */
/***********************************************************************/

/*  Makes brush for drawing polygon fill, stipples and bit patterns
 */
void CMapView::CreateCustomBrush(CDC* dc, ShapeInfo* si, CBrush* newBrush)
{
	// creation of pattern brushes in separate function
	if (si->fillStipple == fsRaster || si->fillStipple == fsNone)
	{
		newBrush->CreateSolidBrush(si->fillClr);
	}
	else if( si->fillStipple != fsNone)
	{
		int hatchStyle;
		DECODE_FILL_STIPPLE(si->fillStipple, hatchStyle);
		
		/* lsu: we use fill color for stipple when there is no background		 */
		/* fill color supports color schemes and it's more interesting option   */
		if (si->transparentStipple)
			newBrush->CreateHatchBrush(hatchStyle, si->fillClr);
		else
			newBrush->CreateHatchBrush(hatchStyle, si->stippleLineClr);
	}
}

/***********************************************************************/
/*							CreateCustomPen()				           */
/***********************************************************************/

/*  Makes pen, defined by line drawing settings of the layer
 */
inline  void CMapView::CreateCustomPen(ShapeInfo* si,long udLineStipple, CPen* newPen)
{
	if (si->lineWidth == 0 || !(si->flags & slfDrawLine))
	{
		newPen->CreatePen(PS_NULL, (int)si->lineWidth,si->lineClr);
	}
	else
	{
		if( si->lineStipple == lsNone )
		{
			newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
		}
		else if (si->lineStipple == lsCustom)
		{
			int num;
			LOGBRUSH logBrush;
			logBrush.lbColor = si->lineClr;
			logBrush.lbStyle = BS_SOLID;
			DWORD * dashp = Utility::cvtUCharToDword(udLineStipple, num);
			newPen->CreatePen(PS_GEOMETRIC|PS_USERSTYLE, (int)si->lineWidth, &logBrush, num, dashp);
		}
		else
		{
			int penStyle;
			DECODE_LINE_STIPPLE(si->lineStipple, penStyle);
			if (si->lineWidth >1)
			{
				LOGBRUSH logBrush;
				logBrush.lbColor = si->lineClr;
				logBrush.lbStyle = BS_SOLID;
				newPen->CreatePen(PS_GEOMETRIC|penStyle, (int)si->lineWidth, &logBrush, 0, 0);
			}
			else
			{
				newPen->CreatePen(penStyle, (int)si->lineWidth,si->lineClr);
			}
		}
	}
}

/***********************************************************************/
/*						Image2PatternBrush()				           */
/***********************************************************************/
/*  Sets an pattern fill to a previously declared brush. Returns true on 
 *  success, and false otherwise.
 */
bool CMapView::Image2PatternBrush(CDC* dc, IImage* rasterFill, CBrush* brushRaster)
{
	long width, height;	
	rasterFill->get_Height(&height);
	rasterFill->get_Width(&width);
	if (width == 0 || height == 0) return false;
	CBitmap bmp;
	bmp.CreateCompatibleBitmap(dc, width, height);
	
	BITMAPINFO bif;
	BITMAPINFOHEADER bih;
	bih.biBitCount=24;
	bih.biWidth= width;
	bih.biHeight= height;
	bih.biPlanes=1;
	bih.biSize=sizeof(BITMAPINFOHEADER);
	bih.biCompression=0;
	bih.biXPelsPerMeter=0;
	bih.biYPelsPerMeter=0;
	bih.biClrUsed=0;
	bih.biClrImportant=0;
	bih.biSizeImage=width * height * 3;
	bif.bmiHeader = bih;

	unsigned char* bits = new unsigned char[width * height * 3];
	long value;
	for(int j = 0; j < height; j++ )
	{	
		for(int i = 0; i < width; i++ )
		{	
			rasterFill->get_Value(height - j - 1, i, &value); 
			bits[j*width*3+i*3] = GetBValue(value);
			bits[j*width*3+i*3+1] = GetGValue(value);
			bits[j*width*3+i*3+2] = GetRValue(value);
		}
	}
	int res;
	res = SetDIBits(dc->GetSafeHdc(), (HBITMAP)bmp.GetSafeHandle(), 0, height, bits, &bif, DIB_RGB_COLORS);
	
	brushRaster->CreatePatternBrush(&bmp);
	delete [] bits;
	//12-Oct-2009 Rob Cairns
	bmp.DeleteObject();

	return true;

	/* -----------------------------------------------------------------*/
	/* An effort to use Image.GetImageBitsDC method; it must work		*/
	/* faster for in memory bitmaps. But I was unable to make it work	*/
	/* -----------------------------------------------------------------*/
	//CDC dcTemp;
	//dcTemp.CreateCompatibleDC(dc);
	//CBitmap* bmp;
	//bmp->CreateCompatibleBitmap(&dcTemp, width, height);
	//dcTemp.SelectObject(bmp);
	//rasterFill->GetImageBitsDC((long)dcTemp.GetSafeHdc(), &vbretval);
	//bmp = dcTemp.GetCurrentBitmap();
	//bool b;
	//CBrush brush;
	//b = brush.CreatePatternBrush(bmp);
	//oldBrush = dc->SelectObject(&brush);
	/* -----------------------------------------------------------------*/
}

// **************************************************************
//		get_LineStipplePen()
// **************************************************************
void CMapView::get_LineStipplePen(ShapeLayerInfo* sli, ShapeInfo* si, CPen* newPen)
{
	if( si->lineStipple != lsNone )
	{	
		switch( si->lineStipple )
		{
			case( lsDotted ):
			{
				newPen->CreatePen(PS_DOT, (int)si->lineWidth,si->lineClr);
				break;
			}
			case( lsDashed ):
			{
				newPen->CreatePen(PS_DASH,1,si->lineClr);
				break;
			}
			case( lsDashDotDash ):
			{
				newPen->CreatePen(PS_DASHDOT,1,si->lineClr);
				break;
			}
			case ( lsCustom ):
			{
				/* Create a custom geometric pen.*/
				createCustomPen(sli, si, newPen);
				break;
			}
			case ( lsDoubleSolid ):
			{
				newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->fillClr);
				break;
			}
			case ( lsDashDotDot ):
			{
				newPen->CreatePen(PS_DASHDOTDOT, (int)si->lineWidth,si->fillClr);
				break;
			}
			default:
			{
				newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->fillClr);
				break;
			}
		}
	}
	else
	{ 
		newPen->CreatePen(PS_SOLID, (int)si->lineWidth,si->lineClr);
	}
}