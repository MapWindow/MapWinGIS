#include "stdafx.h"
#include "map.h"
#include "Image.h"
#include "ImageDrawing.h"

// *****************************************************************
//		BuildImageGroups
// *****************************************************************
// Here we'll make groups from the images with the same size and positions
// Group number will be written to the each image groupID property
void CMapView::BuildImageGroups(std::vector<ImageGroup*>& imageGroups)
{
	imageGroups.clear();

	for(size_t i = 0; i < m_activeLayers.size(); i++)
	{
		Layer * l = m_allLayers[m_activeLayers[i]];
		if( l != NULL )
		{	
			if(l->type == ImageLayer)
			{
				IImage* iimg = NULL;
				//l->object->QueryInterface(IID_IImage, (void**)&iimg);
				//if ( iimg != NULL )
				if (l->QueryImage(&iimg))
				{
					CImageClass* img = (CImageClass*)iimg;
					img->m_groupID = -1;
					
					if (l->flags & Visible)
					{
						if ( img->_canUseGrouping)
						{
							double dx, dy, xllCenter, yllCenter;
							LONG width, height;

							img->get_OriginalHeight(&height);
							img->get_OriginalWidth(&width);
							
							img->get_OriginalDX(&dx);
							img->get_OriginalDY(&dy);
							img->get_OriginalXllCenter(&xllCenter);
							img->get_OriginalYllCenter(&yllCenter);

							//img->GetOriginal_dX(&dx);
							//img->GetOriginal_dY(&dy);
							//img->GetOriginalXllCenter(&xllCenter);
							//img->GetOriginalYllCenter(&yllCenter);

							bool groupFound = false;
							for(size_t j = 0; j < imageGroups.size(); j++)
							{
								ImageGroup* group = imageGroups[j];
								
								if ((group->dx == dx) && 
									(group->dy == dy) && 
									(group->width == width) && 
									(group->height == height) &&
									(group->xllCenter == xllCenter) && 
									(group->yllCenter == yllCenter))
								{
									groupFound = true;
									group->imageIndices.push_back(i);
									break;
								}
							}
							
							if (! groupFound )
							{
								// adding new group
								ImageGroup* group = new ImageGroup(dx, dy, xllCenter, yllCenter, width, height);
								imageGroups.push_back(group);
								imageGroups[imageGroups.size() - 1]->imageIndices.push_back(i);
							}
						}
					}
				}
			}
		}
	}

	// now we'll check whether the pixels of image are scarce enough for us
	// the group wil work only in case there is more then 1 suitable image
	int groupId = 0;
	IImage* iimg = NULL;
	for (size_t i = 0; i < imageGroups.size(); i++)
	{
		std::vector<int>* indices = &imageGroups[i]->imageIndices;
		int groupSize = indices->size();

		if (groupSize > 1)
		{
			for (size_t j = 0; j < indices->size(); j++ )
			{
				Layer * l = m_allLayers[m_activeLayers[(*indices)[j]]];
				//l->object->QueryInterface(IID_IImage, (void**)&iimg);
				//if (iimg)
				if (l->QueryImage(&iimg))
				{
					CImageClass* img = (CImageClass*)iimg;
					
					if (!img->_pixelsSaved)				// it's the first time we try to draw image or transparency color chnaged
					{
						if (!img->SaveNotNullPixels())	// analysing pixels...
						{
							(*indices)[j] = -1;
							img->put_CanUseGrouping(VARIANT_FALSE);	//  don't try this image any more - there are to many data pixels in it
							groupSize--;
						}
					}
					iimg->Release();
				}
			}
		}
		
		// saving the valid groups
		if (groupSize > 1)
		{
			imageGroups[i]->isValid = true;
			for (size_t i = 0; i< indices->size(); i++)
			{
				int imageIndex = (*indices)[i];
				if (imageIndex != -1)
				{
					Layer * l = m_allLayers[m_activeLayers[imageIndex]];
					//l->object->QueryInterface(IID_IImage, (void**)&iimg);
					//if (iimg)
					if (l->QueryImage(&iimg))
					{
						CImageClass* img = (CImageClass*)iimg;
						img->m_groupID = groupId;
						iimg->Release();
					}
				}
			}
			groupId++;
		}
		else
		{
			imageGroups[i]->isValid = false;
		}
	}
}

// *****************************************************************
//		ChooseInterpolationMode
// *****************************************************************
// Choosing the mode with better quality from the pair
tkInterpolationMode CMapView::ChooseInterpolationMode(tkInterpolationMode mode1, tkInterpolationMode mode2)
{
	if (mode1 == imHighQualityBicubic || mode2 == imHighQualityBicubic )
	{
		return imHighQualityBicubic;
	}
	else if (mode1 == imHighQualityBilinear || mode2 == imHighQualityBilinear )
	{
		return imHighQualityBilinear;
	}
	else if (mode1 == imBicubic || mode2 == imBicubic )
	{
		return imBicubic;
	}
	else if (mode1 == imBilinear || mode2 == imBilinear )
	{
		return imBilinear;
	}
	else
	{
		return imNone;
	}
}

