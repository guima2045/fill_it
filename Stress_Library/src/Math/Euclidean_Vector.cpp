/**
 *
 * @file		Euclidean_Vector.cpp
 * @author		Matt Guimarey
 * @date		29/05/2017
 * @version		1.0
 * 
 * This class defines some functionality for a euclidean vector
 *
 */

#include "Math/Euclidean_Vector.h"
#include <cmath>
#include <iostream>

Euclidean_Vector::Euclidean_Vector()
{
	set_vector(0.0, 0.0, 0.0);
}

/**
 *	@brief	Constructor also setting object properties (runs set_vector)
 *
 *	@param	i, a double representing the x component of the vector
 * 	@param 	j, a double representing the y component of the vector
 *	@param	k, a double representing the z component of the vector
 */

Euclidean_Vector::Euclidean_Vector(double i, double j, double k)
{
	set_vector(i, j, k);
}

Euclidean_Vector::~Euclidean_Vector()
{

}

/**
 *	@brief	Sets the variables of the vector (i, j, k) and then calculates the 
 *			magnitude of the vector, also checks if the magnitude is 1.0 and 
 * 			hence normalized (unit vector).
 *
 *	@param	i, a double representing the x component of the vector
 * 	@param 	j, a double representing the y component of the vector
 *	@param	k, a double representing the z component of the vector
 *	@return	void
 */

void Euclidean_Vector::set_vector(double i, double j, double k)
{
	// Set object variables
	pI = i;
	pJ = j;
	pK = k;
	pMagnitude = sqrt(pow(pI, 2) + pow(pJ, 2) + pow(pK, 2));
	// Check if normalized condition is true (Magnitude is equal to 1.0)
	if (pMagnitude == 1.0) {
		pNormalized = true;
	} else {
		pNormalized = false;
	}
}	

double Euclidean_Vector::get_magnitude()
{
	return pMagnitude;
}

double Euclidean_Vector::get_i()
{
	return pI;
}

double Euclidean_Vector::get_j()
{
	return pJ;
}

double Euclidean_Vector::get_k()
{
	return pK;
}

/**
 *	@brief	Calculates the dot product of this vector (object) and another vector, 
 * 			the dot product of two vectors is defined as:
 *			
 *			a.b = a_i * b_i + a_j * b_j + a_k * b_k
 *
 *	@param	vector_b, the Euclidean_Vector to be used in computing the dot product
 *	@return	double, the computed dot product
 */

double Euclidean_Vector::dot_product(Euclidean_Vector &vector_b)
{
	return pI * vector_b.get_i() + pJ * vector_b.get_j() + pK * vector_b.get_k();
}

/**
 *	@brief	Calculates the angle in radians between this vector (object) and another 
 *			vector, computation of the angle between two vectors is defined as:
 *
 *			a.b = ||a|| * ||b|| * cos(theta)
 *
 *	@param	vector_b, the Euclidean_Vector to be used in computing the angle
 *	@return	double, the computed angle in radians
 */

double Euclidean_Vector::get_angle(Euclidean_Vector &vector_b)
{
	double ab_ab = dot_product(vector_b) / (pMagnitude * vector_b.get_magnitude());
	if (std::isnan(ab_ab)) {
		// Nan if one of the vectors is zero length, just return 0 radians
		return 0.0;
	} else if (ab_ab > 1.0) {
		return acos(1.0);
	} else if (ab_ab < -1.0) {
		return acos(-1.0);
	} else {
		return acos(ab_ab);
	}
}

/**
 *	@brief	Calculates the vector cross product between this vector (vector_a) and another 
 *			vector (vector_b), this defines a new vector normal to the two vectors as per 
 *			the right hand rule.  The vector cross product is defined as:
 *			
 *			s = a x b
 *
 *			s_i = a_j * b_k - a_k * b_j
 *			s_j = a_k * b_i - a_i * b_k
 *			s_k = a_i * b_j - a_j * b_i
 *
 *	@param	vector_b, the Euclidean_Vector to be used in computing the cross product
 *	@return	Euclidean_Vector, the computed vector cross product
 */

Euclidean_Vector Euclidean_Vector::cross_product(Euclidean_Vector &vector_b)
{
	Euclidean_Vector returnVector(pJ * vector_b.get_k() - pK * vector_b.get_j(), pK * vector_b.get_i() - pI * vector_b.get_k(), pI * vector_b.get_j() - pJ * vector_b.get_i());
	return returnVector;
}

/**
 *	@brief	Projects this vector (vector_a) onto a plane defined by the normal 
 *			vector (vector_b), the project vector is calculated:
 *			
 *			ap = a - a1				where:
 *
 *			a1 = a.(b^) * b^ 		where: b^ is the unit vector of b
 *
 *	@param	vector_b, the Euclidean_Vector defining the normal of the plane
 *	@return	Euclidean_Vector, the computed projected vector
 */

