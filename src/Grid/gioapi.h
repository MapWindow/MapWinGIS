
/*
****************************************************
 
Grid IO API
---------------

Function Naming Conventions
---------------------------

Function names are multi-word with no underscores,
and the first letter of each word capitalized.
For example : CellLyrOpen, GetWindowRectangle.

****************************************************
*/


/*
Constants Defining String Handling
*/

#ifndef _strlen
#define	_strlen		129		/* Max string Length */
#endif

/*
Defined C constants
*/
#ifndef TRUE
#define TRUE		1
#endif

#ifndef FALSE
#define FALSE		0
#endif

#ifndef NULL
#define	NULL		0
#endif

/*
Constants Defining CellLayer ReadWrite Flag
*/
#ifndef READONLY
#define	READONLY	1
#endif
#ifndef READWRITE
#define	READWRITE	2
#endif
#ifndef WRITEONLY
#define WRITEONLY       3
#endif

/*
Constants Defining CellLayer I/O Mode
Buffer Cache Allocation is dependent on IO mode.
*/

#define	ROWIO		1

#define	CELLIO		2

#define	BLOCKIO		3

#define ROWRUNIO	4
#define BLOCKRUNIO	5

#define	REGIONIO	6	
#define REGIONRUNIO	7


typedef	int		CELLTYPE;	/* cell values are 32 bit quantities */

#define	CELLINT		1		/* 32 bit signed integers */
#define	CELLFLOAT	2		/* 32 bit floating point numbers*/

#define	CELLMAX		 2147483646	/* maximum integer value  (2**31 - 2) */
#define	CELLMIN		-2147483646	/* minimum integer value -(2**31 - 2) */



#define	MISSINGINT	-2147483647	/* CELLMIN - 1 */


void  GetMissingFloat(float *);
          

/*
 * ------------------------------------------------------------------
 */


/*
 * Grid Management Functions
 * -------------------------
 *
 * GridExists - check for existence of a grid 
 * GridCopy - copy a grid given from and to pathnames
 * GridRename - rename a grid given old and new pathnames
 * GridDelete - delete a grid removing all 
 *              component files including info files 
 * GridKill - delete a grid with options for component removal 
 * DescribeGridDbl - Describe spatial, statistical and attribute
 *                   characteristics of a grid
 * BndCellRead - read the bounding box for a grid
 */

int GridExists(char *grid_name);
int GridCopy(char *from_grid, char *to_grid);
int GridRename(char *from_grid, char *to_grid);
int GridDelete(char *grid_name);
int GridKill(char *grid_name, int option);

int DescribeGridDbl(char *grdnam, double *cellsz, int *gridsz, 
                    double *box, double *sta, 
                    int *datatyp, int *nclass, int *reclen);


int BndCellRead (char *grdnam, double *box); 




/* 
 * CLYRIO : Access operations on Grids
 * -----------------------------------
 */

/*
 * PURPOSE
 *
 * CLYRIO performs I/O operations on grids.
 * It consists of a library of C functions that allow the user
 * to read and write raster data by row, rectangle or cell
 * to or from a specified area of a grid at a specified resolution. 
 * It handles windowing, resampling, 
 * attribute access and data compression 
 * in a manner transparent to the application program.
 */

/*
 * IMPLEMENTATION
 *
 * Grids are based on a locational data model in which space is partitioned
 * into square cells. Each cell in a grid stores a numeric value.
 * Integer Grids are also accompanied by Value Attribute Tables that
 * can store attributes defined on the set of values for a grid. 
 *
 * A grid exists in a cartesian coordinate system, and the rows and columns in the
 * grid are parallel to the axes of the coordinate system.
 * Grids in a database that are used for joint analysis
 * need not be at the same resolution
 * or cover exactly the same spatial area
 * but they must be in the same map projection.
 * 
 * Grids are implemented using a tiled raster data structure
 * in which the basic unit of data storage is 
 * a rectangular block of cells. 
 *
 * A Cell Layer is a raster layer defined
 * by a grid name and optionally an item name in the VAT.
 * The CLYRIO module allows an aplication program to
 * create, open and close cell layers and 
 * to read and write data from and
 * to cell layers. 
 * All access to the data in a cell layer is 
 * made through a rectangular window in cartesian (map) coordinate space.
 * The extent and resolution of 
 * this window must be set by the application program by calling
 * the WindowSet routine, prior to performing any
 * I/O operations.
 * Once a window has been set, an application program can access the cells 
 * in a cell layer by row, rectangle, or cell - all specified 
 * relative to the virtual raster represented by the window.
 *
 * The I/O units used by the application program are  
 * the Window-Row spanning the width of the window, the Window-Band consisting
 * of a set of contiguos rows, the Window-Rectangle consisting of a 2D rectangle
 * contained in the window and, the Window-Cell consisting of a single cell
 * specified by its window row and column coordinates.
 * All cell data is automatically resampled to the window cell size on retrieval
 * during row and cell access. 
 *
 * An application program can either set a global window 
 * that is applied to all cell layers accessed or set a separate 
 * window for each layer accessed. The latter mode is suitable 
 * when transforming a grid from one coordinate system to another
 * such as during warping or projection. 
 * 
 * Grids may be integer or floating point.
 * Integer grids are accompanied by a Value Attribute Table.
 * The CLYRIO module supports spatial access to the numeric value 
 * attributes associated with a grid by supporting 
 * a grid.item naming convention for cell layers that correspond 
 * to grid value attributes.
 * If an item name is specified when a Cell Layer is opened
 * for read access then the values returned for the cells retrieved
 * will be the values for the specified item.
 * 
 */


 
