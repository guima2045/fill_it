/**
 *
 * @file		PSHELL.cpp
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PSHELL card
 *
 */

#include "NASTRAN_General/PSHELL.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 

PSHELL::PSHELL()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

PSHELL::PSHELL(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	parseBDFData(BDF_Data, LongFormatFlag);
}

PSHELL::~PSHELL()
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

void PSHELL::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store seperated line entries for parsing
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		for (int i = 8; i < 72; i += 8) {
			LineData.push_back(BDF_Data[0].substr(i, 8));
		}

	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
        for (int i = 0; i < BDF_Data.size(); i++) {
            for (int j = 8; j < 72; j += 16) {
                LineData.push_back(BDF_Data[i].substr(j, 16));
            }
        }
	}
	// Parse Longs
	for (int i = 0; i < 2; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	std::string str = check_exp(LineData[3]);
	if (str.compare("") == 0) {
		pLongData[2] = pLongData[1];
	} else {
		pLongData[2] = atol(str.c_str());
	}
	str = check_exp(LineData[5]);
	if (str.compare("") == 0) {
		pLongData[3] = pLongData[1];
	} else {
		pLongData[3] = atol(str.c_str());
	}
	// Parse doubles
	pDoubleData[0] = atof(check_exp(LineData[2]).c_str());
	str = check_exp(LineData[4]);
	if (str.compare("") == 0) {
		pDoubleData[1] = 1.0;
	} else {
		pDoubleData[1] = atof(str.c_str());
	}
	str = check_exp(LineData[6]);
	if (str.compare("") == 0) {
		pDoubleData[2] = 0.833333;
	} else {
		pDoubleData[2] = atof(str.c_str());
	}	
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables. Property ID 
 *			is returned as default.
 *
 *			i = 0 -> Property ID
 *			i = 1 -> Material 1 ID
 *			i = 2 -> Material 2 ID
 *			i = 3 -> Material 3 ID
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long PSHELL::operator[] (unsigned int i)
{
	if (i < 4) {
		return pLongData[i];
	} else {
		return pLongData[0];
	}
}

/**
 *	@brief	Overload the () operator to provide direct access to the class variables. Thickness is returned 
 *			as default.
 *
 *			i = 0 -> Thickness
 *			i = 1 -> Inertia Ratio
 *			i = 2 -> Shear Thickness Ratio
 *
 *	@param	i, integer parameter designating position
 *	@return	double, returned class variable
 */

double PSHELL::operator() (unsigned int i)
{
	if (i < 3) {
		return pDoubleData[i];
	} else {
		return pDoubleData[0];
	}
}


std::string PSHELL::check_exp(std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    std::size_t last = str.find_last_not_of(' ');
	std::string tempStr = str.substr(first, (last-first+1));
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
