/**
 *
 * @file		Euclidean_Vector.h
 * @author		Matt Guimarey
 * @date		29/05/2017
 * @version		1.0
 * 
 * This class defines some functionality for a euclidean vector
 *
 */

#ifndef EUCLIDEAN_VECTOR_H
#define EUCLIDEAN_VECTOR_H

#include "Math/Coordinate.h"
#include <vector>

class Euclidean_Vector
{
	public:
		Euclidean_Vector();

		// This constructor sets the vector dimensions of i, j and k
		Euclidean_Vector(double i, double j, double k);

		~Euclidean_Vector();
		void set_vector(double i, double j, double k);

		// Functions returning vector properties
		double get_magnitude();
		double get_i();
		double get_j();
		double get_k();

		/* Returns the dot product between this vector and vector_b 
			-> https://en.wikipedia.org/wiki/Dot_product */
		double dot_product(Euclidean_Vector &vector_b);

		// Returns angle between this vector and vector_b in radians
		double get_angle(Euclidean_Vector &vector_b);

		/* Returns the vector cross product of this vector and vector_b 
			-> https://en.wikipedia.org/wiki/Cross_product */
		Euclidean_Vector cross_product(Euclidean_Vector &vector_b);

		/* Projects this vector onto a plane whose defined by vector_b
			-> https://en.wikipedia.org/wiki/Vector_projection */
		Euclidean_Vector project_to_plane(Euclidean_Vector vector_b);
		
		/* Projects a coordinate point onto a vector in space returning a 
			vector pair a1 and a2
			-> https://en.wikipedia.org/wiki/Vector_projection 2nd Figure */
		
		std::pair<Euclidean_Vector, Euclidean_Vector> project_to_vector(Coordinate point_a);

		/* Performs a rodrigues rotation of this vector by theta (radians) around the normal of a plane defined by vector_k 
			-> https://en.wikipedia.org/wiki/Rodrigues%27_rotation_formula */
		Euclidean_Vector rodrigues_rotation(Euclidean_Vector vector_k, double theta);
	
		// Divides all components of the vector by the magnitude hence setting the vector magnitude to 1.0
		void normalize();

		// Operator [] can directly access components of the vector 0 = i, 1 = j, 2 = k and 3 = Magnitude
		double operator[](int i);

		// Operator * multiplies all components of the vector by x
		Euclidean_Vector operator*(double x);

		// Operator / divides all components of the vector by x
		Euclidean_Vector operator/(double x);

		// Operator - subtracts a vector from this one
		Euclidean_Vector operator-(Euclidean_Vector vector_b);

		// Operator + adds a vector to this one
		Euclidean_Vector operator+(Euclidean_Vector vector_b);

	private:
		double pI, pJ, pK, pMagnitude;	///< Doubles hold vector data i, j, k and magnitude
		bool pNormalized;				///< Bool variable flags whether vector has been normalized and the magnitude is 1.0
};

#endif // EUCLIDEAN_VECTOR_H
