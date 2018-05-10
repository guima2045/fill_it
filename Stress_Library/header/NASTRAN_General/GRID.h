/**
 *
 * @file		GRID.h
 * @author		Matt Guimarey
 * @date		01/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Grid point entry
 *
 */

#ifndef GRID_H
#define GRID_H

#include "COORD.h"
#include "Math/Coordinate.h"

#include <vector>
#include <string>
#include <map>

class GRID
{
	public:
		GRID();

		// 	Constructor parsing bdf data
		GRID(std::vector<std::string> BDF_data, bool long_format_flag);
		
		// 	Constructor entring a grid ID
		GRID(unsigned long grid_ID);

		~GRID();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parse_BDF_data(std::vector<std::string> BDF_data, bool long_format_flag);
		
		//	Get a coordinate class for the coordinate points in specified coordinate system ID
		Coordinate get_coordinate(unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map);

		//	Adds the element ID to the list of CQUAD elements connected to this GRID
		bool addCQUAD_Connect(unsigned long ElementID);

		//	Adds the element ID to the list of CBUSH elements connected to this GRID
		bool addCBUSH_Connect(unsigned long ElementID);

		//	Adds the RBE2 ID to the list of RBE2 elements connected to this GRID
		bool addRBE2_Connect(unsigned long ElementID);

		//	Adds the RBE3 ID to the list of RBE3 elements connected to this GRID
		bool addRBE3_Connect(unsigned long ElementID);	
		
		//	Gets the list of RBE2 elements connected to this GRID
		std::vector<unsigned long> getRBE2_List();

		//	Gets the list of RBE3 elements connected to this GRID
		std::vector<unsigned long> getRBE3_List();		
		
		std::vector<unsigned long> getCQUAD_List(); // Gets connectivity of Grid with respect to CQUAD Elements
		// Punch Results Methods 
/*		void addGPForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID); // Adds a grid point force result, line entry vector corresponds to a single grid point force result
		std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > getForceResult(unsigned long SubcaseID); // Gets grid point force result vector for subcase ID
*/

		/* 	Operator [] directly accesses grid long data: i = 0 -> Grid ID, 1 -> Ref Coordinate System ID, 
			2 -> Analysis Coordinate System ID */
		unsigned long operator[](int i);

		/* 	Operator () directly accesses coordinate point locations: i = 0 -> x, 1 -> y, 2 -> z */
		double operator()(int i);
	private:
		unsigned long pLongData[3];					///< Unsigned Long Integer array storing Class Data
		Coordinate pCoordinate;						///< Coordinate point data
		std::vector<unsigned long> pCQUAD_List;		///< Unsigned Long Integer vector storing IDs of CQUAD elements connected to this GRID
		std::vector<unsigned long> pCBUSH_List;
		std::vector<unsigned long> pRBE2_List;
		std::vector<unsigned long> pRBE3_List;
		/*std::map<unsigned long, std::vector<std::pair<std::pair<std::vector<double>, std::string> , unsigned long> > > GPF_Map;*/
		
		//	Private function that checks string data for exponent
		std::string check_exp(std::string &str);	
};


#endif // GRID_H
