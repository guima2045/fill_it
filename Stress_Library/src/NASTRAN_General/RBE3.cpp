/**
 *
 * @file		RBE3.cpp
 * @author		Matt Guimarey
 * @date		20/09/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN RBE3 element as used by Airbus in fastener modelling
 *
 */

#include "NASTRAN_General/RBE3.h"
#include "NASTRAN_General/GRID.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <map>

RBE3::RBE3()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

RBE3::RBE3(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	parseBDFData(BDF_Data, LongFormatFlag);
}

RBE3::~RBE3()
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

void RBE3::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store separated line entries for parsing
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 8) {
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}
		}
	} else {
		// Long Format, data fields every 16 characters after initial 8 characters (80 chars max)
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}
		}	
	}
	// Parse long array data, RBE3_ID, REFGRID and REFC
	pLongData[0] = atol(LineData[0].c_str());
	for (int i = 2; i < 4; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	// Parse independent nodes
	bool um = false, CTE = false, DOF_flag = false;
	double weight_factor = 0.0;
	unsigned long DOF = 0;
	for (int i = 4; i < LineData.size(); i++) {
		std::string str = check_exp(LineData[i]);
		if (CTE) {
			pThermal = atof(str.c_str());
			CTE = false;
			continue;
		}
		if (str.compare("ALPHA") == 0) {
			CTE = true;
			um = false;
			DOF_flag = false;
			continue;
		}
		if (str.compare("") == 0) {
			CTE = false;
			um = false;
			DOF_flag = false;
			continue;
		}		
		if (um) {
			if (DOF_flag) {
				DOF_flag = false;
				pDOFs.push_back(atol(str.c_str()));
				continue;
			} else {
				DOF_flag = true;
				pIndependantNodes.push_back(atol(str.c_str()));
				continue;
			}
		}
		if (str.compare("UM") == 0) {
			um = true;
			continue;
		}
		if (str.compare("") == 0) {
			um = DOF_flag = CTE = false;
			continue;
		}
		if (checkReal(str)) {
			pWeightingFactors.push_back(atof(str.c_str()));
			DOF_flag = true;
			continue;
		} else {
			if (DOF_flag) {
				pDOFs.push_back(atol(str.c_str()));
				DOF_flag = false;
			} else {
				pIndependantNodes.push_back(atol(str.c_str()));
				continue;				
			}

		}
	}
}

/**
 *	@brief	Returns the number of dependant nodes connecting to the RBE3 element.
 *
 *	@return	int, the number of dependant nodes
 */

int RBE3::get_num_independant_nodes()
{
	return pIndependantNodes.size();
}

/**
 *	@brief	Returns the thermal expansion cooefficient for the RBE3
 *
 *	@return	double, thermal expansion coefficient
 */

double RBE3::get_TEC()
{
	return pThermal;
}

/**
 *	@brief	Finds GRID entries in model for all RBE3 nodes and updates the GRID entry connectivity 
 *			array.
 *
 *	@param	GRID_Map, a map of all the GRIDs present in the model
 *	@return	void
 */

void RBE3::nodeConnect(std::map<unsigned long, GRID*> &GRID_Map)
{
	if (GRID_Map.find(pLongData[1]) != GRID_Map.end()) {
			GRID_Map[pLongData[1]]->addRBE3_Connect(pLongData[0]);
		}
	for (int i = 0; i < pIndependantNodes.size(); i++) {
		if (GRID_Map.find(pIndependantNodes[i]) != GRID_Map.end()) {
			GRID_Map[pIndependantNodes[i]]->addRBE3_Connect(pLongData[0]);
		}
	}
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			RBE3 ID, Independant Node ID, DOF and the Dependant Nodes (IDs)
 *			RBE3 ID is returned as default.
 *
 *			i = 0 -> RBE3 ID
 *			i = 1 -> Independant Node ID
 *			i = 2 -> DOF
 *			i = 3... -> Dependant Nodes (IDs)
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long RBE3::operator[](unsigned int i)
{
	switch (i) {
		case 0: case 1: case 2:
			return pLongData[i];
		default:
			if (i < pIndependantNodes.size() + 3) {
				return pIndependantNodes[i - 3];
			} else {
				return pLongData[0];
			}
	}
}

std::string RBE3::check_exp(std::string& str)
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

bool RBE3::checkReal(std::string &str)
{
	std::size_t found = str.find_first_of('.');
	if (found == std::string::npos) {
		return false;
	} else {
		return true;
	}
}
