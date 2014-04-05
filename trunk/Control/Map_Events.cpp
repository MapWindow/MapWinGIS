
#pragma region Include
#include "stdafx.h"
#include "MapWinGis.h"
#include "Map.h"
#include "Shapefile.h"
#include "Measuring.h"
#pragma endregion

// ***************************************************************
//		UpdateCursor
// ***************************************************************
void CMapView::UpdateCursor(tkCursorMode cursor)
{
	m_cursorMode = cursor;
	OnSetCursor(this,HTCLIENT,0);
	OnCursorModeChanged();
}

// ***************************************************************
//		OnKeyUp
// ***************************************************************
void CMapView::OnKeyUp(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	switch(nChar)
	{
		case VK_SPACE:
			if (m_cursorMode == cmPan)
			{
				// releasing capture for panning operation
				ReleaseCapture();
				//this is the only mode we care about for this event
				m_bitbltClickDown = CPoint(0,0);
				m_bitbltClickMove = CPoint(0,0);
				
				this->SetExtentsCore(this->extents, false);
				LockWindow(lmUnlock);
				
				UpdateCursor(_lastCursorMode);
				_lastCursorMode = cmNone;
				if (m_cursorMode == cmMeasure)
				{
					m_measuring->put_Persistent(_measuringPersistent ? VARIANT_TRUE: VARIANT_FALSE);
				}
			}
			break;
	}
}

// ***************************************************************
//		OnKeyDown
// ***************************************************************
void CMapView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{ 
	double dx = (this->extents.right - this->extents.left)/4.0;
	double dy = (this->extents.top - this->extents.bottom)/4.0;
	
	IExtents* box = NULL;
	bool arrows = nChar == VK_LEFT || nChar == VK_RIGHT || nChar == VK_UP || nChar == VK_DOWN;
	if (arrows)
		CoCreateInstance(CLSID_Extents,NULL,CLSCTX_INPROC_SERVER,IID_IExtents,(void**)&box);
	
	bool ctrl = GetKeyState(VK_CONTROL) & 0x8000 ? true: false;

	switch(nChar)
	{
		case VK_SPACE:
			if (m_cursorMode != cmPan)
			{
				if (m_cursorMode == cmMeasure)
				{
					VARIANT_BOOL vb;
					m_measuring->get_Persistent(&vb);
					_measuringPersistent = vb ? true: false;
					m_measuring->put_Persistent(VARIANT_TRUE);
				}
				_lastCursorMode = (tkCursorMode)m_cursorMode;
				UpdateCursor(cmPan);
			}
			break;
		case 'Z':
			UpdateCursor(cmZoomIn);
			break;
		case 'M':
			UpdateCursor(cmMeasure);
			break;
		case VK_BACK:
			ZoomToPrev();
			break;
		case VK_ADD:
			ZoomIn(0.3);
			break;
		case VK_SUBTRACT:
			ZoomOut(0.3);
			break;
		case VK_MULTIPLY:
			int zoom;
			m_tiles->get_CurrentZoom(&zoom);
			ZoomToTileLevel(zoom);
			break;
		case VK_HOME:
			ZoomToMaxExtents();
			break;
		case VK_LEFT:
			if (ctrl) {
				// moving to previous layer
				_activeLayerPosition--;
				if (m_activeLayers.size() > 0)
				{
					if (_activeLayerPosition < 0) {
						_activeLayerPosition = m_activeLayers.size() - 1;
					}
					int handle = GetLayerHandle(_activeLayerPosition);
					ZoomToLayer(handle);
				}
			}
			else
			{
				box->SetBounds(extents.left - dx, extents.bottom, 0.0, extents.right - dx, extents.top, 0.0);		
				this->SetExtents(box);
			}
			break;
		case VK_RIGHT:
			if (ctrl) {
				// moving to the next layer
				_activeLayerPosition++;
				if (m_activeLayers.size() > 0)
				{
					if (_activeLayerPosition >= (int)m_activeLayers.size()) {
						_activeLayerPosition = 0;
					}
					int handle = GetLayerHandle(_activeLayerPosition);
					ZoomToLayer(handle);
				}
			}
			else
			{
				box->SetBounds(extents.left + dx, extents.bottom, 0.0, extents.right + dx, extents.top, 0.0);		
				this->SetExtents(box);
			}
			break;
		case VK_UP:
			box->SetBounds(extents.left, extents.bottom + dy, 0.0, extents.right, extents.top + dy, 0.0);		
			this->SetExtents(box);
			break;
		case VK_DOWN:
			box->SetBounds(extents.left, extents.bottom - dy, 0.0, extents.right, extents.top - dy, 0.0);		
			this->SetExtents(box);
			break;
		default:
			break;
	}
	if (box)
		box->Release();
} 

