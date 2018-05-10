
#include "FH_MODEL.h"
#include "NASTRAN_General/CBUSH.h"
#include "NASTRAN_General/BASIC_MODEL.h"

#include <string>
#include <map>
#include <iostream>
#include <fstream>
#include <sstream>

/*
FH_MODEL::FH_MODEL() : BASIC_MODEL()
{

}

FH_MODEL::FH_MODEL() : BASIC_MODEL()
{

}
*/

FH_MODEL::FH_MODEL(std::string fileName) : BASIC_MODEL(fileName)
{

}

void FH_MODEL::generate_FH_List()
{
	std::vector<CBUSH*> entry_vector;
	unsigned long entry_long;
	std::pair<std::map<unsigned long, std::vector<CBUSH*> >::iterator, bool> ret;
	for (CBUSH_it = CBUSH_Map.begin(); CBUSH_it != CBUSH_Map.end(); CBUSH_it++) {
		entry_vector.push_back(CBUSH_it->second);
		entry_long = (*CBUSH_it->second)[4];
		ret = FH_Map_Node_A.insert(std::pair <unsigned long, std::vector<CBUSH*> > (entry_long, entry_vector));
		if (!ret.second) {
			ret.first->second.push_back(CBUSH_it->second);
		}
		entry_long = (*CBUSH_it->second)[5];
		ret = FH_Map_Node_B.insert(std::pair <unsigned long, std::vector<CBUSH*> > (entry_long, entry_vector));
		if (!ret.second) {
			ret.first->second.push_back(CBUSH_it->second);
		}
		entry_vector.clear();
	}
	// Check for connected CBUSH elements
	for (std::map<unsigned long, std::vector<CBUSH*> >::iterator Map_A_it = FH_Map_Node_A.begin(); Map_A_it != FH_Map_Node_A.end(); Map_A_it++) {
		// Search for Map_B grid in Map_A
		std::map<unsigned long, std::vector<CBUSH*> >::iterator Map_B_it = FH_Map_Node_B.find(Map_A_it->first);
		if (Map_B_it != FH_Map_Node_B.end()) {
			// Found, remove from Map_B and add to Map_A
			Map_A_it->second.push_back(Map_B_it->second[0]);
			FH_Map_Node_B.erase(Map_B_it);
		}
	}
}

