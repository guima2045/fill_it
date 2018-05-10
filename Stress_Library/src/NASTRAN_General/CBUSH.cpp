/**
 *
 * @file		CBUSH.cpp
 * @author		Matt Guimarey
 * @date		11/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN CBUSH element entry
 *
 */

#include "NASTRAN_General/CBUSH.h"
#include "NASTRAN_General/GRID.h"
#include "NASTRAN_General/CQUAD.h"
#include "NASTRAN_General/RBE2.h"
#include "NASTRAN_General/RBE3.h"
#include "NASTRAN_General/COORD.h"
#include "NASTRAN_General/LOAD_CASE.h"
#include "Math/Transformation.h"
#include "Math/Coordinate.h"
#include "Math/Euclidean_Vector.h"


#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <math.h>
#include <map>
#include <deque>
#include <algorithm>

CBUSH::CBUSH()
{
	pOrienType = 0;
	pTransFlag = false;
	pNodeSort = false;
}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

CBUSH::CBUSH(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	pOrienType = 0;
	parseBDFData(BDF_Data, LongFormatFlag);
}

CBUSH::CBUSH(unsigned long ElementID)
{
	pLongData[0] = ElementID;
}

CBUSH::~CBUSH()
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

void CBUSH::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store seperated line entries for parsing
	std::string str(80, ' ');
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		while (BDF_Data.size() < 2) {
			BDF_Data.push_back(str);
		}
		for (int i = 0; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 8) {
				LineData.push_back(BDF_Data[i].substr(j, 8));
			}
		}
	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
		while (BDF_Data.size() < 4) {
			BDF_Data.push_back(str);
		}
		for (int i = 0; i< BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_Data[i].substr(j, 16));
			}
		}
	}
	//	Parse Longs - Element ID to GRID B
	for (int i = 0; i < 4; i++) { pLongData[i] = atol(LineData[i].c_str()); }
	// Parse Orientation
	str = check_exp(LineData[4]);
	if (str.compare("") == 0) {
		// Orientation by CID
		pOrienType = 3;
		pLongData[5] = atol(LineData[7].c_str());
	} else if (!checkReal(str)) {
		// Orientation by GO
		pOrienType = 1;
		pLongData[4] = atol(str.c_str());
	} else {
		// Orientation by Coordinates
		pOrienType = 2;
		pCoordinate[0].set_coordinates(atof(str.c_str()), atof(check_exp(LineData[5]).c_str()), atof(check_exp(LineData[6]).c_str()));
	}
	//	S value
	str = check_exp(LineData[8]);
	if (str.compare("") == 0) {
		pS = 0.5;
	} else {
		pS = atof(str.c_str());
	}
	//	OCID
	str = check_exp(LineData[9]);
	if (str.compare("") == 0 || str.compare("-1") == 0) {
		pLongData[6] = -1;
		pCoordinate[1].set_coordinates(0.0, 0.0, 0.0);
	} else {
		pLongData[6] = atol(str.c_str());
		pCoordinate[1].set_coordinates(atof(check_exp(LineData[10]).c_str()), atof(check_exp(LineData[11]).c_str()), atof(check_exp(LineData[12]).c_str()));
	}
}

/**
 *	@brief	Finds GRID entries in model for the CBUSH nodes and updates the GRID entry connectivity 
 *			array. Function also calculates the CBUSH fastener nodes for cases where zero length 
 *			CBUSH elements are connected via RBE2 elements.
 *
 *	@param	GRID_Map, a map of all the GRIDs present in the model
 * 	@param 	RBE2_Map, a map of all the RBE2 elements present in the model
 *	@return	void
 */

void CBUSH::nodeConnect(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, RBE2*> &RBE2_Map)
{
	// Iterate through the two CBUSH nodes (A and B)
	for (int i = 2; i < 4; i++) {
		// Initially set the fastener node (node connected to the plate) as the CBUSH node
		fastenerNodes[i - 2] = pLongData[i];
		// Find CBUSH node in GRID map
		if (GRID_Map.find(pLongData[i]) != GRID_Map.end()) {
			// Add CBUSH connectivity and get list of RBE2 elements connected to CBUSH node
			GRID_Map[pLongData[i]]->addCBUSH_Connect(pLongData[0]);
			std::vector<unsigned long> RBE2_List = GRID_Map[pLongData[i]]->getRBE2_List();
			// Check number of RBE2 elements connected to node (must be 1 to satisfy an RBE2 connecting shear plane to plate
			if (RBE2_List.size() > 0 && RBE2_List.size() < 2) {
				// Find RBE2 elements in RBE2 Map
				if (RBE2_Map.find(RBE2_List[0]) != RBE2_Map.end()) {
					// Check if number of dependant numbers satisfies condition for fastener model (equal to 1)
					if (RBE2_Map[RBE2_List[0]]->get_num_dependant_nodes() == 1) {
						unsigned long ind_Node = (*RBE2_Map[RBE2_List[0]])[1], dep_Node = (*RBE2_Map[RBE2_List[0]])[3];
						if (ind_Node == pLongData[i]) {
							fastenerNodes[i - 2] = dep_Node;
						} else if (dep_Node == pLongData[i]) {
							fastenerNodes[i - 2] = ind_Node;
						}
					}
				}
			}
		}
	}
}

