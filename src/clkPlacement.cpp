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
  this->initialLogic2CLK();
  this->mapInitialLogic2CLKNets();
  this->stitchCLKrows();
  this->mainCLKdistri();
  this->mergeLayouts();

  // cout << "----------- TESTING VECTOR FUNCTIONS -----------" << endl;

  // vector<unsigned int> fooVec = {0, 1, 2, 3, 4, 5, 6, 7, 8};
  // vector<unsigned int> booVec;

  // booVec =  vecInsert(fooVec, 9, 1);
  // for(unsigned int i = 0; i < booVec.size(); i++){
  //   cout << booVec[i] << ", ";
  // }
  // cout << endl;

  // booVec =  vecInsert(fooVec, 9, 0);
  // for(unsigned int i = 0; i < booVec.size(); i++){
  //   cout << booVec[i] << ", ";
  // }
  // cout << endl;

  // booVec =  vecInsert(fooVec, 9, 8);
  // for(unsigned int i = 0; i < booVec.size(); i++){
  //   cout << booVec[i] << ", ";
  // }
  // cout << endl;

  // booVec =  vecInsert(fooVec, 9, 9);
  // for(unsigned int i = 0; i < booVec.size(); i++){
  //   cout << booVec[i] << ", ";
  // }
  // cout << endl;

  // booVec =  vecInsert(fooVec, 9, 10);
  // for(unsigned int i = 0; i < booVec.size(); i++){
  //   cout << booVec[i] << ", ";
  // }
  // cout << endl;

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
  const auto& w_para    = toml::find(ConfigFile, "Wafer_Parameters");

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
 * [clkChip::initialLogic2CLK description]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::initialLogic2CLK(){
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
        // break;
      }
    }

  }
  this->cellLayout = newCellLayout;
  return 1;
}

