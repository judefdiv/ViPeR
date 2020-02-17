/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-07-3
 * Modified:
 * license:
 * Description: Parser/class for BLIF (The Berkeley Logic Interchange Format).
 * File:				ParserBlif.hpp
 */


#ifndef ParserBlif
#define ParserBlif

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>

struct BlifNode;
struct BlifNet;

#include "viper/genFunc.hpp"
// #include "viper/ParserDef.hpp"

// #define binGVfile "data/bin/raw.gv"

using namespace std;

/* ----------------------------------------------------------------------------------------
   ---------------------------------- Refined blif Parser ---------------------------------
   ---------------------------------------------------------------------------------------- */

#define MaxNumberLevels 124

struct BlifNode{ 			// includes input and outputs
	string name = "\0";										// Must be a unique ID
	string GateType = "\0";								// Must be GDS structure name

	vector<unsigned int> inNets;
	vector<unsigned int> outNets;
	unsigned int clkNet = 0;

	// Levels
	unsigned int CLKlevel = 0;
	unsigned int MaxLevel = 0;
	unsigned int MinLevel = MaxNumberLevels;

	// Chip layout parameters, used in placement.hpp
  int corX = 0;
  int corY = 0;
  int strRef = -1;

  void to_str(){
  	cout << "Node struct:" << endl;
    cout << "\tName: " << name << endl;
    cout << "\tType: " << GateType << endl;
    cout << "\tCLKlevel: " << CLKlevel << endl;
    cout << "\tStrRef: " << strRef << endl;
    cout << "\tCorX: " << corX << endl;
    cout << "\tCorY: " << corY << endl;
    cout << endl;
  }
};

struct netRoute{
	unsigned int layerNo = 0;
	unsigned int viaNo = 0;					// zero implies no via

	vector<int> corX;
	vector<int> corY;
};

struct BlifNet{
	string name = "\0";										// Must be a unique ID

	vector<unsigned int> inNodes;
	vector<unsigned int> outNodes;

	vector<netRoute> route;
};

class SFQBlif{
	private:
		static int no_SFQBlif_inst;

		string modelName = "\0";
		// vector<string> inputs;
		// vector<string> outputs;
		vector<BlifNode> nodes;								// Inputs, outputs then gates
		vector<BlifNet> nets;

		// Stats:
		unsigned int splitCnt = 0;						// Number of splitters
		unsigned int dffCnt = 0;							// Number of DFFs
		unsigned int levelCnt = 0;						// Number of levels, including input and outputs
		// unsigned int CLKCnt = 0;
		unsigned int gateCnt = 0;
		unsigned int inputCnt = 0;
		unsigned int outputCnt = 0;


		// functions
		int FindLevels();
		int recurLevels();										// Recursive function that is used in FindLevels()
		int setLevel();

		int insertSplitter();
		int insertDFF();

	public:
		SFQBlif();
		~SFQBlif(){};

		int importStdBlif(string fileName);

		int genSFQBlif(string fileName);
		int genDot(string fileName);
		void set_levelCnt(unsigned int inVal){this->levelCnt = inVal;};

		// void set_(unsigned int inVal){this-> = inVal;};

		void get_Nodes(vector<BlifNode> &exVec){exVec = nodes;};
		void get_Nets(vector<BlifNet> &exVec){exVec = nets;};
		unsigned int get_splitCnt(){return this->splitCnt;};
		unsigned int get_dffCnt(){return this->dffCnt;};
		unsigned int get_levelCnt(){return this->levelCnt;};
		unsigned int get_inputCnt(){return this->inputCnt;};
		unsigned int get_outputCnt(){return this->outputCnt;};
		unsigned int get_gateCnt(){return this->gateCnt;};
		string get_modelName(){return this->modelName;};
		// unsigned int get_(){return this->;};


		void receiveNodesNets(vector<BlifNode> &exVec0, vector<BlifNet> &exVec1){
			nodes = exVec0;
			nets = exVec1;
		}

		int to_jpg(string fileName);
		int to_blif(string FileName);
		void to_str();
};

/* ----------------------------------------------------------------------------------------
   ------------------------------------- RAW blif Parser ----------------------------------
   ---------------------------------------------------------------------------------------- */

struct BlifGate
{
	string name = "\0";
	string GateType = "\0";

	// net name
	vector<string> inputs;
	vector<string> outputs;

};

class BlifFile{
	private:

		string modelName = "\0";
		vector<string> inputs;
		vector<string> outputs;
		vector<BlifGate> gates;

		int processLine(vector<string> &inVec);

	public:
		BlifFile(){};
		~BlifFile(){};

		int importBlif(string fileName);

		string get_varModelName(){return this->modelName;};
		void get_varInputs(vector<string> &exVec){exVec = inputs;};
		void get_varOutputs(vector<string> &exVec){exVec = outputs;};
		void get_varGates(vector<BlifGate> &exVec){exVec = gates;};

		void to_str();
};

int splitGateNet(vector<string> &fooVec, vector<string> &inputVec, vector<string> &outVec);


#endif