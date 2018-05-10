/**
 *
 * @file		Coordinate.cpp
 * @author		Matt Guimarey
 * @date		29/05/2017
 * @version		1.0
 * 
 * This class defines a coordinate point
 *
 */

#include "Math/Coordinate.h"
#include <cmath>


Coordinate::Coordinate()
{
	set_coordinates(0.0, 0.0, 0.0);
}

Coordinate::Coordinate(double x, double y, double z)
{
	set_coordinates(x, y, z);
}

Coordinate::~Coordinate()
{

}

void Coordinate::set_coordinates(double x, double y, double z)
{
	pCoords[0] = x;
	pCoords[1] = y;
	pCoords[2] = z;
}

void Coordinate::set_reference(unsigned long ref)
{
	pRef = ref;
}

double Coordinate::get_x()
{
	return pCoords[0];
}

double Coordinate::get_y()
{
	return pCoords[1];
}

double Coordinate::get_z()
{
	return pCoords[2];
}

double Coordinate::get_resultant(int axis)
{
	switch (axis) {
		case 12: case 21:
			return sqrt(pow(pCoords[0], 2) + pow(pCoords[1], 2));
		case 13: case 31:
			return sqrt(pow(pCoords[0], 2) + pow(pCoords[2], 2));
		case 23: case 32:
			return sqrt(pow(pCoords[1], 2) + pow(pCoords[2], 2));
		default:
			return sqrt(pow(pCoords[0], 2) + pow(pCoords[1], 2) + pow(pCoords[2], 2));
	}
}

unsigned long Coordinate::get_reference_frame()
{
	return pRef;
}

double Coordinate::operator[](int i)
{
	if (i >= 0 && i <= 2) {
		return pCoords[i];
	} else {
		return pCoords[0];
	}
}

Coordinate Coordinate::operator+(Coordinate coord)
{
	double x = pCoords[0] + coord[0], y = pCoords[1] + coord[1], z = pCoords[2] + coord[2];
	Coordinate returnCoord(x, y, z);
	return returnCoord;
}

Coordinate Coordinate::operator-(Coordinate coord)
{
	double x = pCoords[0] - coord[0], y = pCoords[1] - coord[1], z = pCoords[2] - coord[2];
	Coordinate returnCoord(x, y, z);
	return returnCoord;
}

void Coordinate::operator+=(Coordinate coord)
{
	pCoords[0] += coord[0];
	pCoords[1] += coord[1];
	pCoords[2] += coord[2];
}

void Coordinate::operator-=(Coordinate coord)
{
	pCoords[0] -= coord[0];
	pCoords[1] -= coord[1];
	pCoords[2] -= coord[2];
}

Coordinate Coordinate::operator/(double num)
{
	double x = pCoords[0] / num, y = pCoords[1] / num, z = pCoords[2] / num;
	Coordinate returnCoord(x, y, z);
	return returnCoord;
}

Coordinate Coordinate::operator*(double num)
{
	double x = pCoords[0] * num, y = pCoords[1] * num, z = pCoords[2] * num;
	Coordinate returnCoord(x, y, z);
	return returnCoord;
}

