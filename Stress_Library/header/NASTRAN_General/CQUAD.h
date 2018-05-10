/**
 *
 * @file		CQUAD.h
 * @author		Matt Guimarey
 * @date		07/06/2017
 * @version		1.0
 * 
 * This class defines a NASTRAN CQUAD element
 *
 */

#ifndef CQUAD_H
#define CQUAD_H

#include "MAT.h"
#include "GRID.h"
#include "COORD.h"
#include "PCOMP.h"
#include "PSHELL.h"
#include "Math/Euclidean_Vector.h"
#include "Math/Coordinate.h"
#include "Stress/CAUCHY_STRESS.h"

#include <string>
#include <vector>
#include <map>

class CQUAD
{
	public:
		CQUAD();

        // 	Constructor parsing bdf data
		CQUAD(std::vector<std::string> BDF_Data, bool LongFormatFlag);

        // 	Constructor setting element ID
		CQUAD(unsigned long ElementID);

		~CQUAD();

        // 	Parses BDF data based off a standard bulk data file, can set short or long format
		void parseBDFData(std::vector<std::string> BDF_Data, bool LongFormatFlag);

        /*  Functions return a coordinate point (class) representing the centroid of the CQUAD element. The 
            coordinate frame ID should be specified (0 being BASIC coordinate system) */
		Coordinate getCentroid(unsigned long COORD_ID, std::map<unsigned long, COORD*> &COORD_Map);

        /*  Returns a true or false flagging whether the CQUAD element material orientations is defined by a 
            coordinate frame or not */
		bool checkMCIDValid();

        /*  Function determines whether the CQUAD element has composite properties or not. Maps containing model 
            data are passed as arguments. */
		void parseComposite(std::map<unsigned long, PCOMP*> &PCOMP_Map, std::map<unsigned long, PSHELL*> &PSHELL_Map, std::map<unsigned long, MAT*> &MAT_Map);

        /*  Returns flag for whether the CQUAD element has composite properties or not. Must run parseComposite 
            before this is a valid function */
		bool is_Composite();

		//	Updates GRID entry with connectivity data -> sets this CQUAD ID as connected to GRID
		void nodeConnect(std::map<unsigned long, GRID*> &GRID_Map, std::map<unsigned long, COORD*> &COORD_Map);

		Euclidean_Vector get_material_vector();

		// Punch Results Methods
		void addForceResult(std::vector<std::string> lineEntries, unsigned long SubcaseID, bool in_mat); // Adds a force result, line entry vector corresponds to a single force result
		std::vector<double> getForceResult(unsigned long SubcaseID, bool in_mat, Euclidean_Vector* default_vector, std::map<unsigned long, GRID*> &GRID_Map, 
											std::map<unsigned long, COORD*> &COORD_Map);

		/*	Operator [] provides access to unsigned long data: i = 0/default -> Element ID, 1 -> Property ID, 
            2 -> Grid 1 ID, 3 -> Grid 2 ID, 4 -> Grid 3 ID, 5 -> Grid 4 ID, 6 -> MCID */
        unsigned long operator[](int i);

		/*	Operator () provides access to double data: i = 0/default -> Zoofset, 1 -> Theta, 2 -> Centroid_x 
			(BASIC), 3 -> Centroid_y (BASIC), 4 -> Centroid_z (BASIC), 5 -> Side X Length, 6 -> Side Y Length. 
			The validity of double data is not guaranteed as that is based off input from bulk data file */
        double operator()(int i);

	private:
		unsigned long pLongData[7];     											///< Unisigned long data array storing class data
		double pDoubleData[4];          											///< double data array storing class data
		bool pBoolData[4];              											///< bool storing flags determining validity state of other variables
        Coordinate pCentroid;           											///< Coordinate point, the CQUAD centroid
		std::map<unsigned long, std::pair<CAUCHY_STRESS, bool> > N_Map;				///< Map of CAUCHY_STRESS results representing in-plane forces
		std::map<unsigned long, std::pair<CAUCHY_STRESS, bool> > M_Map;				///< Map of CAUCHY_STRESS results representing moments
		std::map<unsigned long, std::pair<std::vector<double>, bool> > V_Map;		///< Map of std::vector storing out of plane forces Vx, Vy
		double beta, gamma, alpha, elm_theta, mat_angle;							///< CQUAD internal angles used for result output
		Euclidean_Vector x_elm, y_elm, plane_normal;


		std::string check_exp(std::string &str);
		bool checkReal(std::string &str);

};

#endif // CQUAD_H
