/**
 *
 * @file		Transformation.cpp
 * @author		Matt Guimarey
 * @date		30/05/2017
 * @version		1.0
 * 
 * This class defines some functionality for a transformation matrix
 *
 */

#include "Math/Transformation.h"
#include "Math/Euclidean_Vector.h"
#include "Math/Coordinate.h"

#include <iostream>

Transformation::Transformation()
{

}

Transformation::~Transformation()
{

}

/**
 *	@brief	Bulid a transformation matrix from three euclidean vectors that define 
 *			x, y and z axis of a coordinate system.
 *
 *	@param	vector_x, euclidean vector representing the x-axis of the transformation
 *			matrix
 *	@param	vector_y, euclidean vector representing the y-axis of the transformation
 *			matrix
 *	@param	vector_z, euclidean vector representing the z-axis of the transformation
 *			matrix
 *	@return	void
 */

void Transformation::build_matrix(Euclidean_Vector vector_x, Euclidean_Vector vector_y, Euclidean_Vector vector_z)
{
	// Normalize the three vectors into unit vectors
	vector_x.normalize();
	vector_y.normalize();
	vector_z.normalize();
	// Assign values to the transformation matrix
	for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_x[i];};
	for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_y[i];};
	for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_z[i];};
}

/**
 *	@brief	Build a transformation matrix from two euclidean vectors. Vector one defines 
 *			one axis of a coordinate system, while vector two is used to generate the 
 *			remaining axes through cross products. The axis code determines which axes the 
 *			two vectors represent. 
 *			
 *			Example: axis_code = 12 -> vector_one = x axis -> vector_two = vector on x-y plane 
 *
 *	@param	vector_one, euclidean vector representing an axis
 *	@param	vector_two, euclidean vector on the axis_code plane
 *	@param	axis_code, unsigned int defining the first axis and plane
 *	@return	void
 */

void Transformation::build_matrix(Euclidean_Vector vector_one, Euclidean_Vector vector_two, unsigned int axis_code)
{
	// Normalize vectors to create unit vectors - all cross products will be unit vectors too
	vector_one.normalize();
	vector_two.normalize();
	Euclidean_Vector vector_x;
	Euclidean_Vector vector_y;
	Euclidean_Vector vector_z;
	switch (axis_code) {
		case 12:
			// Enter x-axis
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_one[i];}
			// Enter z-axis
			vector_z = vector_one.cross_product(vector_two);
			vector_z.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_z[i];}
			// Enter y-axis
			vector_y = vector_z.cross_product(vector_one);
			vector_y.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_y[i];}
			break;

		case 13:
			// Enter x-axis
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_one[i];}
			// Enter y-axis
			vector_y = vector_two.cross_product(vector_one);
			vector_y.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_y[i];}
			// Enter z-axis
			vector_z = vector_one.cross_product(vector_y);
			vector_z.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_z[i];}
			break;

		case 21:
			// Enter y-axis
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_one[i];}
			// Enter z-axis
			vector_z = vector_two.cross_product(vector_one);
			vector_z.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_z[i];}
			// Enter x-axis
			vector_x = vector_one.cross_product(vector_z);
			vector_x.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_x[i];}
			break;

		case 23:
			// Enter y-axis
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_one[i];}	
			// Enter x-axis
			vector_x = vector_one.cross_product(vector_two);
			vector_x.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_x[i];}
			// Enter z-axis
			vector_z = vector_x.cross_product(vector_one);
			vector_z.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_z[i];}
			break;

		case 31:
			// Enter z-axis
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_one[i];}
			// Enter y-axis
			vector_y = vector_one.cross_product(vector_two);
			vector_y.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_y[i];}
			// Enter x-axis
			vector_x = vector_y.cross_product(vector_one);
			vector_x.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_x[i];}
			break;

		case 32: default:
			// Enter z-axis
			for (int i = 0; i < 3; i++) {pMatrix[2][i] = vector_one[i];}	
			// Enter x-axis
			vector_x = vector_two.cross_product(vector_one);
			vector_x.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[0][i] = vector_x[i];}
			// Enter y-axis
			vector_y = vector_one.cross_product(vector_x);
			vector_y.normalize();
			for (int i = 0; i < 3; i++) {pMatrix[1][i] = vector_y[i];}
	}
}

/**
 *	@brief	Build a transformation matrix from three coordinate points. This follows the same 
 *			method as above for two euclidean vectors, with the difference being that the two 
 *			vectors are built from the coordinate points.
 *
 *			coord_one -> coord_two = vector_one
 *			coord_one -> coord_three = vector_two
 *
 *	@param	coord_one, coordinate point representing the origin of two vectors
 *	@param	coord_two, coordinate point representing the head of vector one
 *	@param	coord_two, coordinate point representing the head of vector two
 *	@param	axis_code, unsigned int defining the first axis and plane
 *	@return	void
 */

void Transformation::build_matrix(Coordinate &coord_one, Coordinate &coord_two, Coordinate &coord_three, unsigned int axis_code)
{
	// Build the two vectors
	Euclidean_Vector vector_one(coord_two[0] - coord_one[0], coord_two[1] - coord_one[1], coord_two[2] - coord_one[2]);
	Euclidean_Vector vector_two(coord_three[0] - coord_one[0], coord_three[1] - coord_one[1], coord_three[2] - coord_one[2]);
	// Build the matrix
	build_matrix(vector_one, vector_two, axis_code);
}

