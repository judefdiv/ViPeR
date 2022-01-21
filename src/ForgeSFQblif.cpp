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

#include "viper/ForgeSFQblif.hpp"

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
	this->blifFile.importStdBlif(fileName);

	this->blifFile.get_Nodes(nodes);
	this->blifFile.get_Nets(nets);

	this->inputCnt = this->blifFile.get_inputCnt();
	this->outputCnt = this->blifFile.get_outputCnt();;
	this->gateCnt = this->nodes.size() - this->inputCnt - this->outputCnt;

	return 1;
}

/**
 * [ForgeSFQBlif::toSFQ - Creates a SFQ circuit from the imported CMOS circuit]
 * @return [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::toSFQ(){
	cout << "Converting CMOS circuit to SFQ." << endl;

	// improved
	
	findLevels();
	// findLevelsES();
	// findLevelsWS();
	printRoutes();
	// printRoutesWS();

	this->insertDFFs();
	this->insertSplitters();

	this->calcCLKlevels();
	this->printCLKlevels();

	// --------------------------------------------------------------

	// this->clockIt();
	// Creating a clock(clk) pin to the input of the circuit
  BlifNode fooNode;
  fooNode.name = "clk";
  fooNode.GateType = "input";
  fooNode.outNets.push_back(this->nets.size());

  BlifNet fooNet;
  fooNet.name = "net_" + to_string(this->nets.size());
  fooNet.inNodes.push_back(this->nodes.size());

  this->nodes.push_back(fooNode);
  this->nets.push_back(fooNet);

  // --------------------------------------------------------------


	this->findLevelsWS();
	// this->printRoutesWS();

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

	vector<unsigned int> fooRoute;
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

	vector<unsigned int> fooRoute;
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
 * [ForgeSFQBlif::findLevelsWS - Determines the levels of all the gates in the circuit]
 * @return [1 - All good; 0 - Error]
 */

int ForgeSFQBlif::findLevelsWS(){
	cout << "Determining all possible routes." << endl;

	vector<unsigned int> fooRoute;
	this->routesWS.clear();

	for(unsigned int i = 0; i < this->blifFile.get_inputCnt(); i++){
		// recursive; start with every inputs
		recurLevelsWS(i, 0, fooRoute);
		// recurLevelsES(i, 0, fooRoute);
	}

	// this->LevelCnt = this->routes[0].size();
	// this->printRoutes();
	cout << "Determining all possible routes done." << endl;
	return 1;
}

/**
 * [ForgeSFQBlif::recurLevels - A recursive function that is used to determine the levels]
 * @param  curNode [The name of the output net(looking for) that one is looking for in the input nets of all the gates]
 * @param  curLev  [The current/level the flow is at]
 * @return         [1 - all good; 0 - error]
 */