Euclidean_Vector CBUSH::get_vector(unsigned int axis, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map)
{
	if (axis == 3) {
		//	Option is to return vector from fastener node A to fastener node B
		Coordinate fastener_coords[2];
		Euclidean_Vector fast_vector(0.0, 0.0, 1.0);
		for (int i = 0; i < 2; i++) {
			if (GRID_Map.find(fastenerNodes[i]) != GRID_Map.end()) {
				fastener_coords[i] = GRID_Map[fastenerNodes[i]]->get_coordinate(COORD_ID, COORD_Map);
			} else {
				//	Error finding fastener node in model...
				return fast_vector;
			}
		}
		//	Return fastener vector
		fast_vector.set_vector(fastener_coords[1][0] - fastener_coords[0][0], fastener_coords[1][1] - fastener_coords[0][1], fastener_coords[1][2] - fastener_coords[0][2]);
		return fast_vector;
	}
	switch (pOrienType) {
		case 1:
			//	TODO
			break;
		case 2:
			//	TODO
			break;
		default:	
			if (COORD_Map.find(pLongData[5]) != COORD_Map.end()) {
				return COORD_Map[pLongData[5]]->get_axis_vector(axis, false, COORD_ID, COORD_Map);
			} else {
				//	COORD System ID not found in model, set CBUSH vectors to GLOABL
				Euclidean_Vector global_vector;
				switch (axis) {
					case 1:
						global_vector.set_vector(0.0, 1.0, 0.0);
						return global_vector;
					case 2:
						global_vector.set_vector(1.0, 0.0, 0.0);
						return global_vector;					
					default:
						global_vector.set_vector(0.0, 0.0, 1.0);
						return global_vector;
				
				}
			}
	}
}

int linear_search(std::vector<unsigned long> &search_vector, unsigned long &search_value) {
	for (int i = 0; i < search_vector.size(); i++) {
		if (search_vector[i] == search_value) {
			return i;
		}
	}
	return -1;
}

/**
 *	@brief	Function processes the CQUAD elements connected to the fastener nodes and iter
 *			ates out a number of times to reach the outer shell of CQUAD elements required 
 *			for the FH analysis.
 *
 *	@param	GRID_Map, a map of all GRIDs in the NASTRAN model
 * 	@param 	CQUAD_Map, a map of all the CQUAD elements in the NASTRAN model
 *	@param	COORD_Map, a map of all the COORD systems in the NASTRAN model
 *	@param	iterations, int of the number of iterations to reach outer shell
 *	@return	void
 */

