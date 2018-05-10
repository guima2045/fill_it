/**
 *
 * @file		LOAD_CASE.h
 * @author		Matt Guimarey
 * @date		10/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Load Case entry
 *
 */

#ifndef LOAD_CASE_H
#define LOAD_CASE_H

#include <vector>
#include <string>

class LOAD_CASE
{
	public:
		LOAD_CASE();

		//	Constructor setting load case
		LOAD_CASE(unsigned long SubcaseID);

		~LOAD_CASE();

		//	Setters
		void addTitle(std::string Title);
		void addSubtitle(std::string Subtitle);
		void addLabel(std::string Label);
		void addSubcaseID(unsigned long SubcaseID);

		// Getters
		std::string getTitle();
		std::string getSubtitle();
		std::string getLabel();
		unsigned long getSubcaseID();
	private:
		std::string pStringData[3];		///< String array sotring class data
		unsigned long pSubcase;			///< Unsigned long storing subcase ID
		std::string trim(std::string &str);
};

#endif // LOAD_CASE_H
