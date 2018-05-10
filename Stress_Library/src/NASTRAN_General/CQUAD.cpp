/**
 *
 * @file		CQUAD.cpp
 * @author		Matt Guimarey
 * @date		07/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN CQUAD element
 *
 */

#include "NASTRAN_General/CQUAD.h"
#include "NASTRAN_General/MAT.h"
#include "NASTRAN_General/GRID.h"
#include "NASTRAN_General/COORD.h"
#include "NASTRAN_General/PCOMP.h"
#include "NASTRAN_General/PSHELL.h"
#include "Math/Euclidean_Vector.h"
#include "Math/Coordinate.h"
#include "Stress/CAUCHY_STRESS.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <math.h> 

#define PI 3.14159265

CQUAD::CQUAD()
{
    for (int i = 1; i < 4; i++) { pBoolData[i] = false; }
    pCentroid.set_coordinates(0.0, 0.0, 0.0);
}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

CQUAD::CQUAD(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	CQUAD();
	parseBDFData(BDF_Data, LongFormatFlag);
}

CQUAD::CQUAD(unsigned long ElementID)
{
	pLongData[0] = ElementID;
}

CQUAD::~CQUAD()
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

void CQUAD::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;  ///< String vector to store seperated line entries for parsing
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
	for (int i = 0; i < 6; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	// Parse the theta/MCID entry
    std::string str = check_exp(LineData[6]);
	if (str.compare("") == 0) {
		// Default value
		pBoolData[0] = false;
		pDoubleData[1] = 0.0;
	} else {
		// Not default
		if (checkReal(str)) {
			// Not an integer
			pBoolData[0] = false;
			pDoubleData[1] = atof(str.c_str());
		} else {
			// Is supposedly an integer/long
			pBoolData[0] = true;
			pLongData[6] = atol(str.c_str());
		}
	}
	// Parse zOffset
	pDoubleData[0] = atof(check_exp(LineData[7]).c_str());
}

/**
 *	@brief	Returns a coordinate point representing the centroid of the CQUAD 
 *			element in a specified coordinate system.
 *
 *	@param	COORD_ID, an unsigned long integer that is the specified COORD system
 * 	@param 	COORD_Map, a map of all the COORD systems present in the model
 *	@return	Coordinate, the returned coordinate point
 */

Coordinate CQUAD::getCentroid(unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map)
{
	// If requested COORD_ID is the model BASIC coordinate system (0), return centroid
	if (COORD_ID == 0) {
		return pCentroid;
	} 
	// Requested coord system not BASIC, prepare list of coordinate frames for transformation
	std::vector<unsigned long> coordinate_frames;
	unsigned long refCoord;
	if (COORD_Map.find(COORD_ID) != COORD_Map.end()) {
		coordinate_frames.push_back(COORD_ID);
		refCoord = (*COORD_Map[COORD_ID])[1];
		// Iterate through anyremaining coordinate frames
		while(refCoord != 0) {
			if (COORD_Map.find(refCoord) != COORD_Map.end()) {
				coordinate_frames.push_back(refCoord);
				refCoord = (*COORD_Map[refCoord])[1];				
			} else {
				refCoord = 0;
			}
		}
	}
	// Perform transformations to the centroid from BASIC to requested coord frame
	Coordinate returnCoordinate = pCentroid;
	for (int i = coordinate_frames.size() - 1; i > -1; i--) {
		returnCoordinate = COORD_Map[coordinate_frames[i]]->transform_coordinate(returnCoordinate, false);
	}
	return returnCoordinate;
}

bool CQUAD::checkMCIDValid()
{
	return pBoolData[0];
}

/**
 *	@brief	Determines if CQUAD element has composite properties and sets flag.
 *
 *	@param	PCOMP_Map, a map of all the PCOMP properties present in the model
 * 	@param 	PSHELL_Map, a map of all the PSHELL properties present in the model
 * 	@param 	MAT_Map, a map of all the MAT classes present in the model
 *	@return	void
 */

void CQUAD::parseComposite(std::map<unsigned long, PCOMP*> &PCOMP_Map, std::map<unsigned long, PSHELL*> &PSHELL_Map, std::map<unsigned long, MAT*> &MAT_Map)
{
	if (PCOMP_Map.find(pLongData[1]) != PCOMP_Map.end()) {
		// Found PCOMP
		pBoolData[1] = true;
	} else {
		// Could not PCOMP, see if PSHELL material is orthotropic
		if (PSHELL_Map.find(pLongData[1]) != PSHELL_Map.end()) {
			if (MAT_Map.find((*PSHELL_Map[pLongData[1]])[1]) != MAT_Map.end()) {
				if (!MAT_Map[(*PSHELL_Map[pLongData[1]])[1]]->is_Isotropic()) {
					pBoolData[1] = true;
				}
			}
		}
	}
}

