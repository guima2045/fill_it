/**
 *
 * @file		PCOMP.cpp
 * @author		Matt Guimarey
 * @date		09/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN PCOMP card
 *
 */


#include "NASTRAN_General/PCOMP.h"
#include "NASTRAN_General/MAT.h"

#include <iostream>
#include <vector>
#include <string>
#include <stdlib.h> 
#include <math.h> 
#include <malloc.h>

#define PI 3.14159265

PCOMP::PCOMP()
{
	pLaminateValues[0] = 0;
	pLaminateValues[1] = 0;
	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			A_Matrix[i][j] = 0.0;
			B_Matrix[i][j] = 0.0;
			D_Matrix[i][j] = 0.0;
		}
	}
}

/**
 *	@brief	Constructor also parsing Bulk Data File data, runs parse_BDF_data
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 */

PCOMP::PCOMP(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	PCOMP();
	parseBDFData(BDF_Data, LongFormatFlag);
}

PCOMP::~PCOMP()
{

}

/**
 *	@brief	Parses Bulk Data File input lines to populate class data. Bulk Data 
			File has a short and a long format recognised by NASTRAN, the flag 
			modifies how the input data is parsed for these occurences.
 *
 *	@param	BDF_Data, a string vector containing Bulk Data File input lines
 * 	@param 	LongFormatFlag, a boolean flagging format type
 *	@return	void
 */

void PCOMP::parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag)
{
	std::vector<std::string> LineData;	///< String vector to store seperated line entries for parsing
	std::vector<std::string> PlyData;	///< String vector to store seperated ply entries for parsing
	if (!LongFormatFlag) {
		// Short Format, data fields every 8 characters (80 chars max)
		for (int i = 8; i < 72; i += 8) {
			LineData.push_back(BDF_Data[0].substr(i, 8));
		}
		for (int i = 1; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 8) {
				PlyData.push_back(BDF_Data[i].substr(j, 8));
			}	
		}
	} else {
		// Long Format, data fields every 16 characters after inital 8 characters (80 chars max)
		for (int i = 0; i < 2; i++) {
			for (int j = 8; j < 72; j += 16) {
				LineData.push_back(BDF_Data[i].substr(j, 16));
			}	
		}			
		for (int i = 2; i < BDF_Data.size(); i++) {
			for (int j = 8; j < 72; j += 16) {
				PlyData.push_back(BDF_Data[i].substr(j, 16));
			}	
		}		
	}
	pPropertyID = atol(LineData[0].c_str());
	std::string str = check_exp(LineData[1]);
	if (str.compare("") == 0) {
		pZOff = -0.5;
	} else {
		pZOff = atof(str.c_str());
	}
	str = check_exp(LineData[7]);
	if (str.compare("SYM") == 0) {
		pLaminateValues[0] = 1;
	}
	if (str.compare("SMCORE") == 0) {
		pLaminateValues[0] = 2;
	}	
	for (int i = 0; i < PlyData.size(); i += 8) {
		if (check_exp(PlyData[i + 2]).compare("") != 0) {
			pLaminateValues[1]++;
			pMaterialID.push_back(atol(PlyData[i].c_str()));
			pThickness.push_back(atof(check_exp(PlyData[i + 1]).c_str()));
			pOrientation.push_back(atof(check_exp(PlyData[i + 2]).c_str()));
		}
		if (check_exp(PlyData[i + 6]).compare("") != 0) {
			pLaminateValues[1]++;
			pMaterialID.push_back(atol(PlyData[i + 4].c_str()));
			pThickness.push_back(atof(check_exp(PlyData[i + 5]).c_str()));
			pOrientation.push_back(atof(check_exp(PlyData[i + 6]).c_str()));
		}
	}
	switch (pLaminateValues[0]) {
		case 1:
			// TODO: Symmetry
			break;
		case 2:
			// TODO: Symmetry with core
			break;
	}
}

unsigned long PCOMP::getPropertyID()
{
	return pPropertyID;
}

double PCOMP::getZOff()
{
	return pZOff;
}


/**
 *	@brief	Parses ply layup to determine laminate properties E11, E22, G12 and NU12
 *
 * 	@param 	MAT_Map, a MAT class map of materials in the model
 *	@return	void
 */

