#pragma once
#include <Gdiplus.h>
using namespace Gdiplus;

// QColorMatrix
//
// Extension of the GDI+ struct ColorMatrix.
// Adds some member functions so you can actually do something with it.
// Use QColorMatrix like ColorMatrix to update the ImmageAttributes class.
// Use at your own risk. Comments welcome.
//
// See: http://www.sgi.com/grafica/matrix/
// http://www.sgi.com/software/opengl/advanced98/notes/node182.html
//
// (c) 2003, Sjaak Priester, Amsterdam.
// mailto:sjaak@sjaakpriester.nl

class QColorMatrix :
	public ColorMatrix
{
public:
	QColorMatrix()							{ Reset(); }
	QColorMatrix(const QColorMatrix& m)		{ Copy(m); }
	~QColorMatrix()							{ }

	QColorMatrix& operator=(const QColorMatrix& m)
								{ if (this != &m) Copy(m); return * this; }

	// Create a new QColorMatrix that is a copy.	
	QColorMatrix * Clone()		{ return new QColorMatrix(* this); }

	// All functions: return Ok if no error, other valus of the Status enumeration if error.
	// order is MatrixOrderPrepend (parameter is on the left) or MatrixOrderAppend.

	// Update matrix to the identity matrix (1.0f on diagonal, rest 0.0f)
	Status Reset();

	// Multiply the vector by the matrix. v points to an array of four values,
	// representing R, G, B and A.
	Status TransformVector(REAL * v) const;

	// Multiply each color by the matrix. count is the number of colors.
	Status TransformColors(Color * colors, INT count = 1) const;

	// Update this matrix with the product of itself and another matrix.
	Status Multiply(const ColorMatrix * matrix, MatrixOrder order = MatrixOrderPrepend);

	// Update this matrix with the product of itself and a scaling vector.
	Status Scale(REAL scaleRed, REAL scaleGreen, REAL scaleBlue,
				REAL scaleOpacity = 1.0f, MatrixOrder order = MatrixOrderPrepend);

	// Scale just the three colors with the same amount, leave opacity unchanged.
	Status ScaleColors(REAL scale, MatrixOrder order = MatrixOrderPrepend)
								{ return Scale(scale, scale, scale, 1.0f, order); }

	// Scale just the opacity, leave R, G and B unchanged.
	Status ScaleOpacity(REAL scaleOpacity, MatrixOrder order = MatrixOrderPrepend)
								{ return Scale(1.0f, 1.0f, 1.0f, scaleOpacity, order); }

	// Update this matrix with the product of itself and a translation vector.	
	Status Translate(REAL offsetRed, REAL offsetGreen, REAL offsetBlue,
				REAL offsetOpacity, MatrixOrder order = MatrixOrderPrepend);

	// Translate just the three colors with the same amount, leave opacity unchanged.
	Status TranslateColors(REAL offset, MatrixOrder order = MatrixOrderPrepend)
								{ return Translate(offset, offset, offset, 0.0f, order); }

	// Translate just the opacity, leave R, G and B unchanged.
	Status TranslateOpacity(REAL offsetOpacity, MatrixOrder order = MatrixOrderPrepend)
								{ return Translate(0.0f, 0.0f, 0.0f, offsetOpacity, order); }

	// Rotate the matrix around one of the color axes. The color of the rotation
	// axis is unchanged, the other two colors are rotated in color space.
	// The angle phi is in degrees (-180.0f... 180.0f).
	Status RotateRed(REAL phi, MatrixOrder order = MatrixOrderPrepend)
								{ return RotateColor(phi, 2, 1, order); }
	Status RotateGreen(REAL phi, MatrixOrder order = MatrixOrderPrepend)
								{ return RotateColor(phi, 0, 2, order); }
	Status RotateBlue(REAL phi, MatrixOrder order = MatrixOrderPrepend)
								{ return RotateColor(phi, 1, 0, order); }

	// Shear the matrix in one of the color planes. The color of the color plane
	// is influenced by the two other colors.
	Status ShearRed(REAL green, REAL blue, MatrixOrder order = MatrixOrderPrepend)
								{ return ShearColor(0, 1, green, 2, blue, order); }
	Status ShearGreen(REAL red, REAL blue, MatrixOrder order = MatrixOrderPrepend)
								{ return ShearColor(1, 0, red, 2, blue, order); }
	Status ShearBlue(REAL red, REAL green, MatrixOrder order = MatrixOrderPrepend)
								{ return ShearColor(2, 0, red, 1, green, order); }
	
	// Set the saturation of the matrix. Saturation of 0.0f yields B&W, 1.0f is neutral.
	Status SetSaturation(REAL saturation, MatrixOrder order = MatrixOrderPrepend);

	// Rotate the hue around the grey axis, keeping luminance fixed. Greys are fixed,
	// all other colours change.
	Status RotateHue(REAL phi);

	Status SetGreyscale(MatrixOrder order);

	// intensity is expected to be in [0, 1]
	Status Colorize(OLE_COLOR color, float intensity, MatrixOrder order = MatrixOrderPrepend);

protected:
	Status RotateColor(REAL phi, int x, int y, MatrixOrder order = MatrixOrderPrepend);
	Status ShearColor(int x, int y1, REAL d1, int y2, REAL d2, MatrixOrder order = MatrixOrderPrepend);

	static QColorMatrix preHue;
	static QColorMatrix postHue;

private:
	void Copy(const QColorMatrix& m);
	static void InitHue();
	static bool initialized;

#ifdef _DEBUG
public:
	void Dump(CDumpContext& dc) const;
#endif
};