bool CQUAD::is_Composite()
{
	return pBoolData[1];
}

/**
 *	@brief	Finds GRID entries in model for the four CQUAD nodes and updates the GRID entry connectivity 
 *			array. Function also calculates the CQUAD element centroid and stores the Coorindate point.
 *
 *	@param	GRID_Map, a map of all the GRIDs present in the model
 * 	@param 	COORD_Map, a map of all the COORD systems present in the model
 *	@return	void
 */

void CQUAD::nodeConnect(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, COORD*> &COORD_Map)
{
	Coordinate pNodes[4];
	std::map<unsigned long, GRID*>::iterator Grid_it;
	for (int i = 2; i < 6; i++) {
		//	Find CQUAD corner GRIDs in the GRID map for the model
		Grid_it = GRID_Map.find(pLongData[i]);
		if (Grid_it != GRID_Map.end()) {
			//	Add CQUAD ID to GRID connectivity
			Grid_it->second->addCQUAD_Connect(pLongData[0]);
			//	Get coordinate point
			pNodes[i - 2] = Grid_it->second->get_coordinate(0, COORD_Map);
			//	Add coordinate point to centroid
			pCentroid += pNodes[i - 2];
		}
	}
	//	Set CQUAD centroid
	pCentroid = pCentroid / 4;
	//	Calculate side lengths
	Euclidean_Vector side_vectors[4];
	for (int i = 0; i < 4; i++) {
		if (i == 4) {
			side_vectors[i].set_vector(pNodes[0][0] - pNodes[i][0], pNodes[0][1] - pNodes[i][1], pNodes[0][2] - pNodes[i][2]);
		} else {
			side_vectors[i].set_vector(pNodes[i + 1][0] - pNodes[i][0], pNodes[i + 1][1] - pNodes[i][1], pNodes[i + 1][2] - pNodes[i][2]);
		}
	}
	//	Get and store side lengths
	pDoubleData[2] = (side_vectors[1][3] + side_vectors[3][3]) / 2; 
	pDoubleData[3] = (side_vectors[0][3] + side_vectors[2][3]) / 2; 
}

Euclidean_Vector CQUAD::get_material_vector()
{
	Euclidean_Vector return_vector(1.0, 0.0, 0.0);
	if (pBoolData[2]) {
		return_vector = plane_normal.rodrigues_rotation(x_elm, mat_angle);
	}
	return return_vector;
}

/**
 *	@brief	Adds a force result entry from a NASTRAN punch file to the Force Result Map.
 *
 *	@param	lineEntries, a string vector stroing raw punch file line entries
 * 	@param 	SubcaseID, unsigned long integer of the subcase ID
 *	@param	in_mat, a bool flagging whether results are in material CID or not
 *	@return	void
 */

void CQUAD::addForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID, bool in_mat)
{
	std::vector<std::string> LineData;
	for (int j = 0; j < lineEntries.size(); j++) {
		for (int i = 18; i < 72; i += 18) {
			LineData.push_back(lineEntries[j].substr(i, 18));
		}
	}
	// Enter in-plane fluxes as CAUCHY_STRESS
	CAUCHY_STRESS entryCauchy(atof(check_exp(LineData[0]).c_str()), atof(check_exp(LineData[1]).c_str()), atof(check_exp(LineData[2]).c_str()));
	N_Map.insert(std::pair<unsigned long, std::pair<CAUCHY_STRESS, bool> > (SubcaseID, std::pair<CAUCHY_STRESS, bool> (entryCauchy, in_mat)));
	// Enter moments as CAUCHY_STRESS
	entryCauchy.set_values(atof(check_exp(LineData[3]).c_str()), atof(check_exp(LineData[4]).c_str()), atof(check_exp(LineData[5]).c_str()));
	M_Map.insert(std::pair<unsigned long, std::pair<CAUCHY_STRESS, bool> > (SubcaseID, std::pair<CAUCHY_STRESS, bool> (entryCauchy, in_mat)));
	// Enter out of plane fluxes
	std::vector<double> entryVector;
	for (int i = 6; i < 8; i++) {
		entryVector.push_back(atof(check_exp(LineData[i]).c_str()));
	}	
	V_Map.insert(std::pair<unsigned long, std::pair<std::vector<double>, bool> > (SubcaseID, std::pair<std::vector<double>, bool> (entryVector, in_mat)));
}

