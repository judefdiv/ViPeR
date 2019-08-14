/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-07-1
 * Modified:
 * license: 
 * Description: Created a SFQ blif blif from a standard SFQ blif file
 * File:				ForgeSFQblif.cpp
 */

#include "die2sim/ForgeSFQblif.hpp"

/**
 * Constructor
 */

ForgeSFQBlif::ForgeSFQBlif(){

}

/**
 * [ForgeSFQBlif::importBlif description]
 * @param  fileName [description]
 * @return          [description]
 */

int ForgeSFQBlif::importBlif(string fileName){
	blifFile.importStdBlif(fileName);

	blifFile.get_Nodes(nodes);
	blifFile.get_Nets(nets);

	return 1;
}

/**
 * [ForgeSFQBlif::toSFQ - Creates a SFQ circuit from the imported CMOS circuit]
 * @return [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::toSFQ(){
	cout << "Converting CMOS circuit to SFQ." << endl;
	
	// Old
	// this->insertSplitters();
	// this->insertDFFs();
	// this->cleanLevels();
	// this->insertDFFsPost();

	// this->calcCLKlevels();
	// this->printCLKlevels();
	

	// improved
	this->insertDFFs();
	this->insertSplitters();
	
	this->calcCLKlevels();
	this->printCLKlevels();



	return 1;
}


/**
 * [ForgeSFQBlif::FindLevels - Determines the levels of all the gates in the circuit]
 * @return [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::findLevels(){
	cout << "Determining levels." << endl;

	// resetting the levels variables
	for(unsigned int i = 0; i < this->nodes.size(); i++){
		// this->nodes[i].LevelOcc.clear();
		this->nodes[i].MaxLevel = 0;
		this->nodes[i].MinLevel = MaxNumberLevels;
	}

	vector<int> fooRoute;
	this->routes.clear();

	for(unsigned int i = 0; i < this->blifFile.get_inputCnt(); i++){
		// recursive; start with every inputs
		recurLevels(i, 0, fooRoute);
		// recurLevelsES(i, 0, fooRoute);
	}

	this->LevelCnt = this->routes[0].size();
	// this->printRoutes();
	cout << "Determining levels done." << endl;
	return 1;
}

/**
 * [ForgeSFQBlif::findLevelsES - Determines the levels of all the gates in the circuit EXCLUDING SPLITTERS]
 * @return [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::findLevelsES(){
	cout << "Determining levels without splitters." << endl;

	// for(unsigned int i = 0; i < this->nodes.size(); i++){
	// 	// this->nodes[i].LevelOcc.clear();
	// 	this->nodes[i].MaxLevel = 0;
	// 	this->nodes[i].MinLevel = MaxNumberLevels;
	// }

	vector<int> fooRoute;
	this->routes.clear();

	for(unsigned int i = 0; i < this->blifFile.get_inputCnt(); i++){
		// recursive; start with every inputs
		recurLevelsES(i, 0, fooRoute);
	}

	this->LevelCnt = this->routes[0].size();
	// this->printRoutes();
	cout << "Determining levels without splitters done." << endl;
	return 1;
}

/**
 * [ForgeSFQBlif::recurLevels - A recursive function that is used to determine the levels]
 * @param  curNode [The name of the output net(looking for) that one is looking for in the input nets of all the gates]
 * @param  curLev  [The current/level the flow is at]
 * @return         [1 - all good; 0 - error]
 */

int ForgeSFQBlif::recurLevels(unsigned int curNode, unsigned int curLev, vector<int> fooRoute){
	fooRoute.push_back(curNode);
	// Checking for the end.
	if(!this->nodes[curNode].GateType.compare("output")){
		this->routes.push_back(fooRoute);
		fooRoute.clear();
		this->setLevel(curNode, curLev);
		return 1;
	}

	for(unsigned int i = 0; i < this->nodes[curNode].outNets.size(); i++){
		for(unsigned int j = 0; j < this->nets[this->nodes[curNode].outNets[i]].outNodes.size(); j++){
			this->setLevel(curNode, curLev);
			this->recurLevels(this->nets[this->nodes[curNode].outNets[i]].outNodes[j], curLev+1, fooRoute);
		}
	}

	// cout << "These aren't the droids you are looking for." << endl;
	return 0;
}