void CBUSH::parse_FH(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, RBE2*> &RBE2_Map, 
		std::map<unsigned long, RBE3*> &RBE3_Map, std::map<unsigned long, COORD*> &COORD_Map, int grid_size, int side_no, unsigned int axis_type)
{
	std::vector<unsigned long> QUAD_Vector;		///< A vector to store the CQUAD IDs of current iteration
	std::vector<unsigned long> GRID_Vector;		///< A vector to store GRID IDs of next iteration
	// Fastener Node A -> 0 and Fastener Node B -> 1
	//	Start process by adding initial fastener node to the GRID_Vector
	if (GRID_Map.find(fastenerNodes[side_no]) != GRID_Map.end()) {
		GRID_Vector.push_back(fastenerNodes[side_no]);
		//	Start iterations
		int i = grid_size / 2;
		while (i > 0) {
			// Add CQUAD elements
			for (int z = 0; z < GRID_Vector.size(); z++) {
				if (GRID_Map.find(GRID_Vector[z]) != GRID_Map.end()) {
					std::vector<unsigned long> retrieved_vector = GRID_Map[GRID_Vector[z]]->getCQUAD_List();
					//	Loop through retrieved vector and look for ID in node_QUADS vector
					for (int k = 0; k < retrieved_vector.size(); k++) {
						if (!std::binary_search(node_QUADS[side_no].begin(), node_QUADS[side_no].end(), retrieved_vector[k])) {
							//	Not found, add to QUAD_Vector
							QUAD_Vector.push_back(retrieved_vector[k]);
						}
					}
				}
			}
			// Check if any CQUAD elements were connected to the fastener node
			if (QUAD_Vector.size() == 0) {
				if (GRID_Map.find(GRID_Vector[0]) != GRID_Map.end()) {
					std::vector<unsigned long> RBE_List = GRID_Map[GRID_Vector[0]]->getRBE2_List();
					bool check_RBE3 = false;
					int num_dep_nodes = 0;
					for (int k = 0; k < RBE_List.size(); k++) {
						if (RBE2_Map.find(RBE_List[k]) != RBE2_Map.end()) {
							// Check number of connected nodes
							num_dep_nodes = RBE2_Map[RBE_List[k]]->get_num_dependant_nodes();
							if (num_dep_nodes > 1) {
								// RBE2 element is spidered onto a CQUAD element, get dependant nodes
								GRID_Vector.clear();
								for (int z = 3; z < (3 + num_dep_nodes); z++) {
									GRID_Vector.push_back((*RBE2_Map[RBE_List[k]])[z]);
								}
								check_RBE3 = true;
								break;
							}
						}
					}
					if (check_RBE3) {
						// No appropriate RBE2 connectivity, check for RBE3 connectivity
						RBE_List = GRID_Map[GRID_Vector[0]]->getRBE3_List();
						for (int k = 0; k < RBE_List.size(); k++) {
							if (RBE3_Map.find(RBE_List[k]) != RBE3_Map.end()) {
								num_dep_nodes = RBE3_Map[RBE_List[k]]->get_num_independant_nodes();
								if (num_dep_nodes > 1) {
									// RBE3 element is spidered onto a CQUAD element, get dependant nodes
									GRID_Vector.clear();
									for (int z = 3; z < (3 + num_dep_nodes); z++) {
										GRID_Vector.push_back((*RBE3_Map[RBE_List[k]])[z]);
									}
									break;
								}								
							}
						}
					}
					// Get common CQUAD
					if (GRID_Vector.size() > 0) {
						std::map<unsigned long, GRID*>::iterator GRID_it = GRID_Map.find(GRID_Vector[0]);
						if (GRID_it != GRID_Map.end()) {
							QUAD_Vector = GRID_it->second->getCQUAD_List();
							// Iterate through additional grids
							for (int k = 1; k < GRID_Vector.size(); k++) {
								GRID_it = GRID_Map.find(GRID_Vector[k]);
								if (GRID_it != GRID_Map.end()) {
									std::vector<unsigned long> check_vector = GRID_it->second->getCQUAD_List();
									for (int z = 0; z < QUAD_Vector.size(); z++) {
										if (linear_search(check_vector, QUAD_Vector[z]) < 0) {
											// Item not found
											QUAD_Vector.erase(QUAD_Vector.begin() + z);
											z--;
										}
									}
								}
						
							}
						}
					}
					node_QUADS[side_no] = QUAD_Vector;
					QUAD_Vector.clear();
				}

			} else {
				//	Remove duplicates from QUAD_Vector
				std::sort(QUAD_Vector.begin(), QUAD_Vector.end());
				std::vector<unsigned long>::iterator last = std::unique(QUAD_Vector.begin(), QUAD_Vector.end());
				QUAD_Vector.erase(last, QUAD_Vector.end());
				//	Assign QUAD_Vector to node_QUADS
				node_QUADS[side_no] = QUAD_Vector;
				QUAD_Vector.clear();
				i--;
				//	Add next layer of GRID points
				if (i != 0) {
					std::vector<unsigned long> GRID_Vector_2;
					//	Inspect CQUAD elements
					for (int z = 0; z < node_QUADS[side_no].size(); z++) {
						if (CQUAD_Map.find(node_QUADS[side_no][z]) != CQUAD_Map.end()) {
							// CQUAD found in model data, retrieve GRID IDs
							for (int k = 2; k < 6; k++) {
								unsigned long check_ID = (*CQUAD_Map[ node_QUADS[side_no][z] ])[k];
								//	Search for GRID ID in previous list
								if (!std::binary_search(GRID_Vector.begin(), GRID_Vector.end(), check_ID)) {
									//	Not found so add to new list
									GRID_Vector_2.push_back(check_ID);
								}
							}
						}
					}
					//	Remove duplicates from GRID_Vector_2
					std::sort(GRID_Vector_2.begin(), GRID_Vector_2.end());
					std::vector<unsigned long>::iterator last = std::unique(GRID_Vector_2.begin(), GRID_Vector_2.end());
					GRID_Vector_2.erase(last, GRID_Vector_2.end());
					//	Assign GRID_Vector_2 to GRID_Vector for next iteration
					GRID_Vector = GRID_Vector_2;
				}				
				
			}


		}
		GRID_Vector.clear();
	} 
	order_FH(GRID_Map, CQUAD_Map, COORD_Map, grid_size, side_no, axis_type);
}

/*
std::vector<unsigned long> CBUSH::get_FH_NodeA()
{
	return node_QUADS[0];
}

std::vector<unsigned long> CBUSH::get_FH_NodeB()
{
	return node_QUADS[1];
}
*/

/**
 *	@brief	Adds a force result entry from a NASTRAN punch file to the Force Result Map.
 *
 *	@param	lineEntries, a string vector stroing raw punch file line entries
 * 	@param 	SubcaseID, unsigned long integer of the subcase ID
 *	@return	void
 */

void CBUSH::addForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID)
{
	std::vector<std::string> LineData;
	for (int j = 0; j < lineEntries.size(); j++) {
		for (int i = 18; i < 72; i += 18) {
			LineData.push_back(lineEntries[j].substr(i, 18));
		}
	}
	//	Enter Force results
	Euclidean_Vector entry_vector;
	for (int i = 0; i < 2; i++) {
		entry_vector.set_vector(atof(check_exp(LineData[(3 * i) + 0]).c_str()), atof(check_exp(LineData[(3 * i) + 1]).c_str()), atof(check_exp(LineData[(3 * i) + 2]).c_str()));
		Force_Res_Map[i].insert(std::pair<unsigned long, Euclidean_Vector > (SubcaseID, entry_vector));
	}
}

/**
 *	@brief	Returns a vector of the CBUSH force results for a specified load case in a specified 
 *			coordinate system.
 *
 * 	@param 	SubcaseID, unsigned long integer of the subcase ID
 *	@return	std::vector<Euclidean_Vector>, returned result
 */

