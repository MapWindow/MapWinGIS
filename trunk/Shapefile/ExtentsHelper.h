#pragma once
class ExtentsHelper
{
public:
	static IExtents* Populate(Extent& extent);
	static IExtents* GetWorldBounds();
};

