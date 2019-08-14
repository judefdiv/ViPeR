/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-07-1
 * Modified:
 * license: 
 * Description: Created a SFQ blif blif from a standard SFQ blif file
 * File:				ForgeSFQblif.hpp
 */


#ifndef ForgeSFQblif
#define ForgeSFQblif

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
// #include <set>

#include "die2sim/genFunc.hpp"
#include "die2sim/ParserBlif.hpp"

using namespace std;

class ForgeSFQBlif{
	private:
		static int no_SFQBlif_inst;

		vector<BlifNode> nodes;								// Inputs, outputs then gates
		vector<BlifNet> nets;

		vector<vector<int>> routes;
		vector<vector<int>> CLKlevel;					// [levels][nodes]
		unsigned int LevelCnt = 0;

		SFQBlif blifFile;

		// functions
		int findLevels(); // <----- use
		int recurLevels(unsigned int curNode, unsigned int curLev, vector<int> fooRoute);
		int findLevelsES(); // <----- use
		int recurLevelsES(unsigned int curNode, unsigned int curLev, vector<int> fooRoute);
		int calcCLKlevels();

		int isLevelsBalanced();

		int setLevel(unsigned int nodeIndex, unsigned int curLev);

		int insertSplitters();
		int insertDFFs();
		int insertDFFsPost();			// <- can be removed
		int cleanLevels();			  // <- can be removed

		int insertGate(string GateType, unsigned int netNo, unsigned int noGates);
		int deleteGate(unsigned GateIndex);			// <- can be removed

	public:
		ForgeSFQBlif();
		~ForgeSFQBlif(){};

		int importBlif(string fileName);

		int toSFQ();

		void printRoutes();
		void printCLKlevels();
		int to_blif(string fileName);
		int to_jpg(string fileName);
		int to_jpgAdv(string fileName);
		void to_str();
};

#endif