std::vector<Euclidean_Vector> CBUSH::getForceResult(unsigned long SubcaseID, bool as_is, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map)
{
	std::vector<Euclidean_Vector> return_vector;
	std::map<unsigned long, Euclidean_Vector>::iterator res_it;
	// Get force results from map
	for (int i = 0; i < 2; i++) {
		res_it = Force_Res_Map[i].find(SubcaseID);
		if (res_it != Force_Res_Map[i].end()) {
			return_vector.push_back(res_it->second);
		} else {
			return_vector.push_back(Euclidean_Vector(0.0, 0.0, 0.0));
		}
	}
	// Return vector is as_is results are selected
	if (as_is) {
		return return_vector;
	}
	// Transform result vector into BASIC coordinate
	set_transform(COORD_Map, GRID_Map);
	for (int i = 0; i < return_vector.size(); i++) {
		return_vector[i] = pTransformation[1].transform_vector(return_vector[i]);
	}
	// Check if requested COORD_ID is BASIC
	if (COORD_ID == 0) {
		return return_vector;
	}
	// Not BASIC so build transformation order
	std::vector<COORD*> COORD_List;
	std::map<unsigned long, COORD*>::iterator COORD_it = COORD_Map.find(COORD_ID);
	if (COORD_it != COORD_Map.end()) {
		COORD_List.push_back(COORD_it->second);
		unsigned long ref_coord = (*COORD_it->second)[1];
		while (ref_coord != 0) {
			COORD_it = COORD_Map.find(ref_coord);
			if (COORD_it != COORD_Map.end()) {
				COORD_List.push_back(COORD_it->second);
				ref_coord = (*COORD_it->second)[1];
			} else {
				ref_coord = 0;
			}
		}
	}
	// Perform transformations
	for (int i = COORD_List.size() - 1; i > -1; i--) {
		for (int j = 0; j < return_vector.size(); j++) {
			return_vector[j] = COORD_List[i]->transform_vector(return_vector[j], false);
		}
	}
	return return_vector;
}

double get_alignment_angle(Euclidean_Vector vector_array[4], unsigned int axis_alignment) {
	switch (axis_alignment) {
		case 12: case 13:
			return vector_array[3].get_angle(vector_array[0]);
		case 21: case 23:
			return vector_array[3].get_angle(vector_array[1]);
		default:
			return vector_array[3].get_angle(vector_array[2]);
	}
}

Euclidean_Vector* get_alignment_vector(Euclidean_Vector vector_array[4], unsigned int axis_alignment) {
	switch (axis_alignment) {
		case 21: case 31:
			return &(vector_array[0]);
		case 12: case 32:
			return &(vector_array[1]);
		default:
			return &(vector_array[2]);
	}
}

/**
 *	@brief	Function generates the input loads for FH analysis. Default for FH results 
 *			is in the orientation of the CBUSH coordinate system, an option to use the 
 *			CQUAD material coordinate system is available.
 *
 * 	@param 	CQUAD_Map, a map of all the CQUAD elements in the NASTRAN model
 *	@param	COORD_Map, a map of all the COORD systems in the NASTRAN model
 *	@param	GRID_Map, a map of all GRIDs in the NASTRAN model
 *	@param	CBUSH_Map, a map of all CBUSH elements in the NASTRAN model -- ??
 *	@param	other_CBUSH, a pointer to CBUSH element on other side of plate
 *	@param	side_no, an int that defines side of CBUSH to be considered for FH generation 
 *			(A or B -> 0 or 1)
 *	@param	mat_flag, bool flagging option to use CQUAD material coordinate system, otherwise translate 
 *			fluxes into CBUSH orientation
 *	@param	CBUSH_as_is, bool flagging option to use CBUSH forces as is, otherwise translate 
 *			into mat direction
 *	@return	void
 */

