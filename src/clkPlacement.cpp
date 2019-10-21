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
 * [clkChip::execute - The script that creates the clock]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::execute(){
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


  // --------------------------- Method 1 ---------------------------
  // cout << "H-Tree clocking recursion starting." << endl;
  // unsigned int foo = this->nodes[this->clkSplitIndex].outNets[0];
  // recurCreateCLK(0, clkLev, xOffset, 0, false, foo);


  // --------------------------- Method 2 ---------------------------
  this->bottomUp();
  this->mapNets();
  this->mergeLayouts();

  cout << "Clocking all the gates done." << endl;

  return 1;
}


void clkChip::fetchData(vector<BlifNode> inNodes,
                        vector<BlifNet> inNets,
                        vector<cellDis> inGateList,
                        string configFile,
                        unsigned int gateCnt,
                        vector<vector<unsigned int>> layout){
  this->nodes = inNodes;
  this->nets = inNets;
  this->gateList = inGateList;
  this->clkPinCnt = gateCnt;
  this->cellLayout = layout;

  for(unsigned int i = 0; i < this->gateList.size(); i++){
    if(!this->gateList[i].name.compare("SPLIT")){
      this->splitIndex = i;
      break;
    }
  }

  this->clkSplitIndex = this->nodes.size()-1;

  const auto ConfigFile = toml::parse(configFile);
  const auto& w_para    = toml::find(ConfigFile, "wafer_parameters");

  this->veriticalHeightGap = toml::find<int>(w_para, "vertical_gap") * 1000;
  this->cellHeight         = toml::find<int>(w_para, "cell_height") * 1000;
  this->padHeightGap       = toml::find<int>(w_para, "pad_ver_gap") * 1000;

}

/* ----------------------------------------------------------------------------------
   ------------------------------------ Method 1 ------------------------------------
   ---------------------------------------------------------------------------------- */
/**
 * Recursive H-Tree
 */


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

/* ----------------------------------------------------------------------------------
   ------------------------------------ Method 2 ------------------------------------
   ---------------------------------------------------------------------------------- */
/**
 * Building bottom up
 */

