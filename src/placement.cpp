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
  this->printLayout();
  this->importCellDef(configFName);
  this->to_str();

  return 1;
}

/**
 * [plek::importCellDef - Imports the definitions of the cells]
 * @param  configFName [The config .toml file]
 * @return             [1 - Good; 0 - Error]
 */

int plek::importCellDef(string configFName){
  cout << "Importing parameters." << endl;

  const auto mainConfig = toml::parse(configFName);
  const auto& w_para    = toml::find(mainConfig, "wafer_parameters");
  const auto& r_para    = toml::find(mainConfig, "run_parameters");

  // -----------------------------------------------------------------

  this->rAlign     = toml::find<string>(w_para, "row_align");
  this->vGap       = toml::find<int>(w_para, "vertical_gap") * GDSunitScale;
  this->hGap       = toml::find<int>(w_para, "horizontal_gap")  * GDSunitScale;
  this->cellHeight = toml::find<int>(w_para, "cell_height") * GDSunitScale;

  // -----------------------------------------------------------------

  string workDir      = toml::find<string>(r_para, "work_dir");
  string cellLibFName = toml::find<string>(r_para, "cell_dis_gds");
  cellLibFName        = cellLibFName.insert(0, workDir);

  // vector<string> celllist2 = toml::get<vector<string>>(mainConfig);

  // -----------------------------------------------------------------

  const auto cellLibFile   = toml::parse(cellLibFName);

  // -----------------------------------------------------------------

  const auto& g_para = toml::find(cellLibFile, "ALL_GATES");

  auto fooElement    = toml::find(g_para, "port_size");
  // vector<int> intVec = toml::get<vector<int>>(fooElement);
  vector<float> fltVec = toml::get<vector<float>>(fooElement);
  this->port_size_x  = fltVec[0] * GDSunitScale;
  this->port_size_y  = fltVec[1] * GDSunitScale;

  // fooElement      = toml::find(g_para, "ptl_width");
  // fltVec          = toml::get<vector<float>>(fooElement);
  this->ptl_width = toml::find<float>(g_para, "ptl_width") * GDSunitScale;

  fooElement       = toml::find(g_para, "via_size");
  fltVec           = toml::get<vector<float>>(fooElement);
  this->via_size_x = fltVec[0] * GDSunitScale;
  this->via_size_y = fltVec[1] * GDSunitScale;

  fooElement          = toml::find(g_para, "bias_p_size");
  fltVec              = toml::get<vector<float>>(fooElement);
  this->pillar_size_x = fltVec[0] * GDSunitScale;
  this->pillar_size_y = fltVec[1] * GDSunitScale;

  // -----------------------------------------------------------------

  const auto& cellLibConfig = toml::find(cellLibFile, "CONFIG");
  const auto& CellList      = toml::find(cellLibConfig, "list");
  vector<string> cellList   = toml::get<vector<string>>(CellList);

  // ----------------- Cell library definitions ----------------------

  // Importing cells
  unsigned int index = 0;
  this->gateList.resize(this->used_gates.size());
  for(unsigned int i = 0; i < cellList.size(); i++){

    if(this->used_gates.find(cellList[i]) == this->used_gates.end()){
      continue;
    }
    auto cellTbl = toml::find(cellLibFile, cellList[i]);

    this->gateList[index].name = cellList[i];

    fooElement = toml::find(cellTbl, "size");
    // fltVec = toml::get<vector<float>>(fooElement);
    vector<int> intVec = toml::get<vector<int>>(fooElement);
    this->gateList[index].sizeX = intVec[0] * GDSunitScale;
    this->gateList[index].sizeY = intVec[1] * GDSunitScale;

    fooElement = toml::find(cellTbl, "origin");
    // fltVec = toml::get<vector<float>>(fooElement);
    intVec = toml::get<vector<int>>(fooElement);
    this->gateList[index].originX = intVec[0] * GDSunitScale;
    this->gateList[index].originY = intVec[1] * GDSunitScale;

    fooElement = toml::find(cellTbl, "gds_name");
    this->gateList[index].gds_name = toml::get<string>(fooElement);

    fooElement = toml::find(cellTbl, "gds_f_name");
    this->gateList[index].gds_file_name = toml::get<string>(fooElement);


    // Pillar:
    fooElement = toml::find(cellTbl, "pillar");
    intVec = toml::get<vector<int>>(fooElement);

    if(intVec[0] != -1){
      this->gateList[index].pillars_x.push_back(intVec[0] * GDSunitScale);
      this->gateList[index].pillars_y.push_back(intVec[1] * GDSunitScale);
    }

    // Defining Pins
    fooElement = toml::find(cellTbl, "pins");
    vector<string> strVec = toml::get<vector<string>>(fooElement);
    string pinType;
    string pinName;       // the name of the pin in the .toml file, P1, P2, ..., Pn

    for(unsigned int j = 0; j < strVec.size(); j++){
      pinType = strVec[j];
      pinName = "P" + to_string(j+1);

      fooElement = toml::find(cellTbl, pinName);
      // fltVec = toml::get<vector<float>>(fooElement);
      intVec = toml::get<vector<int>>(fooElement);

      if(!pinType.compare("in")){
        this->gateList[index].pins_in_x.push_back(intVec[0] * GDSunitScale); // x
        this->gateList[index].pins_in_y.push_back(intVec[1] * GDSunitScale); // y
      }
      else if(!pinType.compare("out")){
        this->gateList[index].pins_out_x.push_back(intVec[0] * GDSunitScale); // x
        this->gateList[index].pins_out_y.push_back(intVec[1] * GDSunitScale); // y
      }
      else if(!pinType.compare("clk")){
        this->gateList[index].clk_x.push_back(intVec[0] * GDSunitScale); // x
        this->gateList[index].clk_y.push_back(intVec[1] * GDSunitScale); // y
      }
      else if(!pinType.compare("in_out")){
        this->gateList[index].pins_in_out_x.push_back(intVec[0] * GDSunitScale); // x
        this->gateList[index].pins_in_out_y.push_back(intVec[1] * GDSunitScale); // y
      }
      else{
        cout << "Pin naming error with \"" << cellList[i] << "\" in \"" << cellLibFName << "\"" << endl;
        return 0;
      }
    }

    index++;
  }

  for(unsigned int i = 0; i < this->nodes.size(); i++){
    for(unsigned int k = 0; k < this->gateList.size(); k++){
      if(!this->gateList[k].name.compare(this->nodes[i].GateType)){
        this->nodes[i].strRef = k;
        break;
      }
    }
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

  // Find longest routes
  unsigned int longestRoute = 0;

  for(unsigned int i = 0; i < this->routesWS.size(); i++){
    if(longestRoute < this->routesWS[i].size()){
      longestRoute = this->routesWS[i].size();
    }
  }

  this->layout.resize(longestRoute-2);
  bool found = false;
  for(unsigned int i = 0; i < this->routesWS.size(); i++){
    for(unsigned int j = 1; j < this->routesWS[i].size()-1; j++){
      found = false;
      for(unsigned int k = 0; k < this->layout.size(); k++){
        for(unsigned int l = 0; l < this->layout[k].size(); l++){
          if(this->layout[k][l] == this->routesWS[i][j]){
            found = true;
            break;
          }
        }
      }
      if(!found){
        this->layout[j-1].push_back(this->routesWS[i][j]);
        this->used_gates.insert(this->nodes[this->routesWS[i][j]].GateType);
      }
    }
  }

  set<string>::iterator it;
    cout << "Gates used: ";
    for (it = this->used_gates.begin(); it != this->used_gates.end(); it++){
        cout << *it << "; ";
    }
    cout << endl;

  cout << "Optimizing the layouts of the gates done." << endl;

  return 1;
}

/**
 * [plek::defSTR Defines all the GDS structures that will be referenced]
 * @return [1 - Good; 0 - Error]
 */

int plek::defSTR(){
  // Defining pin

  this->arrSTR.resize(this->gateList.size());
  for(unsigned int i = 0; i < this->gateList.size(); i++){
    // Gate name
    this->arrSTR[i].name = this->gateList[i].name;

    // Cell outline
    this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(1, 0, 0, this->gateList[i].sizeX, this->gateList[i].sizeY));

    // Pins
    for(unsigned int j = 0; j < this->gateList[i].pins_in_x.size(); j++){
      this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(2, this->gateList[i].pins_in_x[j] - (this->port_size_x/2),
                                                        this->gateList[i].pins_in_y[j] - (this->port_size_y/2),
                                                        this->gateList[i].pins_in_x[j] + (this->port_size_x/2),
                                                        this->gateList[i].pins_in_y[j] + (this->port_size_y/2)));
    }
    for(unsigned int j = 0; j < this->gateList[i].pins_out_x.size(); j++){
      this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(2, this->gateList[i].pins_out_x[j] - (this->port_size_x/2),
                                                       this->gateList[i].pins_out_y[j] - (this->port_size_y/2),
                                                       this->gateList[i].pins_out_x[j] + (this->port_size_x/2),
                                                       this->gateList[i].pins_out_y[j] + (this->port_size_y/2)));
    }
    for(unsigned int j = 0; j < this->gateList[i].pins_in_out_x.size(); j++){
      this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(2, this->gateList[i].pins_in_out_x[j] - (this->port_size_x/2),
                                                        this->gateList[i].pins_in_out_y[j] - (this->port_size_y/2),
                                                        this->gateList[i].pins_in_out_x[j] + (this->port_size_x/2),
                                                        this->gateList[i].pins_in_out_y[j] + (this->port_size_y/2)));
    }
    for(unsigned int j = 0; j < this->gateList[i].clk_x.size(); j++){
      this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(2, this->gateList[i].clk_x[j] - (this->port_size_x/2),
                                                        this->gateList[i].clk_y[j] - (this->port_size_y/2),
                                                        this->gateList[i].clk_x[j] + (this->port_size_x/2),
                                                        this->gateList[i].clk_y[j] + (this->port_size_y/2)));
    }

    // Pillars
    // for(unsigned int j = 0; j < this->gateList[i].pillars_x.size(); j++){
    //   this->arrSTR[i].BOUNDARY.push_back(draw2ptBox(3, this->gateList[i].pillars_x[j] - (this->pillar_size_x/2),
    //                                               this->gateList[i].pillars_y[j] - (this->pillar_size_y/2),
    //                                               this->gateList[i].pillars_x[j] + (this->pillar_size_x/2),
    //                                               this->gateList[i].pillars_y[j] + (this->pillar_size_y/2)));
    // }

  }

  return 1;
}