void CBUSH::generate_FH_Result(std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map, 
std::map<unsigned long, LOAD_CASE*> &LOAD_CASE_Map, std::map<unsigned long, CBUSH*> &CBUSH_Map, CBUSH* other_CBUSH, int side_no, bool mat_flag, bool CBUSH_as_is, 
unsigned int CBUSH_axis[2])
{
	//	Determine vectors of the CBUSH orientation
	Euclidean_Vector CBUSH_vectors[4];
	for (int i = 0; i < 4; i++) {
		CBUSH_vectors[i] = get_vector(i, 0, COORD_Map, GRID_Map);
	}
	//
	//	Check CBUSH alignment, this determines whether loads from CBUSH need to be modified
	bool CBUSH1[2] = {false, false};		///< bool array determines sign reversal for CBUSH nodes
	bool CBUSH2[2] = {false, false};		///< As above but for possible CBUSH element on other side of plate
	double angle_between = get_alignment_angle(CBUSH_vectors, CBUSH_axis[0]);
	if (angle_between < 1.570796) {
		//	Less than 90 degrees, node A is aligned
		CBUSH1[0] = true;
	}
	if (CBUSH1[0] == true) {
		if (side_no == 0) {
			CBUSH1[1] = true;
		} 
	} else {
		if (side_no == 1) {
			CBUSH1[1] = true;
		} 
	}
	//	Check alignment of CBUSH on opposite side of plate
	if (other_CBUSH != NULL) {
		Euclidean_Vector CBUSH2_Vectors[4];
		for (int i = 0; i < 4; i++) {
			CBUSH2_Vectors[i] = other_CBUSH->get_vector(i, 0, COORD_Map, GRID_Map);
		}
		angle_between = get_alignment_angle(CBUSH2_Vectors, CBUSH_axis[1]);
		if (angle_between < 1.570796) {
			//	Less than 90 degrees
			CBUSH2[0] = true;
		}	
		if (fastenerNodes[side_no] == (*other_CBUSH)[4]) {
			if (CBUSH2[0] == true) {
				CBUSH2[1] = true;
			}
		} else {
			if (CBUSH2[0] == false) {
				CBUSH2[1] = true;
			}			
		}	
	}
	//
	//	Parse FH results per Load Case
	Euclidean_Vector* alignment_vector = get_alignment_vector(CBUSH_vectors, CBUSH_axis[0]);
	for (std::map<unsigned long, LOAD_CASE*>::iterator LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
		std::vector<double> FH_result, elm_result_data;	///< Container to hold the element result entity before processing
		double side_1_data[4] = {0.0, 0.0, 0.0, 0.0};
		double side_2_data[4] = {0.0, 0.0, 0.0, 0.0};
		double side_3_data[4] = {0.0, 0.0, 0.0, 0.0};
		double side_4_data[4] = {0.0, 0.0, 0.0, 0.0};
		double side_lengths[4] = {0.0, 0.0, 0.0, 0.0};
		//	Iterate side 1 elements
		for (int i = 0; i < side_1[side_no].size(); i++) {
			elm_result_data = side_1[side_no][i]->getForceResult(LOAD_CASE_it->first, mat_flag, alignment_vector, GRID_Map, COORD_Map);
			//	Fyy
			side_1_data[0] += elm_result_data[1] * (*side_1[side_no][i])[6];
			//	Fxy
			side_1_data[1] += elm_result_data[2] * (*side_1[side_no][i])[6];
			//	Myy
			side_1_data[2] += elm_result_data[4] * (*side_1[side_no][i])[6];
			//	Mxy
			side_1_data[3] += elm_result_data[5] * (*side_1[side_no][i])[6];
			//	Total side length
			side_lengths[0] += (*side_1[side_no][i])[6];
		}
		//	Iterate side 2 elements
		for (int i = 0; i < side_2[side_no].size(); i++) {
			elm_result_data = side_2[side_no][i]->getForceResult(LOAD_CASE_it->first, mat_flag, alignment_vector, GRID_Map, COORD_Map);
			//	Fxx
			side_2_data[0] += elm_result_data[0] * (*side_2[side_no][i])[5];
			//	Fxy
			side_2_data[1] += elm_result_data[2] * (*side_2[side_no][i])[5];
			//	Mxx
			side_2_data[2] += elm_result_data[3] * (*side_2[side_no][i])[5];
			//	Mxy
			side_2_data[3] += elm_result_data[5] * (*side_2[side_no][i])[5];	
			//	Total side length
			side_lengths[1] += (*side_2[side_no][i])[5];		
		}
		//	Iterate side 3 elements
		for (int i = 0; i < side_3[side_no].size(); i++) {
			elm_result_data = side_3[side_no][i]->getForceResult(LOAD_CASE_it->first, mat_flag, alignment_vector, GRID_Map, COORD_Map);
			//	Fyy
			side_3_data[0] += elm_result_data[1] * (*side_3[side_no][i])[6];
			//	Fxy
			side_3_data[1] += elm_result_data[2] * (*side_3[side_no][i])[6];
			//	Myy
			side_3_data[2] += elm_result_data[4] * (*side_3[side_no][i])[6];
			//	Mxy
			side_3_data[3] += elm_result_data[5] * (*side_3[side_no][i])[6];
			//	Total side length
			side_lengths[2] += (*side_3[side_no][i])[6];
		}	
		//	Iterate side 4 elements
		for (int i = 0; i < side_4[side_no].size(); i++) {
			elm_result_data = side_4[side_no][i]->getForceResult(LOAD_CASE_it->first, mat_flag, alignment_vector, GRID_Map, COORD_Map);
			//	Fxx
			side_4_data[0] += elm_result_data[0] * (*side_4[side_no][i])[5];
			//	Fxy
			side_4_data[1] += elm_result_data[2] * (*side_4[side_no][i])[5];
			//	Mxx
			side_4_data[2] += elm_result_data[3] * (*side_4[side_no][i])[5];
			//	Mxy
			side_4_data[3] += elm_result_data[5] * (*side_4[side_no][i])[5];	
			//	Total side length
			side_lengths[3] += (*side_4[side_no][i])[5];		
		}
		//	Enter fluxes into FH_result vector
		//	Fxx
		if (fabs(side_2_data[0] / side_lengths[1]) < fabs(side_4_data[0] / side_lengths[3])) {
			FH_result.push_back(side_2_data[0] / side_lengths[1]);
		} else {
			FH_result.push_back(side_4_data[0] / side_lengths[1]);
		}
		//	Fyy
		if (fabs(side_1_data[0] / side_lengths[0]) < fabs(side_3_data[0] / side_lengths[2])) {
			FH_result.push_back(side_1_data[0] / side_lengths[0]);
		} else {
			FH_result.push_back(side_3_data[0] / side_lengths[2]);
		}
		//	Fxy
		double S1 = side_1_data[1] / side_lengths[0], S2 = side_2_data[1] / side_lengths[1], S3 = side_3_data[1] / side_lengths[2], S4 = side_4_data[1] / side_lengths[3];
		if (fabs(S1) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S1);
		} else if (fabs(S2) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S2);
		} else if (fabs(S3) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S3);
		} else {
			FH_result.push_back(S4);
		}
		//	Mxx
		if (fabs(side_2_data[2] / side_lengths[1]) < fabs(side_4_data[2] / side_lengths[3])) {
			FH_result.push_back(side_2_data[2] / side_lengths[1]);
		} else {
			FH_result.push_back(side_4_data[2] / side_lengths[1]);
		}
		//	Myy
		if (fabs(side_1_data[2] / side_lengths[0]) < fabs(side_3_data[2] / side_lengths[2])) {
			FH_result.push_back(side_1_data[2] / side_lengths[0]);
		} else {
			FH_result.push_back(side_3_data[2] / side_lengths[2]);
		}	
		//	Mxy
		S1 = side_1_data[3] / side_lengths[0];
		S2 = side_2_data[3] / side_lengths[1];
		S3 = side_3_data[3] / side_lengths[2];
		S4 = side_4_data[3] / side_lengths[3];
		if (fabs(S1) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S1);
		} else if (fabs(S2) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S2);
		} else if (fabs(S3) == fmin(fmin(fabs(S1), fabs(S2)), fmin(fabs(S3), fabs(S4)))) {
			FH_result.push_back(S3);
		} else {
			FH_result.push_back(S4);
		}
		//	CBUSH Forces
		std::vector<Euclidean_Vector> CBUSH1_Forces, CBUSH2_Forces;
		// Generate transformation from CBUSH orientation to a projected material orientation vector
		Transformation mat_transformation;
		if (!CBUSH_as_is) {
			Euclidean_Vector mat_vector = side_1[side_no][0]->get_material_vector();
			mat_vector = pTransformation[0].transform_vector(mat_vector);
			switch (CBUSH_axis[0]) {
				case 12: case 13:
					mat_transformation.build_matrix(Euclidean_Vector(1.0, 0.0, 0.0), mat_vector, CBUSH_axis[0]);
					break;
				case 21: case 23:
					mat_transformation.build_matrix(Euclidean_Vector(0.0, 1.0, 0.0), mat_vector, CBUSH_axis[0]);
					break;
				default:
					mat_transformation.build_matrix(Euclidean_Vector(0.0, 0.0, 1.0), mat_vector, CBUSH_axis[0]);
			}
		}
		// Get forces from adjacent CBUSH if it exists
		if (other_CBUSH != NULL) {
			CBUSH2_Forces = other_CBUSH->getForceResult(LOAD_CASE_it->first, false, 0, COORD_Map, GRID_Map);
			if (CBUSH2[1]) {
				CBUSH2_Forces[0] = CBUSH2_Forces[0] * -1.0;
			}
			// Translate into CBUSH1 system
			set_transform(COORD_Map, GRID_Map);
			CBUSH2_Forces[0] = pTransformation[0].transform_vector(CBUSH2_Forces[0]);
			if (!CBUSH_as_is) {
				CBUSH2_Forces[0] = mat_transformation.transform_vector(CBUSH2_Forces[0]);
			}
		} else {
			CBUSH2_Forces.push_back(Euclidean_Vector(0.0, 0.0, 0.0));
		}
		// Get forces for CBUSH element
		// As is
		CBUSH1_Forces = getForceResult(LOAD_CASE_it->first, true, 0, COORD_Map, GRID_Map);
		// Determine pull through load
		double pull_through = 0.0;
		if (other_CBUSH == NULL) {
			// Determine which component is the fastener axial direction
			switch (CBUSH_axis[0]) {
				case 12: case 13:
					pull_through = CBUSH1_Forces[0][0];
					break;
				case 21: case 23:
					pull_through = CBUSH1_Forces[0][1];
					break;
				default:
					pull_through = CBUSH1_Forces[0][2];
			}
			// Set pull through load if fastener is in tension
			if ((CBUSH1[0] && pull_through > 0.0) || (!CBUSH1[0] && pull_through < 0.0)) {
				pull_through = fabs(pull_through);
			} else {
				pull_through = 0.0;
			}
		}
		if (CBUSH1[1]) {
			CBUSH1_Forces[0] = CBUSH1_Forces[0] * -1.0;
		}
		if (!CBUSH_as_is) {
			CBUSH1_Forces[0] = mat_transformation.transform_vector(CBUSH1_Forces[0]);
		}
		// Add CBUSH2 to CBUSH1
		CBUSH1_Forces[0] = CBUSH1_Forces[0] + CBUSH2_Forces[0];
		// Push back fastener loads to FH_result
		switch (CBUSH_axis[0]) {
			case 12:
				FH_result.push_back(CBUSH1_Forces[0][1]);
				FH_result.push_back(CBUSH1_Forces[0][2]);
				break;
			case 13:
				FH_result.push_back(CBUSH1_Forces[0][2]);
				FH_result.push_back(CBUSH1_Forces[0][1]);
				break;
			case 21:
				FH_result.push_back(CBUSH1_Forces[0][0]);
				FH_result.push_back(CBUSH1_Forces[0][2]);
				break;
			case 23:
				FH_result.push_back(CBUSH1_Forces[0][2]);
				FH_result.push_back(CBUSH1_Forces[0][0]);
				break;
			case 31:
				FH_result.push_back(CBUSH1_Forces[0][0]);
				FH_result.push_back(CBUSH1_Forces[0][1]);
				break;
			default:
				FH_result.push_back(CBUSH1_Forces[0][1]);
				FH_result.push_back(CBUSH1_Forces[0][0]);
		}

		FH_result.push_back(pull_through);
		// Insert into map
		FH_Res_Map[side_no].insert(std::pair<unsigned long, std::vector<double> > (LOAD_CASE_it->first, FH_result));	
	}

}


