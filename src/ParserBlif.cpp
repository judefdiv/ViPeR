/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-07-3
 * Modified:
 * license:
 * Description: Parser/class for BLIF (The Berkeley Logic Interchange Format).
 * File:				ParserBlif.cpp
 */

#include "viper/common.hpp"
#include "viper/ParserBlif.hpp"

/* ----------------------------------------------------------------------------------------
   ---------------------------------- Refined blif Parser ---------------------------------
   ---------------------------------------------------------------------------------------- */

int SFQBlif::no_SFQBlif_inst = 0;

/**
 * Constructor
 */

SFQBlif::SFQBlif(){
	SFQBlif::no_SFQBlif_inst++;
}

/**
 * [SFQBlif::importStdBlif - Imports a normal/standard blif file.]
 * @param  fileName [The file name of the to be imported blif file]
 * @return          [1 - All good; 0 - error]
 */

int SFQBlif::importStdBlif(string fileName){
	vector<BlifGate> stdGate;
	vector<string> stdInputs;
	vector<string> stdOutputs;

	BlifFile cmosCir;
	cmosCir.importBlif(fileName);
	// cmosCir.to_str();

	cmosCir.get_varInputs(stdInputs);
	cmosCir.get_varOutputs(stdOutputs);
	cmosCir.get_varGates(stdGate);

	BlifNode fooGate;
	BlifNet fooNet;

	/* ------------------- Define Node Basics -------------------- */

	this->modelName =  cmosCir.get_varModelName();

	// Inputs
	this->inputCnt = stdInputs.size();
	for(unsigned int i = 0; i < this->inputCnt; i++){
		fooGate.name = stdInputs[i];
		fooGate.GateType = "input";
		fooGate.outNets.resize(1);
		this->nodes.push_back(fooGate);
	}
	fooGate.outNets.resize(0);

	// Outputs
	this->outputCnt = stdOutputs.size();
	for(unsigned int i = 0; i < this->outputCnt; i++){
		fooGate.name = stdOutputs[i];
		fooGate.GateType = "output";
		fooGate.inNets.resize(1);
		this->nodes.push_back(fooGate);
	}

	// Gates
	this->gateCnt = stdGate.size();
	for(unsigned int i = 0; i < this->gateCnt; i++){
		fooGate.name = stdGate[i].GateType + "_" + to_string(i) + "";
		fooGate.GateType = stdGate[i].GateType;
		fooGate.inNets.resize(stdGate[i].inputs.size());
		fooGate.outNets.resize(stdGate[i].outputs.size());
		this->nodes.push_back(fooGate);
	}

	/* ------------------- Define Nets -------------------- */
	// inputs
	for(unsigned int i = 0; i < this->inputCnt; i++){
		fooNet.name = "net_" + to_string(this->nets.size());
		fooNet.inNodes.push_back(i);

		for(unsigned int j = 0; j < stdGate.size(); j++){
			for(unsigned int k = 0; k < stdGate[j].inputs.size(); k++){
				if(!stdInputs[i].compare(stdGate[j].inputs[k])){
					fooNet.outNodes.push_back(j + this->inputCnt + this->outputCnt);
					this->nodes[i].outNets[0] = this->nets.size();
					this->nodes[j + this->inputCnt + this->outputCnt].inNets[k] = this->nets.size();
				}
			}
		}

		this->nets.push_back(fooNet);
		fooNet.inNodes.clear();
		fooNet.outNodes.clear();
	}

	// gates. Assuming gates only have 1 output before SFQ conversion
	for(unsigned int i = 0; i < this->gateCnt; i++){
		fooNet.name = "net_" + to_string(this->nets.size());
		fooNet.inNodes.push_back(i + this->inputCnt + this->outputCnt);
		this->nodes[i + this->inputCnt + this->outputCnt].outNets[0] = this->nets.size();

		for(unsigned int j = 0; j < this->gateCnt; j++){
			for(unsigned int k = 0; k < stdGate[j].inputs.size(); k++){
				if(!stdGate[i].outputs[0].compare(stdGate[j].inputs[k])){
					fooNet.outNodes.push_back(j +  + this->inputCnt + this->outputCnt);
					// this->nodes[i + this->inputCnt + this->outputCnt].outNets[0] = this->nets.size();
					this->nodes[j + this->inputCnt + this->outputCnt].inNets[k] = this->nets.size();
					// this->nets.push_back(fooNet);
				}
			}
		}

		if(fooNet.outNodes.size() != 0){
			this->nets.push_back(fooNet);
		}

		// this->nets.push_back(fooNet);
		fooNet.inNodes.clear();
		fooNet.outNodes.clear();
	}


	for(unsigned int i = 0; i < this->gateCnt; i++){
		fooNet.name = "net_" + to_string(this->nets.size());

		for(unsigned int j = 0; j < this->outputCnt; j++){
			if(!stdGate[i].outputs[0].compare(stdOutputs[j])){
				fooNet.inNodes.push_back(i  + this->inputCnt + this->outputCnt);
				fooNet.outNodes.push_back(j + this->inputCnt);
				this->nodes[i + this->inputCnt + this->outputCnt].outNets[0] = this->nets.size();
				this->nodes[j + this->inputCnt].inNets[0] = this->nets.size();
				this->nets.push_back(fooNet);
				fooNet.inNodes.clear();
				fooNet.outNodes.clear();
				break;
			}
		}
	}

	return 1;
}

