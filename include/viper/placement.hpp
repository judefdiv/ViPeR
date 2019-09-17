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

    vector<cellDis> gateList;
    map<string, int> gateListMap; // GDS structure name, index of gateList

    // GDS goodies
    int defSTR();
    int alignLeft();
    int alignRight();
    int alignCentre();
    int alignJustify();

    // wafer parameters
    string rAlign = "\0";
    int vGap = 0;
    int hGap = 0;

    int optiLayout();
    int importCellDef(string configFName);

  public:
    plek();
    ~plek(){};

    int populate(string configFName);
    int to_gds(string fileName);

    void fetchSFQblif(vector<BlifNode> inNodes, vector<BlifNet> inNets, vector<vector<unsigned int>> inRoutes, vector<vector<unsigned int>> inRoutesWS, vector<vector<unsigned int>> inCLKlevels);

    void printRoutes();
    void printLayout();
    void printCLKlevels();

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

    int originX = 0;
    int originY = 0;

    string gds_name = "\0";

    void to_str();
};


#endif