Euclidean_Vector Euclidean_Vector::project_to_plane(Euclidean_Vector vector_b)
{
	// Normalize the normal vector defining the plane (make unit vector b^)
	vector_b.normalize();
	// Define vector a1 by multiplying vector b^ by the dot product of this and b^
	Euclidean_Vector a1 = vector_b * (dot_product(vector_b));
	// Return the projected vector (a - a1)
	return *this - a1;
}

std::pair<Euclidean_Vector, Euclidean_Vector> Euclidean_Vector::project_to_vector(Coordinate point_a)
{
	// Create vector to coordinate point
	Euclidean_Vector a(point_a[0], point_a[1], point_a[2]);
	Euclidean_Vector b = *this;
	b.normalize();
	Euclidean_Vector a1 = b * a.dot_product(b);
	Euclidean_Vector a2 = a - a1;
	return std::pair<Euclidean_Vector, Euclidean_Vector> (a1, a2);
}

/**
 *	@brief	Rotates this vector (vector_v) around a plane with normal defined by 
 *			vector_k. The vector is rotated anti-clockwise by theta radians. The 
 *			formula for rodrigues vector rotation is defined:
 *			
 *			vrot = v * cos(theta) + (k x v) * sin(theta) + k * (k . v) * (1 - cos(theta))
 *			
 *			where: vector 'k' is a unit vector
 *
 *	@param	vector_k, the Euclidean_Vector defining the normal of the plane
 *	@param	theta, the rotation angle in radians
 *	@return	Euclidean_Vector, the computed projected vector
 */

Euclidean_Vector Euclidean_Vector::rodrigues_rotation(Euclidean_Vector vector_k, double theta)
{
	// Normalize the normal vector
	vector_k.normalize();
	// Declare and assign the return vector
	Euclidean_Vector vrot = *this * cos(theta) + (vector_k.cross_product(*this) * sin(theta)) + 
						(vector_k * (vector_k.dot_product(*this)) * (1 - cos(theta)));
	return vrot;
}

/**
 *	@brief	Normalizes the vector creating a unit vector with a magnitude of 1.0, 
 *			also sets the normalized flag.
 *
 *	@return	void
 */

void Euclidean_Vector::normalize()
{
	if (!pNormalized) {
		pI /= pMagnitude;
		pJ /= pMagnitude;
		pK /= pMagnitude;
		pMagnitude = 1.0;
		pNormalized = true;
	}
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			pI, pJ, pK and pMagnitude.
 *
 *	@param	i, integer parameter: i = 0 -> pI, 1 -> pJ, 2 -> pK, 3 -> pMagnitude
 *	@return	double, returned class variable
 */

double Euclidean_Vector::operator[](int i)
{
	switch (i) {
		case 1:
			return pJ;
		case 2:
			return pK;
		case 3:
			return pMagnitude;
		default:
			return pI;
	}
}

/**
 *	@brief	Overload the * operator to allow multiplication of vector by a number, 
 *			this multiplies each component of the vector by the parameter and returns 
 *			a new Euclidean_Vector.
 *
 *	@param	x, double parameter to multply vector components
 *	@return	Euclidean_Vector, result of multiplication
 */

Euclidean_Vector Euclidean_Vector::operator*(double x)
{
	Euclidean_Vector returnVector(pI * x, pJ * x, pK * x);
	return returnVector;
}

/**
 *	@brief	Overload the / operator to allow division of vector by a number, 
 *			this divides each component of the vector by the parameter and returns 
 *			a new Euclidean_Vector.
 *
 *	@param	x, double parameter to divide vector components
 *	@return	Euclidean_Vector, result of division
 */

Euclidean_Vector Euclidean_Vector::operator/(double x)
{
	Euclidean_Vector returnVector(pI / x, pJ / x, pK / x);
	return returnVector;
}

/**
 *	@brief	Overload the - operator to subtract a Euclidean_Vector from this vector
 *
 *	@param	Euclidean_Vector, vector to subtract from this vector
 *	@return	Euclidean_Vector, result of subtraction
 */

Euclidean_Vector Euclidean_Vector::operator-(Euclidean_Vector vector_b)
{
	Euclidean_Vector returnVector(pI - vector_b[0], pJ - vector_b[1], pK - vector_b[2]);
	return returnVector;
}

/**
 *	@brief	Overload the + operator to add a Euclidean_Vector to this vector
 *
 *	@param	Euclidean_Vector, vector to add to this vector
 *	@return	Euclidean_Vector, result of addition
 */

Euclidean_Vector Euclidean_Vector::operator+(Euclidean_Vector vector_b)
{
	Euclidean_Vector returnVector(pI + vector_b[0], pJ + vector_b[1], pK + vector_b[2]);
	return returnVector;
}
