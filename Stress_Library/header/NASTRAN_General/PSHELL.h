/**
 *
 * @file		PSHELL.h
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PSHELL card
 *
 */

#ifndef PSHELL_H
#define PSHELL_H

#include <vector>
#include <string>

class PSHELL
{
	public:
		PSHELL();

		// 	Constructor parsing bdf data
		PSHELL(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		~PSHELL();

		// 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

		/*	Operator [] provides access to unsigned long data: i = 0/default -> Property ID, 1 -> Material 1 ID, 
            2 -> Material 2 ID, 3 -> Material 3 ID */
		unsigned long operator[] (unsigned int i);

		/*	Operator () provides access to double data: i = 0/default -> Thickness, 1 -> Inertia Ratio, 2 -> Shear 
			Thickness Ratio  */
		double operator() (unsigned int i);

	private:
		unsigned long pLongData[4];		///< Unisigned long data array storing class data
		double pDoubleData[3];			///< Double data array storing class data

		std::string check_exp(std::string &str);
};

#endif // PSHELL_H
