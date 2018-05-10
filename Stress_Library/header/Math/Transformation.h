/**
 *
 * @file		Transformation.h
 * @author		Matt Guimarey
 * @date		30/05/2017
 * @version		1.0
 * 
 * This class defines some functionality for a transformation matrix
 *
 */

#ifndef TRANSFORMATION_H
#define TRANSFORMATION_H

#include "Math/Euclidean_Vector.h"
#include "Math/Coordinate.h"

class Transformation
{
	public:
		Transformation();
		~Transformation();

		/* 	Bulid a transformation matrix from three euclidean vectors that define x, y and 
			z axis of a coordinate system. */
		void build_matrix(Euclidean_Vector vector_x, Euclidean_Vector vector_y, Euclidean_Vector vector_z);
		
		/* 	Build a transformation matrix from two euclidean vectors. Vector one defines one 
			axis of a coordinate system, while vector two is used to generate the remaining axes 
			through cross products. The axis code determines which axes the two vectors represent
			Example: axis_code = 12 -> vector_one = x axis -> vector_two = vector on x-y plane */
		void build_matrix(Euclidean_Vector vector_one, Euclidean_Vector vector_two, unsigned int axis_code);
		
		/* 	Build a transformation matrix from three coordinate points. This follows the same 
			method as above for two euclidean vectors, with the difference being that the two 
			vectors are built from the coordinate points.
			coord_one -> coord_two = vector_one
			coord_one -> coord_three = vector_two */
		void build_matrix(Coordinate &coord_one, Coordinate &coord_two, Coordinate &coord_three, unsigned int axis_code);

		// 	Build a transformation from three euler rotation angles in radians
		void build_matrix(double alpha, double beta, double gamma);
	
		Transformation inverse_matrix();
		
		// 	Transform euclidean vector or coordinate
		Euclidean_Vector transform_vector(Euclidean_Vector &vector_a);
		Coordinate transform_coordinate(Coordinate &coord_a);

		/* 	Get a component from the transformation matrix -> a_i,j */
		double get_component(unsigned int i, unsigned int j);
	private:
		double pMatrix[3][3];
};

#endif // TRANSFORMATION_H
