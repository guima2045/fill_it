/**
 *
 * @file		RBE2.cpp
 * @author		Matt Guimarey
 * @date		03/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN RBE2 element
 *
 */

#include "NASTRAN_General/RBE2.h"
#include "NASTRAN_General/GRID.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <map>

RBE2::RBE2()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

RBE2::RBE2(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	parseBDFData(BDF_Data, LongFormatFlag);
}

RBE2::~RBE2()
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

void RBE2::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
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
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}
		}	
	}
	// Parse long array data, RBE2_ID, Ind Node and DOF
	for (int i = 0; i < 3; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	for (int i = 3; i < LineData.size(); i++) {
		std::string str = check_exp(LineData[i]);
		if (str.compare("") != 0) {
			if (checkReal(str)) {
				pThermal = atof(str.c_str());
			} else {
				pDependantNodes.push_back(atol(str.c_str()));
			}
		}
	}
}

/**
 *	@brief	Returns the number of dependant nodes connecting to the RBE2 element.
 *
 *	@return	int, the number of dependant nodes
 */

int RBE2::get_num_dependant_nodes()
{
	return pDependantNodes.size();
}

/**
 *	@brief	Returns the thermal expansion cooefficient for the RBE2
 *
 *	@return	double, thermal expansion coefficient
 */

double RBE2::get_TEC()
{
	return pThermal;
}

/**
 *	@brief	Finds GRID entries in model for all RBE2 nodes and updates the GRID entry connectivity 
 *			array.
 *
 *	@param	GRID_Map, a map of all the GRIDs present in the model
 *	@return	void
 */

void RBE2::nodeConnect(std::map<unsigned long, GRID*> &GRID_Map)
{
	if (GRID_Map.find(pLongData[1]) != GRID_Map.end()) {
			GRID_Map[pLongData[1]]->addRBE2_Connect(pLongData[0]);
		}
	for (int i = 0; i < pDependantNodes.size(); i++) {
		if (GRID_Map.find(pDependantNodes[i]) != GRID_Map.end()) {
			GRID_Map[pDependantNodes[i]]->addRBE2_Connect(pLongData[0]);
		}
	}
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			RBE2 ID, Independant Node ID, DOF and the Dependant Nodes (IDs)
 *			RBE2 ID is returned as default.
 *
 *			i = 0 -> RBE2 ID
 *			i = 1 -> Independant Node ID
 *			i = 2 -> DOF
 *			i = 3... -> Dependant Nodes (IDs)
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long RBE2::operator[](unsigned int i)
{
	switch (i) {
		case 0: case 1: case 2:
			return pLongData[i];
		default:
			if (i < pDependantNodes.size() + 3) {
				return pDependantNodes[i - 3];
			} else {
				return pLongData[0];
			}
	}
}

std::string RBE2::check_exp(std::string& str)
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

bool RBE2::checkReal(std::string &str)
{
	std::size_t found = str.find_first_of('.');
	if (found == std::string::npos) {
		return false;
	} else {
		return true;
	}
}
