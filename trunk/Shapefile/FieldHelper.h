#pragma once
class FieldHelper
{
public:
	static bool FieldsAreEqual(IField* field1, IField* field2);
	static bool UniqueFieldNames(IShapefile* sf);
	static long FindNewShapeID(IShapefile* sf, long FieldIndex);
};

