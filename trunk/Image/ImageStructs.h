#ifndef __ImageStructs_h_
#define __ImageStructs_h_

typedef struct{
	char sig[6];
	unsigned short screenwidth, screenheight;
	unsigned char flags, background, aspect;
} GIFHEADER;

typedef struct {
	unsigned short left, top, width, height;
	unsigned char flags;
} GIFIMAGEBLOCK;

typedef struct {
	char blocksize;
	char flags;
	unsigned short delay;
	unsigned char transparent_color;
	char terminator;
} GIFCONTROLBLOCK;

typedef struct {
	char blocksize;
	unsigned short left, top;
	unsigned short gridwidth, gridheight;
	char cellwidth, cellheight;
	char forecolor, backcolor;
} GIFPLAINTEXT;

typedef struct {
	char blocksize;
	char applstring[8];
	char authentication[3];
} GIFAPPLICATION;

struct colort{
	colort(){}
	colort(int red, int green, int blue):r(red),g(green),b(blue){}
	colort(int c){r = c; g = c>>8; b = c>>16;}
	unsigned char r, g, b;
	bool operator==(colort c){return !memcmp(this, &c, 3);}
	//bool operator==(colort c){return c.r==r && c.g==g && c.b == b;}
	operator int(){return r | g << 8 | b << 16;}
};

struct ColorEntry{
	ColorEntry(){next=NULL;}
	ColorEntry(colort col, ColorEntry * ent):c(col),next(ent){}
	bool operator!=(ColorEntry ent){return !(c == ent.c && next == ent.next);}
	colort c;
	ColorEntry * next; // the next color to to plot
};

#endif