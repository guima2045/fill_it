#ifndef FH_MODEL_H
#define FH_MODEL_H

#include "NASTRAN_General/BASIC_MODEL.h"

#include <string>
#include <fstream>

class FH_MODEL: public BASIC_MODEL
{
	public:
		FH_MODEL(std::string fileName);
		void generate_FH_List();
		void calculate_FH_elements(std::vector<unsigned long> &NODE_List, std::vector<unsigned int> &Axis_List_1, 
				std::vector<unsigned int> &Axis_List_2, std::vector<int> &Iterations, bool &Composite, 
				std::vector<bool> &Mat_Orientation, std::vector<bool> &AS_IS, std::vector<unsigned long> &Axis_Check);
		void generate_FH_output(std::string output_file, std::vector<unsigned long> &NODE_List, std::vector<unsigned long> &SUBCASE_List, 
				std::vector<int> &Iterations, bool Composite);
	private:
		std::map<unsigned long, std::vector<CBUSH*> > FH_Map_Node_A, FH_Map_Node_B;
		void print_result(std::fstream &res_file, std::map<unsigned long, std::vector<CBUSH*> >::iterator &Map_it, int side_no, LOAD_CASE* Load_Case_Pointer, int &max_CQUADs);
		std::string ulong_to_str(unsigned long Num);
		std::string int_to_str(int Num);
		std::string double_to_str(double Num);
};


#endif // FH_MODEL_H