/**
 *	@brief	Build a transformation matrix from three euler rotation angles.
 *
 *	@param	aplha, double representing rotation alpha in radians about z-axis
 *	@param	beta, double representing rotation beta in radians about x'-axis
 *	@param	gamma, double representing rotation alpha in radians about z''-axis
 *	@return	void
 */

void Transformation::build_matrix(double alpha, double beta, double gamma)
{
	// Create initial axis vectors
	Euclidean_Vector vector_x(1,0,0);
	Euclidean_Vector vector_y(0,1,0);
	Euclidean_Vector vector_z(0,0,1);
	// Rotate vector_x and vector_y by alpha about vector_z
	vector_x = vector_x.rodrigues_rotation(vector_z, alpha);
	vector_y = vector_y.rodrigues_rotation(vector_z, alpha);
	// Rotate vector_y and vector_z by beta about vector_x
	vector_y = vector_y.rodrigues_rotation(vector_x, beta);
	vector_z = vector_z.rodrigues_rotation(vector_x, beta);
	// Rotate vector_x and vector_y by gamma about vector_z
	vector_x = vector_x.rodrigues_rotation(vector_z, gamma);
	vector_y = vector_y.rodrigues_rotation(vector_z, gamma);
	// Build transformation matrix from vectors
	build_matrix(vector_x, vector_y, vector_z);
}

/**
 *	@brief	Perform a matrix inversion
 *
 *	| a b c |-1      1    | A D G |
 *	| d e f |   =  -----  | B E H |
 *	| g h i |      det(A) | C F I |
 *
 *	A = (ei - fh)	D = -(bi - ch)	G = (bf - ce)
 *	B = -(di - fg)	E = (ai - cg)	H = -(af - cd)
 *	C = (dh - eg)	F = -(ah - bg)	I = (ae - bd)
 *
 *	det(A) = a * A + b * B + c * C
 *
 *	@return	void
 */

Transformation Transformation::inverse_matrix()
{
	// Generate A, B, C, D, E, F, G, H and I coefficients
	double A = pMatrix[1][1] * pMatrix[2][2] - pMatrix[1][2] * pMatrix[2][1], B = pMatrix[1][2] * pMatrix[2][0] - pMatrix[1][0] * pMatrix[2][2],
			C = pMatrix[1][0] * pMatrix[2][1] - pMatrix[1][1] * pMatrix[2][1], D = pMatrix[0][2] * pMatrix[2][1] - pMatrix[0][1] * pMatrix[2][2],
			E = pMatrix[0][0] * pMatrix[2][2] - pMatrix[0][2] * pMatrix[2][0], F = pMatrix[0][1] * pMatrix[2][0] - pMatrix[0][0] * pMatrix[2][1],
			G = pMatrix[0][1] * pMatrix[1][2] - pMatrix[0][2] * pMatrix[1][1], H = pMatrix[0][2] * pMatrix[1][0] - pMatrix[0][0] * pMatrix[1][2],
			I = pMatrix[0][0] * pMatrix[1][1] - pMatrix[0][1] * pMatrix[1][0];
	// Calculate determinant
	double det = pMatrix[0][0] * A + pMatrix[0][1] * B + pMatrix[0][2] * C;
	// Create vectors for the inverse matrix
	Euclidean_Vector vector_x(A / det, D / det, G / det);
	Euclidean_Vector vector_y(B / det, E / det, H / det);
	Euclidean_Vector vector_z(C / det, F / det, I / det);
	// Create and build new matrix
	Transformation return_Transformation;
	return_Transformation.build_matrix(vector_x, vector_y, vector_z);
	return return_Transformation;
}

/**
 *	@brief	Transform a euclidean vector by matrix multiplication
 *
 *	@param	vector_a, Euclidean_Vector to transform
 *	@return	Euclidean_Vector, transformed vector
 */

Euclidean_Vector Transformation::transform_vector(Euclidean_Vector &vector_a)
{
	double x = 0, y = 0, z = 0;
	for (int i = 0; i < 3; i++) {x += vector_a[i] * pMatrix[0][i];};
	for (int i = 0; i < 3; i++) {y += vector_a[i] * pMatrix[1][i];};
	for (int i = 0; i < 3; i++) {z += vector_a[i] * pMatrix[2][i];};
	Euclidean_Vector return_Vector(x, y, z);
	return return_Vector;
}

/**
 *	@brief	Transform a coordinate by matrix multiplication
 *
 *	@param	coord_a, Coordinate to transform
 *	@return	Coordinate, transformed coordinate
 */

Coordinate Transformation::transform_coordinate(Coordinate &coord_a)
{
	double x = 0, y = 0, z = 0;
	for (int i = 0; i < 3; i++) {x += coord_a[i] * pMatrix[0][i];};
	for (int i = 0; i < 3; i++) {y += coord_a[i] * pMatrix[1][i];};
	for (int i = 0; i < 3; i++) {z += coord_a[i] * pMatrix[2][i];};
	Coordinate return_Coord(x, y, z);
	return return_Coord;
}

/**
 *	@brief	Function provides access to the components of the transformation matrix.
 *
 *			a_i,j
 *
 *	@param	i, integer parameter designating row position
 *	@param	j, integer parameter designating column position
 *	@return	unsigned long, returned class variable
 */

double Transformation::get_component(unsigned int i, unsigned int j)
{
	if (i < 3 && j < 3) {
		return pMatrix[i][j];
	} else {
		return pMatrix[0][0];
	}
}