void PCOMP::parse_Plies(std::map<unsigned long, MAT*> &MAT_Map)
{
	// TODO: Debug this :
	//return;

	//

	std::vector<double> materialProperties;
	double totalThickness = 0.0;
	//double Q_Values[pLaminateValues[1]][4];

	double **Q_Values;
	Q_Values = new double*[pLaminateValues[1]];
	for (int i = 0; i < pLaminateValues[1]; i++) {
		Q_Values[i] = new double[4];
	}
		

	for (int i = 0; i < pLaminateValues[1]; i++) {
		totalThickness += pThickness[i];
		if (MAT_Map.find(pMaterialID[i]) != MAT_Map.end()) {
			// Found material
			double nu21 = (*MAT_Map[pMaterialID[i]])[1] * (*MAT_Map[pMaterialID[i]])[2] / (*MAT_Map[pMaterialID[i]])[0];
			Q_Values[i][0] = (*MAT_Map[pMaterialID[i]])[0] / (1 - (*MAT_Map[pMaterialID[i]])[2] * nu21); // Q11
			Q_Values[i][1] = (*MAT_Map[pMaterialID[i]])[1] / (1 - (*MAT_Map[pMaterialID[i]])[2] * nu21); // Q22
			Q_Values[i][2] = nu21 * (*MAT_Map[pMaterialID[i]])[0] / (1 - (*MAT_Map[pMaterialID[i]])[2] * nu21); // Q12
			Q_Values[i][3] = (*MAT_Map[pMaterialID[i]])[3]; // Q66
		} else {
			// Could not find material
			for (int j = 0; j < 4; j++) {
				Q_Values[i][j] = 0.0;
			}
		}
	}
	// Generate A B D Matrix
	double Q_star[4] = {0.0, 0.0, 0.0, 0.0};
	double h = -1 * totalThickness / 2;
	for (int i = 0; i < pLaminateValues[1]; i++) {
		Q_star[0] = Q_Values[i][0] * pow(cos(pOrientation[i] * PI / 180), 4) + 2 * (Q_Values[i][2] + 2 * Q_Values[i][3]) * pow(sin(pOrientation[i] * PI / 180), 2) * pow(cos(pOrientation[i] * PI / 180), 2)
			+ Q_Values[i][1] * pow(sin(pOrientation[i] * PI / 180), 4); // Q*11
		Q_star[1] = Q_Values[i][0] * pow(sin(pOrientation[i] * PI / 180), 4) + 2 * (Q_Values[i][2] + 2 * Q_Values[i][3]) * pow(cos(pOrientation[i] * PI / 180), 2) * pow(sin(pOrientation[i] * PI / 180), 2)
			+ Q_Values[i][1] * pow(cos(pOrientation[i] * PI / 180), 4); // Q*22
		Q_star[2] = (Q_Values[i][0] + Q_Values[i][1] - 4 * Q_Values[i][3]) * pow(sin(pOrientation[i] * PI / 180), 2) * pow(cos(pOrientation[i] * PI / 180), 2)
			+ Q_Values[i][2] * (pow(sin(pOrientation[i] * PI / 180), 4) + pow(cos(pOrientation[i] * PI / 180), 4)); // Q*12
		Q_star[3] = (Q_Values[i][0] + Q_Values[i][1] - 2 * Q_Values[i][2] - 2 * Q_Values[i][3]) * pow(sin(pOrientation[i] * PI / 180), 2) * pow(cos(pOrientation[i] * PI / 180), 2)
			+ Q_Values[i][3] * (pow(sin(pOrientation[i] * PI / 180), 4) + pow(cos(pOrientation[i] * PI / 180), 4)); // Q*66
		// A Matrix
		A_Matrix[0][0] += Q_star[0] * (h + pThickness[i] - h); // A11 = Q*11 * (hi - hi-1)
		A_Matrix[1][1] += Q_star[1] * (h + pThickness[i] - h); // A22 = Q*22 * (hi - hi-1)
		A_Matrix[0][1] = A_Matrix[1][0] += Q_star[2] * (h + pThickness[i] - h); // A12 = Q*12 * (hi - hi-1)
		A_Matrix[2][2] += Q_star[3] * (h + pThickness[i] - h); // A66 = Q*66 * (hi - hi-1)
		// B Matrix
		B_Matrix[0][0] += Q_star[0] * (pow((h + pThickness[i]), 2) - pow(h, 2)) / 2; // B11 = 0.5 * Q*11 * (hi ^ 2 - hi-1 ^ 2)
		B_Matrix[1][1] += Q_star[1] * (pow((h + pThickness[i]), 2) - pow(h, 2)) / 2; // B22 = 0.5 * Q*22 * (hi ^ 2 - hi-1 ^ 2)
		B_Matrix[0][1] = B_Matrix[1][0] += Q_star[2] * (pow((h + pThickness[i]), 2) - pow(h, 2)) / 2; // B12 = 0.5 * Q*12 * (hi ^ 2 - hi-1 ^ 2)
		B_Matrix[2][2] += Q_star[3] * (pow((h + pThickness[i]), 2) - pow(h, 2)) / 2; // B66 = 0.5 * Q*66 * (hi ^ 2 - hi-1 ^ 2)
		// D Matrix
		D_Matrix[0][0] += Q_star[0] * (pow((h + pThickness[i]), 3) - pow(h, 3)) / 3; // D11 = 0.333 * Q*11 * (hi ^ 3 - hi-1 ^ 3)
		D_Matrix[1][1] += Q_star[1] * (pow((h + pThickness[i]), 3) - pow(h, 3)) / 3; // D22 = 0.333 * Q*22 * (hi ^ 3 - hi-1 ^ 3)
		D_Matrix[0][1] = D_Matrix[1][0] += Q_star[2] * (pow((h + pThickness[i]), 3) - pow(h, 3)) / 3; // D12 = 0.333 * Q*12 * (hi ^ 3 - hi-1 ^ 3)
		D_Matrix[2][2] += Q_star[3] * (pow((h + pThickness[i]), 3) - pow(h, 3)) / 3; // D66 = 0.333 * Q*66 * (hi ^ 3 - hi-1 ^ 3)		
		// Increment h
		h += pThickness[i];
	}
	Laminate_Stiffness[0] = A_Matrix[0][0] / totalThickness - pow(A_Matrix[0][1] / totalThickness, 2) / (A_Matrix[1][1] / totalThickness); // E11
	Laminate_Stiffness[1] = A_Matrix[1][1] / totalThickness - pow(A_Matrix[0][1] / totalThickness, 2) / (A_Matrix[0][0] / totalThickness); // E22
	Laminate_Stiffness[2] = (A_Matrix[0][1] / totalThickness) / (A_Matrix[1][1] / totalThickness); // NU12
	Laminate_Stiffness[3] = (A_Matrix[0][1] / totalThickness) / (A_Matrix[0][0] / totalThickness); // NU21
	Laminate_Stiffness[4] = A_Matrix[2][2] / totalThickness; // G12
	Laminate_Stiffness[5] = totalThickness;	
	for (int i = 0; i < pLaminateValues[1]; ++i)
		delete[] Q_Values[i];
	delete[] Q_Values;

}


