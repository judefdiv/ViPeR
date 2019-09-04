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
#include <cmath>

#include "viper/genFunc.hpp"
#include "viper/ParserBlif.hpp"

using namespace std;

class ForgeSFQBlif{
	private:
		static int no_SFQBlif_inst;

		vector<BlifNode> nodes;								// Inputs, outputs then gates
		vector<BlifNet> nets;

		vector<vector<unsigned int>> routes;
		vector<vector<unsigned int>> CLKlevel;					// [levels][nodes]
		unsigned int LevelCnt = 0;
		vector<unsigned int> finCLKsplit;

		SFQBlif blifFile;

		// Stats:
		unsigned int inputCnt = 0;
		unsigned int outputCnt = 0;
		unsigned int gateCnt = 0;
		unsigned int splitCnt = 0;
		unsigned int DFFCnt = 0;

		// functions
		int findLevels(); // <----- use
		int recurLevels(unsigned int curNode, unsigned int curLev, vector<unsigned int> fooRoute);

		// Determines levels without splitters
		int findLevelsES(); // <----- use
		int recurLevelsES(unsigned int curNode, unsigned int curLev, vector<unsigned int> fooRoute);
		int calcCLKlevels();

		int clockIt();	// <---- use
		int recurCreateCLK(unsigned int net, unsigned int curLev, unsigned int maxLev);
		int createSplitC(unsigned int netNo);

		int calcStats();

		int isLevelsBalanced();

		int setLevel(unsigned int nodeIndex, unsigned int curLev);

		int insertSplitters();
		int insertDFFs();

		int insertGate(string GateType, unsigned int netNo, unsigned int noGates);
		int deleteGate(unsigned GateIndex);			// <- can be removed

	public:
		ForgeSFQBlif();
		~ForgeSFQBlif(){};

		int importBlif(string fileName);

		int toSFQ();

		vector<BlifNode> get_nodes(){return this->nodes;};
		vector<BlifNet> get_nets(){return this->nets;};
		vector<vector<unsigned int>> get_routes(){return this->routes;};
		vector<vector<unsigned int>> get_CLKlevels(){return this->CLKlevel;};

		void printRoutes();
		void printStats();
		void printCLKlevels();
		int to_blif(string fileName);
		int to_jpg(string fileName);
		int to_jpgAdv(string fileName);
		void to_str();
};

#endif