
# ifndef ESRI_DLL_H
# define ESRI_DLL_H

#include "RegistryKey.h"
#include "gioapi.h"
#include "grdTypes.h"
#include <direct.h>
#include <iostream>
#include <afx.h>

using namespace std;

//using namespace JetByteTools;
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//					Esri Functions
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//

bool initialize_esri();
void shutdown_esri();
bool can_use_esri_grids();

void delete_esri_grid( CString filename );
DATA_TYPE esri_data_type( CString filename );
bool find_esri_dll(bool load_library);
bool find_avgrid_dll(bool load_library);
bool find_aigrid_dll(bool load_library);

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//	#########		###### IMPORTANT #######		#########
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//This DLL needs to have all optimizations set to DISABLED!!! or else the ESRI DLL will crash
// and burn, and cause you to pull your hair out.....!!!!!!#######

//	ie.	Project->Settings->General->Optimizations

//	OR ......
//Wrap the offending ESRI Code with
// #pragma optimize("", off)	and
//
// #pragma optimize("", on)

#define FUNC_TYPE _cdecl

typedef int (FUNC_TYPE *ESRI_GRIDIOSETUP_PROC)(void);
typedef int (FUNC_TYPE *ESRI_GRIDIOEXIT_PROC)(void);
typedef int (FUNC_TYPE *ESRI_CELLLAYEROPEN_PROC)(char *layer_name, int rdwrflag, int iomode, int * celltype, double * cellsize);
typedef int (FUNC_TYPE *ESRI_CELLLYRCLOSE_PROC)(int channel);
typedef int (FUNC_TYPE *ESRI_CELLLYRCLOSENOVAT_PROC)(int channel);
typedef int (FUNC_TYPE *ESRI_BNDCELLREAD_PROC)(char*grdnam,double *box);
typedef int (FUNC_TYPE *ESRI_PRIVATEACCESSWINDOWSET_PROC)(int channel, double box[4], double cellsize, double newbox[4]);
typedef int (FUNC_TYPE *ESRI_PRIVATEWINDOWCOLS_PROC)(int channel);
typedef int (FUNC_TYPE *ESRI_PRIVATEWINDOWROWS_PROC)(int channel);
typedef void (FUNC_TYPE *ESRI_GETMISSINGFLOAT_PROC)(float * floatnull);
char *CAllocate1(int number ,int size );
typedef int (FUNC_TYPE *ESRI_GETWINDOWROWINT_PROC)(int channel, int nrewwinrow, int * rowbuf );
typedef int (FUNC_TYPE *ESRI_GETWINDOWROWFLOAT_PROC)(int channel, int nrewwinrow, float * rowbuf );
typedef int (FUNC_TYPE *ESRI_GETWINDOWROW_PROC)(int channel, int nreswinrow, CELLTYPE * rowbuf );
void CFree1(char *ptr);
typedef int (FUNC_TYPE *ESRI_GETWINDOWCELLINT_PROC)(int channel, int rescol, int resrow, int * cell );
typedef int (FUNC_TYPE *ESRI_CELLLAYERCREATE_PROC)(char * layer_name, int rdwrflag, int iomode, int celltype, double cellsize, double box[4] );
typedef int (FUNC_TYPE *ESRI_CELLLYREXISTS_PROC)( char * layer_name );
typedef int (FUNC_TYPE *ESRI_GRIDDELETE_PROC)( char * layer_name );
typedef int (FUNC_TYPE *ESRI_PUTWINDOWROW_PROC)( int channel, int nreswinrow, CELLTYPE* rowbuf );
typedef int (FUNC_TYPE *ESRI_PUTWINDOWROWFLOAT_PROC)( int channel, int nreswinrow, float * rowbuf );
typedef int (FUNC_TYPE *ESRI_CELLLYRDELETE_PROC)(char * layer_name );
typedef int (FUNC_TYPE *ESRI_GRIDCOPY_PROC)(char * from_grid, char * to_grid);
typedef int (FUNC_TYPE *ESRI_GRIDEXISTS_PROC)(char * grid_name );

//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//
//				End of Esri Functions
//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//\\//


# endif