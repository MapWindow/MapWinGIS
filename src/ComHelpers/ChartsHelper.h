#pragma once
class ChartsHelper
{
public:
	static bool ReadChartFields(IShapefile* sf, std::vector<double*>* values);
	static bool ReadChartField(IShapefile* sf, std::vector<double>* values, int fieldIndex);
};

