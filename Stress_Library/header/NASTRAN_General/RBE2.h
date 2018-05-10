/**
 *
 * @file		RBE2.h
 * @author		Matt Guimarey
 * @date		03/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN RBE2 element
 *
 */

#ifndef RBE2_H
#define RBE2_H

#include "GRID.h"

#include <vector>
#include <string>
#include <map>

class RBE2
{
	public:
		RBE2();

		// 	Constructor parsing bdf data
		RBE2(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~RBE2();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		//	Get number of dependant nodes
		int get_num_dependant_nodes();	

		//	Return the thermal expansion coefficient
		double get_TEC();

		//	Updates GRID entry with connectivity data -> sets this RBE2 ID as connected to GRID	
		void nodeConnect(std::map<unsigned long, GRID*> &GRID_Map);

		/*	Operator [] provides access to unsigned long data: i = 0/default -> RBE2 ID, 1 -> Independant 
			Node ID, 2 -> DOF, 3... -> Dependant Nodes */
		unsigned long operator[](unsigned int i);

	private:
		unsigned long pLongData[3];						///< unsigned long data array storing class data (RBE_ID, Ind Node, DOF)
		std::vector<unsigned long> pDependantNodes;		///< unsigned long vector containing list of dependant nodes
		double pThermal;								///< double for thermal expansion coefficient


		std::string check_exp(std::string &str);
		bool checkReal(std::string &str);
};

#endif // RBE2_H
