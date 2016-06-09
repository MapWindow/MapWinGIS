#pragma once
class ChartsHelper
{
public:
	static bool ReadChartFields(IShapefile* sf, std::vector<double*>* values);
	static bool ReadChartField(IShapefile* sf, std::vector<double>* values, int fieldIndex);
	static long GetCollisionBuffer(ICharts* charts);
	static long GetOffsetX(ICharts* charts);
	static long GetOffsetY(ICharts* charts);
};