// *****************************************************************
//		DrawImageGroups
// *****************************************************************
// groupIndex - index of group that should be drawn
void CMapView::DrawImageGroups(const CRect& rcBounds, Gdiplus::Graphics* graphics, int groupIndex)
{
	CImageDrawer imgDrawer(graphics, &extents, m_pixelPerProjectionX, m_pixelPerProjectionY, m_viewWidth, m_viewHeight);
	IImage* iimg = NULL;

	ImageGroup* group = (*m_imageGroups)[groupIndex];
	if (! group->isValid ) 
		return;
	
	// in case the image was drawn at least once at current resolution, we can use screenBitmap
	ScreenBitmap* bmp = NULL;
	bmp = group->screenBitmap;
	if (bmp != NULL)
	{
		if (bmp->extents == extents &&
			bmp->pixelPerProjectionX == m_pixelPerProjectionX &&
			bmp->pixelPerProjectionY == m_pixelPerProjectionY &&
			bmp->viewWidth == m_viewWidth &&
			bmp->viewHeight == m_viewHeight)
		{
			//Gdiplus::Graphics g(dc->m_hDC);
			graphics->SetPixelOffsetMode(Gdiplus::PixelOffsetModeHighQuality);
			
			graphics->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			graphics->SetSmoothingMode(Gdiplus::SmoothingModeDefault);
			graphics->SetCompositingQuality(Gdiplus::CompositingQualityHighSpeed);
			graphics->DrawImage(bmp->bitmap, Gdiplus::REAL(bmp->left), Gdiplus::REAL(bmp->top));
			return;
		}
	}
	
	double scale = GetCurrentScale();
	int zoom;
	this->m_tiles->get_CurrentZoom(&zoom);

	if(group->image == NULL)
	{
		// creating a new temporary image		
		IImage* imgGroup = NULL;
		VARIANT_BOOL vbretval;
		CoCreateInstance(CLSID_Image,NULL,CLSCTX_INPROC_SERVER,IID_IImage,(void**)&imgGroup);
		imgGroup->CreateNew(group->width, group->height, &vbretval);

		if ( !vbretval )
		{
			return;
		}
		else
		{
			// setting it's position
			imgGroup->put_dX(group->dx);
			imgGroup->put_dY(group->dy);
			imgGroup->put_XllCenter(group->xllCenter);
			imgGroup->put_YllCenter(group->yllCenter);
			
			tkInterpolationMode downsamplingMode = imNone;
			tkInterpolationMode upsamplingMode = imNone;
			
			// acquiring reference to the destination color array
			unsigned char* data = ((CImageClass*)imgGroup)->get_ImageData();
			colour* dstData = reinterpret_cast<colour*>(data);
			
			// passing the data from all images
			bool visibleLayerExists = false;
			bool useTransparencyColor = true;		
			for(size_t j = 0; j < m_activeLayers.size(); j++)
			{
				Layer * l = m_allLayers[m_activeLayers[j]];
				if( l != NULL )
				{	
					//if(l->type == ImageLayer && (l->flags & Visible))
					if(l->type == ImageLayer && l->IsVisible(scale, zoom))
					{
						//l->object->QueryInterface(IID_IImage, (void**)&iimg);
						//if (iimg)
						if (l->QueryImage(&iimg))
						{
							CImageClass* img = (CImageClass*)iimg;

							if ( img )
							{
								if (img->m_groupID == groupIndex)
								{
									tkInterpolationMode downMode;
									tkInterpolationMode upMode;
									img->get_DownsamplingMode(&downMode);
									img->get_UpsamplingMode(&upMode);
									
									// in case at least one image don't use transparency the grouped bitmap will have white background
									VARIANT_BOOL transp;
									img->get_UseTransparencyColor(&transp);
									if (!transp) 
										useTransparencyColor = false;

									downsamplingMode = ChooseInterpolationMode(downsamplingMode, downMode);
									upsamplingMode = ChooseInterpolationMode(upsamplingMode, upMode);
									
									visibleLayerExists = true;

									DataPixels* pixels = img->m_pixels;
									int pixelsCount = img->m_pixelsCount;

									// passing data
									DataPixels* val;
									for (int p = 0; p < pixelsCount; p++ )
									{
										val = pixels + p;
										memcpy(&(dstData[val->position]), &val->value, sizeof(colour));
										//dstData[val->position] = val->value;
									}
								}
							}
							iimg->Release();
						}
					}
				}
			}
			
			if (useTransparencyColor)
			{
				imgGroup->put_TransparencyColor(RGB(255, 255, 255));
				imgGroup->put_TransparencyColor2(RGB(255, 255, 255));
				imgGroup->put_UseTransparencyColor(VARIANT_TRUE);
			}
			else
			{
				imgGroup->put_UseTransparencyColor(VARIANT_FALSE);
			}

			if (!visibleLayerExists)
			{
				return;
			}
			else
			{
				// setting sampling mode
				imgGroup->put_UpsamplingMode(upsamplingMode);
				imgGroup->put_DownsamplingMode(downsamplingMode);
				group->image = imgGroup;
			}
		}
	}
	
	// drawing; in case we draw it first time screen bitmap will be saved, for not doing resampling when loading each new tile
	/* ScreenBitmap*  */
	bmp = imgDrawer.DrawImage(rcBounds, group->image, true);
	if (bmp)
	{
		if (group->screenBitmap != NULL)
		{
			delete group->screenBitmap;
			group->screenBitmap = NULL;
		}
		
		int width = bmp->bitmap->GetWidth();
		int height = bmp->bitmap->GetHeight();
		
		group->screenBitmap = bmp;	// saving bitmap in screen resolution
	}
}

// *****************************************************************
//		ImageGroupsAreEqual()
// *****************************************************************
bool CMapView::ImageGroupsAreEqual(std::vector<ImageGroup*>& groups1, std::vector<ImageGroup*>& groups2)
{
	if (groups1.size() != groups2.size())
	{
		return false;
	}
	else
	{
		for (size_t i = 0; i < groups1.size(); i++)
		{
			if (!(groups1[i] == groups2[i]))
			{
				return false;
			}
		}
	}
	return true;
}

