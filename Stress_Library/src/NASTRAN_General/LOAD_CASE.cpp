/**
 *
 * @file		LOAD_CASE.cpp
 * @author		Matt Guimarey
 * @date		10/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN Load Case entry
 *
 */


#include "NASTRAN_General/LOAD_CASE.h"

#include <string>
#include <iostream>

LOAD_CASE::LOAD_CASE()
{

}

LOAD_CASE::LOAD_CASE(unsigned long SubcaseID)
{
	addSubcaseID(SubcaseID);
}

LOAD_CASE::~LOAD_CASE()
{

}

void LOAD_CASE::addTitle(std::string Title){
	pStringData[0] = trim(Title);
}

void LOAD_CASE::addSubtitle(std::string Subtitle){
	pStringData[1] = trim(Subtitle);
}

void LOAD_CASE::addLabel(std::string Label){
	pStringData[2] = trim(Label);
}

void LOAD_CASE::addSubcaseID(unsigned long SubcaseID){
	pSubcase = SubcaseID;
}

std::string LOAD_CASE::getTitle()
{
	return pStringData[0];
}

std::string LOAD_CASE::getSubtitle()
{
	return pStringData[1];
}

std::string LOAD_CASE::getLabel()
{
	return pStringData[2];
}

unsigned long LOAD_CASE::getSubcaseID()
{
	return pSubcase;
}


std::string LOAD_CASE::trim(std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    std::size_t last = str.find_last_not_of(' ');
	std::string tempStr = str.substr(first, (last-first+1));
	return tempStr;
}
