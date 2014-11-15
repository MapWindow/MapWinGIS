class DraggingState
{
public:
	DraggingState() : Start(0, 0), Move(0, 0), Operation(DragNone), HasMoved(false), Snapped(false),
		LayerHandle(-1), Shapefile(NULL), InitAngle(0.0){};
	DraggingOperation Operation;
	CPoint Start;
	CPoint Move;
	bool HasMoved;
	bool Snapped;
	Point2D Proj;
	Point2D RotateCenter;
	int LayerHandle;
	IShapefile* Shapefile;
	double InitAngle;   // degrees
	void CloseShapefile()
	{
		if (!Shapefile) return;
		VARIANT_BOOL vb;
		Shapefile->Close(&vb);
		Shapefile->Release();
		Shapefile = NULL;
	}
	void SetShapefile(IShapefile* sfNew)
	{
		CloseShapefile();
		Shapefile = sfNew;
		SetShapefileColors();
	}
	bool HasRectangle()
	{
		return !(abs(Start.x - Move.x) < 10 && abs(Start.y - Move.y) < 10);
	}
	CRect GetRectangle()
	{
		CRect r = CRect(Start.x, Start.y, Move.x, Move.y);
		r.NormalizeRect();
		return r;
	};
	void Clear()
	{
		Proj.x = Proj.y = Start.x = Start.y = Move.x = Move.y = 0;
		Operation = DragNone;
		Snapped = false;
		HasMoved = false;
		CloseShapefile();
		LayerHandle = -1;
		InitAngle = 0.0;
	}
	void SetSnapped(double xProj, double yProj){
		Snapped = true;
		Proj.x = xProj;
		Proj.y = yProj;
	}
	int GetOffsetX() { return Move.x - Start.x; }
	int GetOffsetY() { return Move.y - Start.y; }
private:
	void SetShapefileColors()
	{
		if (!Shapefile) return;
		CComPtr<IShapeDrawingOptions> options = NULL;
		Shapefile->get_DefaultDrawingOptions(&options);
		if (options) {
			options->put_FillColor(RGB(255, 0, 0));
			options->put_FillTransparency(100.0f);
			options->put_LineWidth(2.0f);
			options->put_LineColor(RGB(255, 0, 0));
		}
	}
};