// ***************************************************************
// 		OnMouseWheel()					           
// ***************************************************************
//  Processing mouse wheel event. Amount of zoom is determined by MouseWheelsSpeed parameter
BOOL CMapView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	if (m_MouseWheelSpeed < 0.1 || m_MouseWheelSpeed > 10) m_MouseWheelSpeed = 1;
	if (m_MouseWheelSpeed == 1) return FALSE;
	
	RECT rect;
	double width, height;
	double xCent, yCent;
	double dx, dy;

	// absolute cursor position
	this->GetWindowRect(&rect);
	if (pt.x < rect.left || pt.x > rect.right || pt.y < rect.top || pt.y > rect.bottom)
		return false;
	if ((rect.right - rect.left == 0) && (rect.bottom - rect.top == 0))
		return false;

	if (m_Rotate != NULL && m_RotateAngle != 0)
	{
		CPoint curMousePt, origMousePt, rotCentre;
	    
		curMousePt.x = pt.x - rect.left;
		curMousePt.y = pt.y - rect.top;
		rotCentre.x = (rect.right - rect.left) / 2;
		rotCentre.y = (rect.bottom - rect.top) / 2;

		m_Rotate->getOriginalPixelPoint(curMousePt.x, curMousePt.y, &(origMousePt.x), &(origMousePt.y));
		PixelToProj((double)(origMousePt.x), (double)(origMousePt.y), &xCent, &yCent);

		dx = (double)(origMousePt.x) / (double)(rect.right - rect.left);
		dy = (double)(origMousePt.y) / (double)(rect.bottom - rect.top);
	}
	else
	{
		PixelToProj((double)(pt.x - rect.left), (double)(pt.y - rect.top), &xCent, &yCent);
		dx = (double)(pt.x - rect.left) / (rect.right - rect.left);
		dy = (double)(pt.y - rect.top) / (rect.bottom - rect.top);
	}

    // new extents
	double ratio = zDelta > 0?m_MouseWheelSpeed:(1/m_MouseWheelSpeed);
	height = (extents.top - extents.bottom) * ratio;
	width = (extents.right - extents.left) * ratio;
	
	Extent ext;
	ext.left = xCent - width * dx;
	ext.right = xCent + width * (1 - dx);
	ext.bottom = yCent - height * (1 - dy);
	ext.top = yCent + height * dy;

	this->SetExtentsCore(ext);
	return true;
}

