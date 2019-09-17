/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-09-4
 * Modified:
 * license:
 * Description: Places the gates
 * File:        placement.cpp
 */

#include "viper/placement.hpp"



plek::plek(){

}

/**
 * [plek::fetchSFQblif - sets up the blif file]
 * @param inNodes     [The nodes]
 * @param inNets      [The nets]
 * @param inRoutes    [The routes]
 * @param inCLKlevels [The levels of the clocks]
 */

void plek::fetchSFQblif(vector<BlifNode> inNodes, vector<BlifNet> inNets, vector<vector<unsigned int>> inRoutes, vector<vector<unsigned int>> inRoutesWS, vector<vector<unsigned int>> inCLKlevels){
  this->nodes = inNodes;
  this->nets = inNets;
  this->routes = inRoutes;
  this->routesWS = inRoutesWS;
  this->CLKlevel = inCLKlevels;
}

/**
 * [plek::populate - Places the cells]
 * @param  configFName [The config .toml file]
 * @return             [1 - Good; 0 - Error]
 */

int plek::populate(string configFName){
  this->optiLayout();
  this->importCellDef(configFName);
  // this->to_str();

  // for(unsigned int i = 0; i < this->nodes.size(); i++){
  //   cout << "\t\tGate No: " << i << endl;
  //   cout << "\t\tType: " << this->nodes[i].GateType << endl;
  //   cout << "\t\tName: " << this->nodes[i].name << endl;
  //   cout << "\t\tStrRef: " << this->nodes[i].strRef << endl;
  //   cout << "\t\tCorX: " << this->nodes[i].corX << endl;
  //   cout << "\t\tCorY: " << this->nodes[i].corY << endl;
  //   cout << endl;
  // }

  return 1;
}

/**
 * [plek::importCellDef - Imports the definitions of the cells]
 * @param  configFName [The config .toml file]
 * @return             [1 - Good; 0 - Error]
 */

int plek::importCellDef(string configFName){
  cout << "Importing parameters." << endl;

  const auto mainConfig  = toml::parse(configFName);
  const auto& w_para = toml::find(mainConfig, "wafer_parameters");
  const auto& r_para = toml::find(mainConfig, "run_parameters");

  this->rAlign = toml::find<string>(w_para, "row_align");
  this->vGap = toml::find<int>(w_para, "vertical_gap");
  this->hGap = toml::find<int>(w_para, "horizontal_gap");

  string workDir = toml::find<string>(r_para, "work_dir");
  string cellLibFName = toml::find<string>(r_para, "cell_dis_gds");
  cellLibFName = cellLibFName.insert(0, workDir);

  // Cell library definitions
  const auto cellLibFile   = toml::parse(cellLibFName);
  const auto cellLibConfig = toml::find(cellLibFile, "CONFIG");
  const auto CellList      = toml::find(cellLibConfig, "list");
  vector<string> cellList  = toml::get<vector<string>>(CellList);

  // Importing cells

  this->gateList.resize(cellList.size());
  for(unsigned int i = 0; i < this->gateList.size(); i++){
    auto cellTbl = toml::find(cellLibFile, cellList[i]);

    this->gateList[i].name = cellList[i];

    auto fooElement = toml::find(cellTbl, "size");
    vector<int> intVec = toml::get<vector<int>>(fooElement);
    this->gateList[i].sizeX = intVec[0];
    this->gateList[i].sizeY = intVec[1];

    fooElement = toml::find(cellTbl, "origin");
    intVec = toml::get<vector<int>>(fooElement);
    this->gateList[i].originX = intVec[0];
    this->gateList[i].originY = intVec[1];

    fooElement = toml::find(cellTbl, "gds_name");
    this->gateList[i].gds_name = toml::get<string>(fooElement);

    // fooElement = toml::find(cellTbl, "");
    // intVec = toml::get<vector<int>>(fooElement);
    // this->gateList[i]. = intVec[0];
    // this->gateList[i]. = intVec[1];

    // fooElement = toml::find(cellTbl, "gate_delay");
    // this->gateList[i].gate_delay = toml::get<float>(fooElement);
  }

  for(unsigned int i = 0; i < gateList.size(); i++){
    this->gateListMap[gateList[i].name] = i;
  }

  map<string, int>::iterator it;

  for(unsigned int i = 0; i < this->nodes.size(); i++){
    it = this->gateListMap.find(this->nodes[i].GateType);
    if(it == this->gateListMap.end()){
      cout << "Gate type: \"" << this->nodes[i].GateType << "\" NOT found in \"" << cellLibFName << "\"." << endl;
      // return 0;
    }
    this->nodes[i].strRef = it->second;
  }


  cout << "Importing parameters done." << endl;
  return 1;
}

/**
 * [plek::optiLayout - Optimize the layout of the IC]
 * @return [1 - Good; 0 - Error]
 */