void FH_MODEL::calculate_FH_elements(std::vector<unsigned long> &NODE_List, std::vector<unsigned int> &Axis_List_1, 
				std::vector<unsigned int> &Axis_List_2, std::vector<int> &Iterations, bool &Composite, 
				std::vector<bool> &Mat_Orientation, std::vector<bool> &AS_IS, std::vector<unsigned long> &Axis_Check)
{
	std::map<unsigned long, std::vector<CBUSH*> >::iterator Map_it;
	// Check if a selection of CBUSH elements are provided, if not then all are requested
	unsigned int axis_array[2];
	if (NODE_List.size() == 0) {
		// All selected
		axis_array[0] = Axis_List_1[0];
		axis_array[1] = Axis_List_2[0];
		for (Map_it = FH_Map_Node_A.begin(); Map_it != FH_Map_Node_A.end(); Map_it++) {
			Map_it->second[0]->parse_FH(GRID_Map, CQUAD_Map, RBE2_Map, RBE3_Map, COORD_Map, Iterations[0], 0, Axis_List_1[0]);
			if (Map_it->second.size() > 1) {
				Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, Map_it->second[1], 0, 
								Mat_Orientation[0], AS_IS[0], axis_array);
			} else {
				Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, NULL, 0, 
								Mat_Orientation[0], AS_IS[0], axis_array);
			}				
		}
		for (Map_it = FH_Map_Node_B.begin(); Map_it != FH_Map_Node_B.end(); Map_it++) {
			Map_it->second[0]->parse_FH(GRID_Map, CQUAD_Map, RBE2_Map, RBE3_Map, COORD_Map, Iterations[0], 1, Axis_List_1[0]);
			if (Map_it->second.size() > 1) {
				Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, Map_it->second[1], 1, 
								Mat_Orientation[0], AS_IS[0], axis_array);
			} else {
				Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, NULL, 1, 
								Mat_Orientation[0], AS_IS[0], axis_array);
			}
		}
	} else {
		//	Individual nodes selected
		//	Loop through selected nodes
		
		for (int i = 0; i < NODE_List.size(); i++) {
			Map_it = FH_Map_Node_A.find(NODE_List[i]);
			if (Map_it != FH_Map_Node_A.end()) {
				// Arrange axis orientation
				if ((*Map_it->second[0])[0] ==  Axis_Check[i]) {
					// Matches first
					axis_array[0] = Axis_List_1[i];
					axis_array[1] = Axis_List_2[i];
				} else {
					axis_array[1] = Axis_List_1[i];
					axis_array[0] = Axis_List_2[i];	
				}

				Map_it->second[0]->parse_FH(GRID_Map, CQUAD_Map, RBE2_Map, RBE3_Map, COORD_Map, Iterations[i], 0, Axis_List_1[i]);
				if (Map_it->second.size() > 1) {
					Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, Map_it->second[1], 0, 
									Mat_Orientation[i], AS_IS[i], axis_array);
				} else {
					Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, NULL, 0, 
									Mat_Orientation[i], AS_IS[i], axis_array);
				}				
			}
			Map_it = FH_Map_Node_B.find(NODE_List[i]);
			if (Map_it != FH_Map_Node_B.end()) {
				// Arrange axis orientation
				if ((*Map_it->second[0])[0] ==  Axis_Check[i]) {
					// Matches first
					axis_array[0] = Axis_List_1[i];
					axis_array[1] = Axis_List_2[i];
				} else {
					axis_array[1] = Axis_List_1[i];
					axis_array[0] = Axis_List_2[i];	
				}
				Map_it->second[0]->parse_FH(GRID_Map, CQUAD_Map, RBE2_Map, RBE3_Map, COORD_Map, Iterations[i], 1, Axis_List_1[i]);
				if (Map_it->second.size() > 1) {
					Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, Map_it->second[1], 1, 
									Mat_Orientation[i], AS_IS[i], axis_array);
				} else {
					Map_it->second[0]->generate_FH_Result(CQUAD_Map, COORD_Map, GRID_Map, LOAD_CASE_Map, CBUSH_Map, NULL, 1, 
									Mat_Orientation[i], AS_IS[i], axis_array);
				}				
			}
		}	
	}
}