int ForgeSFQBlif::recurLevelsES(unsigned int curNode, unsigned int curLev, vector<int> fooRoute){
	if(this->nodes[curNode].GateType.compare("SPLIT")){
		// cout << "Gate: " << this->nodes[curNode].GateType << endl;
		fooRoute.push_back(curNode);
	}

	if(!this->nodes[curNode].GateType.compare("output")){
		this->routes.push_back(fooRoute);
		fooRoute.clear();
		// this->setLevel(curNode, curLev);
		this->nodes[curNode].CLKlevel = curLev;
		return 1;
	}

	for(unsigned int i = 0; i < this->nodes[curNode].outNets.size(); i++){
		for(unsigned int j = 0; j < this->nets[this->nodes[curNode].outNets[i]].outNodes.size(); j++){
			// this->setLevel(curNode, curLev);
			this->nodes[curNode].CLKlevel = curLev;
			
			if(this->nodes[curNode].GateType.compare("SPLIT")){
				this->recurLevelsES(this->nets[this->nodes[curNode].outNets[i]].outNodes[j], curLev+1, fooRoute);
			}
			else{
				this->recurLevelsES(this->nets[this->nodes[curNode].outNets[i]].outNodes[j], curLev, fooRoute);
			}
		}
	}

	// cout << "These aren't the droids you are looking for." << endl;
	return 0;
}

/**
 * [ForgeSFQBlif::calcCLKlevels - Calculates the clock levels of the gates]
 * @return [1 - All good; 0 - error]
 */

int ForgeSFQBlif::calcCLKlevels(){
	if(this->isLevelsBalanced() == 0){
		cout << "The circuit is NOT balanced" << endl;
		return 0;
	}

	this->findLevelsES();
	this->printRoutes();
	this->CLKlevel.clear();
	this->CLKlevel.resize(this->nodes[this->routes[0].back()].CLKlevel+1);
	// this->CLKlevel.resize(this->routes[0].size());

	for(unsigned int i = 0; i < this->nodes.size(); i++){
		this->CLKlevel[this->nodes[i].CLKlevel].push_back(i);
	}

	return 1;
}

/**
 * [ForgeSFQBlif::isLevelsBalanced - Checks if all the routes are balanced]
 * @return [1 - Balanced; 0 - NOT]
 */

int ForgeSFQBlif::isLevelsBalanced(){
	this->findLevelsES();			// Does not have to be called here...

	unsigned int routeLen = this->routes[0].size();

	for(unsigned int i = 1; i < this->routes.size(); i++){
		if(routeLen == this->routes[i].size()){
			// still good
			routeLen = this->routes[i].size();
		}
		else{
			// fail
			cout << "The routes are NOT balanced." << endl;
			this->LevelCnt = 0;
			return 0;
		}
 	}

 	cout << "The routes are balanced." << endl;

	this->LevelCnt = this->routes[0].size();

	return 1;
}