/**
 *	@brief	Gets a force result entry from the force result maps. Results can be requested in 
 *			material axis system, or in the direction of a specific vector.
 *
 * 	@param 	SubcaseID, unsigned long integer of the subcase ID
 *	@param	in_mat, a bool flagging whether results are in material CID or not
 *	@param	default_vector, a Euclidean_Vector defining the axis of output if material axis is not selected
 *	@param	GRID_Map, a map of all the GRIDs present in the model
 * 	@param 	COORD_Map, a map of all the COORD systems present in the model
 *	@return	std::vector<double>, returned results
 */

std::vector<double> CQUAD::getForceResult(unsigned long SubcaseID, bool in_mat, Euclidean_Vector* default_vector, std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, COORD*> &COORD_Map)
{
	//	Check whether internal angles have been calculated
	if (!pBoolData[2]) {
		//	Get CQUAD corner node coordinates
		Coordinate QUAD_Coords[4];
		for (int i = 2; i < 6; i++) {
			if (GRID_Map.find(pLongData[i]) != GRID_Map.end()) {
				QUAD_Coords[i - 2] = GRID_Map[pLongData[i]]->get_coordinate(0, COORD_Map);
			} else {
				QUAD_Coords[i - 2].set_coordinates(0.0, 0.0, 0.0);
			}
		}
		//	Define vectors to calculate internal angles
		Euclidean_Vector QUAD_Vectors[5];
		QUAD_Vectors[0].set_vector(QUAD_Coords[1][0] - QUAD_Coords[0][0], QUAD_Coords[1][1] - QUAD_Coords[0][1], QUAD_Coords[1][2] - QUAD_Coords[0][2]);
		QUAD_Vectors[1] = QUAD_Vectors[0] * -1.0;
		QUAD_Vectors[2].set_vector(QUAD_Coords[2][0] - QUAD_Coords[0][0], QUAD_Coords[2][1] - QUAD_Coords[0][1], QUAD_Coords[2][2] - QUAD_Coords[0][2]);
		QUAD_Vectors[3].set_vector(QUAD_Coords[3][0] - QUAD_Coords[1][0], QUAD_Coords[3][1] - QUAD_Coords[1][1], QUAD_Coords[3][2] - QUAD_Coords[1][2]);
		QUAD_Vectors[4].set_vector(QUAD_Coords[3][0] - QUAD_Coords[0][0], QUAD_Coords[3][1] - QUAD_Coords[0][1], QUAD_Coords[3][2] - QUAD_Coords[0][2]);
		//	Calculate angles
		beta = QUAD_Vectors[0].get_angle(QUAD_Vectors[2]);
		gamma = QUAD_Vectors[3].get_angle(QUAD_Vectors[1]);
		alpha =	(beta + gamma) / 2;
		//	Calculate CQUAD analysis vectors
		plane_normal = QUAD_Vectors[0].cross_product(QUAD_Vectors[4]);
		plane_normal.normalize();
		x_elm = QUAD_Vectors[0].rodrigues_rotation(plane_normal, beta - alpha);
		x_elm.normalize();
		y_elm = plane_normal.cross_product(x_elm);
		y_elm.normalize();
		//	Calculate material angle
		if (pBoolData[0]) {
			//	Get material coord system
			if (COORD_Map.find(pLongData[6]) != COORD_Map.end()) {
				Euclidean_Vector mat_vector = COORD_Map[pLongData[6]]->get_axis_vector(0, false, 0, COORD_Map);
				mat_angle = x_elm.get_angle(mat_vector);
				double check_angle = y_elm.get_angle(mat_vector);
				if (check_angle > 1.570796) {
					mat_angle *= -1;
				}
			} else {
				mat_angle = 0.0;
			}
		} else {
			mat_angle = pDoubleData[1];
		}
		//	Internal angles and vectors have been calculated, set flag to true
		pBoolData[2] = true;
	}
	//	Retrieve requested force result
	std::vector<double> returnVector;
	std::map<unsigned long, std::pair<CAUCHY_STRESS, bool> >::iterator map_it;
	if (in_mat)	{
		//	Results requested in material coordinate system
		map_it = N_Map.find(SubcaseID);
		//	In-Plane Forces
		if (map_it != N_Map.end()) {
			if (map_it->second.second) {
				//	Results already in material coordinate system
				for (int i = 0; i < 3; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			} else {
				//	Results not in material coordinate system, apply rotation
				map_it->second.first.rotate(mat_angle);
				for (int i = 4; i < 7; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			}
		}
		//	In-Plane Moments -> As above for in-plane forces
		map_it = M_Map.find(SubcaseID);
		if (map_it != M_Map.end()) {
			if (map_it->second.second) {
				for (int i = 0; i < 3; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			} else {
				map_it->second.first.rotate(mat_angle);
				for (int i = 4; i < 7; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			}
		}		
	} else {
		//	Results not requested in material coordinate system
		if (default_vector == NULL) {
			//	Default vector for results not given, return results in CQUAD element coordinate system
			map_it = N_Map.find(SubcaseID);
			//	In-Plane Forces
			if (map_it != N_Map.end()) {
				if (!map_it->second.second) {
					//	Results already in CQUAD analysis coordinate system
					for (int i = 0; i < 3; i++) {
						returnVector.push_back(map_it->second.first[i]);
					}
				} else {
					//	Results not in CQUAD analysis coordinate system, apply rotation
					map_it->second.first.rotate(0.0 - mat_angle);
					for (int i = 4; i < 7; i++) {
						returnVector.push_back(map_it->second.first[i]);
					}
				}				
			}
			map_it = M_Map.find(SubcaseID);
			//	In-Plane Moments -> As above for in-plane forces
			if (map_it != M_Map.end()) {
				if (!map_it->second.second) {
					for (int i = 0; i < 3; i++) {
						returnVector.push_back(map_it->second.first[i]);
					}
				} else {
					map_it->second.first.rotate(0.0 - mat_angle);
					for (int i = 4; i < 7; i++) {
						returnVector.push_back(map_it->second.first[i]);
					}
				}				
			}
		} else {
			//	Vector supplied for results, calculate angle for translation
			//	Project vector onto CQUAD plane
			Euclidean_Vector proj_vector = default_vector->project_to_plane(plane_normal);
			double rot_angle = x_elm.get_angle(proj_vector);
			if (x_elm.get_angle(proj_vector) > 1.570796) {
				rot_angle *= -1;
			}
			//	In-Plane Forces
			map_it = N_Map.find(SubcaseID);
			if (map_it != N_Map.end()) {
				if (map_it->second.second) {
					//	Results are in material axis, apply rotation
					map_it->second.first.rotate(rot_angle - mat_angle);
				} else {
					//	Results are in CQUAD analysis coordinate system, apply rotation
					map_it->second.first.rotate(rot_angle);
				}
				//	Push results back to return_Vector
				for (int i = 4; i < 7; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			}
			//	In-Plane Moments -> As above for in-plane forces
			map_it = M_Map.find(SubcaseID);
			if (map_it != M_Map.end()) {
				if (map_it->second.second) {
					map_it->second.first.rotate(rot_angle - mat_angle);
				} else {
					map_it->second.first.rotate(rot_angle);
				}
				for (int i = 4; i < 7; i++) {
					returnVector.push_back(map_it->second.first[i]);
				}
			}			
		}
	}
	//	Check return_Vector and then return
	while (returnVector.size() < 6) {
		returnVector.push_back(0.0);
		//	Add an error message???
	}
	return returnVector;
}



/*
void CQUAD::addPlyStressResult(std::vector<std::string> lineEntries, unsigned long SubcaseID, bool in_mat)
{
	std::vector<std::string> LineData;
	for (int j = 0; j < lineEntries.size(); j++) {
		for (int i = 18; i < 72; i += 18) {
			LineData.push_back(lineEntries[j].substr(i, 18));
		}
	}	
	std::vector<double> entryVector;
	unsigned int ply_number;
	try {
		ply_number = atol(trim(LineData[0]).c_str());
	} catch (int e) {
		ply_number = 0;
	}
	for (int i = 1; i < 10; i++) {
		try {
			entryVector.push_back(atof(trim(LineData[i]).c_str()));
		} catch (int e) {
			entryVector.push_back(0.0);
		}
	}
	std::pair<std::vector<double>, bool> ply_data = std::pair<std::vector<double>, bool> (entryVector, in_mat);
	std::map<unsigned long, std::map<unsigned int, std::pair<std::vector<double>, bool> > >::iterator Ply_Stress_Res_it = Ply_Stress_Res_Map.find(SubcaseID);
	if (Ply_Stress_Res_it == Ply_Stress_Res_Map.end()) {
		std::map<unsigned int, std::pair<std::vector<double>, bool> > entry_map;
		entry_map.insert(std::pair<unsigned int, std::pair<std::vector<double>, bool> > (ply_number, ply_data));
		Ply_Stress_Res_Map.insert(std::pair<unsigned long, std::map<unsigned int, std::pair<std::vector<double>, bool> > > (SubcaseID, entry_map));
	} else {
		Ply_Stress_Res_it->second.insert(std::pair<unsigned int, std::pair<std::vector<double>, bool> > (ply_number, ply_data));
	}
	pBoolData[2] = true;
}

std::map<unsigned int, std::pair<std::vector<double>, bool> > CQUAD::getPlyStresses(unsigned long SubcaseID)
{
	if (Ply_Stress_Res_Map.find(SubcaseID) != Ply_Stress_Res_Map.end()) {
		// Found
		return Ply_Stress_Res_Map[SubcaseID];
	} else {
		// Not found create dummy and return
		std::vector<double> dummyVector;
		std::map<unsigned int, std::pair<std::vector<double>, bool> > dummy_Map;
		dummy_Map.insert(std::pair<unsigned int, std::pair<std::vector<double>, bool> > (SubcaseID, std::pair <std::vector<double>, bool> (dummyVector, false)));
		return dummy_Map;
	}
}

void CQUAD::addStressResult(std::vector<std::string> lineEntries, unsigned long SubcaseID, bool in_mat)
{
	std::vector<std::string> LineData;
	for (int j = 0; j < lineEntries.size(); j++) {
		for (int i = 18; i < 72; i += 18) {
			LineData.push_back(lineEntries[j].substr(i, 18));
		}
	}
	std::vector<double> entryVector;
	for (int i = 0; i < 16; i++) {
		try {
			entryVector.push_back(atof(trim(LineData[i]).c_str()));
		} catch (int e) {
			entryVector.push_back(0.0);
		}
	}
	Stress_Res_Map.insert(std::pair<unsigned long, std::pair<std::vector<double>, bool> > (SubcaseID, std::pair<std::vector<double>, bool> (entryVector, in_mat)));
	pBoolData[3] = true;
}

std::pair<std::vector<double>, bool> CQUAD::getStressResult(unsigned long SubcaseID)
{
	if (Stress_Res_Map.find(SubcaseID) != Stress_Res_Map.end()) {
		//Found
		return Stress_Res_Map[SubcaseID];
	} else {
		// Not found make dummy
		std::vector<double> dummyVector;
		return std::pair<std::vector<double>, bool> (dummyVector, false);
	}
}
*/


/**
 *	@brief	Overload the [] operator to provide direct access to the class variables. Element ID 
 *			is returned as default.
 *
 *			i = 0 -> Element ID
 *			i = 1 -> Property ID
 *			i = 2 -> Grid 1 ID
 *			i = 3 -> Grid 2 ID
 *			i = 4 -> Grid 3 ID
 *			i = 5 -> Grid 4 ID
 *			i = 6 -> MCID
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long CQUAD::operator[] (int i)
{
	switch (i) {
		case 1: case 2: case 3: case 4: case 5: case 6:
			return pLongData[i];
		default:
			return pLongData[0];
	}
}

/**
 *	@brief	Overload the () operator to provide direct access to the class variables. Z_Offset is returned 
 *			as default.
 *
 *			i = 0 -> Z_Offset
 *			i = 1 -> Theta
 *			i = 2 -> Centroid_X (BASIC Coordinate Frame)
 *			i = 3 -> Centroid_Y (BASIC Coordinate Frame)
 *			i = 4 -> Centroid_Z (BASIC Coordinate Frame)
 *			i = 5 -> Side X Length
 *			i = 6 -> Side Y Length
 *
 *	@param	i, integer parameter designating position
 *	@return	double, returned class variable
 */

double CQUAD::operator() (int i)
{
	switch (i) {
		case 1:
			return pDoubleData[i];
		case 2: case 3: case 4:
			return pCentroid[i - 2];
		case 5: case 6:
			return pDoubleData[i - 3];
		default:
			return pDoubleData[0];
	}
}

std::string CQUAD::check_exp(std::string& str)
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

bool CQUAD::checkReal(std::string &str)
{
	std::size_t found = str.find_first_of('.');
	if (found == std::string::npos) {
		return false;
	} else {
		return true;
	}
}




/*
std::vector<double> CQUAD::vector_Cross(std::vector<double> vector_A, std::vector<double> vector_B)
{
	std::vector<double> returnVector;
	if (vector_A.size() > 2 && vector_B.size() > 2) {
		returnVector.push_back(vector_A[1] * vector_B[2] - vector_A[2] * vector_B[1]);
		returnVector.push_back(vector_A[2] * vector_B[0] - vector_A[0] * vector_B[2]);
		returnVector.push_back(vector_A[0] * vector_B[1] - vector_A[1] * vector_B[0]);
		double magnitude = pow(pow(returnVector[0], 2) + pow(returnVector[1], 2) + pow(returnVector[2], 2), 0.5);
		returnVector[0] = returnVector[0] / magnitude;
		returnVector[1] = returnVector[1] / magnitude;
		returnVector[2] = returnVector[2] / magnitude;
	} else {
		for (int i = 0; i < 3; i++) {
			returnVector.push_back(0.0);
		}
	}
	return returnVector;
}

std::vector<double> CQUAD::plane_Rotate(std::vector<double> vector_A, std::vector<double> plane_Normal, double angle)
{
	std::vector<double> returnVector;

	if (vector_A.size() > 2 && plane_Normal.size() > 2) {
		std::vector<double> e1, e2 = vector_Cross(plane_Normal, vector_A);
		double vector_A_Mag = pow(pow(vector_A[0], 2) + pow(vector_A[1], 2) + pow(vector_A[2], 2), 0.5);
		e1.push_back(vector_A[0] / vector_A_Mag);
		e1.push_back(vector_A[1] / vector_A_Mag);
		e1.push_back(vector_A[2] / vector_A_Mag);
		returnVector.push_back(vector_A[0] * cos(angle) + e2[0] * sin(angle));
		returnVector.push_back(vector_A[1] * cos(angle) + e2[1] * sin(angle));
		returnVector.push_back(vector_A[2] * cos(angle) + e2[2] * sin(angle));
	} else {
		for (int i = 0; i < 3; i++) {
			returnVector.push_back(0.0);
		}	
	}
	return returnVector;
}

std::vector<double> CQUAD::project_Vector(std::vector<double> vector_A, std::vector<double> plane_Normal)
{
	std::vector<double> returnVector;
	if (vector_A.size() > 2 && plane_Normal.size() > 2) {
		double plane_Normal_Mag = pow(pow(plane_Normal[0], 2) + pow(plane_Normal[1], 2) + pow(plane_Normal[2], 2), 0.5);
		double a1 = vector_A[0] * plane_Normal[0] / plane_Normal_Mag + vector_A[1] * plane_Normal[1] / plane_Normal_Mag + vector_A[2] * plane_Normal[2] / plane_Normal_Mag;
		std::vector<double> a1_vector;
		a1_vector.push_back(a1 * plane_Normal[0]);
		a1_vector.push_back(a1 * plane_Normal[1]);
		a1_vector.push_back(a1 * plane_Normal[2]);
		returnVector.push_back(vector_A[0] - a1_vector[0]);
		returnVector.push_back(vector_A[1] - a1_vector[1]);
		returnVector.push_back(vector_A[2] - a1_vector[2]);
	} else {
		for (int i = 0; i < 3; i++) {
			returnVector.push_back(0.0);
		}		
	}
	
	return returnVector;
}

double CQUAD::angle_Vectors(std::vector<double> vector_A, std::vector<double> vector_B)
{
	if (vector_A.size() > 2 && vector_B.size() > 2) {
		double vector_A_Mag = pow(pow(vector_A[0], 2) + pow(vector_A[1], 2) + pow(vector_A[2], 2), 0.5);
		double vector_B_Mag = pow(pow(vector_B[0], 2) + pow(vector_B[1], 2) + pow(vector_B[2], 2), 0.5);
		double returnDouble = acos((vector_A[0] * vector_B[0] + vector_A[1] * vector_B[1] + vector_A[2] * vector_B[2]) / (vector_A_Mag * vector_B_Mag));
		return returnDouble;
	} else {
		return 0.0;
	}
	
}
*/