/**
 * [SFQBlif::to_blif - Creates a .blif file from the class]
 * @param  FileName [The .blif file name to be created]
 * @return          [1 - All good, 0 - Error]
 */

int SFQBlif::to_blif(string FileName){

	cout << "Generating Blif file:\"" << FileName << "\"" << endl;
	FILE *SFQblif;
  SFQblif = fopen(FileName.c_str(), "w");

  string lineStr;

	lineStr = "# Created \"" + FileName + "\" using ViPeR.\n";
	fputs(lineStr.c_str(), SFQblif);

	lineStr = ".model " + this->modelName + "\n";
  fputs(lineStr.c_str(), SFQblif);

	lineStr = ".inputs";

  for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("input")){
		  lineStr = lineStr + " " + this->nodes[i].name;
		}
  }
	lineStr = lineStr + "\n";
  fputs(lineStr.c_str(), SFQblif);

	lineStr = ".outputs";
	for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("output")){
		  lineStr = lineStr + " " + this->nodes[i].name;
		}
  }
	lineStr = lineStr + "\n";
  fputs(lineStr.c_str(), SFQblif);

  // --------------- Gates -------------------

  for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("input") || !this->nodes[i].GateType.compare("output")){
		  continue;
		}

  	if(!this->nodes[i].GateType.compare("SC")){
		  continue;
		}

		// cout << this->nodes[i].name << endl;

		lineStr = ".gate " + this->nodes[i].GateType;

		for(unsigned int j = 0; j < this->nodes[i].inNets.size(); j++){
			if(!this->nodes[this->nets[this->nodes[i].inNets[j]].inNodes[0]].GateType.compare("input")){
				lineStr += " in" + to_string(j) + "=" + this->nodes[this->nets[this->nodes[i].inNets[j]].inNodes[0]].name;
			}
			else{
				lineStr += " in" + to_string(j) + "=" + this->nets[this->nodes[i].inNets[j]].name;
			}
		}

		// Assuming that the output of the circuit can only be connected to a net with 1 output being it
		for(unsigned int j = 0; j < this->nodes[i].outNets.size(); j++){
			// if(this->nets[this->nodes[i].outNets[j]].outNodes[0] < this->inputCnt + this->outputCnt){
			if(!this->nodes[this->nets[this->nodes[i].outNets[j]].outNodes[0]].GateType.compare("output")){
				lineStr += " out" + to_string(j) + "=" + this->nodes[this->nets[this->nodes[i].outNets[j]].outNodes[0]].name;
			}
			else{
				lineStr += " out" + to_string(j) + "=" + this->nets[this->nodes[i].outNets[j]].name;
			}
		}

		lineStr = lineStr + "\n";
	  fputs(lineStr.c_str(), SFQblif);
  }

	lineStr = ".end";
  fputs(lineStr.c_str(), SFQblif);

  fclose(SFQblif);
  cout << ".blif file done." << endl;

	return 1;
}

