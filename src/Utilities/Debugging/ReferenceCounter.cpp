#include "stdafx.h"
#include "ReferenceCounter.h"
#if DEBUG_ALLOCATED_OBJECTS
#include "Shape.h"
#include "ShapeDrawingOptions.h"
#include "Shapefile.h"
#endif

// ********************************************************
//     WriteReport()
// ********************************************************
void ReferenceCounter::WriteReport(bool unreleasedOnly)
{
	CString s = GetReport(unreleasedOnly);
	Debug::WriteLine(s);
	Debug::WriteLine("-------------------------");
}

// ********************************************************
//     GetReport()
// ********************************************************
CString ReferenceCounter::GetReport(bool unreleasedOnly)
{
	CString s, temp;
	s += unreleasedOnly ? "UNRELEASED COM REFERENCES:\n": "COM REFERENCES (unreleased/allocated):\n";
	for(int i = 0; i < INTERFACES_COUNT; i++)
	{
		if (unreleasedOnly)
		{
			if (referenceCounts[i] != 0)	
			{
				temp.Format("Class %s: %d\n", ComHelper::GetInterfaceName((tkInterface)i), referenceCounts[i]);
				s += temp;
			}
		}
		else
		{
			if (totalCounts[i] != 0)	
			{
				temp.Format("Class %s: %d/%d\n", ComHelper::GetInterfaceName((tkInterface)i), referenceCounts[i], totalCounts[i]);
				s += temp;
			}
		}
	}

#if DEBUG_ALLOCATED_OBJECTS
	if (unreleasedOnly) {
		if (referencePtrs.size() > 0) {
			temp.Format("referencePtrs.count: %d\r\n", static_cast<int>(referencePtrs.size()));
			s += temp;
			for (auto it = referencePtrs.begin(); it != referencePtrs.end();) {
				const auto ptr = *it;

				//temp.Format("0x%016llx\r\n", ptr);
				//s += temp;

				/*auto shp = static_cast<CShapefile*>(ptr);
				CComBSTR tmp;
				shp->Serialize2(VARIANT_FALSE, VARIANT_TRUE, &tmp);
				temp = CString(tmp);
				s += temp; */

				/*auto sdOptions = static_cast<CShapeDrawingOptions*>(ptr);
				CComBSTR tmp;
				sdOptions->Serialize(&tmp);
				temp = CString(tmp);
				s += temp;*/

				/*auto shape = static_cast<CShape*>(ptr);
				CComBSTR key;
				shape->get_Key(&key);
				if (key != "") {
					s += key;
				} */

				++it;
			}
		}
	}
	else
		referencePtrs.clear();
#endif

	if (temp.GetLength() == 0) {
		s += "<none>";
	}

	return s;
}

#if DEBUG_ALLOCATED_OBJECTS
CString ReferenceCounter::GetReferenceReport()
{
	CString s, temp;

	if (!referencePtrs.empty()) {
		temp.Format("referencePtrs.count: %d\r\n", static_cast<int>(referencePtrs.size()));
		s += temp;
		for (auto it = referencePtrs.begin(); it != referencePtrs.end();) {
			const auto ptr = *it;

			//temp.Format("0x%016llx\r\n", ptr);
			//s += temp;

			/*auto shp = static_cast<CShapefile*>(ptr);
			CComBSTR tmp;
			shp->Serialize2(VARIANT_FALSE, VARIANT_TRUE, &tmp);
			temp = CString(tmp);
			s += temp; */

			/*auto sdOptions = static_cast<CShapeDrawingOptions*>(ptr);
			CComBSTR tmp;
			sdOptions->Serialize(&tmp);
			temp = CString(tmp);
			s += temp;*/

			/*auto shape = static_cast<CShape*>(ptr);
			CComBSTR key;
			shape->get_Key(&key);
			if (key != "") {
				s += CString(key) + " ";
			}
			CComBSTR bstr;
			shape->ExportToWKT(&bstr);
			s += "WKT: " + CString(bstr) + "\r\n"; */

			++it;
		}
	}

	return s;
}
#endif