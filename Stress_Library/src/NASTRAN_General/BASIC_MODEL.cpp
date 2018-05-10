/**
 *
 * @file		BASIC_MODEL.cpp
 * @author		Matt Guimarey
 * @date		11/06/2017
 * @version		1.0
 * 
 * This class defines NASTRAN model data
 *
 */

#include "NASTRAN_General/BASIC_MODEL.h"
#include "NASTRAN_General/CQUAD.h"
#include "NASTRAN_General/GRID.h"
#include "NASTRAN_General/CBUSH.h"
#include "NASTRAN_General/COORD.h"
#include "NASTRAN_General/PBUSH.h"
#include "NASTRAN_General/PCOMP.h"
#include "NASTRAN_General/PSHELL.h"
#include "NASTRAN_General/MAT.h"
#include "NASTRAN_General/RBE2.h"
#include "NASTRAN_General/LOAD_CASE.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <cstring>
#include <stdlib.h> 
#include <sstream>

BASIC_MODEL::BASIC_MODEL()
{
	for (int i = 0; i < 7; i++) {
		pBoolData[i] = false;
	}
	pStringData[0] = "CQUAD_Forces_Material";
	pStringData[1] = "CQUAD_Forces";
	pStringData[2] = "CBUSH_Forces";
	pStringData[3] = "CQUAD_Stress_Material";
	pStringData[4] = "CQUAD_Stress";
	pStringData[5] = "CQUAD_Ply_Stress";
	pStringData[6] = "Grid_Point_Forces";
}

BASIC_MODEL::BASIC_MODEL(std::string fileName)
{
	BASIC_MODEL();
	parseModel(fileName);
}

BASIC_MODEL::~BASIC_MODEL()
{
	for (CQUAD_it = CQUAD_Map.begin(); CQUAD_it != CQUAD_Map.end(); CQUAD_it++) {
		delete CQUAD_it->second;
	}
	for (GRID_it = GRID_Map.begin(); GRID_it != GRID_Map.end(); GRID_it++) {
		delete GRID_it->second;
	}
	for (CBUSH_it = CBUSH_Map.begin(); CBUSH_it != CBUSH_Map.end(); CBUSH_it++) {
		delete CBUSH_it->second;
	}	
	for (COORD_it = COORD_Map.begin(); COORD_it != COORD_Map.end(); COORD_it++) {
		delete COORD_it->second;
	}	
	for (PBUSH_it = PBUSH_Map.begin(); PBUSH_it != PBUSH_Map.end(); PBUSH_it++) {
		delete PBUSH_it->second;
	}	
	for (PCOMP_it = PCOMP_Map.begin(); PCOMP_it != PCOMP_Map.end(); PCOMP_it++) {
		delete PCOMP_it->second;
	}	
	for (PSHELL_it = PSHELL_Map.begin(); PSHELL_it != PSHELL_Map.end(); PSHELL_it++) {
		delete PSHELL_it->second;
	}	
	for (MAT_it = MAT_Map.begin(); MAT_it != MAT_Map.end(); MAT_it++) {
		delete MAT_it->second;
	}	
	for (RBE2_it = RBE2_Map.begin(); RBE2_it != RBE2_Map.end(); RBE2_it++) {
		delete RBE2_it->second;
	}	
	for (RBE3_it = RBE3_Map.begin(); RBE3_it != RBE3_Map.end(); RBE3_it++) {
		delete RBE3_it->second;
	}	
	for (LOAD_CASE_it = LOAD_CASE_Map.begin(); LOAD_CASE_it != LOAD_CASE_Map.end(); LOAD_CASE_it++) {
		delete LOAD_CASE_it->second;
	}
}

CQUAD* BASIC_MODEL::getCQUAD(unsigned long ElementID)
{
	if (CQUAD_Map.find(ElementID) != CQUAD_Map.end()) {
		return CQUAD_Map[ElementID];
	} else {
		return NULL;
	}
}

GRID* BASIC_MODEL::getGRID(unsigned long GridID)
{
	if (GRID_Map.find(GridID) != GRID_Map.end()) {
		return GRID_Map[GridID];
	} else {
		return NULL;
	}
}

CBUSH* BASIC_MODEL::getCBUSH(unsigned long ElementID)
{
	if (CBUSH_Map.find(ElementID) != CBUSH_Map.end()) {
		return CBUSH_Map[ElementID];
	} else {
		return NULL;
	}
}