/**
 * [plek::straightRoute - Creates straight routes between all the pins]
 * @return [1 - Good; 0 - Error]
 */

// int plek::straightRoute(){
//   unsigned int toNode, fromNode;
//   vector<int> corX;
//   vector<int> corY;

//   corX.resize(2);
//   corY.resize(2);

//   this->arrSTR.resize(this->arrSTR.size()+1);
//   this->arrSTR.back().name = "routes";

//   for(unsigned int i = 0; i < this->nodes.size(); i++){
//     if(!this->nodes[i].GateType.compare("SC") ||
//       !this->nodes[i].GateType.compare("input") ||
//       !this->nodes[i].GateType.compare("output")){
//       continue;
//     }

//     for(unsigned int j = 0; j < this->nodes[i].outNets.size(); j++){
//       cout << "here " << this->nodes[i].name << endl;
//       toNode = this->nets[this->nodes[i].outNets[j]].inNodes[0];
//       fromNode = this->nets[this->nodes[i].outNets[j]].outNodes[0];

//       corX[0] = this->nodes[fromNode].corX + this->gateList[this->nodes[fromNode].strRef].pins_out_x[0];
//       corY[0] = this->nodes[fromNode].corY + this->gateList[this->nodes[fromNode].strRef].pins_out_y[0];

