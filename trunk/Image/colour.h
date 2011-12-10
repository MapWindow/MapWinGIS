#ifndef COLORS_H
#define COLORS_H

# pragma pack(push,1)
class colour
{
public:
	colour();
	colour(unsigned char r, unsigned char g, unsigned char b);
	colour( const colour & c );
	~colour();

	unsigned char	blue;
	unsigned char	green;
	unsigned char	red;	

	bool operator==( const colour & c );
	colour operator=( const colour & c );
};
# pragma pack(pop)

#define C_MAROON		colour(127,0,0);
#define C_RED			colour(255,0,0);
#define	C_FOREST		colour(0,127,0);
#define C_BROWN		colour(127,127,0);
#define C_ORANGE		colour(255,127,0);
#define C_GREEN		colour(0,255,0);
#define C_LTGREEN		colour(127,255,0);
#define C_YELLOW		colour(255,255,);
#define C_NAVY		colour(0,0,100);
#define C_PURPLE		colour(127,0,100);
#define C_GRAY		colour(127,127,127);
#define C_PINK		colour(255,127,127);
#define C_TURQUOISE	colour(0,255,127);
#define C_LIME		colour(127,255,127);
#define C_BLUE		colour(0,0,255);
#define C_LTBLUE		colour(127,127,255);
#define C_VIOLET		colour(255,0,255);

#endif 