// ************************************************************
//		OnLButtonDown
// ************************************************************
void CMapView::OnLButtonDown(UINT nFlags, CPoint point)
{
   m_leftButtonDown = TRUE;

	CPoint rotPoint = point;
	if (m_Rotate != NULL && m_RotateAngle != 0)
	{
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));
		m_clickDown = rotPoint;
    }
	else
		m_clickDown = point;

   m_clickDownExtents = extents;

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	if( m_cursorMode == cmZoomIn )
	{
		if (m_sendMouseDown)
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				this->FireMouseDown(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
			else
				this->FireMouseDown(MK_LBUTTON, (short)vbflags,point.x,point.y -1);
		}

		m_ttip.Activate(FALSE);
		
		//Selection Box
		CMapTracker selectBox = CMapTracker( this,
			CRect(0,0,0,0),
			CRectTracker::solidLine +
			CRectTracker::resizeOutside );
		selectBox.m_sizeMin = 0;

		bool selected = selectBox.TrackRubberBand( this, point, TRUE ) ? true : false;
		m_ttip.Activate(TRUE);

		CRect rect = selectBox.m_rect;
		rect.NormalizeRect();

		if( ( rect.BottomRight().x - rect.TopLeft().x ) < 10 &&
			( rect.BottomRight().y - rect.TopLeft().y ) < 10 )
			selected = false;

		if( selected == true )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
			{
				CRect rectTmp = rect;
				long tmpX = 0, tmpY = 0;
				// adjust rectangle to unrotated coordinates
				m_Rotate->getOriginalPixelPoint(rect.left, rect.top, &tmpX, &tmpY);
				rectTmp.TopLeft().x = tmpX;
				rectTmp.TopLeft().y = tmpY;
				m_Rotate->getOriginalPixelPoint(rect.right, rect.bottom, &tmpX, &tmpY);
				rectTmp.BottomRight().x = tmpX;
				rectTmp.BottomRight().y = tmpY;
				rect = rectTmp;
			}
			
			double zrx = extents.right, zby = extents.bottom;
			double zlx = extents.left, zty = extents.top;
			PixelToProjection( rect.TopLeft().x, rect.TopLeft().y, zrx, zby );
			PixelToProjection( rect.BottomRight().x, rect.BottomRight().y, zlx, zty );

			double cLeft = MINIMUM( zrx, zlx );
			double cRight = MAXIMUM( zrx, zlx );
			double cBottom = MINIMUM( zty, zby );
			double cTop = MAXIMUM( zty, zby );

			this->SetExtentsCore(Extent(cLeft, cRight, cBottom, cTop));

			if( m_sendSelectBoxFinal == TRUE )
			{
				long iby = rect.BottomRight().y;
				long ity = rect.TopLeft().y;
				this->FireSelectBoxFinal( rect.TopLeft().x, rect.BottomRight().x, iby, ity );
			}
		}
		else
		{
			ZoomIn( m_zoomPercent );

			if( m_sendMouseUp == TRUE )
			{
				if (m_Rotate != NULL && m_RotateAngle != 0)
					this->FireMouseUp(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
				else
					this->FireMouseUp( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
			}
		}
	}
	else if( m_cursorMode == cmZoomOut )
	{
		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				this->FireMouseDown(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
			else
				this->FireMouseDown( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
		}
		
        ZoomOut( m_zoomPercent );
	}
	else if( m_cursorMode == cmPan )
	{
		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				FireMouseDown(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
			else
				FireMouseDown(MK_LBUTTON, (short)vbflags, point.x, point.y - 1);
		}

		CRect rcBounds(0,0,m_viewWidth,m_viewHeight);
		m_bitbltClickDown = point;
		m_bitbltClickMove = point;
		this->LogPrevExtent();
		this->SetCapture();
		this->LockWindow(lmLock);
	}
	else if( m_cursorMode == cmSelection )
	{
		m_ttip.Activate(FALSE);
		//Selection Box

		CMapTracker selectBox = CMapTracker( this,
			CRect(0,0,0,0),
			CRectTracker::solidLine +
			CRectTracker::resizeOutside );

		selectBox.m_sizeMin = 0;

		bool selected = selectBox.TrackRubberBand( this, point, TRUE ) ? true : false;
		m_ttip.Activate(TRUE);

		CRect rect = selectBox.m_rect;
		rect.NormalizeRect();

		if( ( rect.BottomRight().x - rect.TopLeft().x ) < 10 &&
			( rect.BottomRight().y - rect.TopLeft().y ) < 10 )
			selected = false;
		
		if (!selected || !m_sendSelectBoxFinal)
		{
			if( m_sendMouseDown == TRUE )
			{
				if (m_Rotate != NULL && m_RotateAngle != 0)
					this->FireMouseDown(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
				else
					this->FireMouseDown(MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
			}
		}

		if( selected )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
			{
				CRect rectTmp = rect;
				long tmpX = 0, tmpY = 0;
				// adjust rectangle to unrotated coordinates
				m_Rotate->getOriginalPixelPoint(rect.left, rect.top, &tmpX, &tmpY);
				rectTmp.TopLeft().x = tmpX;
				rectTmp.TopLeft().y = tmpY;
				m_Rotate->getOriginalPixelPoint(rect.right, rect.bottom, &tmpX, &tmpY);
				rectTmp.BottomRight().x = tmpX;
				rectTmp.BottomRight().y = tmpY;
				rect = rectTmp;
			}

			if( m_sendSelectBoxFinal == TRUE )
			{	
				long iby = rect.BottomRight().y;
				long ity = rect.TopLeft().y;
				this->FireSelectBoxFinal( rect.TopLeft().x, rect.BottomRight().x,	iby, ity );
				return; // exit out so that the FireMouseUp does not get called! DB 12/10/2002
			}
		}
		
		//the MapTracker interferes with the OnMouseUp event so we will call it manually
		if( m_sendMouseUp == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				this->FireMouseUp(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
			else
				this->FireMouseUp( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
		}
	}
	else if( m_cursorMode == cmMeasure )
	{
		bool found = false;
		double x, y;
		int closePointIndex= -1;
		
		tkMeasuringType measureType;
		m_measuring->get_MeasuringType(&measureType);

		if ( nFlags & MK_CONTROL && measureType == tkMeasuringType::MeasureDistance)
		{
			// closing to show area
			double minDist = DBL_MAX;
			double xTemp, yTemp;
			
			CMeasuring* m = (CMeasuring*)m_measuring;
			int size = m->points.size() - 2;
			for(int i = 0; i < size; i++)
			{
				ProjToPixel(m->points[i]->Proj.x, m->points[i]->Proj.y, &xTemp, &yTemp);
				double dist = sqrt( pow(point.x - xTemp, 2.0) + pow(point.y - yTemp, 2.0));
				if (dist < minDist)
				{
					minDist = dist;
					closePointIndex = i;
					found = true;
				}
			}

			if (minDist != DBL_MAX)
			{
				x = m->points[closePointIndex]->Proj.x;
				y = m->points[closePointIndex]->Proj.y;
			}
			else
			{
				// no points to close near by
			}
		}
		else if ( nFlags & MK_SHIFT )	// when shift is pressed we seek for nearby point across all layers
		{
			double tolerance = 20;   // pixels;   TODO: make a parameter
			
			if (FindSnapPoint(tolerance, point.x, point.y, &x, &y))
				found = true;
			else
			{	// simply don't add a point, there is nothing to snap near by
			}
		}
		else
		{
			this->PixelToProjection( point.x, point.y, x, y );
			found = true;
		}
		
		if (found)
		{
			double pixelX, pixelY;
			ProjToPixel(x, y, &pixelX, &pixelY);

			((CMeasuring*)m_measuring)->AddPoint(x, y, pixelX, pixelY);
			
			if (closePointIndex != -1)
				((CMeasuring*)m_measuring)->ClosePoly(closePointIndex);

			FireMeasuringChanged(m_measuring, tkMeasuringAction::PointAdded);
			_canUseMainBuffer = false;

			// better to call it before the redraw
			if( m_sendMouseDown ) {
				this->FireMouseDown( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
			}
			this->Refresh();		// update the map without redrawing the layers
		}
	}
	else //if( m_cursorMode == cmNone )
	{
		SetCapture();
		if( m_sendMouseDown == TRUE )
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
				this->FireMouseDown(MK_LBUTTON, (short)vbflags,rotPoint.x,rotPoint.y -1);
			else
				this->FireMouseDown( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );
		}
	}
}

// ************************************************************
//		FindSnapPoint
// ************************************************************
VARIANT_BOOL CMapView::FindSnapPoint(double tolerance, double xScreen, double yScreen, double* xFound, double* yFound)
{
	double x, y, x2, y2;
	this->PixelToProjection( xScreen, yScreen, x, y );
	this->PixelToProjection( xScreen + tolerance, yScreen + tolerance, x2, y2 );
	double maxDist = sqrt(pow(x - x2, 2.0) + pow(y - y2, 2.0));

	long shapeIndex;
	long pointIndex;
	VARIANT_BOOL vb;
	double distance;
	
	double minDist = DBL_MAX;
	IShapefile* foundShapefile = NULL;
	long foundShapeIndex;
	long foundPointIndex;

	for(long i = 0; i < this->GetNumLayers(); i++)
	{
		IShapefile* sf = this->GetShapefile(this->GetLayerHandle(i));
		if (sf != NULL)
		{
			sf->GetClosestVertex(x, y, maxDist, &shapeIndex, &pointIndex, &distance, &vb);
			if (vb)
			{
				if (distance < minDist)
				{
					minDist = distance;
					foundShapefile = sf;
					foundPointIndex = pointIndex;
					foundShapeIndex = shapeIndex;
				}
			}
			sf->Release();
		}
	}

	bool result = false;
	if (minDist != DBL_MAX && foundShapefile)
	{
		IShape* shape = NULL;
		foundShapefile->get_Shape(foundShapeIndex, &shape);
		if (shape)
		{
			shape->get_XY(foundPointIndex, xFound, yFound, &vb);
			shape->Release();
			result = true;
		}
	}
	return result;
}

// ************************************************************
//		OnLButtonDblClick
// ************************************************************
void CMapView::OnLButtonDblClk(UINT nFlags, CPoint point)
{
   OnLButtonDown(nFlags, point);
   if (m_cursorMode == cmMeasure)
   {
	   m_measuring->FinishMeasuring();
	   FireMeasuringChanged(m_measuring, tkMeasuringAction::MesuringStopped);	
   }
}

// ************************************************************
//		OnLButtonUp
// ************************************************************
void CMapView::OnLButtonUp(UINT nFlags, CPoint point)
{
	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;
  
	if (m_Rotate != NULL && m_RotateAngle != 0)
	{
		CPoint rotPoint = point;
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));
		point = rotPoint;
	}

	if( m_sendMouseUp == TRUE && m_leftButtonDown )
		FireMouseUp( MK_LBUTTON, (short)vbflags, point.x, point.y - 1 );

    m_leftButtonDown = FALSE;

	ReleaseCapture();

	if( m_cursorMode == cmPan )
	{
		//this is the only mode we care about for this event
		m_bitbltClickDown = CPoint(0,0);
		m_bitbltClickMove = CPoint(0,0);
		
		this->SetExtentsCore(this->extents, false);
		LockWindow(lmUnlock);
	}
}

// ************************************************************
//		OnMouseMove
// ************************************************************
void CMapView::OnMouseMove(UINT nFlags, CPoint point)
{
	VARIANT_BOOL vbretval;
	CPoint rotPoint = point;
	if (m_Rotate != NULL && m_RotateAngle != 0)
		m_Rotate->getOriginalPixelPoint(point.x, point.y, &(rotPoint.x), &(rotPoint.y));

	if( m_showingToolTip )
	{
		CToolInfo cti;
		m_ttip.GetToolInfo(cti,this,IDC_TTBTN);
		cti.rect.left = point.x - 2;
		cti.rect.right = point.x + 2;
		cti.rect.top = point.y - 2;
		cti.rect.bottom = point.y + 2;
		m_ttip.SetToolInfo(&cti);
		m_ttip.SetWindowPos(&wndTop,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

		MSG pMsg;
		pMsg.hwnd = this->m_hWnd;
		pMsg.message = WM_MOUSEMOVE;
		pMsg.wParam = nFlags;
		pMsg.lParam = MAKELPARAM(point.x, point.y);
		pMsg.pt = point;
		m_ttip.RelayEvent(&pMsg);
	}

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

   CPoint movePnt = point;

	if (m_Rotate != NULL && m_RotateAngle != 0)
		movePnt = rotPoint;

	if( m_sendMouseMove == TRUE )
	{	
		long mbutton = 0;
		if( nFlags & MK_LBUTTON )
			mbutton = 1;
		else if( nFlags & MK_RBUTTON )
			mbutton = 2;
		else if( nFlags & MK_MBUTTON )
			mbutton = 3;

		if( (m_cursorMode == cmPan) && (nFlags & MK_LBUTTON ))
		{
			//Do Not Send the Event
		}
		else
		{
			if (m_Rotate != NULL && m_RotateAngle != 0)
  				this->FireMouseMove( (short)mbutton, (short)vbflags, rotPoint.x, rotPoint.y );  // rotPoint.y - 1
			else
 				this->FireMouseMove( (short)mbutton, (short)vbflags, point.x, point.y ); // point.y - 1
		}
	}

	if( m_cursorMode == cmPan )
	{
		if( (nFlags & MK_LBUTTON) && m_leftButtonDown )
		{
			m_bitbltClickMove = point;

         double xAmount = (m_clickDown.x - movePnt.x)*m_inversePixelPerProjectionX;
			double yAmount = (movePnt.y - m_clickDown.y)*m_inversePixelPerProjectionY;

			extents.left = m_clickDownExtents.left + xAmount;
			extents.right = m_clickDownExtents.right + xAmount;
			extents.bottom = m_clickDownExtents.bottom + yAmount;
			extents.top = m_clickDownExtents.top + yAmount;

			if (m_UseSeamlessPan)
			{
				_canUseLayerBuffer = FALSE;	// lsu (07/03/2009) added for seamless panning; suggested by Bobby at http://www.mapwindow.org/phorum/read.php?3,13099
				LockWindow(lmUnlock);	

				FireExtentsChanged(); 
				ReloadImageBuffers(); 
			}	
			else
				InvalidateControl();

			return;
		}
	}

	bool refreshNeeded = false;

	if( m_cursorMode == cmMeasure ) 
	{
		CMeasuring* m =((CMeasuring*)m_measuring);
		if (!m->IsStopped() && m->points.size() > 0)
		{
			double x, y;
			this->PixelToProjection( point.x, point.y, x, y );
			m->mousePoint.x = point.x;	// save the current position; it will be drawn during invalidation
			m->mousePoint.y = point.y;
			_canUseMainBuffer = false;
			refreshNeeded = true;
		}
	}

	// Hot tracking
	if (_hasHotTracking)
	{
		double pixX = point.x;
		double pixY = point.y;
		double prjX, prjY;
		this->PixelToProj(pixX, pixY, &prjX, &prjY);
		
		bool found = false;
		IShapefile * sf = NULL;

		for(int i = 0; i < (int)m_activeLayers.size(); i++ )
		{
			Layer* layer = m_allLayers[m_activeLayers[i]];
			if (layer->type == ShapefileLayer)
			{
				if (layer->QueryShapefile(&sf))
				{
					VARIANT_BOOL hotTracking = VARIANT_FALSE;
					sf->get_HotTracking(&hotTracking);
					if (hotTracking)
					{
						std::vector<long> shapes;

						double tol = 0.0;
						ShpfileType type;
						sf->get_ShapefileType(&type);
						type = Utility::ShapeTypeConvert2D(type);
						if (type == SHP_MULTIPOINT || type == SHP_POINT)
							tol = 5.0/this->PixelsPerMapUnit();

						((CShapefile*)sf)->SelectShapesCore(Extent(prjX, prjX, prjY, prjY), tol, SelectMode::INCLUSION, shapes);
						
						if (shapes.size() > 0)
						{
							if (m_activeLayers[i] != m_hotTracking.LayerHandle || shapes[0] != m_hotTracking.ShapeId)
							{
								IShape* shape = NULL;
								sf->get_Shape(shapes[0], &shape);
								if (shape)
								{
									IShape* shpClone = NULL;
									shape->Clone(&shpClone);	// TODO: why are we crashing without it for in-memory shapefiles?
																// on the first glance shape shouldn't be released on closing the shapefile
									ULONG cnt = shape->Release();

									if (!m_hotTracking.Shapefile)
										CoCreateInstance(CLSID_Shapefile,NULL,CLSCTX_INPROC_SERVER,IID_IShapefile,(void**)&(m_hotTracking.Shapefile));
									else
										m_hotTracking.Shapefile->Close(&vbretval);

									if (m_hotTracking.Shapefile)
									{
										ShpfileType type;
										sf->get_ShapefileType(&type);
										
										((CShapefile*)m_hotTracking.Shapefile)->CreateNewCore(A2BSTR(""), type, false, &vbretval);
										long index = 0;
										m_hotTracking.Shapefile->EditInsertShape(shpClone, &index, &vbretval);
										m_hotTracking.Shapefile->RefreshExtents(&vbretval);
										m_hotTracking.LayerHandle = m_activeLayers[i];
										m_hotTracking.ShapeId = shapes[0];

										IShapeDrawingOptions* options = NULL;
										sf->get_SelectionDrawingOptions(&options);
										if (options)
										{
											m_hotTracking.Shapefile->put_DefaultDrawingOptions(options);
											options->Release();
										}
										
									}
									shpClone->Release();   // there is one reference in new shapefile
									found = true;
								}
							}
						}
					}
					
					sf->Release();
					sf = NULL;
				}
			}

			if (found)
			{
				// pasing event to the caller
				this->FireShapeHighlighted(m_hotTracking.LayerHandle, m_hotTracking.ShapeId);
				_canUseMainBuffer = false;
				refreshNeeded = true;
				break;
			}
		}

		if (!found && m_hotTracking.ShapeId != -1)
		{
			m_hotTracking.ShapeId = -1;
			m_hotTracking.LayerHandle = -1;
			if (m_hotTracking.Shapefile)
				m_hotTracking.Shapefile->Close(&vbretval);
			
			// pasing event to the caller
			this->FireShapeHighlighted(-1, -1);
		}
	}

	if (_showCoordinates != cdmNone) {
		refreshNeeded = true;
	}

	if (refreshNeeded) {
		this->Refresh();
	}
}

// ************************************************************
//		OnRButtonDblClick
// ************************************************************
void CMapView::OnRButtonDblClk(UINT nFlags, CPoint point)
{
   OnRButtonDown(nFlags, point);
}

// ************************************************************
//		OnRButtonDown
// ************************************************************
void CMapView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// Added QUAY 3.16.09
	if (DoTrapRMouseDown == TRUE )
	{
		long vbflags = 0;
		if( nFlags & MK_SHIFT )
			vbflags |= 1;
		if( nFlags & MK_CONTROL )
			vbflags |= 2;

		m_clickDown = point;
		m_clickDownExtents = extents;

		VARIANT_BOOL redraw;
		if( m_cursorMode == cmMeasure )
		{
			((CMeasuring*)m_measuring)->UndoPoint(&redraw);
			FireMeasuringChanged(m_measuring, tkMeasuringAction::PointRemoved);
			_canUseMainBuffer = false;
		}

		if( m_sendMouseDown == TRUE )
			this->FireMouseDown( MK_RBUTTON, (short)vbflags, point.x, point.y - 1 );

		_reverseZooming = true;

		if( m_cursorMode == cmZoomOut )
		{
			double zx = extents.left, zy = extents.bottom;
			PixelToProjection( point.x, point.y, zx, zy );

			double halfxRange = (extents.right - extents.left)*.5;
			double halfyRange = (extents.top - extents.bottom)*.5;

			extents.left = zx - halfxRange;
			extents.right = zx + halfxRange;
			extents.bottom = zy - halfyRange;
			extents.top = zy + halfyRange;

			ZoomIn( m_zoomPercent );
			::SetCursor( m_cursorZoomin );

			FireExtentsChanged();
			ReloadImageBuffers();
		}
		else if( m_cursorMode == cmZoomIn )
		{
			double zx = extents.left, zy = extents.bottom;
			PixelToProjection( point.x, point.y, zx, zy );

			double halfxRange = (extents.right - extents.left)*.5;
			double halfyRange = (extents.top - extents.bottom)*.5;

			extents.left = zx - halfxRange;
			extents.right = zx + halfxRange;
			extents.bottom = zy - halfyRange;
			extents.top = zy + halfyRange;

			ZoomOut( m_zoomPercent );
			::SetCursor( m_cursorZoomout );

			FireExtentsChanged();
			ReloadImageBuffers();
		}
	
		if( redraw ) {
			this->Refresh();
		}
	}
}

// *********************************************************
//		OnRButtonUp()
// *********************************************************
void CMapView::OnRButtonUp(UINT nFlags, CPoint point)
{
	COleControl::OnRButtonUp(nFlags, point);
	ReleaseCapture();//why is this being called, capture isn't set on RButtonDown as far as I can see...

	_reverseZooming = false;
	if (m_cursorMode == cmZoomIn) ::SetCursor( m_cursorZoomin );
	if (m_cursorMode == cmZoomOut) ::SetCursor( m_cursorZoomout );

	long vbflags = 0;
	if( nFlags & MK_SHIFT )
		vbflags |= 1;
	if( nFlags & MK_CONTROL )
		vbflags |= 2;

	if( m_sendMouseUp == TRUE )
		this->FireMouseUp( MK_RBUTTON, (short)vbflags, point.x, point.y - 1 );
}

// *********************************************************
//		OnMButtonUp()
// *********************************************************
void CMapView::OnMButtonUp(UINT nFlags, CPoint point)
{
	double zx = extents.left, zy = extents.bottom;
	PixelToProjection( point.x, point.y, zx, zy );

	double halfxRange = (extents.right - extents.left)*.5;
	double halfyRange = (extents.top - extents.bottom)*.5;

	extents.left = zx - halfxRange;
	extents.right = zx + halfxRange;
	extents.bottom = zy - halfyRange;
	extents.top = zy + halfyRange;

	this->SetExtentsCore(extents);
}

void CMapView::ResizeBuffers(int cx, int cy)
{
	if (m_bufferBitmap)
	{
		delete m_bufferBitmap;
		m_bufferBitmap = NULL;
	}
	m_bufferBitmap = new Gdiplus::Bitmap(cx, cy);

	if (m_tilesBitmap)
	{
		delete m_tilesBitmap;
		m_tilesBitmap = NULL;
	}
	m_tilesBitmap = new Gdiplus::Bitmap(cx, cy);

	if (m_drawingBitmap)
	{
		delete m_drawingBitmap;
		m_drawingBitmap = NULL;
	}
	m_drawingBitmap = new Gdiplus::Bitmap(cx, cy);

	if (m_layerBitmap)
	{
		delete m_layerBitmap;
		m_layerBitmap = NULL;
	}
	m_layerBitmap = new Gdiplus::Bitmap(cx, cy);
}

// *************************************************************
//		CMapView::OnSize()
// *************************************************************
void CMapView::OnSize(UINT nType, int cx, int cy)
{
	// the redraw is prohibited before the job here is done
	m_isSizing = true;

	COleControl::OnSize(nType, cx, cy);

	CDC* pDC = GetDC();
	
	ResizeBuffers(cx, cy);
	
	// we shall fill the new regions with back color
	if (cx > m_viewWidth)
	{
		pDC->FillSolidRect(m_viewWidth, 0, cx - m_viewWidth, cy, m_backColor);
	}
	if (cy > m_viewHeight)
	{
		pDC->FillSolidRect(0, m_viewHeight, cx, cy - m_viewHeight, m_backColor);
	}

	ReleaseDC(pDC);

	if( cx > 0 && cy > 0 )
	{
		m_viewWidth = cx;
		m_viewHeight = cy;
		m_aspectRatio = (double)m_viewWidth/(double)m_viewHeight;
		m_isSizing = false;
		
		this->SetExtentsCore(extents, false, true);
	}
	else
	{
		m_isSizing = false;
	}
}

// *******************************************************
//		OnDropFiles()
// *******************************************************
void CMapView::OnDropFiles(HDROP hDropInfo)
{
	long numFiles = DragQueryFile( hDropInfo, 0xFFFFFFFF, NULL, 0 );

	register int i;
	for( i = 0; i < numFiles; i++ )
	{
		long fsize = DragQueryFile( hDropInfo, i, NULL, 0 );
		if( fsize > 0 )
		{	char * fname = new char[fsize + 2];
			DragQueryFile( hDropInfo, i, fname, fsize + 1 );
			FireFileDropped( fname );
			delete [] fname;
		}
	}
	COleControl::OnDropFiles(hDropInfo);
}

// *******************************************************
//		OnBackColorChanged()
// *******************************************************
void CMapView::OnBackColorChanged()
{
	_canUseLayerBuffer = FALSE;
	if( !m_lockCount )
		InvalidateControl();
}

// *******************************************************
//		OnMapCursorChanged()
// *******************************************************
void CMapView::OnMapCursorChanged()
{	
	OnSetCursor(this,0,0);
}

// *******************************************************
//		OnSetCursor()
// *******************************************************
BOOL CMapView::OnSetCursor(CWnd* pWnd, UINT nHitTest, UINT message)
{
   HCURSOR NewCursor = NULL;

	if( nHitTest != HTCLIENT )
	{
		COleControl::OnSetCursor( pWnd, nHitTest, message );
		return TRUE;
	}

	switch( m_mapCursor )
	{
		case crsrMapDefault:

			switch( m_cursorMode )
			{
				case cmZoomIn:
					NewCursor = _reverseZooming ?  m_cursorZoomout : m_cursorZoomin;
					break;

				case cmZoomOut:
					NewCursor = _reverseZooming ?  m_cursorZoomin : m_cursorZoomout;
					break;

				case cmPan:
					NewCursor = m_cursorPan;
					break;

				case cmSelection:
					NewCursor = m_cursorSelect;
					break;
				
				case cmMeasure:
					NewCursor = m_cursorMeasure;
					break;

				case cmNone:
					NewCursor = (HCURSOR)m_uDCursorHandle;
					break;
			}
			break;
	
		case crsrAppStarting:
			NewCursor = LoadCursor(NULL, IDC_APPSTARTING);
			break;

		case crsrArrow:
			NewCursor = LoadCursor(NULL, IDC_ARROW);
			break;

		case crsrCross:
			NewCursor = LoadCursor(NULL, IDC_CROSS);
			break;

		case crsrHelp:
			NewCursor = LoadCursor(NULL, IDC_HELP);
			break;

		case crsrIBeam:
			NewCursor = LoadCursor(NULL, IDC_IBEAM);
			break;

		case crsrNo:
			NewCursor = LoadCursor(NULL, IDC_NO);
			break;

		case crsrSizeAll:
			NewCursor = LoadCursor(NULL, IDC_SIZEALL);
			break;

		case crsrSizeNESW:
			NewCursor = LoadCursor(NULL, IDC_SIZENESW);
			break;

		case crsrSizeNS:
			NewCursor = LoadCursor(NULL, IDC_SIZENS);
			break;

		case crsrSizeNWSE:
			NewCursor = LoadCursor(NULL, IDC_SIZENWSE);
			break;

		case crsrSizeWE:
			NewCursor = LoadCursor(NULL, IDC_SIZEWE);
			break;

		case crsrUpArrow:
			NewCursor = LoadCursor(NULL, IDC_UPARROW);
			break;
		
		case crsrWait:

			if (!m_DisableWaitCursor)
				NewCursor = LoadCursor(NULL, IDC_WAIT);
			break;

		case crsrUserDefined:
			NewCursor = (HCURSOR)m_uDCursorHandle;
			break;
	}

	if (NewCursor != NULL)
		::SetCursor( NewCursor );
	else
		COleControl::OnSetCursor( pWnd, nHitTest, message );

	return TRUE;
}

// *************************************************************
//		OnTimer()
// *************************************************************
#ifdef WIN64
void CMapView::OnTimer(UINT_PTR nIDEvent)
#else
void CMapView::OnTimer(UINT nIDEvent)
#endif
{
	// TODO: Add your message handler code here and/or call default
	if( nIDEvent == SHOWTEXT )
	{	KillTimer(SHOWTEXT);
		m_showingToolTip = TRUE;
	}
	else if( nIDEvent == HIDETEXT)
	{	KillTimer(HIDETEXT);
		m_showingToolTip = FALSE;

		CToolInfo cti;
		m_ttip.GetToolInfo(cti,this,IDC_TTBTN);
		cti.rect.left = -1;
		cti.rect.top = - 1;
		cti.rect.right = - 1;
		cti.rect.bottom = - 1;
		m_ttip.SetToolInfo(&cti);
		m_ttip.Pop();
	}

	COleControl::OnTimer(nIDEvent);
}

// *********************************************************
//		OnResetState 
// *********************************************************
// Reset control to default state
void CMapView::OnResetState()
{
	COleControl::OnResetState();  // Resets defaults found in DoPropExchange
	//SetDefaults();		// TODO: Reset any other control state here.
}

// *********************************************************
//		OnResetState 
// *********************************************************
void CMapView::OnCursorModeChanged()
{
	if (m_measuring)
	{
		VARIANT_BOOL vb;
		m_measuring->get_Persistent(&vb);
		if (m_cursorMode != cmMeasure && !vb)
			m_measuring->Clear();
	}
}

// *********************************************************
//		Unimplemented events
// *********************************************************
void CMapView::OnExtentPadChanged(){}
void CMapView::OnExtentHistoryChanged(){}
void CMapView::OnKeyChanged(){}
void CMapView::OnDoubleBufferChanged(){}
void CMapView::OnZoomPercentChanged(){}
void CMapView::OnUDCursorHandleChanged(){}
void CMapView::OnSendMouseDownChanged(){}
void CMapView::OnSendOnDrawBackBufferChanged(){}
void CMapView::OnSendMouseUpChanged(){}
void CMapView::OnSendMouseMoveChanged(){}
void CMapView::OnSendSelectBoxDragChanged(){}
void CMapView::OnSendSelectBoxFinalChanged(){}