std::vector<double> CBUSH::get_FH_Result(unsigned long SubcaseID, int side_no)
{
	std::vector<double> returnVector;
	if (FH_Res_Map[side_no].find(SubcaseID) != FH_Res_Map[side_no].end()) {
		returnVector = FH_Res_Map[side_no][SubcaseID];
	} else {
		for (int i = 0; i < 9; i++) {
			returnVector.push_back(0.0);
		}
	}
	return returnVector;
}

std::vector<unsigned long> CBUSH::get_FH_CQUADs(int side_no)
{
	if (!pNodeSort) {
		for (int i = 0; i < 2; i++) {
			node_QUADS[i].clear();
			for (int j = 0; j < side_1[i].size(); j++) {
				node_QUADS[i].push_back((*side_1[i][j])[0]);
			}
			for (int j = 0; j < side_2[i].size(); j++) {
				node_QUADS[i].push_back((*side_2[i][j])[0]);
			}
			for (int j = 0; j < side_3[i].size(); j++) {
				node_QUADS[i].push_back((*side_3[i][j])[0]);
			}
			for (int j = 0; j < side_4[i].size(); j++) {
				node_QUADS[i].push_back((*side_4[i][j])[0]);
			}
		}
		pNodeSort = true;
	}
	return node_QUADS[side_no];
}