/**
 * [SFQBlif::to_jpg - Creates a .jpg file using .gv(dot) file]
 * @param  fileName [The name of the .jpg file to be created]
 * @return          [1 - All good; 0 - Error]
 */

int SFQBlif::to_jpg(string fileName){
	cout << "Generating Dot file for \"" << fileName << "\"" << endl;
	FILE *dotFile;

	string gvFile = fileExtensionRenamer(fileName, ".gv");


  dotFile = fopen(gvFile.c_str(), "w");

  string lineStr;

  lineStr = "digraph " + this->modelName + " {\n";
  // lineStr = "digraph GenCir {\n";
  fputs(lineStr.c_str(), dotFile);


  // Define the inputs and outputs (not necessary)
  for(unsigned int i = 0; i < this->inputCnt; i++){
	  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=red]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }
	for(unsigned int i = this->inputCnt; i < this->inputCnt + this->outputCnt; i++){
	  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=blue]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }
  for(unsigned int i = this->inputCnt + this->outputCnt; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare(DFF_NAME)){
		  lineStr = "\t" + this->nodes[i].name + " [color=blue]; \n";
		  fputs(lineStr.c_str(), dotFile);
  	}
  }
  for(unsigned int i = this->inputCnt + this->outputCnt; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare(CLK_GATE_NAME)){
		  lineStr = "\t" + this->nodes[i].name + " [color=red]; \n";
		  fputs(lineStr.c_str(), dotFile);
  	}
  }

  // Still assuming that each net can only have 1 input node

  for(unsigned int i = 0; i < this->nets.size(); i++){
  	if(!this->nets[i].name.compare("NULL")){
  		continue;
  	}
  	for(unsigned int j = 0; j < this->nets[i].outNodes.size(); j++){
	  	lineStr = "\t" + this->nodes[this->nets[i].inNodes[0]].name + " -> " + this->nodes[this->nets[i].outNodes[j]].name + ";\n";
		  fputs(lineStr.c_str(), dotFile);
  	}
	}

  lineStr = "}";
  fputs(lineStr.c_str(), dotFile);

  fclose(dotFile);
  cout << "Dot file done." << endl;

  cout << "Creating \"" << fileName << "\"" << endl;

	stringstream sstr;
	sstr << "dot -Tjpg " << gvFile << " -o " << fileName;
	string bashCmd = sstr.str();

	if(system(bashCmd.c_str()) == -1){
		cout << "Bash command :\"" << bashCmd << "\" error." << endl;
	}

	return 1;
}



/**
 * [SFQBlif::to_str - Displays all the data stored in the class.]
 */

