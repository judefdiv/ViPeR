/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-3
 * Modified:
 * license:
 * Description: routing of tracks between pins
 * File:        routing.cpp
 */

#include "viper/routing.hpp"


/**
 * [roete::fetchData - Retrieves all the necessary data that the class needs]
 * @return            [1 - Good; 0 - Error]
 */

int roete::fetchData(vector<BlifNode> inNodes, vector<BlifNet> inNets, vector<cellDis> inGateList, string configFile){

  this->nodes = inNodes;
  this->nets = inNets;
  this->gateList = inGateList;


  const auto ConfigFile = toml::parse(configFile);
  const auto& r_para    = toml::find(ConfigFile, "run_parameters");

  string workDir      = toml::find<string>(r_para, "work_dir");
  string cellLibFName = toml::find<string>(r_para, "cell_dis_gds");
  cellLibFName        = cellLibFName.insert(0, workDir);
  const auto CellLibFile   = toml::parse(cellLibFName);

  const auto& g_para = toml::find(CellLibFile, "ALL_GATES");

  this->ptl_width = toml::find<float>(g_para, "ptl_width") * GDSunitScale;

  return 1;
}

/**
 * [roete::straightRoute - creates a straight line between all the pins that are connected]
 * @return [1 - Good; 0 - Error]
 */

int roete::straightRoute(){
  cout << "Routing nets, straight." << endl;
  unsigned int toNode, fromNode;
  vector<int> corX;
  vector<int> corY;

  vector<int> nodesInputCnt;
  vector<int> nodesOutputCnt;

  corX.resize(2);
  corY.resize(2);

  nodesInputCnt.resize(this->nodes.size());
  nodesOutputCnt.resize(this->nodes.size());

  //finding the PAD in the gateList
  for(unsigned int i = 0; i < this->gateList.size(); i++){
    if(!this->gateList[i].name.compare("PAD")){
      this->pad_index = i;
      break;
    }
  }

  for(unsigned int i = 0; i < nodesInputCnt.size(); i++){
    nodesInputCnt[i] = 0;
    nodesOutputCnt[i] = 0;
  }

  for(unsigned int i = 0; i < this->nets.size(); i++){
    for(unsigned int j = 0; j < this->nets[i].outNodes.size(); j++){
      // if(this->nets[i].outNodes.size() > 0){
      //   if(!this->nodes[this->nets[i].outNodes[j]].GateType.compare("SC")){
      //     continue;
      //   }
      // }
      // if(this->nets[i].inNodes.size() > 0){
      //   if(!this->nodes[this->nets[i].inNodes[0]].GateType.compare("SC")){
      //     continue;
      //   }
      // }


      fromNode = this->nets[i].inNodes[0];
      toNode = this->nets[i].outNodes[j];

      if(this->nodes[fromNode].strRef == this->pad_index){
        corX[0] = this->nodes[fromNode].corX + this->gateList[this->nodes[fromNode].strRef].pins_in_out_x[0];
        corY[0] = this->nodes[fromNode].corY + this->gateList[this->nodes[fromNode].strRef].pins_in_out_y[0];
      }
      else{
        corX[0] = this->nodes[fromNode].corX + this->gateList[this->nodes[fromNode].strRef].pins_out_x[nodesOutputCnt[fromNode]];
        corY[0] = this->nodes[fromNode].corY + this->gateList[this->nodes[fromNode].strRef].pins_out_y[nodesOutputCnt[fromNode]];
        nodesOutputCnt[fromNode]++;
      }

      // check if toNode is connecting with CLK
      if(this->nodes[toNode].clkNet == i){        // check if toNode is connecting with CLK
        corX[1] = this->nodes[toNode].corX + this->gateList[this->nodes[toNode].strRef].clk_x[0];
        corY[1] = this->nodes[toNode].corY + this->gateList[this->nodes[toNode].strRef].clk_y[0];
      }
      else if(this->nodes[toNode].strRef == this->pad_index){
        corX[1] = this->nodes[toNode].corX + this->gateList[this->nodes[toNode].strRef].pins_in_out_x[0];
        corY[1] = this->nodes[toNode].corY + this->gateList[this->nodes[toNode].strRef].pins_in_out_y[0];
      }
      else{
        corX[1] = this->nodes[toNode].corX + this->gateList[this->nodes[toNode].strRef].pins_in_x[nodesInputCnt[toNode]];
        corY[1] = this->nodes[toNode].corY + this->gateList[this->nodes[toNode].strRef].pins_in_y[nodesInputCnt[toNode]];
        nodesInputCnt[toNode]++;
      }

      this->nets[i].route.resize(this->nets[i].route.size()+1);
      this->nets[i].route.back().layerNo = 4;
      this->nets[i].route.back().corX = corX;
      this->nets[i].route.back().corY = corY;
    }
  }
  cout << "Routing nets, straight. Done." << endl;
  return 1;
}

/**
 * [roete::route2gds - Converts the nets' route to GDS format]
 * @return [The GDS PATH record of the tracks]
 */

gdsSTR roete::route2gds(){
  gdsSTR resultSTR;

  resultSTR.name = "NETs";

  for(unsigned int i = 0; i < this->nets.size(); i++){
    for(unsigned int j = 0; j < this->nets[i].outNodes.size(); j++){
      // if(this->nets[i].outNodes.size() > 0){
      //   if(!this->nodes[this->nets[i].outNodes[j]].GateType.compare("SC")){
      //     continue;
      //   }
      // }
      // if(this->nets[i].inNodes.size() > 0){
      //   if(!this->nodes[this->nets[i].inNodes[0]].GateType.compare("SC")){
      //     continue;
      //   }
      // }

      for(unsigned int k = 0; k < this->nets[i].route.size(); k++){
        if(this->nodes[this->nets[i].inNodes[0]].name.find("SC") == string::npos){
          resultSTR.PATH.push_back(drawPath(4, this->ptl_width, this->nets[i].route[k].corX, this->nets[i].route[k].corY));
        }
        else{
          resultSTR.PATH.push_back(drawPath(5, this->ptl_width, this->nets[i].route[k].corX, this->nets[i].route[k].corY));
        }
      }
    }
  }

  return resultSTR;
}