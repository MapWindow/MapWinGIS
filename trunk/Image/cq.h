#ifndef CQ_H_
#define CQ_H_

#define MAXCOLOR	256
#define	RED	2
#define	GREEN	1
#define BLUE	0

extern unsigned char * Ir;
extern unsigned char * Ig;
extern unsigned char * Ib;

extern unsigned char lut_r[MAXCOLOR], lut_g[MAXCOLOR], lut_b[MAXCOLOR];
extern unsigned short int *Qadd;
extern int size;
extern int K;

void main_cq();

	/* output lut_r, lut_g, lut_b as color look-up table contents,
	   Qadd as the quantized image (array of table addresses). */

#endif CQ_H_