COORD* BASIC_MODEL::getCOORD(unsigned long CoordID)
{
	if (COORD_Map.find(CoordID) != COORD_Map.end()) {
		return COORD_Map[CoordID];
	} else {
		return NULL;
	}
}

PBUSH* BASIC_MODEL::getPBUSH(unsigned long PropertyID)
{
	if (PBUSH_Map.find(PropertyID) != PBUSH_Map.end()) {
		return PBUSH_Map[PropertyID];
	} else {
		return NULL;
	}
}

PCOMP* BASIC_MODEL::getPCOMP(unsigned long PropertyID)
{
	if (PCOMP_Map.find(PropertyID) != PCOMP_Map.end()) {
		return PCOMP_Map[PropertyID];
	} else {
		return NULL;
	}
}

PSHELL* BASIC_MODEL::getPSHELL(unsigned long PropertyID)
{
	if (PSHELL_Map.find(PropertyID) != PSHELL_Map.end()) {
		return PSHELL_Map[PropertyID];
	} else {
		return NULL;
	}
}

MAT* BASIC_MODEL::getMAT(unsigned long MaterialID)
{
	if (MAT_Map.find(MaterialID) != MAT_Map.end()) {
		return MAT_Map[MaterialID];
	} else {
		return NULL;
	}
}

RBE2* BASIC_MODEL::getRBE2(unsigned long ElementID)
{
	if (RBE2_Map.find(ElementID) != RBE2_Map.end()) {
		return RBE2_Map[ElementID];
	} else {
		return NULL;
	}
}

RBE3* BASIC_MODEL::getRBE3(unsigned long ElementID)
{
	if (RBE3_Map.find(ElementID) != RBE3_Map.end()) {
		return RBE3_Map[ElementID];
	} else {
		return NULL;
	}
}

LOAD_CASE* BASIC_MODEL::getLOAD_CASE(unsigned long SubcaseID)
{
	if (LOAD_CASE_Map.find(SubcaseID) != LOAD_CASE_Map.end()) {
		return LOAD_CASE_Map[SubcaseID];
	} else {
		return NULL;
	}	
}

std::map<unsigned long, CQUAD*> BASIC_MODEL::getCQUADMap()
{
	return CQUAD_Map;
}

std::map<unsigned long, GRID*> BASIC_MODEL::getGRIDMap()
{
	return GRID_Map;
}

std::map<unsigned long, CBUSH*> BASIC_MODEL::getCBUSHMap()
{
	return CBUSH_Map;
}

std::map<unsigned long, COORD*> BASIC_MODEL::getCOORDMap()
{
	return COORD_Map;
}

std::map<unsigned long, PBUSH*> BASIC_MODEL::getPBUSHMap()
{
	return PBUSH_Map;
}

std::map<unsigned long, PCOMP*> BASIC_MODEL::getPCOMPMap()
{
	return PCOMP_Map;
}

std::map<unsigned long, PSHELL*> BASIC_MODEL::getPSHELLMap()
{
	return PSHELL_Map;
}

std::map<unsigned long, MAT*> BASIC_MODEL::getMATMap()
{
	return MAT_Map;
}

std::map<unsigned long, RBE2*> BASIC_MODEL::getRBE2Map()
{
	return RBE2_Map;
}

std::map<unsigned long, LOAD_CASE*> BASIC_MODEL::getLOAD_CASEMap()
{
	return LOAD_CASE_Map;
}