/**
 * [clkChip::mapInitialLogic2CLKNets - Maps/joins the splitter nodes]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::mapInitialLogic2CLKNets(){
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
 * [clkChip::stitchCLKrows - stitches and adds splitter until each row only requires 1 input clk signal. Bottom up approach]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::stitchCLKrows(){
  cout << "Stitching clk row splitters." << endl;
  // find the widest row
  unsigned int widestRow;
  unsigned int widestRowCnt = 0;
  for(unsigned int i = 0; i < this->clkLayout.size(); i++){
    if(this->clkLayout[i].size() > widestRowCnt){
      widestRowCnt = this->clkLayout[i].size();
      widestRow = i;
    }
  }

  // calc number of levels needed
  int noLevels = ceil(log(widestRowCnt)/log(2));

  // loop through levels
  vector<unsigned int> splitJoin0, splitJoin1, splitPos, insertedNodes;
  int sIndex0, sIndex1;    // split index
  for(unsigned int i = 0; i < this->clkLayout.size(); i++){
    for(unsigned int j = 0; j < noLevels; j++){
        sIndex0 = -1;
        sIndex1 = -1;
        splitJoin0.clear();
        splitJoin1.clear();
        splitPos.clear();
        insertedNodes.clear();
      for(unsigned int k = 0; k < this->clkLayout[i].size(); k++){
        // find the next 2 splitters
        if(sIndex0 == -1 && this->nodes[this->clkLayout[i][k]].inNets.size() == 0){
          sIndex0 = k;
          splitJoin0.push_back(sIndex0);
        }
        else if(sIndex1 == -1 && this->nodes[this->clkLayout[i][k]].inNets.size() == 0){
          sIndex1 = k;
          splitJoin1.push_back(sIndex1);
        }

        if(sIndex0 != -1 && sIndex1 != -1){
          sIndex0 = -1;
          sIndex1 = -1;
        }
        else if(sIndex0 != -1 && k == this->clkLayout[i].size() -1){
          // the odd splitter
          // cout << "Odd splitter, thats a bit unfortunate" << endl;
        }
      }

      for(unsigned int k = 0; k < splitJoin0.size(); k++){
        if(splitJoin1.size() > k){
          insertedNodes.push_back(createCLKnode(this->clkLayout[i][splitJoin0[k]], this->clkLayout[i][splitJoin1[k]]));
          splitPos.push_back((splitJoin0[k] + splitJoin1[k]) / 2 +1);
        }
        else{
          insertedNodes.push_back(createCLKnodeAlone(this->clkLayout[i][splitJoin0[k]]));
          splitPos.push_back((splitJoin0[k] + 0) / 2 +1);
        }
      }

      for(int l = splitPos.size()-1; l >= 0; l--){
        // vector<unsigned int>::iterator itSplitPos;
        // itSplitPos = clkLayout[i].begin() + splitPos[l];
        // clkLayout[i].insert(itSplitPos, insertedNodes[l]);
        vecInsert(clkLayout, i, insertedNodes[l], splitPos[l]);
      }
    }

    for(unsigned int j = 0; j < this->clkLayout[i].size(); j++){
      if(this->nodes[this->clkLayout[i][j]].inNets.size() == 0){
        this->rowCLKin.push_back(this->clkLayout[i][j]);
        break;
      }
    }
  }

  cout << "Stitching clk row splitters done." << endl;
  return 1;
}

// int clkChip::stitchCLKrows(){
//   cout << "Stitching clk row splitters." << endl;
//   // find the widest row
//   unsigned int widestRow;
//   unsigned int widestRowCnt = 0;
//   for(unsigned int i = 0; i < this->clkLayout.size(); i++){
//     if(this->clkLayout[i].size() > widestRowCnt){
//       widestRowCnt = this->clkLayout[i].size();
//       widestRow = i;
//     }
//   }

//   // calc number of levels needed
//   int noLevels = ceil(log(widestRowCnt)/log(2));

//   // loop through levels
//   vector<unsigned int> splitJoin0, splitJoin1, splitPos, insertedNodes;
//   int sIndex0, sIndex1;    // split index
//   for(unsigned int i = 0; i < this->clkLayout.size(); i++){
//     for(unsigned int j = 0; j < noLevels; j++){
//         sIndex0 = -1;
//         sIndex1 = -1;
//         splitJoin0.clear();
//         splitJoin1.clear();
//         splitPos.clear();
//         insertedNodes.clear();
//       for(unsigned int k = 0; k < this->clkLayout[i].size(); k++){
//         // find the next 2 splitters
//         if(sIndex0 == -1 && this->nodes[this->clkLayout[i][k]].inNets.size() == 0){
//           sIndex0 = k;
//           splitJoin0.push_back(sIndex0);
//         }
//         else if(sIndex1 == -1 && this->nodes[this->clkLayout[i][k]].inNets.size() == 0){
//           sIndex1 = k;
//           splitJoin1.push_back(sIndex1);
//         }

//         if(sIndex0 != -1 && sIndex1 != -1){
//           sIndex0 = -1;
//           sIndex1 = -1;
//         }
//         else if(sIndex0 != -1 && k == this->clkLayout[i].size() -1){
//           // the odd splitter
//           // cout << "Odd splitter, thats a bit unfortunate" << endl;
//         }
//       }

//       for(unsigned int k = 0; k < splitJoin0.size(); k++){

//         if(splitJoin1.size() > k){
//           insertedNodes.push_back(createCLKnode(this->clkLayout[i][splitJoin0[k]], this->clkLayout[i][splitJoin1[k]]));
//         }
//         else{
//           insertedNodes.push_back(createCLKnodeAlone(this->clkLayout[i][splitJoin0[k]]));
//         }

//         if(splitJoin1.size() > 0){
//           splitPos.push_back((splitJoin0[k] + splitJoin1[k]) / 2 +1);
//         }
//         else{
//           splitPos.push_back((splitJoin0[k] + 0) / 2 +1);
//         }
//       }

//       // for(unsigned int k = 0; k < splitPos.size(); k++){
//       //   cout << "[" << k << "] = " << splitPos[k] << endl;
//       // }

//       // inserting splitters.... must improve
//       // vector<unsigned int>::iterator itSplitPos;
//       for(int l = splitPos.size()-1; l >= 0; l--){
//         // itSplitPos = clkLayout[i].begin() + splitPos[l];
//         // clkLayout[i].insert(itSplitPos, insertedNodes[l]);
//         vecInsert(clkLayout, i, insertedNodes[l], splitPos[l]);
//       }
//     }

//     for(unsigned int j = 0; j < this->clkLayout[i].size(); j++){
//       if(this->nodes[this->clkLayout[i][j]].inNets.size() == 0){
//         this->rowCLKin.push_back(this->clkLayout[i][j]);
//         break;
//       }
//     }
//   }

//   cout << "Stitching clk row splitters done." << endl;
//   return 1;
// }

/**
 * [clkChip::mainCLKdistro - Creates the main clock distribution. top down approach]
 * @return [1 - All good; 0 - Error]
 */

