# ifndef TRIANGLE_TABLE_H
# define TRIANGLE_TABLE_H

# include "tintypes.h"
# include "table_row_node.h"
# include "table_row.h"
# include "TinHeap.h"

enum COLUMN { VTX_ONE, VTX_TWO, VTX_THREE,	//long
			  BDR_ONE, BDR_TWO, BDR_THREE,	//long
			  DEV_VERTEX,					//vertex
			  MAX_DEV };					//double

class triangleTable
{
	public:
		triangleTable();
		~triangleTable();

		void * getValue( COLUMN column, long row );
		void setValue( COLUMN column, long row, void * value );	

		long addRow( tinTableRow & r );
		void setRow( tinTableRow & r, long row );
					
		long size();
		void clear();

	private:

		long addRow( tableRowNode *& root, tableRowNode * r, long cnt = 0 );		
		std::deque< tableRowNode * > rows;			
};

# endif