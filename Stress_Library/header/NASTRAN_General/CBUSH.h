/**
 *
 * @file		CBUSH.h
 * @author		Matt Guimarey
 * @date		11/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN CBUSH element entry
 *
 */

#ifndef CBUSH_H
#define CBUSH_H

#include "GRID.h"
#include "CQUAD.h"
#include "RBE2.h"
#include "RBE3.h"
#include "COORD.h"
#include "LOAD_CASE.h"
#include "Math/Transformation.h"
#include "Math/Coordinate.h"

#include <vector>
#include <string>
#include <map>

class CBUSH
{
	public:
		CBUSH();

		// 	Constructor parsing bdf data
		CBUSH(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		// Constructor entering Element ID
		CBUSH(unsigned long ElementID);

		~CBUSH();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		//	Updates GRID entry with connectivity data -> sets this CBUSH ID as connected to GRID	
		void nodeConnect(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, RBE2*> &RBE2_Map);

		//	Get orientation vector for CBUSH element
		Euclidean_Vector get_vector(unsigned int axis, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map);

		void parse_FH(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, RBE2*> &RBE2_Map, 
				std::map<unsigned long, RBE3*> &RBE3_Map, std::map<unsigned long, COORD*> &COORD_Map, int grid_size, int side_no, unsigned int axis_type); // Determines the CQUAD elements used for the Filled Hole analysis
/*		std::vector<unsigned long> get_FH_NodeA(); // Gets a vector of CQUAD IDs for Filled Hole at Fastener Node A
		std::vector<unsigned long> get_FH_NodeB(); // Gets a vector of CQUAD IDs for Filled Hole at Fastener Node B
*/		// Punch Results Methods
		void addForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID); // Adds a force result, line entry vector corresponds to a single force result
		std::vector<Euclidean_Vector> getForceResult(unsigned long SubcaseID, bool as_is, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map); // Gets a force result for the subcase ID
		// FH Results Parse
		void generate_FH_Result(std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map, 
								std::map<unsigned long, LOAD_CASE*> &LOAD_CASE_Map, std::map<unsigned long, CBUSH*> &CBUSH_Map, CBUSH* other_CBUSH, int side_no, 
								bool mat_flag, bool CBUSH_as_is, unsigned int CBUSH_axis[2]); // Method generates the Filled Hole force result for a specific fastener node (side_no 0 = A & 1 = B)


		std::vector<double> get_FH_Result(unsigned long SubcaseID, int side_no); 

		std::vector<unsigned long> get_FH_CQUADs(int side_no);
		
		bool FH_Composite(int side_no, std::map<unsigned long, CQUAD*> &CQUAD_Map);

		/* 	Operator [] directly accesses element long data: i = 0/default -> Element ID, 1 -> Property ID, 
			2 -> Grid A ID, 3 -> Grid B ID, 4 -> Fastener Node A ID, 5 -> Fastener Node B ID, 6 -> OCID */
		unsigned long operator[] (unsigned int i);

	private:
		unsigned long pLongData[7];			///< Unsigned long array, defined class data
		double pS;							///< Double, location of CBUSH spring damper
		unsigned short pOrienType;			///< Unsigned short integer, defines type used for CBUSH orientation
		unsigned long fastenerNodes[2];		///< Unsigned long array, GRID ID of the fastener nodes (Takes into account RBE2 method)
		Coordinate pCoordinate[2];			///< Coordinate array, stores defined class data
		Transformation pTransformation[2];		///< Transformation matrix, contains the orientation matrix of the CBUSH element with respect to BASIC Coordinate Frame
		bool pTransFlag;					///< Bool flags the status of the transformation matrix
		bool pNodeSort;						///< Bool flags the sort status of the node_QUAD sort array

		std::vector<unsigned long> node_QUADS[2];
		std::vector<CQUAD*> side_1[2];
		std::vector<CQUAD*> side_2[2];
		std::vector<CQUAD*> side_3[2];
		std::vector<CQUAD*> side_4[2];
		std::map<unsigned long, Euclidean_Vector> Force_Res_Map[2];
		std::map<unsigned long, std::vector<double> > FH_Res_Map[2];

		std::string check_exp(std::string &str);
		bool checkReal(std::string &str);

		void order_FH(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, CQUAD*> &CQUAD_Map, std::map<unsigned long, COORD*> &COORD_Map, int &grid_size, int &side_no, unsigned int &axis_type);
		void set_transform(std::map<unsigned long, COORD*> &COORD_Map, std::map<unsigned long, GRID*> &GRID_Map);
};

#endif // CBUSH_H
