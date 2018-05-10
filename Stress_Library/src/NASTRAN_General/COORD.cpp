/**
 *
 * @file		COORD.cpp
 * @author		Matt Guimarey
 * @date		02/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Coordinate System Entry
 *
 */


#include "NASTRAN_General/COORD.h"
#include "Math/Euclidean_Vector.h"
#include "Math/Coordinate.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <math.h> 
#include <map>


COORD::COORD()
{

}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parseBDFData
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

COORD::COORD(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	parseBDFData(BDF_Data, LongFormatFlag);
}

COORD::~COORD()
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

void COORD::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	// Initiate a string vector to store split data
	std::vector<std::string> LineData;
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
	// Parse COORD ID and Ref Coordinate ID
	for (int i = 0; i < 2; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	// Parse Origin, Point B and Point C
	double tempArray[9];
	for (int i = 0; i < 9; i++) { tempArray[i] = atof(check_exp(LineData[i + 2]).c_str()); }
	pOrigin.set_coordinates(tempArray[0], tempArray[1], tempArray[2]);
	Coordinate point_B(tempArray[3], tempArray[4], tempArray[5]);
	Coordinate point_C(tempArray[6], tempArray[7], tempArray[8]);
	// Build matrices
	pTransMatrices[0].build_matrix(pOrigin, point_B, point_C, 31);
	pTransMatrices[1] = pTransMatrices[0].inverse_matrix();
}

/**
 *	@brief	Returns the chosen axis vector for the transformation matrix (inverse_flag 
 *			== false) or the inverse transformation matrix (inverse_flag == true). The 
 *			vector is returned in the a chosen coordinate system.
 *
 *			default axis -> x-axis
 *			i = 1 -> y-axis
 *			i = 2 -> z-axis
 *
 *	@param	i, an unsigned int determining the axis that is output
 * 	@param 	inverse_flag, a boolean determining inverse transformation or standard
 *	@param 	COORD_ID, an unsigned long for the requested coordinate system ID for output
 *	@param	COORD_Map, a map of all the COORD systems in the model
 *	@return	Euclidean_Vector, the axis vector
 */

Euclidean_Vector COORD::get_axis_vector(unsigned int i, bool inverse_flag, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map)
{
	// Determine transformation matrix to extract axis
	int j = 0;
	if (inverse_flag) { j = 1; }
	// Get axis vector
	Euclidean_Vector axis_vector;
	switch (i) {
		case 1: case 2:
			axis_vector.set_vector(pTransMatrices[j].get_component(i,0), pTransMatrices[j].get_component(i,1), pTransMatrices[j].get_component(i,2));
			break;
		default:
			axis_vector.set_vector(pTransMatrices[j].get_component(0,0), pTransMatrices[j].get_component(0,1), pTransMatrices[j].get_component(0,2));
	}
	// Transform back to BASIC coordinate system -> 0
	unsigned long refCoord = pLongData[1];
	while (refCoord != 0) {
		if (COORD_Map.find(refCoord) != COORD_Map.end()) {
			refCoord = (*COORD_Map[refCoord])[1];
			// Translate vector back to reference Coordinate System
			axis_vector = COORD_Map[refCoord]->transform_vector(axis_vector, true);
			// If the ref Coord matches the target COORD_ID, return the axis_vector
			if (refCoord == COORD_ID) {return axis_vector;}
		} else {
			refCoord = 0;
		}
	}
	/*	If target coord_ID has not been reached, the axis_vector is in BASIC coordinate 
		system. Determine the order of translations by working backwards from the target COORD_ID */
	std::vector<unsigned long> coordinate_frames;
	if (COORD_Map.find(COORD_ID) != COORD_Map.end()) {
		coordinate_frames.push_back(COORD_ID);
		refCoord = (*COORD_Map[COORD_ID])[1];
		// Iterate through anyremaining translations
		while (refCoord != 0) {
			if (COORD_Map.find(refCoord) != COORD_Map.end()) {
				coordinate_frames.push_back(refCoord);
				refCoord = (*COORD_Map[COORD_ID])[1];
			} else {
				refCoord = 0;
			}
		}
	}
	// Transform axis_vector to the target COORD_ID
	for (int i = coordinate_frames.size() - 1; i > -1; i--) {
		axis_vector = COORD_Map[coordinate_frames[i]]->transform_vector(axis_vector, false);
	}
	return axis_vector;
}

/**
 *	@brief	Transforms a coordinate point from the reference coordinate system to this 
 *			one if inverse_flag is not set, or transforms a coordinate point from this 
 *			coordinate system to the reference coordinate system if the inverse_flag is 
 *			set.
 *
 *	@param	coord, coordinate point to transform
 * 	@param 	inverse_flag, a boolean determining inverse transformation or standard
 *	@return	Coordinate, transformed coordinate point to return
 */

Coordinate COORD::transform_coordinate(Coordinate coord, bool inverse_flag)
{
	Coordinate returnCoordinate;
	if (inverse_flag) {
		returnCoordinate = pTransMatrices[1].transform_coordinate(coord);
		returnCoordinate += pOrigin;
	} else {
		coord -= pOrigin;
		returnCoordinate = pTransMatrices[0].transform_coordinate(coord);
	}
	return returnCoordinate;
}

/**
 *	@brief	Transforms a euclidean vector from the reference coordinate system to this 
 *			one if inverse_flag is not set, or transforms a euclidean vector from this 
 *			coordinate system to the reference coordinate system if the inverse_flag is 
 *			set.
 *
 *	@param	vector, euclidean vector to transform
 * 	@param 	inverse_flag, a boolean determining inverse transformation or standard
 *	@return	Euclidean_Vector, transformed euclidean vector to return
 */

Euclidean_Vector COORD::transform_vector(Euclidean_Vector &vector, bool inverse_flag)
{
	Euclidean_Vector returnVector;
	if (inverse_flag) {
		returnVector = pTransMatrices[1].transform_vector(vector);
	} else {
		returnVector = pTransMatrices[0].transform_vector(vector);
	}
	return returnVector;
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables 
 *			COORD ID and Reference Coordinate System ID. Coord ID is returned as default.
 *
 *			i = 0 -> Coord ID
 *			i = 1 -> Reference Coordinate System ID
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long COORD::operator[](int i)
{
	if (i == 1) {
		return pLongData[i];
	} else {
		return pLongData[0];
	}
}


std::string COORD::check_exp(std::string& str)
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