void SFQBlif::to_str(){
	cout << "SFQBlif Class:" << endl;
	cout << "\tModel name: " << this->modelName << endl;
	cout << "\tNo. Splitters: " << this->splitCnt << endl;
	cout << "\tNo. DFFs: " << this->dffCnt << endl;
	cout << "\tNo. Levels: " << this->levelCnt << endl;

	cout << "\tNodes: " << this->nodes.size() << endl;
	for(unsigned int i = 0; i < this->nodes.size(); i++){
		cout << "\t\tGate No: " << i << endl;
		cout << "\t\tType: " << this->nodes[i].GateType << endl;
		cout << "\t\tName: " << this->nodes[i].name << endl;
		cout << "\t\tStrRef: " << this->nodes[i].strRef << endl;
		cout << "\t\tCorX: " << this->nodes[i].corX << endl;
		cout << "\t\tCorY: " << this->nodes[i].corY << endl;

		cout << "\t\tIn nets:";
		for(unsigned int j = 0; j < this->nodes[i].inNets.size(); j++){
			cout << "\t" << this->nodes[i].inNets[j];
		}
		cout << endl;

		cout << "\t\tOut nets:";
		for(unsigned int j = 0; j < this->nodes[i].outNets.size(); j++){
			cout << "\t" << this->nodes[i].outNets[j];
		}
		cout << endl;

		cout << "\t\tclkNet nets: " << this->nodes[i].clkNet;

		cout << endl;


		cout << "\t\tLevels:";
		cout << "\t\t\tClock: " << this->nodes[i].CLKlevel << endl;
		cout << "\t\t\tMaximum: " << this->nodes[i].MaxLevel << endl;
		cout << "\t\t\tMinimum: " << this->nodes[i].MinLevel << endl;

		cout << endl;
	}

	cout << "\tNets: " << this->nets.size() << endl;

	cout << setw(15) << "Net_Name[n]:" << setw(10) << "inNode" << setw(4) << "->" << setw(10) << "outNode" << endl;

	unsigned int index = 0;
	string foo;
	for(unsigned int i = 0; i < this->nets.size(); i++){
		foo = "[" + to_string(i) + "]:";
		cout <<  setw(10) << foo << setw(10) << this->nets[i].name;

		index = 0;

		while(index < this->nets[i].inNodes.size() || index < this->nets[i].outNodes.size()){
			if(index < this->nets[i].inNodes.size()){
				cout << setw(15) << this->nodes[this->nets[i].inNodes[index]].name << setw(4) << "->";
			}
			else{
				cout << setw(39) << "->";
			}

			if(index < this->nets[i].outNodes.size()){
				// cout << setw(10) << "outNode" << endl;
				cout << setw(10) << this->nodes[this->nets[i].outNodes[index]].name << endl;
			}
			else{
				// blank
				cout << endl;
			}

			index++;
		}

	}

}

/* ----------------------------------------------------------------------------------------
   ------------------------------------- RAW blif Parser ----------------------------------
   ---------------------------------------------------------------------------------------- */

/**
 * [sBlifFile::importBlif description]
 * @param  fileName [The input file name of the .blif file to be imported]
 * @return          [1 - all good; 0 - error]
 */

int BlifFile::importBlif(string fileName){
	cout << "Importing \"" << fileName << "\" with Blif Parser." << endl;
	ifstream blifFile;
	string lineIn;
	char tempChar;
	vector<string> lineSplit;
	int i;

	blifFile.open(fileName);

	if(!blifFile.is_open()){
		cout << ".blif file FAILED to be properly opened" << endl;
		return 0;
	}

	while(getline(blifFile, lineIn)){
		// Concatenates lines split into two with "\" char
		while(lineIn[lineIn.size() - 1] == '\\'){
			lineIn.pop_back();
			string temp;
			getline(blifFile, temp);
			lineIn = lineIn + temp;
		}
		// removes leading white spaces and checks for commented and empty lines

		tempChar = (char)lineIn.front();
		while(tempChar == ' ' || tempChar == '\t'){
			lineIn.erase(0, 1);
			tempChar = (char)lineIn.front();
		}
		if(tempChar == '\n' || tempChar == '#' || tempChar == '\0'){
			continue;
		}

		// processing the line in
		stringSplitVec(lineSplit, lineIn);
		i = this->processLine(lineSplit);
		if(i == 1){
			// all good
		}
		else if(i == 0){
			cout << "Failed to process .blif file." << endl;
			cout << "Error with: \"" << lineIn << "\"" << endl;
			return 0;
		}
		else if(i == 2){
			break;
		}
	}

	return 1;
}

/**
 * [processLine - Assigns the line vector to the correct variables]
 * @param  inVec [The line read in vector, separated by white spaces]
 * @return       [2 - end of file; 1 - all good; 0 - error]
 */

