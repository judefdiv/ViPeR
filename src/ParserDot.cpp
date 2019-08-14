/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-19
 * Modified:
 * license: 
 * Description: Parser for blif to Dot.
 * File:				Parserdot.cpp
 */

#include "die2sim/ParserDot.hpp"


int DotFile::no_Dot_inst = 0;

/**
 * [DotFile::DotFile - Constructor]
 */

DotFile::DotFile(){
	DotFile::no_Dot_inst++;
}


/**
 * [DotFile::setFileName - Sets/defines the output file name for Dot - .gv]
 * @param fileName [description]
 */

void DotFile::setFileName(string fileName){
	this->DotFileName = fileName;
}

/**
 * [DotFile::importBlif - Imports/converts a .blif file to dot format]
 * @param  fileName [File name of the .blif file to be imported]
 * @return          [1 - All good; 0 - error]
 */

int DotFile::importBlif(string fileName){
	cout << "Converting .blif to .gv(Dot)" << endl;
	BlifFile blifF;
	vector<BlifGate> blifGates;
	vector<string> blifInputs;
	vector<string> blifOutputs;

	blifF.importBlif(fileName);
	blifF.get_varGates(blifGates);
	blifF.get_varOutputs(blifOutputs);
	blifF.get_varInputs(blifInputs);

	this->setInputNodes(blifInputs);
	this->setOutputNodes(blifOutputs);

	// Mapping the inputs
	// cout << "Mapping the inputs" << endl;
	for(unsigned int i = 0; i < blifInputs.size(); i++){
	// cout << "input[" << i << "]: " <<  blifInputs[i] << endl;
		for(unsigned int j = 0; j < blifGates.size(); j++){
			// cout << "gate[" << j << "]: " <<  blifGates[j].name << endl;
			for(unsigned int k = 0; k < blifGates[j].inputs.size(); k++){
				if(!blifInputs[i].compare(blifGates[j].inputs[k])){
					this->createNet(blifInputs[i], blifGates[j].name);
					break;
				}
			}
		}
	}

	// Mapping the middle???
	// cout << "Mapping the middle???" << endl;
	for(unsigned int i = 0; i < blifGates.size(); i++){
	// cout << "gate[" << i << "]: " <<  blifGates[i].name << endl;
		for(unsigned int j = 0; j < blifGates[i].inputs.size(); j++){
			// cout << "input[" << j << "]: " <<  blifGates[i].inputs[j] << endl;
			for(unsigned int k = 0; k < blifGates.size(); k++){
				for(unsigned int l = 0; l < blifGates[k].outputs.size(); l++)
					if(!blifGates[k].outputs[l].compare(blifGates[i].inputs[j])){
						this->createNet(blifGates[k].name, blifGates[i].name);
						break;
					}
			}
		}
	}

	// Mapping the outputs
	// cout << "Mapping the outputs" << endl;
	for(unsigned int i = 0; i < blifOutputs.size(); i++){
	// cout << "outputs[" << i << "]: " <<  blifOutputs[i] << endl;
		for(unsigned int j = 0; j < blifGates.size(); j++){
			// cout << "gate[" << j << "]: " <<  blifGates[j].name << endl;
			for(unsigned int k = 0; k < blifGates[j].outputs.size(); k++){
				if(!blifOutputs[i].compare(blifGates[j].outputs[k])){
					this->createNet(blifGates[j].name, blifOutputs[i]);
					break;																										// Warning about this
				}
			}
		}
	}

	return 1;
}

/**
 * [DotFile::createNet - Creates a net between 2 nodes.gates]
 * @param fromN [The name of the node coming from]
 * @param toN   [The name of the node going to]
 */

void DotFile::createNet(string fromN, string toN){
	this->fromNode.push_back(fromN);
	this->mustJoinNode.push_back(toN);
}


/**
 * [DotFile::genDot - Generates the .gv file from]
 * @return [1 - All good; 0 - error]
 */

int DotFile::genDot(){
	cout << "Generating Dot file:\"" << this->DotFileName.c_str() << "\" file" << endl;
	FILE *dotFile;
  dotFile = fopen(this->DotFileName.c_str(), "w");

  string lineStr;

  // lineStr = "digraph " + modelName + " {";
  lineStr = "digraph GenCir {\n";
  fputs(lineStr.c_str(), dotFile);


  // Define the inputs and outputs (not necessary)
  for(unsigned int i = 0; i < this->inputs.size(); i++){
	  lineStr = this->inputs[i] + " [shape=box, color=red]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }
	for(unsigned int i = 0; i < this->outputs.size(); i++){
	  lineStr = this->outputs[i] + " [shape=box, color=blue]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }

  for(unsigned int i = 0; i < this->fromNode.size(); i++){
  	lineStr = "\t" + this->fromNode[i] + " -> " + this->mustJoinNode[i] + ";\n";
	  fputs(lineStr.c_str(), dotFile);
	}

  lineStr = "}";
  fputs(lineStr.c_str(), dotFile);
  
  fclose(dotFile);
  cout << "Dot file done." << endl;
  return 1;
}

/**
 * [showBlif - Displays a blif file]
 * @param  blifFileName [File name of .blif]
 * @param  gvFileName   [File name of the dot - .gv (output)]
 * @param  jpgFileName  [File name of .jpg (output)]
 * @return              [1 - All good; 0 - error]
 */

int runDot(string blifFileName, string gvFileName, string jpgFileName){
	string bashCmd;
	bashCmd = "dot -Tjpg " + gvFileName + " -o " + jpgFileName;

	// drawDot(blifFileName, gvFileName);
	
	DotFile DotF;
	DotF.setFileName(gvFileName);
	DotF.importBlif(blifFileName);
	DotF.genDot();

	if(system(bashCmd.c_str()) == -1){
		cout << "Bash command :\"" << bashCmd << "\" error." << endl;
	}
	return 1;
}