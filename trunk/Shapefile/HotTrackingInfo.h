class HotTrackingInfo
{
public:
	IShapefile* Shapefile;
	int ShapeIndex;
	int LayerHandle;
	
	HotTrackingInfo() : Shapefile(NULL), ShapeIndex(-1), LayerHandle(-1) {}
	~HotTrackingInfo()
	{
		if (Shapefile)
		{
			VARIANT_BOOL vb;
			Shapefile->Close(&vb);
			Shapefile->Release();
		}
	}
	bool IsEmpty()
	{
		return ShapeIndex == 0;
	}
	bool IsSame(LayerShape& info) {
		return LayerHandle == info.LayerHandle && ShapeIndex == info.ShapeIndex;
	}
	void Clear() 
	{
		VARIANT_BOOL vb;
		ShapeIndex = -1;
		LayerHandle = -1;
		if (Shapefile)
			Shapefile->Close(&vb);
	}
	void Update(IShapefile* source, IShape* shp, int layerHandle, int shapeIndex);
	void UpdateStyle(IShapeDrawingOptions* options)
	{
		if (Shapefile) {
			Shapefile->put_DefaultDrawingOptions(options);
		}
	}
};
