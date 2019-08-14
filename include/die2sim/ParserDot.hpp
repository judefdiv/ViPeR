/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-19
 * Modified:
 * license: 
 * Description: Parser for blif to Dot.
 * File:				Parserdot.hpp
 */

#ifndef ParserDot
#define ParserDot

#include <string>
#include <iostream>
#include <vector>
#include <fstream>
// #include <iomanip>
#include "die2sim/ParserBlif.hpp"
// #include "die2sim/genFunc.hpp"

using namespace std;


class DotFile{
	private:
		string DotFileName = "\0";
		static int no_Dot_inst;

		string modelName = "\0";
		vector<string> inputs;
		vector<string> outputs;

		// Single nets:
		vector<string> fromNode;
		vector<string> mustJoinNode;

	public:
		DotFile();
		~DotFile(){};
		
		int importBlif(string fileName);

		void setFileName(string fileName);
		
		void setInputNodes(vector<string> &exVec){inputs = exVec;};
		void setOutputNodes(vector<string> &exVec){outputs = exVec;};
		void createNet(string fromN, string toN);
		int genDot();
		

		void to_str();
};



int runDot(string blifFileName, string gvFileName, string jpgFileName);

#endif