int ForgeSFQBlif::recurLevels(unsigned int curNode, unsigned int curLev, vector<unsigned int> fooRoute){
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

int ForgeSFQBlif::recurLevelsES(unsigned int curNode, unsigned int curLev, vector<unsigned int> fooRoute){
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

int ForgeSFQBlif::recurLevelsWS(unsigned int curNode, unsigned int curLev, vector<unsigned int> fooRoute){
	fooRoute.push_back(curNode);
	// Checking for the end.
	if(!this->nodes[curNode].GateType.compare("output")){
		this->routesWS.push_back(fooRoute);
		fooRoute.clear();
		// this->setLevel(curNode, curLev);
		return 1;
	}

	for(unsigned int i = 0; i < this->nodes[curNode].outNets.size(); i++){
		for(unsigned int j = 0; j < this->nets[this->nodes[curNode].outNets[i]].outNodes.size(); j++){
			// this->setLevel(curNode, curLev);
			this->recurLevelsWS(this->nets[this->nodes[curNode].outNets[i]].outNodes[j], curLev+1, fooRoute);
		}
	}

	// cout << "These aren't the droids you are looking for." << endl;
	return 0;
}

// /**
//  * [ForgeSFQBlif::clockIt Clocks all the gates]
//  * @return [1 - All good; 0 - error]
//  */

// int ForgeSFQBlif::clockIt(){
// 	cout << "Clocking all the gates." << endl;

// 	// Creating a clock(clk) pin to the input of the circuit
// 	BlifNode fooNode;
// 	fooNode.name = "clk";
// 	fooNode.GateType = "input";
// 	fooNode.outNets.push_back(this->nets.size());

// 	BlifNet fooNet;
// 	fooNet.name = "net_" + to_string(this->nets.size());
// 	fooNet.inNodes.push_back(this->nodes.size());

// 	this->nodes.push_back(fooNode);
// 	this->nets.push_back(fooNet);

// 	// Recalculate the counts of the gates.
// 	this->calcStats();

// 	// Creating the h-tree clock
// 	unsigned int clkPins = this->gateCnt + this->DFFCnt;
// 	unsigned int clkLev = log(clkPins)/log(2) + 1;
// 	unsigned int clkSplit = pow(2, clkLev -1);

// 	cout << "Clock stats:" << endl;
// 	cout << "\tPins required: " << clkPins << endl;
// 	cout << "\tPins max: " << pow(2, clkLev) << endl;
// 	cout << "\tSplit levels: " << clkLev << endl;
// 	cout << "\tOutput splitters: " << clkSplit << endl;
// 	cout << "\tTotal splitters: " << clkSplit*2-1 << endl;

// 	this->recurCreateCLK(this->nets.size()-1, 0, clkLev);

// 	// for(unsigned int i = 0; i < this->finCLKsplit.size(); i++){
// 	// 	cout << this->nodes[this->finCLKsplit[i]].name << ", ";
// 	// }
// 	// cout << endl;

// 	cout << "Clocking all the gates done." << endl;
// 	return 1;
// }

// /**
//  * [ForgeSFQBlif::recurCreateCLK - creates the h-tree clock using depth first recursion]
//  * @param  net      [The previous splitter's index]
//  * @param  curLev   [The current level]
//  * @param  maxLev   [The final level the function must stop at]
//  * @return          [1 - All good; 0 - error]
//  */

// int ForgeSFQBlif::recurCreateCLK(unsigned int net, unsigned int curLev, unsigned int maxLev){

// 	if(curLev == maxLev){
// 		this->finCLKsplit.push_back(this->nets[net].inNodes[0]);
// 		return 1;
// 	}

// 	this->createSplitC(net);

// 	for(unsigned int i = 0; i < this->nodes[this->nets[net].outNodes[0]].outNets.size(); i++){
// 		if(this->recurCreateCLK(this->nodes[this->nets[net].outNodes[0]].outNets[i] , curLev+1, maxLev)){
// 			break;
// 		}
// 	}

// 	return 0;
// }

/**
 * [ForgeSFQBlif::createSplitC - creates a clock splitter]
 * @param  netNo [the net the splitter must split]
 * @return       [1 - All good; 0 - error]
 */

// int ForgeSFQBlif::createSplitC(unsigned int netNo){
// 	BlifNode fooNode;
// 	BlifNet fooNet;

// 	fooNode.name = "SC_" + to_string(this->nodes.size());
// 	fooNode.GateType = "SC";
// 	fooNode.inNets.push_back(netNo);
// 	fooNode.outNets.push_back(this->nets.size());
// 	fooNode.outNets.push_back(this->nets.size()+1);

// 	for(unsigned int i = 0; i < 2; i++){
// 		fooNet.name = "net_" + to_string(this->nets.size());
// 		fooNet.inNodes.clear();
// 		fooNet.inNodes.push_back(this->nodes.size());
// 		this->nets.push_back(fooNet);
// 	}

// 	this->nets[netNo].outNodes.push_back(this->nodes.size());
// 	this->nodes.push_back(fooNode);

// 	return 1;
// }

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
	// this->printRoutes();
	this->CLKlevel.clear();
	this->CLKlevel.resize(this->nodes[this->routes[0].back()].CLKlevel+1);

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
			// insertGate("SPLIT", i, 1);
			this->insertSplit(i);
		}
		else if(this->nets[i].outNodes.size() > 2){
			// cout << "3 Output splitter is required..." << endl;
			this->insertSplitAuto(i);
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

/**********************************************************************************
 *********************************** Beginning ************************************
 **********************************************************************************/

	cout << "Inserting DFFs at the inputs." << endl;

	// insert DFFs for the inputs
	// finding the longed route per input
	this->findLevels();

	unsigned int inputLongestRouteLen[this->blifFile.get_inputCnt()];
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

	cout << "Inserting DFFs at the inputs, done." << endl;

	// this->findLevels();
	// this->printRoutes();

	// using namespace std::this_thread;     // sleep_for, sleep_until
 //  using namespace std::chrono_literals; // ns, us, ms, s, h, etc.
 //  using std::chrono::system_clock;

	// sleep_for(5s);

/**********************************************************************************
 ************************************* Middle *************************************
 **********************************************************************************/

	// insert DFFs for the rest of the circuit
	cout << "Inserting DFFs at the throughout the circuit." << endl;
	this->findLevels();

	unsigned int fooNet;
	unsigned int fooNetLevel;

	// for(int i = this->blifFile.get_inputCnt() + this->blifFile.get_outputCnt(); i < this->nodes.size(); i++){

	for(int i = 0; i < this->nodes.size(); i++){
		if(!this->nodes[i].GateType.compare("input") || !this->nodes[i].GateType.compare("output") || !this->nodes[i].GateType.compare("DFF")){
			cout << "Skipping: " <<  this->nodes[i].name << endl;;
			continue;
		}

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

	// this->findLevels();

	cout << "Inserting DFFs at the throughout the circuit, done." << endl;

	// sleep_for(5s);

	/**********************************************************************************
   ************************************** End ***************************************
   **********************************************************************************/

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

	cout << "Inserting DFFs at the outputs, done." << endl;

	this->findLevels();
	// this->printRoutes();



	cout << "Done inserting DFFs." << endl;
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
 * [ForgeSFQBlif::insertSplit - inserts a splitter and creates 2 nets]
 * @param  netNo [description]
 * @return       [1 - all good; 0 - error]
 */

int ForgeSFQBlif::insertSplit(unsigned int netNo){
	cout << "Inserting splitter for: " << this->nodes[this->nets[netNo].inNodes[0]].name << endl;

	BlifNode splitNode;
	BlifNet  s1net, s2net;
	unsigned int index1, index2;

	splitNode.name = "SPLIT_" + to_string(this->nodes.size());
	splitNode.GateType = "SPLIT";
	splitNode.inNets.push_back(netNo);
	splitNode.outNets.push_back(this->nets.size());
	splitNode.outNets.push_back(this->nets.size()+1);

	s1net.name = "net_" + to_string(this->nets.size());
	s1net.inNodes.push_back(this->nodes.size());
	s1net.outNodes.push_back(this->nets[netNo].outNodes[0]);

	s2net.name = "net_" + to_string(this->nets.size()+1);
	s2net.inNodes.push_back(this->nodes.size());
	s2net.outNodes.push_back(this->nets[netNo].outNodes[1]);

	for(unsigned int i = 0; i < 2; i++){
		if(this->nodes[this->nets[netNo].outNodes[0]].inNets[i] == netNo){
			index1 = i;
		}
	}

	for(unsigned int i = 0; i < 2; i++){
		if(this->nodes[this->nets[netNo].outNodes[1]].inNets[i] == netNo){
			index2 = i;
		}
	}


	this->nodes[this->nets[netNo].outNodes[0]].inNets[index1] = this->nets.size();
	this->nodes[this->nets[netNo].outNodes[1]].inNets[index2] = this->nets.size()+1;

	this->nets[netNo].outNodes.clear();
	this->nets[netNo].outNodes.push_back(this->nodes.size());

	this->nodes.push_back(splitNode);
	this->nets.push_back(s1net);
	this->nets.push_back(s2net);

	return 1;
}

/**
 * [ForgeSFQBlif::insertSplitAuto - Inserts the correct amount of splitters]
 * @param  netNo [The net that must be split]
 * @return       [0 - All good; 1 - Error]
 */
int ForgeSFQBlif::insertSplitAuto(unsigned int netNo){
	cout << "Inserting splitter for: " << this->nodes[this->nets[netNo].inNodes[0]].name << endl;

	if(this->nets[netNo].outNodes.size() > 2){
		cout << "Creating a splitter with: " << this->nets[netNo].outNodes.size() << " outputs." << endl;
	}
	else{
		this->insertSplit(netNo);
		return 0;
	}

  // calc number of levels needed
  int noLevels = ceil(log(this->nets[netNo].outNodes.size() )/log(2)) -1;

  vector<unsigned int> newRow, oldRow, newNodes, looseNodes;

  // define the loose standing nodes
  for(auto &itNodes: this->nets[netNo].outNodes){
	  looseNodes.push_back(itNodes);
	}

	this->nets[netNo].outNodes.clear();
	this->nets[netNo].outNodes.push_back(this->nodes.size());

	BlifNode splitNode;

	splitNode.name = "SPLIT_" + to_string(this->nodes.size());
	splitNode.GateType = "SPLIT";
	splitNode.inNets.push_back(netNo);
	this->nodes.push_back(splitNode);

  
  // first node to be split
  oldRow.push_back(this->nodes.size() -1);

  for(unsigned int i = 0; i < noLevels; i++){
    newRow.clear();
    for(unsigned int j = 0; j < oldRow.size(); j++){
      newNodes = createSplitter(oldRow[j]);
      newRow.insert(newRow.end(), newNodes.begin(), newNodes.end());
    }

    oldRow = newRow;
  }

  // stitching the splitters and the loose gates together
  BlifNet fooNet;
  unsigned int rowIndex = 0;
  for(unsigned int i = 0; i < looseNodes.size(); i++){
    fooNet.name = "net_" + to_string(this->nets.size());
    
    fooNet.inNodes.clear();
    fooNet.inNodes.push_back(oldRow[rowIndex]);
    
    fooNet.outNodes.clear();
    fooNet.outNodes.push_back(looseNodes[i]);

    for(unsigned int j = 0; j < 2; j++){
    	if(netNo == this->nodes[looseNodes[i]].inNets[i]){
    		this->nodes[looseNodes[i]].inNets[j] = this->nets.size();
    	}
    }
    this->nodes[oldRow[rowIndex]].outNets.push_back(this->nets.size());
    rowIndex++;
    if(rowIndex % 2 == 0){
    	rowIndex = 0;
    }
    this->nets.push_back(fooNet);
  }

	return 1;
}

/**
 * [ForgeSFQBlif::createSplitter - creates a splitters]
 * @param  netNo [The net that must be split]
 * @return       [Index of created splitter nodes]
 */
vector<unsigned int> ForgeSFQBlif::createSplitter(unsigned int netNo){
  BlifNode fooNode;
  BlifNet fooNet;

  vector<unsigned int> resVec;
  unsigned int oldNode;

  // oldNode = this->nets[netNo].inNodes[0];
  oldNode = netNo;  // MUST RENAME netNo to nodeNo

  fooNode.GateType = "SPLIT";

  this->nodes[oldNode].outNets.clear();

  this->nodes[oldNode].outNets.push_back(this->nets.size());
  this->nodes[oldNode].outNets.push_back(this->nets.size()+1);

  fooNet.inNodes.push_back(oldNode);

  for(unsigned int i = 0; i < 2; i++){
    fooNode.name = "SPLIT_" + to_string(this->nodes.size());
    fooNode.inNets.clear();
    fooNode.inNets.push_back(this->nets.size());

    fooNet.name = "net_" + to_string(this->nets.size());
    fooNet.outNodes.clear();
    fooNet.outNodes.push_back(this->nodes.size());

    resVec.push_back(this->nodes.size());
    this->nodes.push_back(fooNode);
    this->nets.push_back(fooNet);
  }

  return resVec;
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
 * [ForgeSFQBlif::calcStats - Calculates stats from the circuit]
 * @return [1 - all good; 0 - error]
 */

int ForgeSFQBlif::calcStats(){
	this->inputCnt = 0;
	this->outputCnt = 0;
	this->gateCnt = 0;
	this->splitCnt = 0;
	this->DFFCnt = 0;

	for(unsigned int i = 0; i < this->nodes.size(); i++){
		if(!this->nodes[i].GateType.compare("input")){
			this->inputCnt++;
		}
		else if(!this->nodes[i].GateType.compare("output")){
			this->outputCnt++;
		}
		else if(!this->nodes[i].GateType.compare("DFF")){
			this->DFFCnt++;
		}
		else if(!this->nodes[i].GateType.compare("SPLIT")){
			this->splitCnt++;
		}
		else{
			this->gateCnt++;
		}
	}

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

	string gvFile = fileExtensionRenamer(fileName, ".dot");

  dotFile = fopen(gvFile.c_str(), "w");

  string lineStr;

  lineStr = "digraph " + this->blifFile.get_modelName() + " {\n";
  // lineStr = "digraph GenCir {\n";
  fputs(lineStr.c_str(), dotFile);


  // Define the different shapes and colours for inputs, outputs, DFFs and splitters
  for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("input")){
		  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=red]; \n";
		  fputs(lineStr.c_str(), dotFile);
		}
  }
  for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("outputs")){
		  lineStr = "\t" + this->nodes[i].name + " [shape=box, color=blue]; \n";
		  fputs(lineStr.c_str(), dotFile);
		}
  }
  for(unsigned int i = 0; i < this->nodes.size(); i++){
  	if(!this->nodes[i].GateType.compare("DFF")){
		  lineStr = "\t" + this->nodes[i].name + " [color=blue]; \n";
		  fputs(lineStr.c_str(), dotFile);
  	}
  }
  for(unsigned int i = 0; i < this->nodes.size(); i++){
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
  		// skipping the split clock
	  	if(!this->nodes[this->nets[i].inNodes[0]].GateType.compare("SC")){
	  		continue;
	  	}
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
 * [ForgeSFQBlif::printStats - prints the stats of the circuit]
 */

void ForgeSFQBlif::printStats(){
	this->calcStats();

	cout << "Stats:" << endl;
	cout << "\tGateCnt: " << this->gateCnt << endl;
	cout << "\tInputCnt: " << this->inputCnt << endl;
	cout << "\tOutputCnt: " << this->outputCnt << endl;
	cout << "\tSplitCnt: " << this->splitCnt << endl;
	cout << "\tDFFCnt: " << this->DFFCnt << endl;
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
 * [ForgeSFQBlif::printRoutesWS - Displays the routes with splitters]
 */

void ForgeSFQBlif::printRoutesWS(){
	cout << "All possible routes:" << endl;

	unsigned int j;

	for(unsigned int i = 0; i < this->routesWS.size(); i++){
		for(j = 0; j < this->routesWS[i].size()-1; j++){
				cout << this->nodes[this->routesWS[i][j]].name << " -> ";
		}
		cout << this->nodes[this->routesWS[i][j]].name << endl;
	}
}
/**
 * [ForgeSFQBlif::printCLKlevels - Displays the clock levels]
 */

void ForgeSFQBlif::printCLKlevels(){
	cout << "Clock levels:" << endl;

	unsigned int j;

	for(unsigned int i = 0; i < this->CLKlevel.size(); i++){
		cout << i << ": ";
		for(j = 0; j < this->CLKlevel[i].size()-1; j++){
			cout << "\"" << this->nodes[this->CLKlevel[i][j]].name << "\"; ";
		}
		cout << this->nodes[this->CLKlevel[i][j]].name << endl;
	}
}