int plek::optiLayout(){
  cout << "Optimizing the layouts of the gates." << endl;
  // this->printRoutes();
  // this->printCLKlevels();

  // Find longest routes
  unsigned int longestRoute = 0;

  for(unsigned int i = 0; i < this->routesWS.size(); i++){
    if(longestRoute < this->routesWS[i].size()){
      longestRoute = this->routesWS[i].size();
    }
  }

  this->layout.resize(longestRoute);

  bool found = false;

  for(unsigned int i = 0; i < this->routesWS.size(); i++){
    for(unsigned int j = 0; j < this->routesWS[i].size(); j++){
      found = false;
      for(unsigned int k = 0; k < this->layout.size(); k++)
        for(unsigned int l = 0; l < this->layout[k].size(); l++){
          if(this->layout[k][l] == this->routesWS[i][j]){
            found = true;
            break;
          }
        }
      if(!found){
        this->layout[j].push_back(this->routesWS[i][j]);
      }
    }
  }

  this->printLayout();

  return 1;
}

/**
 * [plek::defSTR Defines all the GDS structures that will be referenced]
 * @return [1 - Good; 0 - Error]
 */

int plek::defSTR(){
  // arrSTR.resize(this->gateListMap.size());
  // int i = 0;
  // for(auto it = gateListMap.begin(); it != gateListMap.end(); it++){
  //   arrSTR[i].name = it->second.name;
  //   arrSTR[i].BOUNDARY.push_back(draw2ptBox(1, 0, 0, it->second.sizeX, it->second.sizeY));
  //   i++;
  // }

  arrSTR.resize(this->gateList.size());
  for(unsigned int i = 0; i < this->gateList.size(); i++){
    arrSTR[i].name = this->gateList[i].name;
    arrSTR[i].BOUNDARY.push_back(draw2ptBox(1, 0, 0, this->gateList[i].sizeX, this->gateList[i].sizeY));
  }

  return 1;
}

/**
 * [plek::alignLeft - Aligns all the gates to the left]
 * @return [1 - Good; 0 - Error]
 */

int plek::alignLeft(){
  int cPtX; // current point on the X-axis
  int cPtY = 0; // current point on the Y-axis
  unsigned int rowHeight = 50;

  unsigned int index;

  for(int i = this->layout.size()-1; i >= 0; i--){
    cPtX = 0;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      index = this->layout[i][j];
      if(!this->nodes[index].GateType.compare("SC") || !this->nodes[index].GateType.compare("input") || !this->nodes[index].GateType.compare
        ("output")){
        continue;
      }

      // cout << "Placed: " << this->nodes[index].GateType << "; x=" << cPtX << " y=" << cPtY << endl;

      this->nodes[index].corX = cPtX;
      this->nodes[index].corY = cPtY;

      cPtX += gateList[(this->nodes[index].strRef)].sizeX;
    }
    cPtY += rowHeight;
  }
  return 1;
}


/**
 * [plek::to_gds - Creates an GDS file of the class]
 * @return [1 - Good; 0 - Error]
 */

int plek::to_gds(string fileName){
  cout << "Creating the GDS file \"" << fileName << "\"" << endl;
  gdscpp gdsFile;

  // defining the gates
  this->defSTR();

  // layout of the cells
  this->alignLeft();

  this->arrSTR.resize(this->arrSTR.size()+1);
  this->arrSTR.back().name = "MyIC";
  for(unsigned int i = 0; i < this->nodes.size(); i++){
    if(!this->nodes[i].GateType.compare("SC") || !this->nodes[i].GateType.compare("input") || !this->nodes[i].GateType.compare("output")){
      continue;
    }
    this->arrSTR.back().SREF.push_back(drawSREF(this->nodes[i].GateType, this->nodes[i].corX, this->nodes[i].corY));
  }



  gdsFile.setSTR(arrSTR);
  gdsFile.write(fileName);

  return 1;
}


/**
 * [plek::printLayout - Displays the layout of the gates]
 */

void plek::printLayout(){
  cout << "Circuit layout:" << endl;

  unsigned int j;

  for(unsigned int i = 0; i < this->layout.size(); i++){
    cout << i << ": ";
    for(j = 0; j < this->layout[i].size()-1; j++){
      cout << "\"" << this->nodes[this->layout[i][j]].name << "\"; ";
    }
    cout << this->nodes[this->layout[i][j]].name << endl;
  }
}

/**
 * [plek::printRoutes - Displays the routes]
 */

void plek::printRoutes(){
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
 * [plek::printCLKlevels - Displays the clock levels]
 */

void plek::printCLKlevels(){
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

void plek::to_str(){
  cout << "Plek Class:" << endl;
  for(unsigned int i = 0; i < this->gateList.size(); i++){
    this->gateList[i].to_str();
  }
}

void cellDis::to_str(){
  cout << "Cell Description:" << endl;
  cout << "  name: " << this->name << endl;
  cout << "  GDS str: " << this->gds_name << endl;
  cout << "  sizeX: " << this->sizeX << endl;
  cout << "  sizeY: " << this->sizeY << endl;
  cout << "  originX: " << this->originX << endl;
  cout << "  originY: " << this->originY << endl;
}