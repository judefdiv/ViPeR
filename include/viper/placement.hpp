/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-09-4
 * Modified:
 * license:
 * Description: Places the gates
 * File:        placement.hpp
 */

#ifndef Placement
#define Placement

#include <string>
#include <sstream>
#include <iostream>
#include <vector>
#include <fstream>
#include <map>

#include "toml/toml.hpp"
#include "viper/ParserBlif.hpp"
#include "gdscpp/gdsCpp.hpp"
#include "gdscpp/gdsForge.hpp"

#define GDSunitScale 1000

using namespace std;

class cellDis;

class plek{
  private:
    vector<gdsSTR> arrSTR;

    vector<BlifNode> nodes;
    vector<BlifNet> nets;

    vector<vector<unsigned int>> routes;
    vector<vector<unsigned int>> routesWS;
    vector<vector<unsigned int>> CLKlevel;

    vector<vector<unsigned int>> layout; // [row][columns]
    vector<unsigned int> layoutInputs; // [row][columns]
    vector<unsigned int> layoutOutputs; // [row][columns]
    set<string> used_gates;

    vector<cellDis> gateList;
    map<string, int> gateListMap; // GDS structure name, index of gateList

    // placement of gates (layout)
    int alignLeft();
    int alignRight();
    int alignCentre();
    int alignJustify();
    int alignJustifyFlush();

    int rounder(int inVal, int roundTo);
    int rounderOffset(int inVal, int roundTo, int offSet);

    // all gates parameters
    unsigned int port_size_x = 0;
    unsigned int port_size_y = 0;
    unsigned int ptl_width = 0;
    unsigned int via_size_x = 0;
    unsigned int via_size_y = 0;
    unsigned int pillar_size_x = 0;
    unsigned int pillar_size_y = 0;

    // wafer parameters
    vector<string> inputPinOrder;
    vector<string> outputPinOrder;
    string rAlign = "\0";
    int xOffset = 0;
    int yOffset = 0;
    int vGap = 0;
    int hGap = 0;
    int cellHeight = 0;
    int padVerGap = 0;
    int padHorHap = 0;
    int gridSize = 0;


    // Others
    unsigned int pad_index; // the index in gateList of the PAD

    int optiLayout();
    int sortRoutes();
    int stackLayout();
    int importCellDef(string configFName);
    int importParameters(string configFName);

    void printRoutesSubset(vector<vector<unsigned int>> inRoute);

  public:
    plek();
    ~plek(){};

    int populate(string configFName);
    int to_gds(string fileName);


    vector<gdsSTR> defSTR();
    gdsSTR gsdLayout();

    vector<BlifNode> get_nodes(){return this->nodes;};
    vector<BlifNet> get_nets(){return this->nets;};
    vector<cellDis> get_GateList(){return gateList;};
    vector<vector<unsigned int>> get_layout(){return layout;};

    void fetchSFQblif(vector<BlifNode> inNodes,
                       vector<BlifNet> inNets,
                       vector<vector<unsigned int>> inRoutes,
                       vector<vector<unsigned int>> inRoutesWS,
                       vector<vector<unsigned int>> inCLKlevels);

    void printRoutes();
    void printLayout();
    void printCLKlevels();

    void setLayout(vector<vector<unsigned int>> inLayout, vector<BlifNode> inNodes){
        this->layout = inLayout;
        this->nodes = inNodes;
    };

    vector<cellDis> get_gateList(){return gateList;};

    void to_str();
};

class cellDis{
  private:

    // Ports
    // Pillars
    // float gate_delay = 0.0;
    // unsigned int jj_count = 0;

  public:
    cellDis(){};
    ~cellDis(){};

    string name;              // The GDS structure name

    unsigned int sizeX = 0;
    unsigned int sizeY = 0;

    vector<int> pins_in_x;
    vector<int> pins_in_y;
    vector<int> pins_out_x;
    vector<int> pins_out_y;
    vector<int> pins_in_out_x;
    vector<int> pins_in_out_y;
    vector<int> clk_x;
    vector<int> clk_y;

    vector<int> pillars_x;
    vector<int> pillars_y;

    int originX = 0;
    int originY = 0;

    string gds_name = "\0";    // The name of the main structure in the GDS file
    string gds_file_name = "\0";
    void to_str();
};


#endif