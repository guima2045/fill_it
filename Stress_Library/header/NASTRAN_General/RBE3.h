/**
 *
 * @file		RBE2.h
 * @author		Matt Guimarey
 * @date		20/09/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN RBE3 element as used by Airbus in fastener modelling
 *
 */

#ifndef RBE3_H
#define RBE3_H

#include "GRID.h"

#include <vector>
#include <string>
#include <map>

class RBE3
{
	public:
		RBE3();

		// 	Constructor parsing bdf data
		RBE3(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~RBE3();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		//	Get number of dependant nodes
		int get_num_independant_nodes();	

		//	Return the thermal expansion coefficient
		double get_TEC();

		//	Updates GRID entry with connectivity data -> sets this RBE2 ID as connected to GRID	
		void nodeConnect(std::map<unsigned long, GRID*> &GRID_Map);

		/*	Operator [] provides access to unsigned long data: i = 0/default -> RBE3 ID, 1 -> Dependant 
			Node ID, 2 -> DOF, 3... -> Independent Nodes */
		unsigned long operator[](unsigned int i);

	private:
		unsigned long pLongData[3];	///< unsigned long data array storing class data (RBE_ID, Dep Node, REFC)
		std::vector<unsigned long> pIndependantNodes;	///< vector of independent node vectors
		std::vector<unsigned long> pDOFs;	///< vector of DOFs
		std::vector<double> pWeightingFactors;	///< vector of weighting factors
		double pThermal;	///< double for thermal expansion coefficient


		std::string check_exp(std::string &str);
		bool checkReal(std::string &str);
};

#endif // RBE3_H
