/**
 *
 * @file		MAT.cpp
 * @author		Matt Guimarey
 * @date		08/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN MAT1 or MAT8 card
 *
 */

#include "NASTRAN_General/MAT.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 

MAT::MAT()
{
	pMaterialType = 0;
}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

MAT::MAT(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	pMaterialType = 0;
	parseBDFData(BDF_Data, LongFormatFlag);
}

MAT::~MAT()
{

}

/**
 *	@brief	Parses Bulk Data File input lines to populate class data. Bulk Data 
			File has a short and a long format recognised by NASTRAN, the flag 
			modifies how the input data is parsed for these occurences.
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 *	@return	void
 */

void MAT::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store seperated line entries for parsing
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		LineData.push_back(BDF_Data[0].substr(0, 8));
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 8) {
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}
		}
		// MAT1 card will may have insufficient number of input lines, add extra data to ensure sufficient size in LineData
		if (BDF_Data.size() < 2) {
			for (int i = 0; i < 2; i++) {
				LineData.push_back("");
			}
		}
	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
		LineData.push_back(BDF_Data[0].substr(0, 8));
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_Data[i].substr(j, 16));
			}
		}
		// MAT1 card will may have insufficient number of input lines, add extra data to ensure sufficient size in LineData
		if (BDF_Data.size() < 3) {
			for (int i = 0; i < 2; i++) {
				LineData.push_back("");
			}
		}		
	}
	// Enter Material ID
	std::string str = check_exp(LineData[0]);
	pMaterialID = atol(LineData[1].c_str());
	if (str.compare("MAT1") == 0 || str.compare("MAT1*") == 0) {
		// Isotropic Material -> MAT1 Card
		pMaterialType = 1;
		pDoubleData[0] = pDoubleData[1] = atof(check_exp(LineData[2]).c_str());
		pDoubleData[2] = atof(check_exp(LineData[4]).c_str());
		str = check_exp(LineData[3]);
		pDoubleData[3] = atof(str.c_str());
		if (str.compare("") == 0) {
			pDoubleData[4] = pDoubleData[5] = -1.0;
		} else {
			pDoubleData[4] = pDoubleData[5] = atof(str.c_str());
		}
		pDoubleData[6] = atof(check_exp(LineData[5]).c_str());
		pDoubleData[7] = pDoubleData[8] = atof(check_exp(LineData[6]).c_str());
	} else if (str.compare("MAT8") == 0 || str.compare("MAT8*") == 0) {
		// Orthotropic Material
		pMaterialType = 2;
		for (int i = 0; i < 9; i++) {
			switch (i) {
				case 1: case 2:
					str = check_exp(LineData[i + 2]);
					if (str.compare("") == 0) {
						pDoubleData[i] = 0.001;
					} else {
						pDoubleData[i] = atof(str.c_str());
					}
					break;
				case 4: case 5:
					str = check_exp(LineData[i + 2]);
					if (str.compare("") == 0) {
						pDoubleData[i] = -1.0;
					} else {
						pDoubleData[i] = atof(str.c_str());
					}
					break;
				default:
					pDoubleData[i] = atof(check_exp(LineData[i + 2]).c_str());
			}
		}
		
	}
}

unsigned long MAT::getMaterialID()
{
	return pMaterialID;
}

bool MAT::is_Isotropic()
{
	if (pMaterialType == 1) {
		return true;
	} else {
		return false;
	}
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			E11, E22, NU12, G12, G13, G23, RHO, A1 and A2
 *
 *			i = 0 -> E11
 *			i = 1 -> E22
 *			i = 2 -> NU12
 *			i = 3 -> G12
 *			i = 4 -> G13
 *			i = 5 -> G23
 *			i = 6 -> RHO
 *			i = 7 -> A1
 *			i = 8 -> A2
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

double MAT::operator[] (unsigned int i)
{
	if (i < 9) {
		return pDoubleData[i];
	} else {
		return pDoubleData[0];
	}
}


std::string MAT::check_exp(std::string& str)
{
	std::string tempStr = str;
    std::size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos) {
		return "";
	} else {
		while (tempStr.find_first_of(' ') != std::string::npos) {
			tempStr.erase(tempStr.find_first_of(' '), 1);
		}
	}
    // Check for NASTRAN exponent
	std::size_t exp = tempStr.find_first_of('-', 1);
	if (exp == std::string::npos) {
		exp = tempStr.find_first_of('+', 1);
	}
	// Insert 'E' if exponent is found
	if (exp != std::string::npos) {
		std::size_t posEbig = tempStr.find_first_of('E');
		std::size_t posEsma = tempStr.find_first_of('e');
		if (posEbig == std::string::npos && posEsma == std::string::npos) {
			tempStr.insert(exp, "E");
		}
	}
	return tempStr;
}