/* 
 * Initialization  and Termination
 * --------------------------------
 *
 * GridIOSetup - Initialize the Grid IO library
 * GridIOExit  - Call when done using the Grid IO library
 */
int GridIOSetup(void);
int GridIOExit(void);


/* 
 * Creating, Opening and Closing Grids :
 * -----------------------------------
 *
 * CellLyrExists - Check if a cell layer exists.
 * CellLayerCreate - Create a new Cell Layer using the default blocksize.
 * CellLayerCreateTiled - Create a new Cell Layer using the specified blocksize.
 * CellLayerOpen - Open an existing Cell Layer.
 * CellLyrClose - Close an opened or created Cell Layer, 
 *                automatically builds VAT and STA.
 * CellLyrCloseNoVat - Close an opened or created Cell Layer, 
 *                     does not build VAT.
 * CellLyrDelete - Delete a cell layer.
 *
 * IO mode 
 * -------
 *
 * The client is responsible for selecting an IO mode
 * when opening or creating a cell layer and for using 
 * the access method appropriate for the current IO mode.
 * The IO mode can be reset while the layer is opened.
 * The permissible values for IO mode are
 * ROWIO, ROWRUNIO, CELLIO, REGIONIO, REGIONRUNIO, BLOCKIO and BLOCKRUNIO
 * For a more detailed description of these modes see the
 * section on IO mode below.
 *
 * ReadWriteFlag
 * -------------
 * A celllayer is opened READONLY or READWRITE 
 *
 * Compression
 * -----------
 *
 * Integer grids are stored in a run length compressed format.
 * Floating point grids are stored in uncompressed format.
 *
 */

#define  ROWIO               1
#define  CELLIO              2
#define  BLOCKIO             3
#define  ROWRUNIO            4
#define  BLOCKRUNIO          5
#define  REGIONIO            6
#define  REGIONRUNIO         7


#define  READONLY            1
#define  READWRITE           2


#define  ADAPTIVE           -1

#define  COMPRESSED          0
#define  UNCOMPRESSED        1

int CellLyrExists (char *clyr_name);

int CellLayerCreate (char *clyr_name, int rdwrflag, int iomode, 
                     int celltype, double cellsize, double box[4]);

int CellLayerCreateTiled (char *clyr_name, int rdwrflag, int iomode, 
                          int  celltype, double cellsize, double box[4], 
                          int blockxcells, int blockycells);

int CellLayerOpen (char *clyr_name, int rdwrflag, int iomode, 
                   int  *celltype, double *cellsize);

int CellLyrClose (int channel);
int CellLyrCloseNoVat (int channel);
int CellLyrDelete (char *clyr_name);

/*
 * AcessDepth 
 *
 * CLYRIO mantains an internal cache of block buffers for each opened grid.
 *
 * The process of performing IO on a cell layer involves
 * mapping data between client buffers and cache buffers
 * for the layer and between the cache buffers and records on disk. 
 * Data compression is applied in the process of mapping data between the 
 * cache buffers and records on disk and is hidden from the client.
 * 
 * When a layer is opened in 32 bit mode the client is responsible
 * for ensuring that all client (input and output) buffers used in the
 * CLYRIO API functions are 32 bits per cell.
 * In 32 bit access mode all the block buffers in the cache for the layer
 * are also 32 bits per cell.
 * The Client is responsible for using the 32 bit API functions.
 * By default a layer is opened in 32 bit mode.
 *
 *
 */
 
 

