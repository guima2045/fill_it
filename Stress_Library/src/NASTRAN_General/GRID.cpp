/**
 *
 * @file		GRID.cpp
 * @author		Matt Guimarey
 * @date		01/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Grid point entry
 *
 */

#include "NASTRAN_General/GRID.h"
#include "Math/Coordinate.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h>
#include <map>

GRID::GRID()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_data, a string vector containing Bulk Data File input lines
 * 	@param 	long_format_flag, a boolean flagging format type
 */

GRID::GRID(std::vector<std::string> BDF_data, bool long_format_flag){
	parse_BDF_data(BDF_data, long_format_flag);
}

GRID::GRID(unsigned long grid_ID)
{
	pLongData[0] = grid_ID;
}

GRID::~GRID()
{

}

/**
 *	@brief	Parses Bulk Data File input lines to populate class data. Bulk Data 
			File has a short and a long format recognised by NASTRAN, the flag 
			modifies how the input data is parsed for these occurences.
 *
 *	@param	BDF_data, a string vector containing Bulk Data File input lines
 * 	@param 	long_format_flag, a boolean flagging format type
 *	@return	void
 */

void GRID::parse_BDF_data(std::vector<std::string> BDF_data, bool long_format_flag)
{
	// Initiate a string vector to store split data
	std::vector<std::string> LineData;
	if (!long_format_flag) {
		// Short Format, data fields every 8 characters (80 chars max)
		for (int i = 8; i < 72; i += 8) {
			LineData.push_back(BDF_data[0].substr(i, 8));
		}

	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
		for (int i = 0; i < BDF_data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_data[i].substr(j, 16));
			}
		}
	}
	// Parse Grid ID and Ref Coordinate ID
	for (int i = 0; i < 2; i++) { pLongData[i] = atol(LineData[i].c_str());	}
	// Parse grid coordinates
	double coords[3];
	for (int i = 0; i < 3; i++) { coords[i] = atof(check_exp(LineData[i + 2]).c_str());	}
	pCoordinate.set_coordinates(coords[0], coords[1], coords[2]);
	// Parse Analysis Coordinate ID
	pLongData[2] = atol(LineData[5].c_str());
}

/**
 *	@brief	Returns a coordinate point that is translated into a specific coordinate 
 *			system given a complete list of all coordinate systems present in the model.
 *
 *	@param	COORD_ID, an unsigned long defining the ID of the return coordinate system
 * 	@param 	COORD_Map, a map of all coordinate systems present in the model
 *	@return	Coordinate, the coordinate point returned
 */


Coordinate GRID::get_coordinate(unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map)
{
	//	Transform coordinate point to BASIC (0)
	Coordinate returnCoordinate = pCoordinate;
	unsigned long refCoord = pLongData[1];
	while (refCoord != 0) {
		if (COORD_Map.find(refCoord) != COORD_Map.end()) {
			refCoord = (*COORD_Map[refCoord])[1];
			//	Translate coordinate back to reference Coordinate System
			returnCoordinate = COORD_Map[refCoord]->transform_coordinate(returnCoordinate, true);
			// If the ref Coord matches the target COORD_ID, return the coordinate point
			if (refCoord == COORD_ID) {return returnCoordinate;}
		} else {
			//	Set refCoord to 0 to break loop
			refCoord = 0;
		}
	}
	/*	If target coord_ID has not been reached, the coordinate point is in BASIC coordinate 
		system. Determine the order of translations by working backwards from the target COORD_ID */
	std::vector<unsigned long> coordinate_frames;
	if (COORD_Map.find(COORD_ID) != COORD_Map.end()) {
		coordinate_frames.push_back(COORD_ID);
		refCoord = (*COORD_Map[COORD_ID])[1];
		// Iterate through any remaining coordinate systems
		while (refCoord != 0) {
			if (COORD_Map.find(refCoord) != COORD_Map.end()) {
				coordinate_frames.push_back(refCoord);
				refCoord = (*COORD_Map[COORD_ID])[1];
			} else {
				refCoord = 0;
			}
		}
	}
	// Transform returnCoordinate to the target COORD_ID
	for (int i = coordinate_frames.size() - 1; i > -1; i--) {
		returnCoordinate = COORD_Map[coordinate_frames[i]]->transform_coordinate(returnCoordinate, false);
	}
	return returnCoordinate;
}

/**
 *	@brief	Adds the element ID to the vector of the list of CQUAD elements connected to this GRID
 *
 *	@param	ElementID, an unsigned long of the element ID to add
 *	@return	bool, returns true if already entered
 */

bool GRID::addCQUAD_Connect(unsigned long ElementID)
{
	for (int i = 0; i < pCQUAD_List.size(); i++) {
		if (pCQUAD_List[i] == ElementID) {
			return true;
		}
	}
	pCQUAD_List.push_back(ElementID);
	return false;
}

/**
 *	@brief	Adds the element ID to the vector of the list of CBUSH elements connected to this GRID
 *
 *	@param	ElementID, an unsigned long of the element ID to add
 *	@return	bool, returns true if already entered
 */


bool GRID::addCBUSH_Connect(unsigned long ElementID)
{
	for (int i = 0; i < pCBUSH_List.size(); i++) {
		if (pCBUSH_List[i] == ElementID) {
			return false;
		}
	}
	pCBUSH_List.push_back(ElementID);
	return true;
}

