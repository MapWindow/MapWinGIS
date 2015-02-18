# ifndef TIN_TABLE_ROW_H
# define TIN_TABLE_ROW_H

# include "tintypes.h"
# include "vertex.h"

class tinTableRow
{
	public:
		tinTableRow();
		~tinTableRow();
		tinTableRow( long v1, long v2, long v3, long b1, long b2, long b3 );
		tinTableRow( long v1, long v2, long v3, long b1, long b2, long b3, double dev, vertex dV );
		tinTableRow( const tinTableRow & row );
		tinTableRow operator=( const tinTableRow & row );

		long vertexOne;
		long vertexTwo;
		long vertexThree;
		long borderOne;
		long borderTwo;
		long borderThree;
		vertex devVertex;
		double maxDev;
};

# endif