/**
 * [clkChip::bottomUp description]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::bottomUp(){
  cout << "Building clock from bottom up" << endl;

  uint32_t cellCnt;
  vector<unsigned int> clkRow;

  for(unsigned int i = 0; i < this->cellLayout.size(); i++){
    cellCnt = 0;
    for(unsigned int j = 0; j < this->cellLayout[i].size(); j++){
      if(this->nodes[cellLayout[i][j]].GateType.compare("SPLIT")){
        cellCnt++;
      }
    }

    if(++i < this->cellLayout.size()){
      for(unsigned int j = 0; j < this->cellLayout[i].size(); j++){
        if(this->nodes[cellLayout[i][j]].GateType.compare("SPLIT")){
          cellCnt++;
        }
      }
    }

    if((cellCnt % 2) > 0){
      cellCnt++;
    }
    cellCnt /= 2;

    for(int i = 0; i < cellCnt; i++){
      clkRow.push_back(this->nodes.size());
      this->drawCLKnodeBasic();
    }

    this->clkLayout.push_back(clkRow);

    this->rowSplitInsert.push_back(i-1);

    clkRow.clear();
  }

  return 1;
}

/**
 * [clkChip::mergeLayouts description]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::mergeLayouts(){
  cout << "Merging logic circuit with clock." << endl;
  vector<vector<unsigned int>> newCellLayout;
  for(unsigned int i = 0; i < this->cellLayout.size(); i++){
    newCellLayout.push_back(this->cellLayout[i]);
    for(unsigned int j = 0; j < this->rowSplitInsert.size(); j++){
      if(i == this->rowSplitInsert[j]){
        newCellLayout.push_back(this->clkLayout[j]);
        break;
      }
    }

  }
  this->cellLayout = newCellLayout;
  return 1;
}

/**
 * [clkChip::mapNets - Maps/joins the splitter nodes]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::mapNets(){
  cout << "Mapping clock nets." << endl;
  vector<unsigned int> rowJoin;
  unsigned int row0, row1, col0, col1, clkRow;
  bool row0end, row1end;

  unsigned int clkSplitIndex, outNetIndex;

  clkRow = 0;

  for(unsigned int i = 0; i < this->cellLayout.size(); i += 2){
    row0 = i;
    row1 = i+1;
    col0 = 0;
    col1 = 0;
    row0end = false;
    row1end = false;

    if(row1 >= this->cellLayout.size()){
      // then last row does not exist
      row1end = true;
    }

    while(row0end == false || row1end == false){
      if(row0end == false){
        if(this->nodes[this->cellLayout[row0][col0]].strRef != this->splitIndex){
          rowJoin.push_back(this->cellLayout[row0][col0]);
        }
      }
      if(row1end == false){
        if(this->nodes[this->cellLayout[row1][col1]].strRef != this->splitIndex){
          rowJoin.push_back(this->cellLayout[row1][col1]);
        }
      }

      col0++;
      col1++;

      if(col0 >= this->cellLayout[row0].size()){
        row0end = true;
      }
      if(col1 >= this->cellLayout[row1].size()){
        row1end = true;
      }
    }

    // cout << "Join Row:  ";
    // for(const auto &nodeIndex: rowJoin){
    //   cout << this->nodes[nodeIndex].name << ";  ";
    // }
    // cout << endl;

  // for(unsigned int j = 0; j < rowJoin.size(); j++){
  //   this->nodes[rowJoin[i]].clkNet =
  // }

  // unsigned int clkSplitIndex;

  clkSplitIndex = 0;
  outNetIndex = 0;

  for(const auto &nodeIndex: rowJoin){
    this->nodes[nodeIndex].clkNet = this->nodes[clkLayout[clkRow][clkSplitIndex]].outNets[outNetIndex];
    this->nets[this->nodes[clkLayout[clkRow][clkSplitIndex]].outNets[outNetIndex]].outNodes.push_back(nodeIndex);

    if(outNetIndex == 1){
      clkSplitIndex++;
    }

    outNetIndex++;

    if(outNetIndex == 2){
      outNetIndex = 0;
    }
  }
  clkRow++;
  rowJoin.clear();
  }



  return 1;
}

/**
 * [clkChip::drawCLKnode - creates/draws the nodes]
 * @return      [1 - All good; 0 - Error]
 */

int clkChip::drawCLKnodeBasic(){
  BlifNode fooNode;
  BlifNet fooNet;

  fooNode.name = "SC_" + to_string(this->nodes.size());
  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;
  // fooNode.corX = corX;
  // fooNode.corY = corY;

  // fooNode.inNets.push_back(this->nets.size());
  fooNode.outNets.push_back(this->nets.size());
  fooNode.outNets.push_back(this->nets.size()+1);

  for(unsigned int i = 0; i < 2; i++){
    fooNet.name = "net_" + to_string(this->nets.size());
    fooNet.inNodes.clear();
    fooNet.inNodes.push_back(this->nodes.size());
    this->nets.push_back(fooNet);
  }

  // this->nets[netNo].outNodes.push_back(this->nodes.size());
  this->nodes.push_back(fooNode);

  return 1;
}



/* ----------------------------------------------------------------------------------
   --------------------------------- Miscellaneous ----------------------------------
   ---------------------------------------------------------------------------------- */

/**
 * [clkChip::to_gds - Gives the GDS structure that references all the clock nodes]
 * @return [The GDS structure]
 */

gdsSTR clkChip::to_gds(){
  gdsSTR foo;
  foo.name = "clk";

  for(unsigned int i = this->clkSplitIndex+1; i < this->nodes.size(); i++){
    foo.SREF.push_back(drawSREF(this->gateList[this->splitIndex].name, this->nodes[i].corX, this->nodes[i].corY));
  }

  return foo;
}

// void plek::printLayout(){
//   cout << "Circuit layout:" << endl;

//   for(unsigned int i = 0; i < this->layout.size(); i++){
//     cout << i << ": ";
//     for(unsigned j = 0; j < this->layout[i].size(); j++){
//       cout << "\"" << this->nodes[this->layout[i][j]].name << "\"; ";
//     }
//     cout << endl;
//   }
// }