//       corX[1] = this->nodes[toNode].corX + this->gateList[this->nodes[toNode].strRef].pins_in_x[0];
//       corY[1] = this->nodes[toNode].corY + this->gateList[this->nodes[toNode].strRef].pins_in_y[0];

//       this->arrSTR.back().PATH.push_back(drawPath(4, this->ptl_width, corX, corY));
//     }
//   }

//   return 1;
// }

int plek::straightRoute(){
  unsigned int toNode, fromNode;
  vector<int> corX;
  vector<int> corY;

  vector<int> nodesInputCnt;
  vector<int> nodesOutputCnt;

  corX.resize(2);
  corY.resize(2);

  nodesInputCnt.resize(this->nodes.size());
  nodesOutputCnt.resize(this->nodes.size());

  for(unsigned int i = 0; i < nodesInputCnt.size(); i++){
    nodesInputCnt[i] = 0;
    nodesOutputCnt[i] = 0;
  }

  this->arrSTR.resize(this->arrSTR.size()+1);
  this->arrSTR.back().name = "routes";

  for(unsigned int i = 0; i < this->nets.size(); i++){
    for(unsigned int j = 0; j < this->nets[i].outNodes.size(); j++){
      // if(this->nets[i].outNodes.size() > 0){
      if(!this->nodes[this->nets[i].outNodes[j]].GateType.compare("output") ||
          !this->nodes[this->nets[i].outNodes[j]].GateType.compare("SC")){
        continue;
        }
      // }
      if(this->nets[i].inNodes.size() > 0){
        if(!this->nodes[this->nets[i].inNodes[0]].GateType.compare("input") ||
            !this->nodes[this->nets[i].inNodes[0]].GateType.compare("SC")){
          continue;
        }
      }

      fromNode = this->nets[i].inNodes[0];
      toNode = this->nets[i].outNodes[j];

      cout << this->nodes[fromNode].name << " -> " << this->nodes[toNode].name << endl;

      corX[0] = this->nodes[fromNode].corX + this->gateList[this->nodes[fromNode].strRef].pins_out_x[nodesOutputCnt[fromNode]];
      corY[0] = this->nodes[fromNode].corY + this->gateList[this->nodes[fromNode].strRef].pins_out_y[nodesOutputCnt[fromNode]];
      nodesOutputCnt[fromNode]++;

      corX[1] = this->nodes[toNode].corX + this->gateList[this->nodes[toNode].strRef].pins_in_x[nodesInputCnt[toNode]];
      corY[1] = this->nodes[toNode].corY + this->gateList[this->nodes[toNode].strRef].pins_in_y[nodesInputCnt[toNode]];
      nodesInputCnt[toNode]++;

      this->arrSTR.back().PATH.push_back(drawPath(4, this->ptl_width, corX, corY));
    }
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
  unsigned int index;

  for(int i = this->layout.size()-1; i >= 0; i--){
    cPtX = 0;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      index = this->layout[i][j];

      this->nodes[index].corX = cPtX;
      this->nodes[index].corY = cPtY;

      // cout << "Placed: " << this->nodes[index].GateType << "; x=" << cPtX << " y=" << cPtY << endl;

      cPtX += gateList[(this->nodes[index].strRef)].sizeX + this->hGap;
    }
    cPtY += this->cellHeight + this->vGap;
  }
  return 1;
}

