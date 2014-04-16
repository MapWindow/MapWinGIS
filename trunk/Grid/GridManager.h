# ifndef GRIDMANAGER_H
# define GRIDMANAGER_H

# include "sGrid.h"
# include "lGrid.h"
# include "fGrid.h"
# include "dGrid.h"
# include "grdTypes.h"
# include "EsriDll.h"
# include <deque>

class __declspec( dllexport ) GridManager
{
public:
	GridManager();
	~GridManager();
	
	DATA_TYPE getGridDataType( const char * filename, GRID_TYPE GridType = USE_EXTENSION );
	GRID_TYPE getGridType( const char * filename );
	bool canUseEsriGrids();
	bool deleteGrid( const char * filename, GRID_TYPE GridType = USE_EXTENSION );
	static bool GridManager::NeedProxyForGrid(CStringW filename, tkGridProxyMode proxyMode, IGrid* grid = NULL);
	static CStringW GetProxyLegendName(CStringW filename);
	static CStringW GridManager::GetProxyWorldFileName(CStringW filename);
	static CStringW GetProxyName(CStringW filename);
	static bool GridManager::HasValidProxy(CStringW filename);
	static CStringW GridManager::GetOverviewsFilename(CStringW filename);
	static bool GridManager::RemoveImageProxy(CStringW gridFilename);
};

# endif