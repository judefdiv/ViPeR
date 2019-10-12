/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-09
 * Modified:
 * license:
 * Description: Creates clock for SFQ circuit
 * File:        clkPlacement.hpp
 */

#ifndef clkPlace
#define clkPlace

#include <string>
#include <iostream>
#include <vector>
// #include <map>

// #include "toml/toml.hpp"
#include "viper/ParserBlif.hpp"
#include "gdscpp/gdsCpp.hpp"
#include "viper/placement.hpp"
#include "viper/routing.hpp"
#include "viper/genFunc.hpp"

#define xOffset 2000000

class clkChip{
  private:
    vector<BlifNode> nodes;
    vector<BlifNet> nets;
    vector<cellDis> gateList;

    unsigned int splitIndex = 0; // index of the splitter in gateList
    unsigned int clkSplitIndex = 0; // index of the start of the clock splitters in the nodes class

    unsigned int clkPinCnt = 0;
    vector<unsigned int> finCLKsplit;

    int recurCreateCLK(unsigned int curLev,
                        unsigned int maxLev,
                        int curX, int curY,
                        bool orienta,
                        unsigned int netNo);

    vector<int> HtreeCalc(int curX, int curY,
                          bool orienta,
                          unsigned int curDepth);

    int drawCLKnode(int corX, int corY, unsigned int netNo);

  public:
    clkChip(){};
    ~clkChip(){};

    void fetchData(vector<BlifNode> inNodes,
                    vector<BlifNet> inNets,
                    vector<cellDis> inGateList,
                    string configFile,
                    unsigned int gateCnt);

    int hitIt();

    vector<BlifNode> get_nodes(){return this->nodes;};
    vector<BlifNet> get_nets(){return this->nets;};
    vector<cellDis> get_GateList(){return gateList;};

    void to_str();
    gdsSTR to_gds();
};

#endif