#include "StdAfx.h"
#include "QColorMatrix.h"

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


#include <math.h>

const REAL pi = 4.0f * (REAL) atan(1.0);
const REAL rad = pi / 180.0f;

// The luminance weight factors for the RGB color space.
// These values are actually preferable to the better known factors of
// Y = 0.30R + 0.59G + 0.11B, the formula which is used in color television technique.
const REAL lumR = 0.3086f;
const REAL lumG = 0.6094f;
const REAL lumB = 0.0820f;

// statics
bool QColorMatrix::initialized = false;
QColorMatrix QColorMatrix::preHue;
QColorMatrix QColorMatrix::postHue;

Status QColorMatrix::Reset()
{
	::ZeroMemory(m, 25 * sizeof(REAL));
	for (int i = 0; i < 5; i++) m[i][i] = 1.0f;

	// GDI+ Matrix::Reset() returns a Status, and so do we,
	// although I fail to see how this could be anything but Ok.
	// (Well, I suppose GDI+ could return GdiplusNotInitialized.)
	return Ok;
}

Status QColorMatrix::Multiply(const ColorMatrix * matrix, MatrixOrder order /* = MatrixOrderPrepend */)
{
	// NOTE: The last column is NOT calculated, because it is (or at least should be)
	// always { 0, 0, 0, 0, 1 }.

	if (! matrix) return InvalidParameter;

	REAL (* a)[5];
	REAL (* b)[5];

	if (order == MatrixOrderAppend)
	{
		a = (REAL (*)[5]) matrix->m;
		b = m;
	}
	else
	{
		a = m;
		b = (REAL (*)[5]) matrix->m;
	}

	REAL temp[5][4];

    for (int y = 0; y < 5 ; y++)
        for (int x = 0; x < 4; x++)
		{
			REAL t = 0;
			for (int i = 0; i < 5; i++) t += b[y][i] * a[i][x];
            temp[y][x] = t;
        }

	for (int y = 0; y < 5; y++)
        for (int x = 0; x < 4; x++)
            m[y][x] = temp[y][x];

	return Ok;
}

// Assumes that v points to (at least) four REALs.
Status QColorMatrix::TransformVector(REAL * v) const
{
	if (! v) return InvalidParameter;

	REAL temp[4];

	for (int x = 0; x < 4; x++)
	{
		temp[x] = m[4][x];
		for (int y = 0; y < 4; y++) temp[x] += v[y] * m[y][x];
	}
	for (int x = 0; x < 4; x++) v[x] = temp[x];
	return Ok;
}

Status QColorMatrix::TransformColors(Color * colors, INT count /* = 1 */) const
{
	if (! colors) return InvalidParameter;

	REAL p[4];

	for (int i = 0; i < count; i++)
	{

		p[0] = (REAL) colors[i].GetRed();
		p[1] = (REAL) colors[i].GetGreen();
		p[2] = (REAL) colors[i].GetBlue();
		p[3] = (REAL) colors[i].GetAlpha();

		/*Status s =*/ TransformVector(p);
//		if (s != Ok) return s;		// Can never happen

		for (int j = 0; j < 4; j++)
		{
			if (p[j] < 0) p[j] = 0.0f;
			else if (p[j] > 255.0f) p[j] = 255.0f;
		}

		colors[i].SetValue(Color::MakeARGB((BYTE) p[3], (BYTE) p[0], (BYTE) p[1], (BYTE) p[2]));
	}
	return Ok;
}

// phi is in degrees
// x and y are the indices of the value to receive the sin(phi) value
Status QColorMatrix::RotateColor(REAL phi, int x, int y, MatrixOrder order /* = MatrixOrderPrepend */)
{
	phi *= rad;
	QColorMatrix m;

	m.m[x][x] = m.m[y][y] = (REAL) cos(phi);

	REAL s = (REAL) sin(phi);
	m.m[y][x] = s;
	m.m[x][y] = - s;

	return Multiply(& m, order);
}

Status QColorMatrix::ShearColor(int x, int y1, REAL d1, int y2, REAL d2, MatrixOrder order /* = MatrixOrderPrepend */)
{
	QColorMatrix m;
	m.m[y1][x] = d1;
	m.m[y2][x] = d2;

	return Multiply(& m, order);
}

Status QColorMatrix::Scale(REAL scaleRed, REAL scaleGreen,
					 REAL scaleBlue, REAL scaleOpacity, MatrixOrder order /* = MatrixOrderPrepend */)
{
	QColorMatrix m;

	m.m[0][0] = scaleRed;
	m.m[1][1] = scaleGreen;
	m.m[2][2] = scaleBlue;
	m.m[3][3] = scaleOpacity;

	return Multiply(& m, order);
}

Status QColorMatrix::Translate(REAL offsetRed, REAL offsetGreen,
				   REAL offsetBlue, REAL offsetOpacity, MatrixOrder order /* = MatrixOrderPrepend */)
{
	QColorMatrix m;

	m.m[4][0] = offsetRed;
	m.m[4][1] = offsetGreen;
	m.m[4][2] = offsetBlue;
	m.m[4][3] = offsetOpacity;

	return Multiply(& m, order);
}