/**
 * [plek::alignCentre - Aligns all the gates to the centre]
 * @return [1 - Good; 0 - Error]
 */

int plek::alignCentre(){
  int cPtX; // current point on the X-axis
  int cPtY = 0; // current point on the Y-axis
  unsigned int index;
  vector<unsigned int> rowWidth; // bottom to top
  unsigned int rowWidthMax = 0;;

  // calculating the width of each row.
  unsigned int fooWdith;
  rowWidth.resize(this->layout.size());
  for(int i = this->layout.size()-1; i >= 0; i--){
    fooWdith = 0;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      fooWdith += gateList[(this->nodes[this->layout[i][j]].strRef)].sizeX;
    }
    rowWidth[i] = fooWdith;
  }
  for(unsigned int i = 0; i < rowWidth.size(); i++){
    if(rowWidthMax < rowWidth[i])
      rowWidthMax = rowWidth[i];
  }

  // The actual layout part
  for(int i = this->layout.size()-1; i >= 0; i--){
    cPtX = (rowWidthMax - rowWidth[i])/2;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      index = this->layout[i][j];

      this->nodes[index].corX = cPtX;
      this->nodes[index].corY = cPtY;

      cPtX += gateList[(this->nodes[index].strRef)].sizeX + this->hGap;
    }
    cPtY += this->cellHeight + this->vGap;
  }
  return 1;
}

/**
 * [plek::alignJustify - Aligns all the gates to the centre with equal spacing]
 * @return [1 - Good; 0 - Error]
 */