/**
 * [ForgeSFQBlif::insertSplitters - Inserts splitters when a net has more than 1 output]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::insertSplitters(){
	cout << "Inserting Splitters." << endl;

	for(unsigned int i = 0; i < this->nets.size(); i++){
		if(this->nets[i].outNodes.size() == 2){
			insertGate("SPLIT", i, 1);
		}
		else if(this->nets[i].outNodes.size() > 2){
			cout << "3 Output splitter is required..." << endl;
			return 0;
		}
	}

	cout << "Done inserting splitters." << endl;

	return 1;
}

/**
 * [ForgeSFQBlif::insertDFFs - Inserts DFFs in order for the levels to be balanced]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::insertDFFs(){
	cout << "Inserting DFFs at the inputs." << endl;

	// insert DFFs for the inputs
	// finding the longed route per input
	this->findLevels();

	unsigned int inputLongestRouteLen[this->blifFile.get_inputCnt()] = {0};
	unsigned int inputLongestRoute = 0;
	
	for(unsigned int i = 0; i < this->routes.size(); i++){
		if(this->routes[i].size() > inputLongestRouteLen[this->routes[i][0]]){
			inputLongestRouteLen[this->routes[i][0]] = this->routes[i].size();
			
			if(inputLongestRoute < inputLongestRouteLen[this->routes[i][0]]){
				inputLongestRoute = inputLongestRouteLen[this->routes[i][0]];
			}
		}
	}
	
	for(int i = 0; i < this->blifFile.get_inputCnt() ; i++){
		if(inputLongestRoute > inputLongestRouteLen[i]){
			this->insertGate("DFF", this->nodes[i].outNets[0], inputLongestRoute - inputLongestRouteLen[i]);
		}
		else if(inputLongestRoute == inputLongestRouteLen[i]){
			// the same length, do nothing
		}
		else{
			cout << "Smoke detected" << endl;
			return 0;
		}
	}

	// insert DFFs for the rest of the circuit
	cout << "Inserting DFFs at the throughout the circuit." << endl;
	this->findLevels();

	unsigned int fooNet;
	unsigned int fooNetLevel;

	for(int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){
		if(this->nodes[i].MaxLevel - this->nodes[i].MinLevel > 0){
			fooNet = 0;
			fooNetLevel = MaxNumberLevels;
			for(unsigned int j = 0; j < this->nodes[i].inNets.size(); j++){
				// assuming that upper nodes are balanced

				// cout << "Checking: " << this->nodes[this->nets[this->nodes[i].inNets[j]].inNodes[0]].name << endl;
					
				if(this->nodes[this->nets[this->nodes[i].inNets[j]].inNodes[0]].MaxLevel < fooNetLevel){
					fooNetLevel = this->nodes[this->nets[this->nodes[i].inNets[j]].inNodes[0]].MaxLevel;
					fooNet = this->nodes[i].inNets[j];

				}

			}

			this->insertGate("DFF", fooNet, this->nodes[i].MaxLevel - this->nodes[i].MinLevel);
			this->findLevels();
			
		}
		else if(this->nodes[i].MaxLevel - this->nodes[i].MinLevel == 0){
			// just do nothing
		}
		else{
			cout << "Smoke detected." << endl;
			return 0;
		}
	}
	
	// insert DFFs for the outputs
	cout << "Inserting DFFs at the outputs." << endl;

	unsigned int maxLev = 0;	

	for(int i = this->blifFile.get_inputCnt(); i < this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i++){
		// cout << "Checking: " << this->nodes[i].name << endl;
		if(this->nodes[i].MaxLevel > maxLev){
			maxLev = this->nodes[i].MaxLevel;
		}
	}

	for(int i = this->blifFile.get_inputCnt(); i < this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i++){
		if(this->nodes[i].MaxLevel < maxLev){
			this->insertGate("DFF", this->nodes[i].inNets[0], maxLev - this->nodes[i].MaxLevel);
		}
	}

	this->findLevels();
	this->printRoutes();


	cout << "Done inserting DFFs." << endl;
	return 1;
}

/**
 * [ForgeSFQBlif::insertDFFsPost - Inserts DFFs in order for the levels to be balanced]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::insertDFFsPost(){
	cout << "Inserting POST DFFs." << endl;

	// insert DFFs for the inputs
	// finding the longed route per input
	this->findLevelsES();

	unsigned int inputLongestRouteLen[this->blifFile.get_inputCnt()] = {0};
	unsigned int inputLongestRoute = 0;
	
	for(unsigned int i = 0; i < this->routes.size(); i++){
		if(this->routes[i].size() > inputLongestRouteLen[this->routes[i][0]]){
			inputLongestRouteLen[this->routes[i][0]] = this->routes[i].size();
			
			if(inputLongestRoute < inputLongestRouteLen[this->routes[i][0]]){
				inputLongestRoute = inputLongestRouteLen[this->routes[i][0]];
			}
		}
	}
	
	for(int i = 0; i < this->blifFile.get_inputCnt() ; i++){
		if(inputLongestRoute > inputLongestRouteLen[i]){
			this->insertGate("DFF", this->nodes[i].outNets[0], inputLongestRoute - inputLongestRouteLen[i]);
		}
		else if(inputLongestRoute == inputLongestRouteLen[i]){
			// the same length, do nothing
		}
		else{
			cout << "Smoke detected" << endl;
			return 0;
		}
	}

	cout << "Done inserting DFFs" << endl;
	return 1;
}

/**
 * [ForgeSFQBlif::cleanLevels - Removes DFFs on levels that have SPLITTERs and DFFs only]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::cleanLevels(){
	cout << "Removing excessive DFFs." << endl;
	string DFFstr = "DFF";
	string SplitterStr = "SPLIT";

	// if(!this->isLevelsBalanced()){
	// 	cout << "Could not clean the levels" << endl;
	// 	return 0;
	// }

	bool levelDelete[this->LevelCnt];


	levelDelete[0] = false;
	levelDelete[this->LevelCnt-1] = false;

	for(unsigned int i = 1; i < this->LevelCnt-1; i++){
		levelDelete[i] = true;
	}

	for(unsigned int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){
		if(!(!this->nodes[i].GateType.compare(DFFstr) || !this->nodes[i].GateType.compare(SplitterStr))){
			levelDelete[this->nodes[i].MaxLevel] = false;
		}
	}

	// cout << "Levels to be cleared:" << endl;
	// for(unsigned int i = 0; i < this->LevelCnt; i++){
	// 	cout << "L[" << i << "]: ";
	// 	if(levelDelete[i]){
	// 		cout << "to be CLEARED" << endl;
	// 	}
	// 	else{
	// 		cout << "good" << endl;
	// 	}
	// }

	for(unsigned int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){
		if(!this->nodes[i].GateType.compare(DFFstr)){
			if(levelDelete[this->nodes[i].MaxLevel] == true){
				this->deleteGate(i);
			}
		}
	}

	cout << "Removing excessive DFFs done." << endl;

	return 1;
}

/**
 * [ForgeSFQBlif::insertGate - Inserting gates at the input of the specified net]
 * @param  GateType [The name of the type of gate to be inserted]
 * @param  netNo    [The index of the net to have gates inserted at its input]
 * @param  noGates  [Number of gates to be inserted]
 * @return          [1 - all good; 0 - error]
 */