Status QColorMatrix::SetSaturation(REAL saturation, MatrixOrder order /* = MatrixOrderPrepend */)
{
	// For the theory behind this, see the web sites at the top of this file.
	// In short: if saturation is 1.0f, m becomes the identity matrix, and this matrix is
	// unchanged. If saturation is 0.0f, each color is scaled by it's luminance weight.
	REAL satCompl = 1.0f - saturation;
	REAL satComplR = lumR * satCompl;
	REAL satComplG = lumG * satCompl;
	REAL satComplB = lumB * satCompl;

	ColorMatrix m =
	{
		satComplR + saturation,	satComplR,	satComplR,	0.0f, 0.0f,
		satComplG,	satComplG + saturation,	satComplG,	0.0f, 0.0f,
		satComplB,	satComplB,	satComplB + saturation,	0.0f, 0.0f,
		0.0f,	0.0f,	0.0f,	1.0f,	0.0f,
		0.0f,	0.0f,	0.0f,	0.0f,	1.0f
	};
	return Multiply(& m, order);
}

Status QColorMatrix::Colorize(OLE_COLOR color, float intensity, MatrixOrder order)
{
	float r = GetRValue(color) / 255.0f / 3.0f * intensity;
	float g = GetGValue(color) / 255.0f / 3.0f * intensity;
	float b = GetBValue(color) / 255.0f / 3.0f * intensity;

	ColorMatrix m =
	{
		(1.0f - intensity) + r, g, b, 0.0f, 0.0f,
		r, (1.0f - intensity) + g, b, 0.0f, 0.0f,
		r, g, (1.0f - intensity) + g, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	return Multiply(&m, order);
}

Status QColorMatrix::SetGreyscale(MatrixOrder order)
{
	ColorMatrix m =
	{
		0.3f, 0.3f, 0.3f, 0.0f, 0.0f,
		0.59f, 0.59f, 0.59f, 0.0f, 0.0f,
		0.11f, 0.11f, 0.11f, 0.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f, 1.0f
	};

	return Multiply(&m, order);
}

Status QColorMatrix::RotateHue(REAL phi)
{
	InitHue();
	ASSERT(initialized);

	// Rotate the grey vector to the blue axis.
	Status s = Multiply(& preHue, MatrixOrderAppend);
//	if (s != Ok) return s;		// Can never happen

	// Rotate around the blue axis
	s = RotateBlue(phi, MatrixOrderAppend);
//	if (s != Ok) return s;

	return Multiply(& postHue, MatrixOrderAppend);
}

void QColorMatrix::Copy(const QColorMatrix& mat)
{
	::CopyMemory(m, mat.m, 25 * sizeof(REAL));
}

#ifdef _DEBUG
void QColorMatrix::Dump(CDumpContext& dc) const
{
	for (int y = 0; y < 5; y++)
	{
		for (int x = 0; x < 5; x++)
			dc << _T("\t") << m[y][x];
		dc << _T("\n");
	}
	dc << _T("\n");
}
#endif

/* static */
void QColorMatrix::InitHue()
{
	const REAL greenRotation = 35.0f;
//	const REAL greenRotation = 39.182655f;

	// NOTE: theoretically, greenRotation should have the value of 39.182655 degrees,
	// being the angle for which the sine is 1/(sqrt(3)), and the cosine is sqrt(2/3).
	// However, I found that using a slightly smaller angle works better.
	// In particular, the greys in the image are not visibly affected with the smaller
	// angle, while they deviate a little bit with the theoretical value.
	// An explanation escapes me for now.
	// If you rather stick with the theory, change the comments in the previous lines.


	if (! initialized)
	{
		initialized = true;
		// Rotating the hue of an image is a rather convoluted task, involving several matrix
		// multiplications. For efficiency, we prepare two static matrices.
		// This is by far the most complicated part of this class. For the background
		// theory, refer to the sgi-sites mentioned at the top of this file.

		// Prepare the preHue matrix.
		// Rotate the grey vector in the green plane.
		preHue.RotateRed(45.0f);

		// Next, rotate it again in the green plane, so it coincides with the blue axis.
		preHue.RotateGreen(- greenRotation, MatrixOrderAppend);

		// Hue rotations keep the color luminations constant, so that only the hues change
		// visible. To accomplish that, we shear the blue plane.
		REAL lum[4] = { lumR, lumG, lumB, 1.0f };

		// Transform the luminance vector.
		preHue.TransformVector(lum);

		// Calculate the shear factors for red and green.
		REAL red = lum[0] / lum[2];
		REAL green = lum[1] / lum[2];

		// Shear the blue plane.
		preHue.ShearBlue(red, green, MatrixOrderAppend);

		// Prepare the postHue matrix. This holds the opposite transformations of the
		// preHue matrix. In fact, postHue is the inversion of preHue.
		postHue.ShearBlue(- red, - green);
		postHue.RotateGreen(greenRotation, MatrixOrderAppend);
		postHue.RotateRed(- 45.0f, MatrixOrderAppend);
	}
}
