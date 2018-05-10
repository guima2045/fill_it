/**
 *
 * @file		PBUSH.cpp
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PBUSH card
 *
 */

#include "NASTRAN_General/PBUSH.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 

PBUSH::PBUSH()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

PBUSH::PBUSH(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	parseBDFData(BDF_Data, LongFormatFlag);
}

PBUSH::~PBUSH()
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

void PBUSH::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store seperated line entries for parsing
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 8) {
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}		
		}
	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_Data[i].substr(j, 16));
			}		
		}
	}
	// Parse property ID
	pLongData[0] = atol(LineData[0].c_str());
	// Parse K Values
	for (int i = 1; i < LineData.size(); i += 8) {
		if (check_exp(LineData[i]).compare("K") == 0) {
			for (int j = 0; j < 6; j++) {
				pDoubleData[j] = atof(check_exp(LineData[i + j + 1]).c_str());
			}
		}
	}
}

unsigned long PBUSH::getPropertyID()
{
	return pLongData[0];
}

/**
 *	@brief	Overload the () operator to provide direct access to the class variables. K1 is returned 
 *			as default.
 *
 *			i = 0 -> K1
 *			i = 1 -> K2
 *			i = 2 -> K3
 *			i = 3 -> K4
 *			i = 4 -> K5
 *			i = 5 -> K6
 *
 *	@param	i, integer parameter designating position
 *	@return	double, returned class variable
 */

double PBUSH::operator[] (unsigned int i)
{
	if (i < 6) {
		return pDoubleData[i];
	} else {
		return pDoubleData[0];
	}
}

std::string PBUSH::check_exp(std::string& str)
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