int plek::alignJustify(){
  int cPtX; // current point on the X-axis
  int cPtY = 0; // current point on the Y-axis
  unsigned int index;
  vector<unsigned int> rowWidth; // bottom to top
  unsigned int rowWidthMax = 0;;

  // calculating the width of each row.
  unsigned int fooWdith;
  rowWidth.resize(this->layout.size());
  for(int i = this->layout.size()-1; i >= 0; i--){
    fooWdith = 0;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      fooWdith += gateList[(this->nodes[this->layout[i][j]].strRef)].sizeX;
    }
    rowWidth[i] = fooWdith;
  }
  for(unsigned int i = 0; i < rowWidth.size(); i++){
    if(rowWidthMax < rowWidth[i])
      rowWidthMax = rowWidth[i];
  }

  // The actual layout part
  unsigned int rowGapSize;
  for(int i = this->layout.size()-1; i >= 0; i--){
    rowGapSize = (rowWidthMax - rowWidth[i])/(this->layout[i].size()+1);
    cPtX = rowGapSize;
    for(unsigned int j = 0; j < this->layout[i].size(); j++){
      index = this->layout[i][j];

      this->nodes[index].corX = cPtX;
      this->nodes[index].corY = cPtY;

      cPtX += gateList[(this->nodes[index].strRef)].sizeX + rowGapSize + this->hGap;
    }
    cPtY += this->cellHeight + this->vGap;
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
  if(!this->rAlign.compare("left")){
    this->alignLeft();
  }
  else if(!this->rAlign.compare("centre")){
    this->alignCentre();
  }
  else if(!this->rAlign.compare("justify")){
    this->alignJustify();
  }
  else{
    this->alignJustify();
  }

  this->straightRoute();

  this->arrSTR.resize(this->arrSTR.size()+1);
  this->arrSTR.back().name = "MyIC";
  this->arrSTR.back().SREF.push_back(drawSREF("routes", 0, 0));
  for(unsigned int i = 0; i < this->nodes.size(); i++){
    if(!this->nodes[i].GateType.compare("SC") ||
        !this->nodes[i].GateType.compare("input") ||
        !this->nodes[i].GateType.compare("output")){
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

  for(unsigned int i = 0; i < this->layout.size(); i++){
    cout << i << ": ";
    for(unsigned j = 0; j < this->layout[i].size(); j++){
      cout << "\"" << this->nodes[this->layout[i][j]].name << "\"; ";
    }
    cout << endl;
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

  cout << "  port_size_x: " << this->port_size_x << endl;
  cout << "  port_size_y: " << this->port_size_y << endl;

  cout << "  via_size_x: " << this->via_size_x << endl;
  cout << "  via_size_y: " << this->via_size_y << endl;

  cout << "  pillar_size_x: " << this->pillar_size_x << endl;
  cout << "  pillar_size_y: " << this->pillar_size_y << endl;

  cout << "  ptl_width: " << this->ptl_width << endl;

  for(unsigned int i = 0; i < this->gateList.size(); i++){
    this->gateList[i].to_str();
  }

  for(unsigned int i = 0; i < this->nodes.size(); i++){
    this->nodes[i].to_str();
  }
}

void cellDis::to_str(){
  cout << "Cell Description:" << endl;
  cout << "  name: " << this->name << endl;
  cout << "  GDS str: " << this->gds_name << endl;
  cout << "  GDS file name: " << this->gds_file_name << endl;
  cout << "  sizeX: " << this->sizeX << endl;
  cout << "  sizeY: " << this->sizeY << endl;
  cout << "  originX: " << this->originX << endl;
  cout << "  originY: " << this->originY << endl;

  cout << "  Pins in:" << endl;
  for(unsigned int i = 0; i < this->pins_in_x.size(); i++){
    cout << "    x[" << i << "] = " << this->pins_in_x[i] << endl;
    cout << "    y[" << i << "] = " << this->pins_in_y[i] << endl;
  }

  cout << "  Pins out:" << endl;
  for(unsigned int i = 0; i < this->pins_out_x.size(); i++){
    cout << "    x[" << i << "] = " << this->pins_out_x[i] << endl;
    cout << "    y[" << i << "] = " << this->pins_out_y[i] << endl;
  }

  cout << "  Pins in_out:" << endl;
  for(unsigned int i = 0; i < this->pins_in_out_x.size(); i++){
    cout << "    x[" << i << "] = " << this->pins_in_out_x[i] << endl;
    cout << "    y[" << i << "] = " << this->pins_in_out_y[i] << endl;
  }

  cout << "  Pins clk:" << endl;
  for(unsigned int i = 0; i < this->clk_x.size(); i++){
    cout << "    x[" << i << "] = " << this->clk_x[i] << endl;
    cout << "    y[" << i << "] = " << this->clk_y[i] << endl;
  }

  cout << "  Pillars:" << endl;
  for(unsigned int i = 0; i < this->pillars_x.size(); i++){
    cout << "    x[" << i << "] = " << this->pillars_x[i] << endl;
    cout << "    y[" << i << "] = " << this->pillars_y[i] << endl;
  }

}