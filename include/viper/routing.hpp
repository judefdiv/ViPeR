/**
 * Author:      Jude de Villiers, Edrich Verburg
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-3
 * Modified:
 * license:
 * Description: routing of tracks between pins
 * File:        routing.hpp
 */

#ifndef routing
#define routing

#include <string>
// #include <sstream>
#include <iostream>
#include <vector>
#include <map>

#include "toml/toml.hpp"
#include "viper/ParserBlif.hpp"
#include "gdscpp/gdsCpp.hpp"
#include "viper/placement.hpp"
#include "viper/ParserDef.hpp"

class roete{
  private:
    vector<BlifNode> nodes;
    vector<BlifNet> nets;
    vector<cellDis> gateList;

    unsigned int pad_index;

    unsigned int ptl_width = 0;

  public:
    roete(){};
    ~roete(){};

    int fetchData(vector<BlifNode> inNodes,
                    vector<BlifNet> inNets,
                    vector<cellDis> inGateList,
                    string configFile);

    int straightRoute();
    int qrouter(const string &fileName);
    gdsSTR route2gds();

    void to_str();
};

#endif