int clkChip::mainCLKdistri(){
  cout << "Stitching main clk row splitters." << endl;

  // Create/connect initial CLK
  BlifNode fooNode;

  fooNode.name = "SC_" + to_string(this->nodes.size());
  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;
  fooNode.inNets.push_back(this->nodes[this->clkSplitIndex].outNets[0]); // clk pad

  this->nets[this->nodes[this->clkSplitIndex].outNets[0]].outNodes.push_back(this->nodes.size());

  this->nodes.push_back(fooNode);

  // // Create/connect initial CLK
  // BlifNode fooNode;
  // BlifNet fooNet;

  // fooNode.name = "SC_" + to_string(this->nodes.size());
  // fooNode.GateType = "SPLIT";
  // fooNode.strRef = this->splitIndex;
  // fooNode.inNets.push_back(this->nets.size()); // clk pad

  // this->nodes[this->clkSplitIndex].outNets[0]

  // fooNet.name = "net_" + to_string(this->nets.size());
  // fooNet.inNodes.push_back(this->clkSplitIndex);
  // fooNet.outNodes.push_back(this->nodes.size());

  // this->nets.push_back(fooNet);
  // this->nodes.push_back(fooNode);

  // calc number of levels needed
  int noLevels = ceil(log(this->rowCLKin.size())/log(2)) -1;

  vector<unsigned int> insertedNodes, newClkNodes, fooVec;
  insertedNodes.push_back(this->nodes.size()-1);    // adding in the first CLK splitter

  for(unsigned int i = 0; i < noLevels; i++){
    newClkNodes.clear();
    // for(unsigned int j = 0; j < insertedNodes.size(); j++){
    for(unsigned int j = 0; j < insertedNodes.size(); j += 2){
      fooVec = create2CLKnode(insertedNodes[j]);
      newClkNodes.insert(newClkNodes.end(), fooVec.begin(), fooVec.end());
    }

    // vector<unsigned int>::iterator itSplitPos;
    // for(unsigned int j = newClkNodes.size()-1; j >= 0; j--){
    for(unsigned int j = 0; j < newClkNodes.size(); j++){
      // itSplitPos = insertedNodes.begin() + j*2;
      // insertedNodes.insert(itSplitPos, newClkNodes[j]);
      insertedNodes = vecInsert(insertedNodes, newClkNodes[j], j*2);
    }
  }

  unsigned int rowIndex = 0;
  unsigned int mainClkindex = 0;
  while(this->rowCLKin.size() > rowIndex){
    this->stitch2Clk(newClkNodes[mainClkindex], this->rowCLKin[rowIndex++]);
    if(this->rowCLKin.size() > rowIndex){
      this->stitch2Clk(newClkNodes[mainClkindex++], this->rowCLKin[rowIndex++]);
    }
  }



  // inserting into clk layout
  unsigned int midPos = this->clkLayout.size() / 2;

  // vector<vector<unsigned int>>::iterator itClkLayout;
  // itClkLayout = this->clkLayout.begin() + midPos;
  // this->clkLayout.insert(itClkLayout, insertedNodes);
  this->clkLayout = vecInsertRow(this->clkLayout, insertedNodes, midPos);

  // vector<unsigned int>::iterator itSplitPos;
  // itSplitPos = rowSplitInsert.begin() + midPos;
  // rowSplitInsert.insert(itSplitPos, (this->cellLayout.size() /2)-1);
  rowSplitInsert =  vecInsert(rowSplitInsert, (this->cellLayout.size() /2)-1, midPos);

  cout << "Stitching main clk row splitters done." << endl;
  return 1;
}

// int clkChip::mainCLKdistri(){
//   cout << "Stitching main clk row splitters." << endl;

//   // Create/connect initial CLK
//   BlifNode fooNode;
//   BlifNet fooNet;

//   fooNode.name = "SC_" + to_string(this->nodes.size());
//   fooNode.GateType = "SPLIT";
//   fooNode.strRef = this->splitIndex;
//   fooNode.inNets.push_back(this->nets.size()); // clk pad

//   fooNet.name = "net_" + to_string(this->nets.size());
//   fooNet.inNodes.push_back(this->clkSplitIndex);
//   fooNet.outNodes.push_back(this->nodes.size());

//   this->nets.push_back(fooNet);
//   this->nodes.push_back(fooNode);


//   // calc number of levels needed
//   int noLevels = ceil(log(this->rowCLKin.size())/log(2));

