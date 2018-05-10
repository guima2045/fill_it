/**
 *
 * @file		PCOMP.h
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PCOMP card
 *
 */

#ifndef PCOMP_H
#define PCOMP_H

#include "MAT.h"
#include <map>
#include <vector>
#include <string>

class PCOMP
{
	public:
		PCOMP();

		// 	Constructor parsing bdf data
		PCOMP(std::vector<std::string> BDF_Data, bool LongFormatFlag); 

		~PCOMP();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		unsigned long getPropertyID();
		double getZOff();

		//	Input material map and parse ply layup
		void parse_Plies(std::map<unsigned long, MAT*> &MAT_Map);

		/*	Operator [] provides access to double data: i = 0/default -> E11, 1 -> E22, 2 -> NU12, 
			3 -> NU21, 4 -> G12, 5 -> t */ 
		double operator[] (unsigned int i);

	private:
		unsigned long pPropertyID;					///< Unsigned long, PCOMP propetry ID
		double pZOff;								///< Double, 1st ply location with respect to element plane
		unsigned short pLaminateValues[2];			///< Unsigned Short Integer, 0 -> layup type, 1 -> No. Plies
		std::vector<unsigned long> pMaterialID;		///< Unsigned Long Vector of Material ID for each ply
		std::vector<double> pThickness;				///< Double Vector of ply thickness
		std::vector<double> pOrientation;			///< Double Vector of ply orientation
		double A_Matrix[3][3];
		double B_Matrix[3][3];
		double D_Matrix[3][3];
		double Laminate_Stiffness[6];				///< Double array with PCOMP laminate stiffness properties
		std::string check_exp(std::string &str);
};

#endif // PCOMP_H
