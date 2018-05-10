/**
 *
 * @file		Coordinate.h
 * @author		Matt Guimarey
 * @date		29/05/2017
 * @version		1.0
 * 
 * This class defines a coordinate point
 *
 */

#ifndef COORDINATE_H
#define COORDINATE_H

class Coordinate
{
	public:
		Coordinate();

		// Constructor setting initial coordinate values
		Coordinate(double x, double y, double z);

		~Coordinate();
		void set_coordinates(double x, double y, double z);

		// Sets a reference ID that represents a coordinate frame that the coordinates are defined in
		void set_reference(unsigned long ref);

		// Functions returning coordinate properties
		double get_x();
		double get_y();
		double get_z();
		double get_resultant(int axis);
		unsigned long get_reference_frame();

		// Operator [] directly accesses the coordinate points i = 0 -> x, 1 -> y, 2 -> z		
		double operator[](int i);

		// Operator + and - adds or subtracts two coordiante points together
		Coordinate operator+(Coordinate coord);
		Coordinate operator-(Coordinate coord);
		
		// Operator += and -= adds or subtracts a coordinate to this one
		void operator+=(Coordinate coord);
		void operator-=(Coordinate coord);

		// Operator / and * divides or multiplies coordinate point by a number
		Coordinate operator/(double num);
		Coordinate operator*(double num);

	private:
		double pCoords[3];		///< double array holding the coordinate points
		unsigned long pRef;		///< unsigned long holding the ID of a reference coordinate frame
};

#endif // COORDINATE_H
