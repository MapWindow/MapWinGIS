#pragma once
class ProjectionHelper
{
public:
	static bool IsEmpty(IGeoProjection* gp);
	static bool IsGeographic(IGeoProjection* gp);
	static bool IsSame(IGeoProjection* gp1, IGeoProjection* gp2, IExtents* bounds, int sampleSize);
};