/**
 *	@brief	Adds the element ID to the vector of the list of RBE2 elements connected to this GRID
 *
 *	@param	ElementID, an unsigned long of the element ID to add
 *	@return	bool, returns true if already entered
 */

bool GRID::addRBE2_Connect(unsigned long ElementID)
{
	for (int i = 0; i < pRBE2_List.size(); i++) {
		if (pRBE2_List[i] == ElementID) {
			return false;
		}
	}
	pRBE2_List.push_back(ElementID);
	return true;
}

/**
 *	@brief	Adds the element ID to the vector of the list of RBE3 elements connected to this GRID
 *
 *	@param	ElementID, an unsigned long of the element ID to add
 *	@return	bool, returns true if already entered
 */

bool GRID::addRBE3_Connect(unsigned long ElementID)
{
	for (int i = 0; i < pRBE3_List.size(); i++) {
		if (pRBE3_List[i] == ElementID) {
			return false;
		}
	}
	pRBE3_List.push_back(ElementID);
	return true;
}

std::vector<unsigned long> GRID::getRBE2_List()
{
	return pRBE2_List;
}

std::vector<unsigned long> GRID::getRBE3_List()
{
	return pRBE3_List;
}

std::vector<unsigned long> GRID::getCQUAD_List()
{
	return pCQUAD_List;
}

/*
std::vector<double> GRID::getGRID_Doubles_Basic(std::map<unsigned long, COORD*> &COORD_Map)
{
	std::vector<double> returnVector;
	for (int i = 0; i < 3; i++) {
		returnVector.push_back(pDoubleData[i]);
	}
	if (pLongData[1] == 0) {
		// GRID coordinates in BASIC coordinate system
		return returnVector;
	} else {
		// GRID coordinates not in BASIC coordinate system
		unsigned long currentRefID = pLongData[1];
		while (currentRefID != 0) {
			if (COORD_Map.find(currentRefID) != COORD_Map.end()) {
				returnVector = COORD_Map[currentRefID]->inverse_transform(returnVector);
				std::vector<unsigned long> tempVector = COORD_Map[currentRefID]->getCOORD_Longs();
				currentRefID = tempVector[1];
			} else {
				currentRefID = 0;
			}
		}
		return returnVector;
	}
}


void GRID::addGPForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID)
{
	std::vector<std::string> LineData;
	for (int j = 0; j < lineEntries.size(); j++) {
		for (int i = 18; i < 72; i += 18) {
			LineData.push_back(lineEntries[j].substr(i, 18));
		}
	}
	std::pair<std::pair<std::vector<double>, std::string> , unsigned long> GPF_Result;
	try {
		GPF_Result.second = atol(trim(LineData[0]).c_str());
	} catch (int e) {
		GPF_Result.second = 0;
	}
	GPF_Result.first.second = trim(LineData[1]);
	for (int i = 3; i < 9; i++) {
		try {
			GPF_Result.first.first.push_back(atof(trim(LineData[i]).c_str()));
		} catch (int e) {
			GPF_Result.first.first.push_back(0.0);
		}
	}
	std::map<unsigned long, std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > >::iterator GPF_it = GPF_Map.find(SubcaseID);
	if (GPF_it == GPF_Map.end()) {
		std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > GPF_Result_Vector;
		GPF_Result_Vector.push_back(GPF_Result);
		GPF_Map.insert(std::make_pair<unsigned long, std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > > (SubcaseID, GPF_Result_Vector));
	} else {
		std::pair<std::pair<std::vector<double>, std::string> , unsigned long> GPF_Check = GPF_Map[SubcaseID][GPF_Map[SubcaseID].size() - 1];
		if (GPF_Check.first.second.compare("*TOTALS*") != 0) {
			GPF_Map[SubcaseID].push_back(GPF_Result);
		}
	}
}

std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > GRID::getForceResult(unsigned long SubcaseID)
{
	if (GPF_Map.find(SubcaseID) != GPF_Map.end()) {
		return GPF_Map[SubcaseID];
	} else {
		std::vector<double> dummyVector;
		std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > returnVector;
		returnVector.push_back(std::make_pair<std::pair<std::vector<double>, std::string> , unsigned long> (std::make_pair<std::vector<double>, std::string> (dummyVector, "ERROR") , 0));
		return returnVector;
	}
}
*/

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			Grid ID, Reference Coordinate System ID and Analysis Coordinate System ID.
 *			Grid ID is returned as default.
 *
 *			i = 0 -> Grid ID
 *			i = 1 -> Reference Coordinate System ID
 *			i = 2 -> Analysis Coordinate System ID
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long GRID::operator[](int i)
{
	// Switch the integer input and return Grid ID as default
	switch (i) {
		case 1: case 2:
			return pLongData[i];
		default:
			return pLongData[0];
	}
}

/**
 *	@brief	Overload the () operator to provide direct access to the coordinate
 *			point values. X is returned as default.
 *
 *			i = 0 -> x
 *			i = 1 -> y
 *			i = 2 -> z
 *
 *	@param	i, integer parameter designating position
 *	@return	double, returned coordinate point value
 */

double GRID::operator()(int i)
{
	// Switch the integer input and return 'x' as default
	switch (i) {
		case 1: case 2:
			return pCoordinate[i];
		default:
			return pCoordinate[0];
	}
}

std::string GRID::check_exp(std::string &str)
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
