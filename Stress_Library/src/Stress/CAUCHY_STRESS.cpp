/**
 *
 * @file		CAUCHY_STRESS.cpp
 * @author		Matt Guimarey
 * @date		20/06/2017
 * @version		1.0
 * 
 * This class defines a Cauchy Stress Tensor
 *
 */

#include "Stress/CAUCHY_STRESS.h"
#include <math.h> 

#define PI 3.14159265

CAUCHY_STRESS::CAUCHY_STRESS()
{

}

CAUCHY_STRESS::CAUCHY_STRESS(double fx, double fy, double fxy)
{
	set_values(fx, fy, fxy);
}

CAUCHY_STRESS::~CAUCHY_STRESS()
{

}

void CAUCHY_STRESS::set_values(double fx, double fy, double fxy)
{
	pDoubleData[0] = pDoubleData[4] = fx;
	pDoubleData[1] = pDoubleData[5] = fy;
	pDoubleData[2] = pDoubleData[6] = fxy;
	pDoubleData[3] = 0.0;
}

void CAUCHY_STRESS::rotate(double theta)
{
	pDoubleData[3] = theta;
	pDoubleData[4] = 0.5 * (pDoubleData[0] + pDoubleData[1]) + 0.5 * (pDoubleData[0] - pDoubleData[1]) * cos(2 * theta) + pDoubleData[2] * sin(2 * theta);
	pDoubleData[5] = 0.5 * (pDoubleData[0] + pDoubleData[1]) + 0.5 * (pDoubleData[0] - pDoubleData[1]) * cos(2 * (theta + PI / 2)) + pDoubleData[2] * sin(2 * (theta + PI / 2));
	pDoubleData[6] = -0.5 * (pDoubleData[0] - pDoubleData[1]) * sin(2 * theta) + pDoubleData[2] * cos(2 * theta);
}

double CAUCHY_STRESS::operator[] (unsigned int i)
{
	if (i < 7) {
		return pDoubleData[i];
	} else {
		return pDoubleData[0];
	}
}
