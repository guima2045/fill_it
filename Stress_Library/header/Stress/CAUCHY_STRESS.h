/**
 *
 * @file		CAUCHY_STRESS.h
 * @author		Matt Guimarey
 * @date		20/06/2017
 * @version		1.0
 * 
 * This class defines a Cauchy Stress Tensor
 *
 */

#ifndef CAUCHY_STRESS_H
#define CAUCHY_STRESS_H

class CAUCHY_STRESS
{
	public:
		CAUCHY_STRESS();

		// 	Constructor setting stress values
		CAUCHY_STRESS(double fx, double fy, double fxy);

		~CAUCHY_STRESS();

		//	Function sets stress values
		void set_values(double fx, double fy, double fxy);

		//	Applies a rotation of theta radians to the cauchy stress tensor, rotated values accessible by []
		void rotate(double theta);

		/* 	Operator [] directly accesses element double data: i = 0/default -> fx, 1 -> fy, 2 -> fxy, 3 -> 
			theta, 4 -> fx (theta), 5 -> fy (theta), 6 -> fxy (theta) */
		double operator[] (unsigned int i);
	
	private:
		double pDoubleData[7];
};


#endif // CAUCHY_STRESS_H
