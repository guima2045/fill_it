/**
 *
 * @file		COORD.h
 * @author		Matt Guimarey
 * @date		02/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Coordinate System Entry
 *
 */

#ifndef COORD_H
#define COORD_H

#include "Math/Euclidean_Vector.h"
#include "Math/Transformation.h"

#include <vector>
#include <string>
#include <map>

class COORD
{
	public:
		COORD();

		// 	Constructor parsing bdf data
		COORD(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~COORD();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);
		
		/*	Returns a euclidean vector for the selected axis of the transformation matrix, i = default -> x, 
			1 -> y, 2 -> z. Inverse flag sets the output to be the inverse matrix. COORD_ID sets the defined 
			output coordinate system */
		Euclidean_Vector get_axis_vector(unsigned int i, bool inverse_flag, unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map);

		/*	Transforms coordinate or euclidean vector 'from' the reference coordinate system or 'to' the 
			reference coordinate system. Inverse flag sets the transformation to be 'to' the reference 
			coordinate system */
		Coordinate transform_coordinate(Coordinate coord, bool inverse_flag);
		Euclidean_Vector transform_vector(Euclidean_Vector &vector, bool inverse_flag);

		// 	Operator [] directly accesses COORD long data: i = 0 -> COORD ID, 1 -> Ref Coordinate System ID, 
		unsigned long operator[](int i);

	private:
		unsigned long pLongData[2];			///< Unsigned Long Integer array storing Class Data
		Coordinate pOrigin;					///< Coordinate point with coordinate system origin
		Transformation pTransMatrices[2];	///< Transformation array storing class transformation matrix and inverse trans matrix

		//	Private function that checks string data for exponent
		std::string check_exp(std::string &str);
};

#endif // COORD_H