int BlifFile::processLine(vector<string> &inVec){
	string key = inVec[0];
	if(!inVec[0].compare(".gate") || !inVec[0].compare(".subckt")){
		BlifGate tempGate;
		// vector<string> tempVec;
		vector<string> fooIns;
		vector<string> fooOuts;

		tempGate.GateType = inVec[1];
		tempGate.name = inVec[1] + "_" + to_string(this->gates.size());

		splitGateNet(inVec, fooIns, fooOuts);

		tempGate.inputs = fooIns;
		tempGate.outputs = fooOuts;

		this->gates.push_back(tempGate);
	}
	else if(!inVec[0].compare(".outputs")){
		for(unsigned int i = 1; i < inVec.size()-1; i++){
			this->outputs.push_back(inVec[i]);
		}
	}
	else if(!inVec[0].compare(".inputs")){
		for(unsigned int i = 1; i < inVec.size()-1; i++){
			this->inputs.push_back(inVec[i]);
		}
	}
	else if(!inVec[0].compare(".model")){
		this->modelName = inVec[1];
	}
	else if(!inVec[0].compare(".end")){
		// Do nothing
	}
	else if(!inVec[0].compare(".names")){
		// Do nothing
	}
	else{
		return 0;
	}

	// cout << "No \".end\" found in .blif file." << endl;
	return 1;
}

/**
 * [sBlifFile::to_str - Displays all the data assigned to the class]
 */

void BlifFile::to_str(){
	cout << "Blif Class:" << endl;
	cout << "\tName: " << this->modelName << endl;

	cout << "\tInputs: ";
	for(unsigned int i = 0; i < this->inputs.size(); i++)
		cout << "\t"<< this->inputs[i];
	cout << endl;

	cout << "\tOutputs: ";
	for(unsigned int i = 0; i < this->outputs.size(); i++)
		cout << "\t"<< this->outputs[i];
	cout << endl;

	cout << "\tGates: " << endl;
	for(unsigned int i = 0; i < this->gates.size(); i++){
		// cout << "\t\tName: " << this->gates[i].name << endl;
		cout << "\t\tType: " << this->gates[i].GateType << endl;

		cout << "\t\tPins: " << endl;
		for(unsigned int j = 0; j < this->gates[i].inputs.size(); j++){
			cout << "\t\t\tIn" << j << " net: " << this->gates[i].inputs[j] << endl;
		}

		for(unsigned int j = 0; j < this->gates[i].outputs.size(); j++){
			cout << "\t\t\tOut" << j << " net: " << this->gates[i].outputs[j] << endl;
		}

	}
}

/**
 * [splitGateNetV2 - Extracts the inputs and the outputs from .gate]
 * @param  fooVec    [The raw input vector of the gate(.gate XOR2T in1=b0 in2=a0 out=s0)]
 * @param  inputVec  [The list of the inputs]
 * @param  outputVec [The list of the outputs]
 * @return           [1 - all good; 0 - error]
 */

int splitGateNet(vector<string> &fooVec, vector<string> &inputVec, vector<string> &outputVec){
	for(unsigned int i = 2; i < fooVec.size()-1; i++){
		size_t EqPos = 0;
		size_t StrLen = 0;

		StrLen = fooVec[i].length();
		// disVector(fooVec);
		EqPos = fooVec[i].find("=");
		if(EqPos == string::npos){
			cout << "Error extracting .blif file. Incorrect naming of pins." << endl;
			return 0;
		}

		EqPos++;

		if(fooVec[i].find("in") != string::npos){
			inputVec.push_back(fooVec[i].substr(EqPos, StrLen));
		}
		else if(fooVec[i].find("out") != string::npos){
			outputVec.push_back(fooVec[i].substr(EqPos, StrLen));
		}
		else{
			cout << "Error extracting .blif file. Incorrect naming of pins." << endl;
			return 0;
		}
	}

	return 1;
}