//   vector<unsigned int> insertedNodes, newClkNodes, fooVec;
//   insertedNodes.push_back(this->nodes.size()-1);    // adding in the first CLK splitter

//   for(unsigned int i = 0; i < noLevels; i++){
//     newClkNodes.clear();
//     for(unsigned int j = 0; j < insertedNodes.size(); j++){
//       fooVec = create2CLKnode(insertedNodes[j]);
//       newClkNodes.insert(newClkNodes.end(), fooVec.begin(), fooVec.end());
//     }

//     vector<unsigned int>::iterator itSplitPos;
//     // for(unsigned int j = newClkNodes.size()-1; j >= 0; j--){
//     for(unsigned int j = 0; j < newClkNodes.size(); j++){
//       // itSplitPos = insertedNodes.begin() + j*2;
//       // insertedNodes.insert(itSplitPos, newClkNodes[j]);
//       insertedNodes = vecInsert(insertedNodes, newClkNodes[j], j*2);
//     }
//   }

//   // connecting main clock to the rest of the clocks
//   unsigned int clkCnt = 0;
//   for(unsigned int i = 0; i < newClkNodes.size(); i++){
//     this->stitch2Clk(newClkNodes[i], this->rowCLKin[clkCnt++]);
//     if(clkCnt < this->rowCLKin.size()){
//       this->stitch2Clk(newClkNodes[i], this->rowCLKin[clkCnt++]);
//     }
//   }

//   // inserting into clk layout
//   unsigned int midPos = this->clkLayout.size() / 2;

//   // vector<vector<unsigned int>>::iterator itClkLayout;
//   // itClkLayout = this->clkLayout.begin() + midPos;
//   // this->clkLayout.insert(itClkLayout, insertedNodes);
//   this->clkLayout = vecInsertRow(this->clkLayout, insertedNodes, midPos);


//   // vector<unsigned int>::iterator itSplitPos;
//   // itSplitPos = rowSplitInsert.begin() + midPos;
//   // rowSplitInsert.insert(itSplitPos, (this->cellLayout.size() /2)-1);
//   rowSplitInsert =  vecInsert(rowSplitInsert, (this->cellLayout.size() /2)-1, midPos);

//   cout << "Stitching main clk row splitters done." << endl;
//   return 1;
// }

/**
 * [clkChip::stitch2Clk description]
 * @param  parentCLK [description]
 * @param  childCLK  [description]
 * @return           [1 - All good; 0 - Error]
 */

int clkChip::stitch2Clk(unsigned int parentCLK, unsigned int childCLK){
  BlifNet fooNet;

  if(this->nodes[parentCLK].GateType.compare("SPLIT")){
    cout << "Clock stitching error, not Splitter" << endl;
    return 0;
  }
  if(this->nodes[parentCLK].outNets.size() == 0){
    this->nodes[parentCLK].outNets.push_back(this->nets.size());
    this->nodes[childCLK].outNets.push_back(this->nets.size());
  }
  else if(this->nodes[parentCLK].outNets.size() == 1){
    this->nodes[parentCLK].outNets.push_back(this->nets.size());
    this->nodes[childCLK].outNets.push_back(this->nets.size());
  }
  else{
    cout << "Clock stitching error, Splitter can only handle 2 outputs" << endl;
    return 0;
  }

  fooNet.name = "net_" + to_string(this->nets.size());
  // fooNet.inNodes.clear();
  fooNet.inNodes.push_back(parentCLK);
  // fooNet.outNodes.clear();
  fooNet.outNodes.push_back(childCLK);

  this->nets.push_back(fooNet);

  return 1;
}

/**
 * [clkChip::createCLKnode description]
 * @param  CLKnode0 [description]
 * @param  CLKnode1 [description]
 * @return          [index of new node]
 */

unsigned int clkChip::createCLKnode(unsigned int CLKnode0, unsigned int CLKnode1){
  BlifNode fooNode;
  BlifNet fooNet;

  fooNode.name = "SC_" + to_string(this->nodes.size());
  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;

  fooNode.outNets.push_back(this->nets.size());
  fooNode.outNets.push_back(this->nets.size()+1);

  this->nodes[CLKnode0].inNets.push_back(this->nets.size());
  this->nodes[CLKnode1].inNets.push_back(this->nets.size()+1);

  for(unsigned int i = 0; i < 2; i++){
    fooNet.name = "net_" + to_string(this->nets.size());
    fooNet.inNodes.clear();
    fooNet.inNodes.push_back(this->nodes.size());
    fooNet.outNodes.clear();
    if(i == 0){
      fooNet.outNodes.push_back(CLKnode0);
    }
    else if(i == 1){
      fooNet.outNodes.push_back(CLKnode1);
    }
    else{
      cout << "Creating CLK error" << endl;
      return 0;
    }

    this->nets.push_back(fooNet);
  }

  this->nodes.push_back(fooNode);

  return this->nodes.size() -1;
}

