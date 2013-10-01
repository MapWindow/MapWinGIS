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
};

# endif