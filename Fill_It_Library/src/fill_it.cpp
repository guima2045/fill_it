#include "FH_MODEL.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <stdlib.h> 

//using namespace std;

std::string trim(std::string &str) {
    std::size_t first = str.find_first_not_of(' ');
	std::string tempStr = str;
    if (first == std::string::npos) {
		return "";
	} else {
		while (tempStr.find_first_of(' ') != std::string::npos) {
			tempStr.erase(tempStr.find_first_of(' '), 1);
		}
	}
    return tempStr;
}

std::vector<std::string> parse_comma(std::string &str) {
	std::vector<std::string> return_vector;
	std::size_t first = str.find_first_not_of(',');
	if (first == std::string::npos) {
		return return_vector;
	}
	std::size_t second = str.find_first_of(',', first);
	while (second != std::string::npos) {
		return_vector.push_back(str.substr(first, second - first));
		first = second + 1;
		second = str.find_first_of(',', first);
	}
	return_vector.push_back(str.substr(first, second - first));
	return return_vector;
}

std::string remove_file_extension(std::string &str) {
	std::size_t last = str.find_last_of('.');
	std::string return_string = str.substr(0, last);
	return return_string;
}

int main(int argc, char* argv[])
{
	std::vector<unsigned long> NODE_List, Axis_Check;
	std::vector<unsigned int> Axis_List_1, Axis_List_2;
	std::vector<int> Iterations;
	std::vector<bool> Mat_Orientation, AS_IS;
	bool Composite;

	//	Check if control file provided in arguments
	if (argc == 1) {
		std::cout << "No control file entered" << std::endl;
		return 0;
	}
	//	Open control file
	std::fstream control_file;
	control_file.open(argv[1], std::ios_base::in);
	//	Get parameters from control file
	std::string input_file = "HELLO WORLD", all_param;
	std::vector<std::string> punch_files, group_files;
	std::vector<unsigned long> subcases;
	std::string input_line;
	if (control_file.is_open()) {		
		bool input_flag = false, punch_flag = false, group_flag = false, all_flag = false, subcase_flag = false;
		while (!control_file.eof()) {
			std::getline(control_file, input_line);
			input_line = trim(input_line);
			if (input_flag) {
				if (input_line.compare("PUNCH") != 0 && input_line.compare("GROUP") != 0 && input_line.compare("ALL") != 0 && input_line.compare("") != 0 && input_line.compare("SUBCASES") != 0) {
					input_file = input_line;
					input_flag = false;
					continue;
				} else {
					input_flag = false;
				}
			}
			if (punch_flag) {
				if (input_line.compare("INPUT") != 0 && input_line.compare("GROUP") != 0 && input_line.compare("ALL") != 0 && input_line.compare("") != 0 && input_line.compare("SUBCASES") != 0) {
					punch_files.push_back(input_line);
					continue;
				} else {
					punch_flag = false;
				}			
			}
			if (group_flag) {
				if (input_line.compare("INPUT") != 0 && input_line.compare("PUNCH") != 0 && input_line.compare("ALL") != 0 && input_line.compare("") != 0 && input_line.compare("SUBCASES") != 0) {
					group_files.push_back(input_line);
					continue;
				} else {
					group_flag = false;
				}			
			}
			if (all_flag) {
				if (input_line.compare("PUNCH") != 0 && input_line.compare("GROUP") != 0 && input_line.compare("INPUT") != 0 && input_line.compare("") != 0 && input_line.compare("SUBCASES") != 0) {
					all_param = input_line;
					all_flag = false;
					continue;
				} else {
					all_flag = false;
				}
			}
			if (subcase_flag) {
				if (input_line.compare("PUNCH") != 0 && input_line.compare("GROUP") != 0 && input_line.compare("INPUT") != 0 && input_line.compare("") != 0 && input_line.compare("ALL") != 0) {
					std::vector<std::string> data_vector = parse_comma(input_line);
					for (int i = 0; i < data_vector.size(); i++) {
						subcases.push_back(atol(trim(data_vector[i]).c_str()));
					}
					continue;
				} else {
					subcase_flag = false;
				}
			}
			if (input_line.compare("INPUT") == 0) {
				input_flag = true;
				continue;
			}
			if (input_line.compare("PUNCH") == 0) {
				punch_flag = true;
				continue;
			}
			if (input_line.compare("GROUP") == 0) {
				group_flag = true;
				continue;
			}
			if (input_line.compare("ALL") == 0) {
				all_flag = true;
				continue;
			}
			if (input_line.compare("SUBCASES") ==0) {
				subcase_flag = true;
				continue;
			}
		}
	} else {
		std::cout << "Unable to open control file: " << argv[1] << std::endl;
		return 0;
	}
	control_file.close();
	if (input_file.compare("HELLO WORLD") == 0) {
		std::cout << "No input file defined in control file" << std::endl;
		return 0;
	}
	if (punch_files.size() == 0) {
		std::cout << "No punch files entered" << std::endl;
		return 0;
	}
	//	Load model	
	FH_MODEL myModel(input_file);
	//	Read Punch Files
	for (int i = 0; i < punch_files.size(); i++) {
		myModel.parsePunchFile(punch_files[i]);
	}
	//	Prepare FH grids
	myModel.generate_FH_List();
	
	std::vector<std::string> data_vector;
	if (group_files.size() == 0) {
		//All CBUSH are selected to parse
		data_vector = parse_comma(all_param);
		if (data_vector.size() > 4) {
			if (trim(data_vector[0]).compare("COMP") == 0) {
				Composite = true;
			} else {
				Composite = false;
			}
			Iterations.push_back(atoi(trim(data_vector[1]).c_str()));
			if (trim(data_vector[2]).compare("MAT") == 0) {
				Mat_Orientation.push_back(true);
			} else {
				Mat_Orientation.push_back(false);
			}						
			if (trim(data_vector[3]).compare("MAT") == 0) {
				AS_IS.push_back(false);
			} else {
				AS_IS.push_back(true);
			}
			Axis_List_1.push_back(atoi(trim(data_vector[4]).c_str()));
			Axis_List_2.push_back(Axis_List_1[0]);
			// Enter
			std::cout << "Calculating FH Results" << std::endl;
			myModel.calculate_FH_elements(NODE_List, Axis_List_1, Axis_List_2, Iterations, Composite, Mat_Orientation, AS_IS, Axis_Check);
			std::cout << "Writing FH Results to: FH_Data.res" << std::endl;
			myModel.generate_FH_output("FH_Data.res", NODE_List, subcases, Iterations, Composite);
		} else {
			std::cout << "Unable to parse parameters in control file" << std::endl;
			return 0;
		}

	} else {
		// Multiple fastener groups selected
		for (int i = 0; i < group_files.size(); i++) {
			//	Open control file
			std::fstream group_file;
			group_file.open(group_files[i].c_str(), std::ios_base::in);
			if (group_file.is_open()) {
				while (!group_file.eof()) {
					std::getline(group_file, input_line);
					data_vector = parse_comma(input_line);
					if (data_vector.size() > 6) {
						NODE_List.push_back(atol(trim(data_vector[0]).c_str()));
						Iterations.push_back(atoi(trim(data_vector[1]).c_str()));
						if (trim(data_vector[2]).compare("MAT") == 0) {
							Mat_Orientation.push_back(true);
						} else {
							Mat_Orientation.push_back(false);
						}
						if (trim(data_vector[3]).compare("MAT") == 0) {
							AS_IS.push_back(false);
						} else {
							AS_IS.push_back(true);
						}
						if (trim(data_vector[4]).compare("SINGLE") == 0) {
							Axis_List_1.push_back(atoi(trim(data_vector[5]).c_str()));
							Axis_List_2.push_back(Axis_List_1[0]);
							Axis_Check.push_back(0);
						} else {
							// Dual CBUSH configuration
							Axis_Check.push_back(atol(trim(data_vector[4]).c_str()));
							Axis_List_1.push_back(atoi(trim(data_vector[5]).c_str()));
							Axis_List_2.push_back(atoi(trim(data_vector[6]).c_str()));						
						}				
					}
				}
			} else {
				std::cout << "Unable to open group file: " << group_files[i] << std::endl;
			}
			group_file.close();
			
			if (NODE_List.size() > 0) {
				std::cout << "Calculating FH Results for: " << group_files[i] << std::endl;
				myModel.calculate_FH_elements(NODE_List, Axis_List_1, Axis_List_2, Iterations, Composite, Mat_Orientation, AS_IS, Axis_Check);
				std::string output_file_name = remove_file_extension(group_files[i]);
				output_file_name.append(".res");
				std::cout << "Writing FH Results to: " << output_file_name << std::endl;
				myModel.generate_FH_output(output_file_name, NODE_List, subcases, Iterations, Composite);
			} else {
				std::cout << "Unable to parse group file: " << group_files[i] << std::endl;
			}
			NODE_List.clear();
			Iterations.clear();
			Mat_Orientation.clear();
			AS_IS.clear();
			Axis_List_1.clear();
			Axis_List_2.clear();
			Axis_Check.clear();
		}

	}
	std::cout << "Parsing complete, press ENTER to exit...";
	std::cin.get();
	
	return 0;

}