/**
 * [clkChip::createCLKnodeAlone description]
 * @param  CLKnode [description]
 * @return         [index of new node]
 */

unsigned int clkChip::createCLKnodeAlone(unsigned int CLKnode){
  BlifNode fooNode;
  BlifNet fooNet;

  fooNode.name = "SC_" + to_string(this->nodes.size());
  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;

  fooNode.outNets.push_back(this->nets.size());
  // fooNode.outNets.push_back(this->nets.size()+1);

  this->nodes[CLKnode].inNets.push_back(this->nets.size());

  for(unsigned int i = 0; i < 1; i++){
    fooNet.name = "net_" + to_string(this->nets.size());
    fooNet.inNodes.clear();
    fooNet.inNodes.push_back(this->nodes.size());

    fooNet.outNodes.push_back(CLKnode);

    this->nets.push_back(fooNet);
  }

  this->nodes.push_back(fooNode);

  return this->nodes.size() -1;
}

/**
 *
 */

vector<unsigned int> clkChip::create2CLKnode(unsigned int CLKnode){
  BlifNode fooNode;
  BlifNet fooNet;

  vector<unsigned int> resVec;

  fooNode.GateType = "SPLIT";
  fooNode.strRef = this->splitIndex;

  this->nodes[CLKnode].outNets.push_back(this->nets.size());
  this->nodes[CLKnode].outNets.push_back(this->nets.size()+1);

  fooNet.inNodes.push_back(CLKnode);

  for(unsigned int i = 0; i < 2; i++){
    fooNode.name = "SC_" + to_string(this->nodes.size());
    fooNode.inNets.clear();
    fooNode.inNets.push_back(this->nets.size());

    fooNet.name = "net_" + to_string(this->nets.size());
    // fooNet.inNodes.clear();
    // fooNet.inNodes.push_back(CLKnode);
    fooNet.outNodes.clear();
    fooNet.outNodes.push_back(this->nodes.size());

    resVec.push_back(this->nodes.size());
    this->nodes.push_back(fooNode);
    this->nets.push_back(fooNet);
  }

  // vector<unsigned int> resVec;

  // resVec.push_back(this->nodes.size() -1);
  // resVec.push_back(this->nodes.size() -2);

  return resVec;
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

void vecInsert(vector<vector<unsigned int>> &vec, unsigned int whichRow,
                unsigned int toBeInserted, unsigned int pos){

  vector<unsigned int> fooVec;
  fooVec = vecInsert(vec[whichRow], toBeInserted, pos);
  vec[whichRow] = fooVec;

}

vector<unsigned int> vecInsert(vector<unsigned int> &vec, unsigned int toBeInserted,
                                unsigned int pos){

  vector<unsigned int> outVec;

  if(pos > vec.size()){
    cout << "Position is out of bounds!!!" << endl;
    cout << "Pos: " << pos << " value: " << toBeInserted << " Size: " << vec.size() << endl;
    outVec.push_back(0);
    return outVec;
  }
  unsigned int index = 0;;
  // outVec.reserve(vec.size()+1);

  while(index < vec.size()){
    if(index == pos){
      outVec.push_back(toBeInserted);
    }
    outVec.push_back(vec[index++]);
  }
  if(index == pos){
    outVec.push_back(toBeInserted);
  }

  return outVec;
}

vector<vector<unsigned int>> vecInsertRow(vector<vector<unsigned int>> &vec, vector<unsigned int> &vecRow,
                                          unsigned int posRow){

  vector<vector<unsigned int>> outVec;

  if(posRow > vec.size()){
    cout << "Position is out of bounds!!!" << endl;
    cout << "Pos: " << posRow << " Size: " << vec.size() << endl;

    outVec.resize(1);
    outVec[0].push_back(0);
    return outVec;
  }

  unsigned int index = 0;;
  // outVec.reserve(vec.size()+1);

  while(index < vec.size()){
    if(index == posRow){
      outVec.push_back(vecRow);
    }
    outVec.push_back(vec[index++]);
  }
  if(index == posRow){
    outVec.push_back(vecRow);
  }

  return outVec;
}