bool CBUSH::FH_Composite(int side_no, std::map<unsigned long, CQUAD*> &CQUAD_Map)
{
	for (int i = 0; i < node_QUADS[side_no].size(); i++) {
		if (CQUAD_Map.find(node_QUADS[side_no][i]) != CQUAD_Map.end()) {
			return CQUAD_Map[node_QUADS[side_no][i]]->is_Composite();
		}
	}
	return false;
}

/**
 *	@brief	Overload the [] operator to provide direct access to the class variables. Element ID 
 *			is returned as default.
 *
 *			i = 0 -> Element ID
 *			i = 1 -> Property ID
 *			i = 2 -> Grid A ID
 *			i = 3 -> Grid B ID
 *			i = 4 -> Fastener Node A ID
 *			i = 5 -> Fastener Node B ID
 *			i = 6 -> OCID
 *
 *	@param	i, integer parameter designating position
 *	@return	unsigned long, returned class variable
 */

unsigned long CBUSH::operator[] (unsigned int i)
{
	if (i < 4) {
		return pLongData[i];
	} else if (i < 6) {
		return fastenerNodes[i - 4];
	} else if (i < 7) {
		return pLongData[i];
	} else {
		return pLongData[0];
	}
}

std::string CBUSH::check_exp(std::string& str)
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

bool CBUSH::checkReal(std::string &str)
{
	std::size_t found = str.find_first_of('.');
	if (found == std::string::npos) {
		return false;
	} else {
		return true;
	}
}

bool sort_x(std::pair<Coordinate, CQUAD*> first_CQUAD, std::pair<Coordinate, CQUAD*> second_CQUAD) {
	return (first_CQUAD.first[0] < second_CQUAD.first[0]);
}

bool sort_y(std::pair<Coordinate, CQUAD*> first_CQUAD, std::pair<Coordinate, CQUAD*> second_CQUAD) {
	return (first_CQUAD.first[1] < second_CQUAD.first[1]);
}

bool sort_z(std::pair<Coordinate, CQUAD*> first_CQUAD, std::pair<Coordinate, CQUAD*> second_CQUAD) {
	return (first_CQUAD.first[2] < second_CQUAD.first[2]);
}

/**
 *	@brief	Function orders the outer shell of the CQUAD elements into the 4 
 *			sides that are used in the FH analysis.
 *
 *	@param	GRID_Map, a map of all GRIDs in the NASTRAN model
 * 	@param 	CQUAD_Map, a map of all the CQUAD elements in the NASTRAN model
 *	@param	COORD_Map, a map of all the COORD systems in the NASTRAN model
 *	@param	iterations, int of the number of iterations to reach outer shell
 *	@return	void
 */