int ForgeSFQBlif::insertGate(string GateType, unsigned int netNo, unsigned int noGates){
	// assuming that each net has only one input!!!!!!!!!!!!!!!
	cout << "Inserting " << noGates << " " << GateType << "s after: " << this->nodes[this->nets[netNo].inNodes[0]].name << endl;

	BlifNode fooNode;
	BlifNet  fooNet;

	for(unsigned int i = 0; i < noGates; i++){
		fooNode.name = GateType + "_" + to_string(this->nodes.size()) + "";
		fooNode.GateType = GateType;
		fooNode.inNets.clear();
		fooNode.outNets.clear();
		fooNode.inNets.push_back(this->nets.size());
		fooNode.outNets.push_back(netNo);

		fooNet.name = "net_" + to_string(this->nets.size());
		fooNet.inNodes.clear();
		fooNet.outNodes.clear();
		fooNet.inNodes.push_back(this->nets[netNo].inNodes[0]);
		fooNet.outNodes.push_back(this->nodes.size());

		this->nodes[this->nets[netNo].inNodes[0]].outNets[0] = this->nets.size();
		this->nets[netNo].inNodes[0] = this->nodes.size();	

		this->nodes.push_back(fooNode);
		this->nets.push_back(fooNet);
		netNo = this->nets.size() -1;
	}

	return 1;
}

/**
 * [ForgeSFQBlif::deleteGate - Removes]
 * @param  GateIndex [The index number of the node]
 * @return           [1 - all good; 0 - error]
 */

int ForgeSFQBlif::deleteGate(unsigned GateIndex){
	// Bunch of error checking that should be implemented...
	// Assuming that node/gate only has one input and one output net, like DFF
	// return 0;

	cout << "Nulling/deleting node[" << GateIndex << "]: " << this->nodes[GateIndex].name << endl;

	
	// Reassigning output node's input
	this->nets[this->nodes[GateIndex].outNets[0]].inNodes[0] = this->nets[this->nodes[GateIndex].inNets[0]].inNodes[0];

	// Reassigning input node's output
	this->nodes[this->nets[this->nodes[GateIndex].inNets[0]].inNodes[0]].outNets[0] = this->nodes[GateIndex].outNets[0];

	// Killing the input node
	this->nets[this->nodes[GateIndex].inNets[0]].name = "NULL";
	this->nets[this->nodes[GateIndex].inNets[0]].outNodes.clear();
	this->nets[this->nodes[GateIndex].inNets[0]].inNodes.clear();

	// Killing the node
	this->nodes[GateIndex].name = "NULL_" + to_string(GateIndex);
	this->nodes[GateIndex].GateType = "NULL";
	this->nodes[GateIndex].inNets.clear();
	this->nodes[GateIndex].outNets.clear();

	return 1;
}