void FH_MODEL::generate_FH_output(std::string output_file, std::vector<unsigned long> &NODE_List, 
		std::vector<unsigned long> &SUBCASE_List, std::vector<int> &Iterations, bool Composite)
{
	std::map<unsigned long, std::vector<CBUSH*> >::iterator Map_it;
	std::fstream res_file;
	res_file.open(output_file.c_str(), std::ios_base::out);
	int max_size = 0;
	for (int i = 0; i < Iterations.size(); i++) {
		int check_value = Iterations[i] * 4 - 4;
		if (check_value > max_size) {
			max_size = check_value;
		}
	}
	if (!res_file.is_open()) {
		std::cout << "Error opening results file: " << output_file << std::endl;
		return;
	}
	res_file << "Subcase,Subtitle,Grid,CBUSH_1,CBUSH_2,Nxx,Nyy,Nxy,Mxx,Myy,Mxy,Fx,Fy,Fz";
	for (int i = 0; i < max_size; i++) {
		res_file << ",CQUAD" << int_to_str(i + 1);
	}
	res_file << std::endl;
	if (NODE_List.size() > 0) {
		for (int i = 0; i < NODE_List.size(); i++) {
			// Node A
			Map_it = FH_Map_Node_A.find(NODE_List[i]);
			if (Map_it != FH_Map_Node_A.end()) {
				if (SUBCASE_List.size() > 0) {
					for (int j = 0; j < SUBCASE_List.size(); j++) {
						LOAD_CASE_it = LOAD_CASE_Map.find(SUBCASE_List[j]);
						if (LOAD_CASE_it != LOAD_CASE_Map.end()) {
							// Load Case found
							print_result(res_file, Map_it, 0, LOAD_CASE_it->second, max_size);
							res_file << std::endl;
						}
					}
				} else {
					// Iterate through load cases
					for (LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
						print_result(res_file, Map_it, 0, LOAD_CASE_it->second, max_size);
						res_file << std::endl;					
					}
				}
			} else {
				// Node B
				Map_it = FH_Map_Node_B.find(NODE_List[i]);
				if (Map_it != FH_Map_Node_B.end()) {
					if (SUBCASE_List.size() > 0) {
						for (int j = 0; j < SUBCASE_List.size(); j++) {
							LOAD_CASE_it = LOAD_CASE_Map.find(SUBCASE_List[j]);
							if (LOAD_CASE_it != LOAD_CASE_Map.end()) {
								// Load Case found
								print_result(res_file, Map_it, 1, LOAD_CASE_it->second, max_size);
								res_file << std::endl;
							}
						}
					} else {
						// Iterate through load cases
						for (LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
							print_result(res_file, Map_it, 1, LOAD_CASE_it->second, max_size);
							res_file << std::endl;					
						}
					}
				}
			}
		}
	} else {
		// Specific GRID points not selected, iterate through all grid points
		bool cont_flag = false;
		for (Map_it = FH_Map_Node_A.begin(); Map_it != FH_Map_Node_A.end(); Map_it++) {
			if (Composite) {
				// Only composite results selected
				cont_flag = Map_it->second[0]->FH_Composite(0, CQUAD_Map);
			} else {
				cont_flag = true;
			}
			if (cont_flag) {
				if (SUBCASE_List.size() > 0) {
					for (int j = 0; j < SUBCASE_List.size(); j++) {
						LOAD_CASE_it = LOAD_CASE_Map.find(SUBCASE_List[j]);
						if (LOAD_CASE_it != LOAD_CASE_Map.end()) {
							// Load Case found
							print_result(res_file, Map_it, 0, LOAD_CASE_it->second, max_size);
							res_file << std::endl;
						}
					}
				} else {
					// Iterate through load cases
					for (LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
						print_result(res_file, Map_it, 0, LOAD_CASE_it->second, max_size);
						res_file << std::endl;					
					}
				}				
			}
		}
		for (Map_it = FH_Map_Node_B.begin(); Map_it != FH_Map_Node_B.end(); Map_it++) {
			if (Composite) {
				// Only composite results selected
				cont_flag = Map_it->second[0]->FH_Composite(1, CQUAD_Map);
			} else {
				cont_flag = true;
			}			
			if (cont_flag) {
				if (SUBCASE_List.size() > 0) {
					for (int j = 0; j < SUBCASE_List.size(); j++) {
						LOAD_CASE_it = LOAD_CASE_Map.find(SUBCASE_List[j]);
						if (LOAD_CASE_it != LOAD_CASE_Map.end()) {
							// Load Case found
							print_result(res_file, Map_it, 1, LOAD_CASE_it->second, max_size);
							res_file << std::endl;
						}
					}
				} else {
					// Iterate through load cases
					for (LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
						print_result(res_file, Map_it, 1, LOAD_CASE_it->second, max_size);
						res_file << std::endl;					
					}
				}				
			}
		}
	}



}

void FH_MODEL::print_result(std::fstream &res_file, std::map<unsigned long, std::vector<CBUSH*> >::iterator &Map_it, int side_no, LOAD_CASE* Load_Case_Pointer, int &max_CQUADs)
{
	// Add subcase and subtitle
	res_file << ulong_to_str(Load_Case_Pointer->getSubcaseID()) << "," << Load_Case_Pointer->getSubtitle();
	// Grid and CBUSH IDs
	res_file << "," << ulong_to_str(Map_it->first) << "," << ulong_to_str((*Map_it->second[0])[0]);
	if (Map_it->second.size() > 1) {
		res_file << "," << ulong_to_str((*Map_it->second[0])[1]);
	} else {
		res_file << ",N/A";
	}
	// Get FH results
	std::vector<double> FH_Result = Map_it->second[0]->get_FH_Result(Load_Case_Pointer->getSubcaseID(), side_no);
	// Add FH result to line
	for (int i = 0; i < FH_Result.size(); i++) {
		res_file << "," << double_to_str(FH_Result[i]);
	}
	// Get CQUAD IDs
	std::vector<unsigned long> FH_CQUADs = Map_it->second[0]->get_FH_CQUADs(side_no);
	for (int i = 0; i < max_CQUADs; i++) {
		if (i < FH_CQUADs.size()) {
			res_file << "," << ulong_to_str(FH_CQUADs[i]);
		} else {
			res_file << ",-";
		}
	}
}

std::string FH_MODEL::ulong_to_str(unsigned long Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}

std::string FH_MODEL::int_to_str(int Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}

std::string FH_MODEL::double_to_str(double Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}

