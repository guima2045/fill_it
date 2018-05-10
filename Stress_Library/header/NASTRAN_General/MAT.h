/**
 *
 * @file		MAT.h
 * @author		Matt Guimarey
 * @date		08/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN MAT1 or MAT8 card
 *
 */

#ifndef MAT_H
#define MAT_H

#include <vector>
#include <string>

class MAT
{
	public:
		MAT();

		// 	Constructor parsing bdf data
		MAT(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~MAT();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		unsigned long getMaterialID();

		// 	Function return flag of material status (Isotropic or Orthotropic
		bool is_Isotropic();

		/*	Operator [] provides access to material property data: i = 0/default -> E11, 1 -> E22, 2 -> NU12, 
			3 -> G12, 4 -> G13, 5 -> G23, 6-> RHO, 7 -> A1, 8 -> A2 */	
		double operator[] (unsigned int i);	

	private:
		unsigned long pMaterialID;		///< Unsigned long, material ID
		double pDoubleData[9];			///< Double data array holding material properties
		unsigned short pMaterialType;	///< Unsigned short, material type 1 -> MAT1, 2 -> MAT8
		std::string check_exp(std::string &str);
};

#endif // MAT_H