void BASIC_MODEL::parseModel(std::string fileName)
{
	std::vector<std::string> fileNames;
	std::vector<std::string> lineEntries;
	fileNames.push_back(fileName);
	std::fstream bdfFile;
	int i = 0;
	std::string inputLine;
	bool parseFlag = false, longFormat = false, valid = false;
	unsigned int inputType;
	//CQUAD* tempCQUAD;
	while (i < fileNames.size()) {
		bdfFile.open(fileNames[i].c_str(), std::ios_base::in);
		if (bdfFile.is_open()) {
			valid = true;
			std::cout << "Reading: " << fileNames[i] << std::endl;
			while(!bdfFile.eof()) {
				std::getline(bdfFile, inputLine);
				checkBDFLine(inputLine);
				if (checkInclude(inputLine, fileNames)) {
					std::cout << "Found include file: " << fileNames[fileNames.size() - 1] << std::endl;
					if (parseFlag) {
						makeEntry(lineEntries, inputType, longFormat);
						parseFlag = false;
					}
				} else if (inputLine.substr(0,8).compare("        ") == 0 || inputLine.substr(0,8).compare("*       ") == 0) {
					if (parseFlag) {
						lineEntries.push_back(inputLine);
					}
				} else {
					checkLine(inputLine, lineEntries, inputType, parseFlag, longFormat);
				}
			}
		} else {
			// File did not open
			std::cout << "Unable to open: " << fileNames[i] << std::endl;
		}	
		bdfFile.close();
		i++;
	}

	if (valid) {
		std::cout << "Parsing composite properties...";
		for (PCOMP_it = PCOMP_Map.begin(); PCOMP_it != PCOMP_Map.end(); PCOMP_it++) {
			PCOMP_it->second->parse_Plies(MAT_Map);
		}
		std::cout << "Complete" << std::endl;
		std::cout << "Parsing grid connectivity...";
		for (CQUAD_it = CQUAD_Map.begin(); CQUAD_it != CQUAD_Map.end(); CQUAD_it++) {
			CQUAD_it->second->parseComposite(PCOMP_Map, PSHELL_Map, MAT_Map);
			CQUAD_it->second->nodeConnect(GRID_Map, COORD_Map);
		}
		for (RBE2_it = RBE2_Map.begin(); RBE2_it != RBE2_Map.end(); RBE2_it++) {
			RBE2_it->second->nodeConnect(GRID_Map);
		}	
		for (RBE3_it = RBE3_Map.begin(); RBE3_it != RBE3_Map.end(); RBE3_it++) {
			RBE3_it->second->nodeConnect(GRID_Map);
		}	
		for (CBUSH_it = CBUSH_Map.begin(); CBUSH_it != CBUSH_Map.end(); CBUSH_it++) {
			CBUSH_it->second->nodeConnect(GRID_Map, RBE2_Map);
		}	
		std::cout << "Complete" << std::endl;	
	}
}


void BASIC_MODEL::parsePunchFile(std::string fileName)
{
	std::fstream punchFile;
	std::string inputLine;
	std::vector<std::string> lineEntries;
	bool load_Case = false;
	bool data_flag = false;
	int int_data[3] = {0, 0, 0};
	punchFile.open(fileName.c_str(), std::ios_base::in);
	if (punchFile.is_open()) {
		std::cout << "Reading: " << fileName << std::endl;
		while(!punchFile.eof()) {
			std::getline(punchFile, inputLine);
			if (inputLine.substr(0,6).compare("$TITLE") == 0 && !load_Case) {
				data_flag = false;
				load_Case = true;
				lineEntries.push_back(inputLine);
				int_data[0] = 6;
			} else if (data_flag || load_Case) {
				if (int_data[0] > 0) {
					lineEntries.push_back(inputLine);
					int_data[0]--;
				}
				if (int_data[0] == 0 && load_Case) {
					checkLoadCase(lineEntries, int_data);
					data_flag = true;
					load_Case = false;
				} else if (int_data[0] == 0 && data_flag) {
					makeLoadEntry(lineEntries, int_data);
					lineEntries.clear();
				}
			}
		}	
		punchFile.close();
	} else {
		// File did not open
		std::cout << "Unable to open: " << fileName << std::endl;	
	}
}