/**
 *	@brief	Overload the [] operator to provide direct access to the laminate properties. E11 
 *			is returned as default.
 *
 *			i = 0 -> E11
 *			i = 1 -> E22
 *			i = 2 -> NU12
 *			i = 3 -> NU21
 *			i = 4 -> G12
 *			i = 5 -> t
 *
 *	@param	i, integer parameter designating position
 *	@return	double, returned laminate property
 */

double PCOMP::operator[] (unsigned int i)
{
	if (i < 6) {
		return Laminate_Stiffness[i];
	} else {
		return Laminate_Stiffness[0];
	}	
}


std::string PCOMP::check_exp(std::string& str)
{
    std::size_t first = str.find_first_not_of(' ');
    if (first == std::string::npos)
        return "";
    std::size_t last = str.find_last_not_of(' ');
	std::string tempStr = str.substr(first, (last-first+1));
    // Check for NASTRAN exponent
	std::size_t exp = tempStr.find_first_of('-', 1);
	if (exp == std::string::npos) {
		exp = tempStr.find_first_of('+', 1);
	}
	// Insert 'E' if exponent is found
	if (exp != std::string::npos) {
		std::size_t posEbig = tempStr.find_first_of('E');
		std::size_t posEsma = tempStr.find_first_of('e');
		if (posEbig == std::string::npos && posEsma == std::string::npos) {
			tempStr.insert(exp, "E");
		}
	}
	return tempStr;
}