/*
 * IO mode 
 *
 * The client is responsible for using 
 * the access method appropriate for the current IO mode.
 * The IO modes supported are :
 *
 * ROWIO - To Get and Put Rows of Cells
 * CELLIO - To Get and Put individual cells
 * REGIONIO - To Get and Put Rectangles of cells
 *
 * The IO mode for an opened layer can be queried and changed
 * with the following API calls
 *
 */
int CellLyrGetIO(int channel);
int CellLyrSetIO(int channel, int mode);


/*
 * Getting information about an opened cell layer given its handle
 *
 * CellLyrName - query the grid name for the cell layer 
 * CellLyrItem - query the item name for the cell layer 
 * CellLyrEvalType - query the cell type for the cell layer, 
 *                   the cell type is based on the type of the item
 * CellLyrBlockSize - query the block size for the cell layer 
 * CellLyrSta - query the statistics for the cell layer 
 */

int CellLyrName (int channel, char *name);
int CellLyrItem (int channel, char *name);
int CellLyrEvalType (int channel);
int CellLyrBlockSize (int channel, int *bxcells, int *bycells);
int CellLyrSta (int channel, double *dmin, double *dmax, 
                double *dmean, double *dstdv);


/* 
 * Setting and Querying the Overlay Window for Multi-Layer registered I/O
 *
 * AccessWindowSet - Set the Overlay Window
 * WindowRows - Return number of rows in the overlay window.
 * WindowCols - Return number of columns in the overlay window.
 * WindowBox - Return the box for the overlay window.
 * XY2WindowColRow - translate from map to window cell coordinates
 * AccessWindowClear - Clear the Overlay Window
 */

int AccessWindowSet(double box[4], double cellsize, double newbox[4]);
int WindowRows(void);
int WindowCols(void);
int WindowBox(double box[4]);
int XY2WindowColRow(double x, double y, int *col, int *row);
int AccessWindowClear(void);

/*
 * 
 * Routines for Setting Separate Windows for each layer 
 *
 * PrivateAccessWindowSet - Set the Window for the specified layer.
 * PrivateWindowRows - Return number of rows in window for specified layer.
 * PrivateWindowColumns - Return number of columns in window for specified layer.
 * PrivateWindowBox - Return the box for the window for specified layer.
 * XY2PrivateWindowColRow - translate from map to window cell coordinates
 * PrivateAccessWindowClear - Clear the window for the specified layer
 */

int PrivateAccessWindowSet(int channel, double box[4], 
                           double cellsize, double newbox[4]);
int PrivateWindowCols(int i);
int PrivateWindowRows(int i);
int PrivateWindowBox(int i, double box[4]);
int XY2PrivateWindowColRow(int i, double x, double y, int *col, int *row);
int PrivateAccessWindowClear(int channel);


/*
 * Memory Allocation for Rows, Bands and Rectangles
 *
 * CAllocate1 - allocate a 1 dimensional  array (Row)
 *              that can be accessed as pointer[i]
 * CAllocate2 - allocate a 2 dimensional "array" (Rectangle, Band)
 *              that can be accessed as pointer[i][j]
 * CAllocate3 - allocate a 3 dimensional "array" 
 *              that can be accessed as pointer[i][j][k]
 * CFree1     - free a 1 dimensional array
 * CFree2     - free a 2 dimensional array
 * CFree3     - free a 3 dimensional array
 *
 */

//char *CAllocate1(int number, int size);
char **CAllocate2(int nrows, int ncols, int size);
char ***CAllocate3(int nlyrs, int nrows, int ncols, int size);

//void CFree1(char *ptr);
void CFree2(char **ptr, int nrows);
void CFree3(char ***ptr, int nlyrs, int nrows);

/* 
 * ACCESS FUNCTIONS :
 * The following functions access the data within the window for a given layer.
 * The window may be the overlay window, or the private window set for the layer.
 */

