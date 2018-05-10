/**
 *
 * @file		PBUSH.h
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PBUSH card
 *
 */

#ifndef PBUSH_H
#define PBUSH_H

#include <vector>
#include <string>

class PBUSH
{
	public:
		PBUSH();

		// 	Constructor parsing bdf data
		PBUSH(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~PBUSH();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		unsigned long getPropertyID();

		/*	Operator [] provides access to double data: i = 0/default -> K1, 1 -> K2, 2 -> K3, 
			3 -> K4, 4 -> K5, 5 -> K6 */ 
		double operator[] (unsigned int i);

	private:
		unsigned long pLongData[1];		///< Unisigned long data array storing class data
		double pDoubleData[6];			///< Double data array storing class data

		std::string check_exp(std::string &str);
};

#endif //PBUSH_H