/**
 * [ForgeSFQBlif::setLevel - Sets the levels for the node]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::setLevel(unsigned int nodeIndex, unsigned int curLev){
	// this->nodes[nodeIndex].LevelOcc.push_back(curLev);

	if(curLev > this->nodes[nodeIndex].MaxLevel){
		this->nodes[nodeIndex].MaxLevel = curLev;
	}

	if(curLev < this->nodes[nodeIndex].MinLevel){
		this->nodes[nodeIndex].MinLevel = curLev;
	}

	if(curLev > this->blifFile.get_levelCnt()){
		this->blifFile.set_levelCnt(curLev);
	}

	return 1;
}

/**
 * [ForgeSFQBlif::to_jpgAdv - Creates an advanced flow diagram of the .blif class]
 * @param  fileName [The file name of the flow diagram to be created]
 * @return          [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::to_jpgAdv(string fileName){
	cout << "Generating Dot file for \"" << fileName << "\"" << endl;
	FILE *dotFile;

  dotFile = fopen(binGVfile, "w");

  string lineStr;

  lineStr = "digraph " + this->blifFile.get_modelName() + " {\n";
  // lineStr = "digraph GenCir {\n";
  fputs(lineStr.c_str(), dotFile);


  // Define the different shapes and colours for inputs, outputs, DFFs and splitters
  for(unsigned int i = 0; i < this->blifFile.get_inputCnt(); i++){
	  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=red]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }
	for(unsigned int i = this->blifFile.get_inputCnt(); i < this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i++){
	  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=blue]; \n";
	  fputs(lineStr.c_str(), dotFile);
  }
  for(unsigned int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("DFF")){
		  lineStr = "\t" + this->nodes[i].name + " [color=blue]; \n";
		  fputs(lineStr.c_str(), dotFile);
  	}
  }
  for(unsigned int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("SPLIT")){
		  lineStr = "\t" + this->nodes[i].name + " [color=red]; \n";
		  fputs(lineStr.c_str(), dotFile);
  	}
  }

  // Drawing in the clock cycles:
  lineStr = "\tranksep=.75; { node [shape=plaintext];";
  fputs(lineStr.c_str(), dotFile);
  lineStr = "\t0";
  for(unsigned int i = 1; i < this->CLKlevel.size(); i++){
  	lineStr += " -> " + to_string(i);
  }
  lineStr += " }";
  fputs(lineStr.c_str(), dotFile);

  for(unsigned int i = 0; i < this->CLKlevel.size(); i++){
  	lineStr = "\t{ rank = same; " + to_string(i) + ";";
  	for(unsigned int j = 0; j < this->CLKlevel[i].size(); j++){
  		if(!this->nodes[this->CLKlevel[i][j]].GateType.compare("NULL") || !this->nodes[this->CLKlevel[i][j]].GateType.compare("SPLIT"))
  			continue;
  		lineStr += " \"" + this->nodes[this->CLKlevel[i][j]].name + "\";";
  	}
	  lineStr += " }\n";
	  fputs(lineStr.c_str(), dotFile);
  }

  // Creating the routes
  // Still assuming that each net can only have 1 input node
  for(unsigned int i = 0; i < this->nets.size(); i++){
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
	sstr << "dot -Tjpg " << binGVfile << " -o " << fileName;
	string bashCmd = sstr.str();

	if(system(bashCmd.c_str()) == -1){
		cout << "Bash command :\"" << bashCmd << "\" error." << endl;
	}

	return 1;
}

/**
 * [ForgeSFQBlif::to_blif - Generating a blif file from the class]
 * @param  fileName [The file name of the .blif file to be created]
 * @return          [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::to_blif(string fileName){
	blifFile.receiveNodesNets(this->nodes, this->nets);
	blifFile.to_blif(fileName);
	return 1;
}

/**
 * [ForgeSFQBlif::to_jpg - Creates a flow diagram of the .blif class]
 * @param  fileName [The file name of the flow diagram to be created]
 * @return          [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::to_jpg(string fileName){
	blifFile.receiveNodesNets(this->nodes, this->nets);
	blifFile.to_jpg(fileName);
	return 1;
}

/**
 * [ForgeSFQBlif::to_str - Displays all the data assigned to the class]
 */

void ForgeSFQBlif::to_str(){
	blifFile.receiveNodesNets(this->nodes, this->nets);
	blifFile.to_str();
}

/**
 * [ForgeSFQBlif::printRoutes - Displays the routes]
 */

void ForgeSFQBlif::printRoutes(){
	cout << "All possible routes:" << endl;

	unsigned int j;

	for(unsigned int i = 0; i < this->routes.size(); i++){
		for(j = 0; j < this->routes[i].size()-1; j++){
				cout << this->nodes[this->routes[i][j]].name << " -> ";
		}
		cout << this->nodes[this->routes[i][j]].name << endl;
	}
}

/**
 * [ForgeSFQBlif::printCLKlevels - Displays the clock levels]
 */

void ForgeSFQBlif::printCLKlevels(){
	cout << "Clock levels:" << endl;

	unsigned int j;

	for(unsigned int i = 0; i < this->CLKlevel.size(); i++){
		for(j = 0; j < this->CLKlevel[i].size()-1; j++){
				cout << "\"" << this->nodes[this->CLKlevel[i][j]].name << "\"; ";
		}
		cout << this->nodes[this->CLKlevel[i][j]].name << endl;
	}
}