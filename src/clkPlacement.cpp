/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-09
 * Modified:
 * license:
 * Description: Creates clock for SFQ circuit
 * File:        clkPlacement.cpp
 */


#include "viper/clkPlacement.hpp"


/**
 * [clkChip::hitIt - The script that creates the clock]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::hitIt(){
  cout << "Clocking the gates." << endl;


  // Creating the h-tree clock
  unsigned int clkLev = log(this->clkPinCnt)/log(2) + 1;
  unsigned int clkSplit = pow(2, clkLev -1);

  cout << "Clock stats:" << endl;
  cout << "\tPins required: " << this->clkPinCnt << endl;
  cout << "\tPins max: " << pow(2, clkLev) << endl;
  cout << "\tSplit levels: " << clkLev << endl;
  cout << "\tOutput splitters: " << clkSplit << endl;
  cout << "\tTotal splitters: " << clkSplit*2-1 << endl;

  unsigned int foo = this->nodes[this->clkSplitIndex].outNets[0];

  cout << "H-Tree clocking recursion starting." << endl;

  // recurCreateCLK(0, clkLev, xOffset, 0, false, foo);
  recurCreateCLK(0, 9, xOffset, 0, false, foo);

  cout << "Clocking all the gates done." << endl;


  return 1;
}


void clkChip::fetchData(vector<BlifNode> inNodes,
                        vector<BlifNet> inNets,
                        vector<cellDis> inGateList,
                        string configFile,
                        unsigned int gateCnt){
  this->nodes = inNodes;
  this->nets = inNets;
  this->gateList = inGateList;
  this->clkPinCnt = gateCnt;

  for(unsigned int i = 0; i < this->gateList.size(); i++){
    if(!this->gateList[i].name.compare("SPLIT")){
      this->splitIndex = i;
      break;
    }
  }

  this->clkSplitIndex = this->nodes.size()-1;

  // const auto ConfigFile = toml::parse(configFile);
  // const auto& r_para    = toml::find(ConfigFile, "run_parameters");

  // string workDir      = toml::find<string>(r_para, "work_dir");
  // string cellLibFName = toml::find<string>(r_para, "cell_dis_gds");
  // cellLibFName        = cellLibFName.insert(0, workDir);
  // const auto CellLibFile   = toml::parse(cellLibFName);

  // const auto& g_para = toml::find(CellLibFile, "ALL_GATES");

  // this->ptl_width = toml::find<float>(g_para, "ptl_width") * GDSunitScale;
}

/**
 * [clkChip::recurCreateCLK - creates the h-tree clock using depth first recursion]
 * @param  net      [The previous splitter's index]
 * @param  curLev   [The current level]
 * @param  maxLev   [The final level the function must stop at]
 * @return          [1 - All good; 0 - Error]
 */

int clkChip::recurCreateCLK(unsigned int curLev, unsigned int maxLev, int curX, int curY, bool orienta, unsigned int netNo){
  vector<int> cor;

  this->drawCLKnode(curX, curY, netNo);

  if(curLev == maxLev-1){
    return 1;
  }

  cor = this->HtreeCalc(curX, curY, orienta, curLev);

  recurCreateCLK(curLev +1, maxLev, cor[0], cor[1], !orienta, this->nodes[this->nets[netNo].outNodes[0]].outNets[0]);
  recurCreateCLK(curLev +1, maxLev, cor[2], cor[3], !orienta, this->nodes[this->nets[netNo].outNodes[0]].outNets[1]);

  return 0;
}

/**
 *
 */

vector<int> clkChip::HtreeCalc(int curX, int curY, bool orient, unsigned int curDepth){
  vector<int> outVec;
  outVec.resize(4);

  int D_Scale = 50000;

  double ScaleF;

  if(curDepth != 0)
    ScaleF = 1 / (curDepth*0.1) * (double)D_Scale;
  else
    ScaleF = 1 / 0.1 * (double)D_Scale;

  if(orient){         // Vertical
    outVec[0] = curX;
    outVec[1] = curY - ScaleF;
    outVec[2] = curX;
    outVec[3] = curY + ScaleF;
  }
  else if(!orient){   // Horizontal
    outVec[0] = curX - ScaleF;
    outVec[1] = curY;
    outVec[2] = curX + ScaleF;
    outVec[3] = curY;
  }
  else{
    cout << "Smoke detected, RUN for your life" << endl;
    outVec[0] = 0;
    outVec[1] = 0;
    outVec[2] = 0;
    outVec[3] = 0;
  }

  return outVec;
}


/**
 * [clkChip::drawCLKnode - creates/draws the nodes]
 * @param  corX [X coordinates]
 * @param  corY [Y coordinate]
 * @return      [1 - All good; 0 - Error]
 */

int clkChip::drawCLKnode(int corX, int corY, unsigned int netNo){
  BlifNode fooNode;
  BlifNet fooNet;

  fooNode.name = "SC_" + to_string(this->nodes.size());
  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;
  fooNode.corX = corX;
  fooNode.corY = corY;

  fooNode.inNets.push_back(netNo);
  fooNode.outNets.push_back(this->nets.size());
  fooNode.outNets.push_back(this->nets.size()+1);

  for(unsigned int i = 0; i < 2; i++){
    fooNet.name = "net_" + to_string(this->nets.size());
    fooNet.inNodes.clear();
    fooNet.inNodes.push_back(this->nodes.size());
    this->nets.push_back(fooNet);
  }

  this->nets[netNo].outNodes.push_back(this->nodes.size());
  this->nodes.push_back(fooNode);

  return 1;
}

/**
 * [clkChip::to_gds - Gives the GDS structure that references all the clock nodes]
 * @return [The GDS structure]
 */

gdsSTR clkChip::to_gds(){
  gdsSTR foo;
  foo.name = "clk";

  for(unsigned int i = this->clkSplitIndex; i < this->nodes.size(); i++){
    foo.SREF.push_back(drawSREF(this->gateList[this->splitIndex].name, this->nodes[i].corX, this->nodes[i].corY));
  }

  return foo;
}
