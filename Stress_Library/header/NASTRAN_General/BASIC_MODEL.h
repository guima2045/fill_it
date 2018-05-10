/**
 *
 * @file		BASIC_MODEL.h
 * @author		Matt Guimarey
 * @date		11/06/2017
 * @version		1.0
 * 
 * This class defines NASTRAN model data
 *
 */

#ifndef BASIC_MODEL_H
#define BASIC_MODEL_H

#include "CQUAD.h"
#include "GRID.h"
#include "CBUSH.h"
#include "COORD.h"
#include "PBUSH.h"
#include "PCOMP.h"
#include "PSHELL.h"
#include "MAT.h"
#include "RBE2.h"
#include "RBE3.h"
#include "LOAD_CASE.h"
#include <string>
#include <vector>
#include <map>

class BASIC_MODEL
{
	public:
		BASIC_MODEL();

		//	Constructor parsing model data, Master .bdf file only
		BASIC_MODEL(std::string fileName);
		~BASIC_MODEL();

		//	Return a pointer to a NASTRAN element/grid class, NULL if not found
		CQUAD* getCQUAD(unsigned long ElementID);
		GRID* getGRID(unsigned long GridID);
		CBUSH* getCBUSH(unsigned long ElementID);
		COORD* getCOORD(unsigned long CoordID);
		PBUSH* getPBUSH(unsigned long PropertyID);
		PCOMP* getPCOMP(unsigned long PropertyID);
		PSHELL* getPSHELL(unsigned long PropertyID);
		MAT* getMAT(unsigned long MaterialID);
		RBE2* getRBE2(unsigned long ElementID);
		RBE3* getRBE3(unsigned long ElementID);
		LOAD_CASE* getLOAD_CASE(unsigned long SubcaseID);

		//	Return a map to NASTRAN elements/grids stored on heap
		std::map<unsigned long, CQUAD*> getCQUADMap();
		std::map<unsigned long, GRID*> getGRIDMap();
		std::map<unsigned long, CBUSH*> getCBUSHMap();
		std::map<unsigned long, COORD*> getCOORDMap();
		std::map<unsigned long, PBUSH*> getPBUSHMap();
		std::map<unsigned long, PCOMP*> getPCOMPMap();
		std::map<unsigned long, PSHELL*> getPSHELLMap();
		std::map<unsigned long, MAT*> getMATMap();
		std::map<unsigned long, RBE2*> getRBE2Map();
		std::map<unsigned long, LOAD_CASE*> getLOAD_CASEMap();

		//	Parse master .bdf file 
		void parseModel(std::string fileName);

		//	Parse a results punch file
		void parsePunchFile(std::string fileName);

	protected:
		std::map<unsigned long, CQUAD*> CQUAD_Map;
		std::map<unsigned long, GRID*> GRID_Map;
		std::map<unsigned long, CBUSH*> CBUSH_Map;
		std::map<unsigned long, COORD*> COORD_Map;
		std::map<unsigned long, PBUSH*> PBUSH_Map;
		std::map<unsigned long, PCOMP*> PCOMP_Map;
		std::map<unsigned long, PSHELL*> PSHELL_Map;
		std::map<unsigned long, MAT*> MAT_Map;
		std::map<unsigned long, RBE2*> RBE2_Map;
		std::map<unsigned long, RBE3*> RBE3_Map;
		std::map<unsigned long, LOAD_CASE*> LOAD_CASE_Map;
		std::map<unsigned long, CQUAD*>::iterator CQUAD_it;
		std::map<unsigned long, GRID*>::iterator GRID_it;
		std::map<unsigned long, CBUSH*>::iterator CBUSH_it;
		std::map<unsigned long, COORD*>::iterator COORD_it;
		std::map<unsigned long, PBUSH*>::iterator PBUSH_it;
		std::map<unsigned long, PCOMP*>::iterator PCOMP_it;
		std::map<unsigned long, PSHELL*>::iterator PSHELL_it;
		std::map<unsigned long, MAT*>::iterator MAT_it;
		std::map<unsigned long, RBE2*>::iterator RBE2_it;
		std::map<unsigned long, RBE3*>::iterator RBE3_it;
		std::map<unsigned long, LOAD_CASE*>::iterator LOAD_CASE_it;
		bool pBoolData[7];
		std::string pStringData[7];
		std::string trim(std::string &str);
		void checkBDFLine(std::string &str);
		bool checkInclude(std::string &str, std::vector<std::string> &fileNames);
		void checkLine(std::string &inputLine, std::vector<std::string> &lineEntries, unsigned int &inputType, bool &parseFlag, bool &longFormat);
		void makeEntry(std::vector<std::string> &lineEntries, unsigned int &inputType, bool &longFormat);
		unsigned long parseElementID(std::string &inputLine, bool &longFormat);
		void checkLoadCase(std::vector<std::string> &lineEntries, int (&int_data)[3]);
		void makeLoadEntry(std::vector<std::string> &lineEntries, int (&int_data)[3]);
		std::string ulong_to_str(unsigned long Num);
		std::string int_to_str(int Num);
		std::string double_to_str(double Num);
};

#endif // BASIC_MODEL_H