void BASIC_MODEL::makeLoadEntry(std::vector<std::string> &lineEntries, int (&int_data)[3])
{
	unsigned long elm_grid_ID;
	std::string tempString;
	try {
		tempString = lineEntries[0].substr(0, 18);
		elm_grid_ID = atol(trim(tempString).c_str());
	} catch (int e) {
		elm_grid_ID = 0;
	}
	if (elm_grid_ID != 0) {
		// Valid entry
		switch (int_data[2]) {
			case 1:
				CQUAD_it = CQUAD_Map.find(elm_grid_ID);
				if (CQUAD_it == CQUAD_Map.end()) {
					std::pair<std::map<unsigned long, CQUAD*>::iterator, bool> ret = CQUAD_Map.insert(std::pair<unsigned long, CQUAD*> (elm_grid_ID, new CQUAD(elm_grid_ID)));
					CQUAD_it = ret.first;
				} 
				switch (int_data[1]) {
					case 1:
						CQUAD_it->second->addForceResult(lineEntries, LOAD_CASE_it->first, true);
						int_data[0] = 3;
						break;
				}
				break;
			case 2:
				CQUAD_it = CQUAD_Map.find(elm_grid_ID);
				if (CQUAD_it == CQUAD_Map.end()) {
					std::pair<std::map<unsigned long, CQUAD*>::iterator, bool> ret = CQUAD_Map.insert(std::pair<unsigned long, CQUAD*> (elm_grid_ID, new CQUAD(elm_grid_ID)));
					CQUAD_it = ret.first;
				} 
				switch (int_data[1]) {
					case 1:
						CQUAD_it->second->addForceResult(lineEntries, LOAD_CASE_it->first, false);
						int_data[0] = 3;
						break;
				}
				break;
			case 3:
				CBUSH_it = CBUSH_Map.find(elm_grid_ID);
				if (CBUSH_it == CBUSH_Map.end()) {
					std::pair<std::map<unsigned long, CBUSH*>::iterator, bool> ret = CBUSH_Map.insert(std::pair<unsigned long, CBUSH*> (elm_grid_ID, new CBUSH(elm_grid_ID)));
					CBUSH_it = ret.first;
				}
				// Add CBUSH Force
				switch (int_data[1]) {
					case 1:
						CBUSH_it->second->addForceResult(lineEntries, LOAD_CASE_it->first);
						int_data[0] = 2;
						break;
				}
				break;			
		}
	}
}

void BASIC_MODEL::checkLoadCase(std::vector<std::string> &lineEntries, int (&int_data)[3])
{
	unsigned long subcaseID;
	std::string tempString;
	try {
		tempString = lineEntries[5].substr(13, 57);
		subcaseID = atol(trim(tempString).c_str());
	} catch (int e) {
		subcaseID = 0;
	}
	LOAD_CASE_it = LOAD_CASE_Map.find(subcaseID);
	if (LOAD_CASE_it == LOAD_CASE_Map.end()) {
		std::pair<std::map<unsigned long, LOAD_CASE*>::iterator, bool> ret = LOAD_CASE_Map.insert(std::pair<unsigned long, LOAD_CASE*> (subcaseID, new LOAD_CASE(subcaseID)));
		LOAD_CASE_it = ret.first;
		tempString = lineEntries[0].substr(10, 62);
		LOAD_CASE_it->second->addTitle(tempString);
		tempString = lineEntries[1].substr(10, 62);
		LOAD_CASE_it->second->addSubtitle(tempString);
		tempString = lineEntries[2].substr(10, 62);
		LOAD_CASE_it->second->addLabel(tempString);
	}
	// Determine load type
	if (lineEntries[3].substr(0, 15).compare("$ELEMENT FORCES") == 0) {
		int_data[1] = 1;
		if	(lineEntries[6].substr(0, 69).compare("$ELEMENT TYPE =          33  QUAD4                           MATERIAL") == 0) {
			int_data[2] = 1;
			int_data[0] = 3;
			pBoolData[0] = true;
		} else if (lineEntries[6].substr(0, 34).compare("$ELEMENT TYPE =          33  QUAD4") == 0) {
			int_data[2] = 2;
			int_data[0] = 3;
			pBoolData[1] = true;
		} else if (lineEntries[6].substr(0, 33).compare("$ELEMENT TYPE =         102  BUSH") == 0) {
			int_data[2] = 3;
			int_data[0] = 2;
			pBoolData[2] = true;
		}
		lineEntries.clear();
	} 	
}

std::string BASIC_MODEL::trim(std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
	std::string tempStr = str;
    if (first == std::string::npos) {
		return "";
	} else {
		while (tempStr.find_first_of(' ') != std::string::npos) {
			tempStr.erase(tempStr.find_first_of(' '), 1);
		}
	}
    return tempStr;
}

void BASIC_MODEL::checkBDFLine(std::string &str)
{
	int i;
	i = 80 - str.length();
	if (i > 0) {
		str.append(i, ' ');
	}
}