void CBUSH::order_FH(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, COORD*> &COORD_Map, int &grid_size, int &side_no, unsigned int &axis_type)
{
	int standardSize = grid_size;								///< int defines the standard size of a side
	std::vector<std::pair<Coordinate, CQUAD*> > all_elements;		///< Vector stores all centroid and pointer pairs for the outer ring of elements
	std::vector<std::pair<Coordinate, CQUAD*> > side_elements;		///< Vector stores the centroid and pointer pairs for a side
	//	Get centroids for all outer CQUAD elements in the CBUSH CID
	if (pOrienType == 1 || pOrienType == 2) {
		set_transform(COORD_Map, GRID_Map);
	}
	Coordinate retrieved_coord, fastener_node_coord(0.0, 0.0, 0.0);
	for (int j = 0; j < node_QUADS[side_no].size(); j++) {
		if (CQUAD_Map.find(node_QUADS[side_no][j]) != CQUAD_Map.end()) {
			switch (pOrienType) {
				case 1: case 2:
					retrieved_coord = CQUAD_Map[node_QUADS[side_no][j]]->getCentroid(0, COORD_Map);
					if (GRID_Map.find(fastenerNodes[side_no]) != GRID_Map.end()) {
						fastener_node_coord = GRID_Map[fastenerNodes[side_no]]->get_coordinate(0, COORD_Map);
					}
					retrieved_coord -= fastener_node_coord;
					retrieved_coord = pTransformation[0].transform_coordinate(retrieved_coord);
					all_elements.push_back(std::pair<Coordinate, CQUAD*> (retrieved_coord, CQUAD_Map[node_QUADS[side_no][j]]));
					break;
				default:
					retrieved_coord = CQUAD_Map[node_QUADS[side_no][j]]->getCentroid(pLongData[5], COORD_Map);
					all_elements.push_back(std::pair<Coordinate, CQUAD*> (retrieved_coord, CQUAD_Map[node_QUADS[side_no][j]]));
			}

		}
	}
	bool continue_flag = true;
	pNodeSort = false;
	// Check size of all_elements vector
	if (all_elements.size() != (standardSize * 4 - 4)) {
		//	all_elements vector does not meet size requirements
		all_elements.clear();
		continue_flag = false;
	}
	if (continue_flag) {
		//
		//	Process side 1
		//
		if (axis_type == 23 || axis_type == 32) { std::sort(all_elements.begin(), all_elements.end(), sort_x); }
		else if (axis_type == 13 || axis_type == 31) { std::sort(all_elements.begin(), all_elements.end(), sort_y); }
		else { std::sort(all_elements.begin(), all_elements.end(), sort_z); }
		for (int j = 0; j < standardSize; j++) { side_elements.push_back(all_elements[j]); }
		//	Order the side elements by the secondary direction
		if (axis_type == 21 || axis_type == 31) { std::sort(side_elements.begin(), side_elements.end(), sort_x); }
		else if (axis_type == 12 || axis_type == 32) { std::sort(side_elements.begin(), side_elements.end(), sort_y); }
		else { std::sort(side_elements.begin(), side_elements.end(), sort_z); }
		for (int j = 0; j < side_elements.size(); j++) { side_1[side_no].push_back(side_elements[j].second); }
		side_elements.clear();
		//
		//	Process side 3 -> Top Side
		//
		for (int j = all_elements.size() - 1; j > all_elements.size() - 1 - standardSize; j--) { side_elements.push_back(all_elements[j]); }
		if (axis_type == 21 || axis_type == 31) { std::sort(side_elements.begin(), side_elements.end(), sort_x); }
		else if (axis_type == 12 || axis_type == 32) { std::sort(side_elements.begin(), side_elements.end(), sort_y); }
		else { std::sort(side_elements.begin(), side_elements.end(), sort_z); }
		for (int j = side_elements.size() - 1; j > -1; j--) { side_3[side_no].push_back(side_elements[j].second); } 	
		side_elements.clear();
		//
		//	Process side 2 -> Right Side
		//
		if (axis_type == 21 || axis_type == 31) { std::sort(all_elements.begin(), all_elements.end(), sort_x); }
		else if (axis_type == 12 || axis_type == 32) { std::sort(all_elements.begin(), all_elements.end(), sort_y); }
		else { std::sort(all_elements.begin(), all_elements.end(), sort_z); }
		for (int j = all_elements.size() - 1; j > all_elements.size() - 1 - standardSize; j--) { side_elements.push_back(all_elements[j]); }
		//	Order the side elements by the second direction
		if (axis_type == 23 || axis_type == 32) { std::sort(side_elements.begin(), side_elements.end(), sort_x); }
		else if (axis_type == 13 || axis_type == 31) { std::sort(side_elements.begin(), side_elements.end(), sort_y); }
		else { std::sort(side_elements.begin(), side_elements.end(), sort_z); }
		for (int j = 0; j < side_elements.size(); j++) { side_2[side_no].push_back(side_elements[j].second); }
		side_elements.clear();
		//
		//	Process side 4 -> Left Side
		//
		for (int j = 0; j < standardSize; j++) { side_elements.push_back(all_elements[j]); }
		if (axis_type == 23 || axis_type == 32) { std::sort(side_elements.begin(), side_elements.end(), sort_x); }
		else if (axis_type == 13 || axis_type == 31) { std::sort(side_elements.begin(), side_elements.end(), sort_y); }
		else { std::sort(side_elements.begin(), side_elements.end(), sort_z); }
		for (int j = side_elements.size() - 1; j > -1; j--) { side_4[side_no].push_back(side_elements[j].second); }  
	}


}

void CBUSH::set_transform(std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map){
	if (!pTransFlag) {
		Euclidean_Vector vector_x = get_vector(0, 0, COORD_Map, GRID_Map);
		Euclidean_Vector vector_y = get_vector(1, 0, COORD_Map, GRID_Map);
		Euclidean_Vector vector_z = get_vector(2, 0, COORD_Map, GRID_Map);
		pTransformation[0].build_matrix(vector_x, vector_y, vector_z);
		pTransformation[1] = pTransformation[0].inverse_matrix();
		pTransFlag= true;
	}
}