/*
 * Routines for 32 bit ROW based access :
 * 
 * GetWindowRow - Get a row spanning the window for a layer
 *                as a buffer of 32 bit values.
 *                Client must interpret the type of the output buffer
 *                to be the type of the layer being read from (32 bit int or float).
 *
 * GetWindowRowFloat - Get a row as a buffer of 32 bit floating points
 * GetWindowRowInt - Get a row as a buffer of 32 bit integers
 *
 * PutWindowRow - Put a row spanning the window for a layer
 *                as a buffer of 32 bit values.
 *                Client must ensure that the type of the input buffer
 *                (32 bit int or float) is the type of the layer being written to.  
 *
 * The following band access functions retrieve contiguos sets of
 * rows into a two dimensional buffer.
 *               
 * GetWindowBand - Get a band of rows spanning the window for a layer.
 *                 Client must interpret the type of the output buffer
 *                 to be the type of the layer being read from (32 bit int or float).
 * GetWindowBandFloat - Get a band of rows as a buffer of 32 bit floating points
 * GetWindowBandInt - Get a band of rows as a buffer of 32 bit ints
 *
 * PutWindowBand - Put a band of rows spanning the window for a layer.
 *                Client must ensure that the type of the input buffer
 *                (32 bit int or float) is the type of the layer being written to.  
 *
 */


int GetWindowRow(int channel, int nreswinrow, CELLTYPE *rowbuf);
int GetWindowRowFloat(int channel, int nreswinrow, float *rowbuf);
int GetWindowRowInt(int channel, int nreswinrow, int *rowbuf);

int PutWindowRow(int channel, int nwinrow, CELLTYPE *rowbuf);

int GetWindowBand(int channel, int startrow, int nrows, CELLTYPE **bandbuf);
int GetWindowBandFloat(int channel, int startrow, int nrows, float **bandbuf);
int GetWindowBandInt(int channel, int startrow, int nrows, int **bandbuf);

int PutWindowBand(int channel, int startrow, int nrows, CELLTYPE **bandbuf);




/*
 * Routines for 32 bit REGION (rectangle) based access :
 *
 * GetWindowRectangle - Get a rectangle of cells within the window for a layer
 *                      as a 2D buffer of 32 bit values.
 *                      Client must interpret the type of the output buffer
 *                      to be the type of the layer being read from 
 *                      (32 bit int or float).
 *
 * GetWindowRectangleInt - Get a rectangle of cells within the window for a layer
 *                         as a 2D buffer of 32 bit integers.
 *
 * GetWindowRectangleFloat - Get a rectangle of cells within the window for a layer
 *                         as a 2D buffer of 32 bit floats.
 *
 * PutWindowRectangle - Put a rectangle of cells within the window for a layer
 *                      as a 2D buffer of 32 bit values.
 *                      Client must ensure that the type of the input buffer
 *                      (32 bit int or float) is the type of the 
 *                      layer being written to.  
 */

int GetWindowRectangle(int channel, int rxul, int ryul, 
                       int rxcells, int rycells, CELLTYPE **rbuf);
int GetWindowRectangleInt(int channel, int rxul, int ryul, 
                          int rxcells, int rycells, int **rbuf);
int GetWindowRectangleFloat(int channel, int rxul, int ryul, 
                            int rxcells, int rycells, float **rbuf);

int PutWindowRectangle(int channel, int rxul, int ryul, 
                       int rxcells, int rycells, CELLTYPE **rbuf);


/*
 * Routines for 32 bit CELL based access :
 *
 * GetWindowCell - Get a cell within the window for a layer,
 *                 Client must interpret the type of the output 32 Bit Ptr
 *                 to be the type of the layer being read from.
 *
 * GetWindowCellFloat - Get a cell within the window for a layer as a 32 Bit Float
 *
 * GetWindowCellInt - Get a cell within the window for a layer as a 32 Bit Integer
 *
 * PutWindowCell - Put a cell within the window for a layer.
 *                 Client must ensure that the type of the input 32 Bit Ptr
 *                 is the type of the layer being read from.
 *
 *
 * PutWindowCellFloat - Put a cell within the window for a layer as a 32 Bit Float
 *
 * PutWindowCellInt - Put a cell within the window for a layer as a 32 Bit Integer
 *
 */
int GetWindowCell(int channel, int rescol, int resrow, CELLTYPE *cell);
int GetWindowCellFloat(int channel, int rescol, int resrow, float *cell);
int GetWindowCellInt(int channel, int rescol, int resrow, int *cell);

int PutWindowCellFloat(int channel, int col, int row, double fcell);
int PutWindowCellInt(int channel, int col, int row, int icell);
int PutWindowCell(int channel, int col, int row, CELLTYPE cell);


/*
 * STAIO : Read Statistics for a cell layer
 *
 * StaGetDbl - get min, max, mean and stdv for a cell layer
 * StaGetMinMaxDbl - get min and max for a cell layer
 *
 */

int StaGetDbl(char *clyr_name, double *dmin, double *dmax, 
                              double *dmean, double *dstdv);

int StaGetMinmaxDbl(char *clyr_name, double *dmin, double *dmax);