bool BASIC_MODEL::checkInclude(std::string &str, std::vector<std::string> &fileNames)
{
	char cstring[8];
	for (int i = 0; i < 7; i++) {
		cstring[i] = str[i];
	}
	for (int i = 0; i < 7; i++) {
		if (cstring[i] <= 122 && cstring[i] >= 97) {
			cstring[i] -= 32;
		}
	}
	cstring[7] = 0;
	if (std::strcmp(cstring, "INCLUDE") == 0) {
		std::size_t first = str.find_first_of("'");
		if (first == std::string::npos) {
			first = str.find_first_of('"');
		}
		std::size_t last = str.find_last_of("'");
		if (last == std::string::npos) {
			last = str.find_last_of('"');
		}
		std::string newFileName = str.substr(first + 1, last - first - 1);
		fileNames.push_back(newFileName);
		return true;
	} else {
		return false;
	}
}

void BASIC_MODEL::checkLine(std::string &inputLine, std::vector<std::string> &lineEntries, unsigned int &inputType, bool &parseFlag, bool &longFormat)
{
	if (parseFlag) {
		makeEntry(lineEntries, inputType, longFormat);
		parseFlag = false;
	}
	std::string tempString = inputLine.substr(0,8);
	if (trim(tempString).compare("CQUAD4") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 1;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("CQUAD4*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 1;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("GRID") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 2;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("GRID*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 2;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("CBUSH") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 3;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("CBUSH*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 3;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("CORD2R") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 4;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("CORD2R*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 4;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("PBUSH") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 5;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("PBUSH*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 5;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("PCOMP") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 6;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("PCOMP*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 6;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("PSHELL") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 7;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("PSHELL*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 7;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("MAT1") == 0 || trim(tempString).compare("MAT8") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 8;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("MAT1*") == 0 || trim(tempString).compare("MAT8*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 8;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("RBE2") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 9;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("RBE2*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 9;
		longFormat = true;
		parseFlag = true;
	} else if (trim(tempString).compare("RBE3") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 10;
		longFormat = false;
		parseFlag = true;
	} else if (trim(tempString).compare("RBE3*") == 0) {
		lineEntries.push_back(inputLine);
		inputType = 10;
		longFormat = true;
		parseFlag = true;
	}
}

void BASIC_MODEL::makeEntry(std::vector<std::string> &lineEntries, unsigned int &inputType, bool &longFormat)
{
	switch (inputType) {
		case 1:
				CQUAD_Map.insert(std::pair<unsigned long, CQUAD*>(parseElementID(lineEntries[0], longFormat), new CQUAD(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 2:
				GRID_Map.insert(std::pair<unsigned long, GRID*>(parseElementID(lineEntries[0], longFormat), new GRID(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 3:
				CBUSH_Map.insert(std::pair<unsigned long, CBUSH*>(parseElementID(lineEntries[0], longFormat), new CBUSH(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 4:
				COORD_Map.insert(std::pair<unsigned long, COORD*>(parseElementID(lineEntries[0], longFormat), new COORD(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 5:
				PBUSH_Map.insert(std::pair<unsigned long, PBUSH*>(parseElementID(lineEntries[0], longFormat), new PBUSH(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 6:
				PCOMP_Map.insert(std::pair<unsigned long, PCOMP*>(parseElementID(lineEntries[0], longFormat), new PCOMP(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 7:
				PSHELL_Map.insert(std::pair<unsigned long, PSHELL*>(parseElementID(lineEntries[0], longFormat), new PSHELL(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 8:
				MAT_Map.insert(std::pair<unsigned long, MAT*>(parseElementID(lineEntries[0], longFormat), new MAT(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 9:
				RBE2_Map.insert(std::pair<unsigned long, RBE2*>(parseElementID(lineEntries[0], longFormat), new RBE2(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
		case 10:
				RBE3_Map.insert(std::pair<unsigned long, RBE3*>(parseElementID(lineEntries[0], longFormat), new RBE3(lineEntries, longFormat)));
				lineEntries.clear();
				inputType = 0;
			break;
	}
}

unsigned long BASIC_MODEL::parseElementID(std::string &inputLine, bool &longFormat)
{
	//std::string tempString;
	if (longFormat) {
		//tempString = inputLine.substr(8,16);
		return atol(inputLine.substr(8,16).c_str());
	} else {
		//tempString = inputLine.substr(8,8);
		return atol(inputLine.substr(8,8).c_str());
	}
}

/*
std::string BASIC_MODEL::ulong_to_str(unsigned long Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}

std::string BASIC_MODEL::int_to_str(int Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}

std::string BASIC_MODEL::double_to_str(double Num)
{
	std::stringstream ss;
	ss << Num;
	return ss.str();
}
*/
