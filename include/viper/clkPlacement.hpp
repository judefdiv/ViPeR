/**
 * Author:      Jude de Villiers, Edrich Verburg
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
#include <cmath>
#include <deque>
// #include <map>

// #include "toml/toml.hpp"
#include "viper/ParserBlif.hpp"
#include "gdscpp/gdsCpp.hpp"
#include "viper/placement.hpp"
#include "viper/routing.hpp"
#include "viper/genFunc.hpp"

#define xOffset 2000000

void vecInsert(vector<vector<unsigned int>> &vec,
                unsigned int whichRow,
                unsigned int toBeInserted,
                unsigned int pos);

vector<unsigned int> vecInsert(vector<unsigned int> &vec,
                                unsigned int toBeInserted,
                                unsigned int pos);

vector<vector<unsigned int>> vecInsertRow(vector<vector<unsigned int>> &vec,
                                          vector<unsigned int> &vecRow,
                                          unsigned int posRow);

class clkChip{
  private:
    vector<BlifNode> nodes;
    vector<BlifNet> nets;
    vector<cellDis> gateList;
    vector<vector<unsigned int>> cellLayout;
    vector<unsigned int> cellRowIndices;

    unsigned int splitIndex = 0; // index of the splitter in gateList
    unsigned int clkSplitIndex = 0; // index of the start of the clock splitters in the nodes class
    unsigned int clkSplitGateIndex = 0;
    unsigned int clkBuffGateIndex = 0;

    bool mergeClkRows = true;

    unsigned int clkPinCnt = 0;
    vector<unsigned int> finCLKsplit;

    unsigned int padHeightGap = 0;
    unsigned int cellHeight = 0;
    unsigned int veriticalHeightGap = 0;

    // const string CLK_GATE_NAME = "CLKSPLIT";
    // const string CLK_GATE_BUFF_NAME = "CLKBUFF";


    // --------------------------- Method 1 ---------------------------

    int recurCreateCLK(unsigned int curLev,
                        unsigned int maxLev,
                        int curX, int curY,
                        bool orienta,
                        unsigned int netNo);

    vector<int> HtreeCalc(int curX, int curY,
                          bool orienta,
                          unsigned int curDepth);

    int drawCLKnode(int corX, int corY, unsigned int netNo);

    // --------------------------- Method 2 ---------------------------

    vector<unsigned int> rowSplitInsert;
    vector<vector<unsigned int>> clkLayout;
    vector<unsigned int> rowCLKin; // the input clk per row


    int initialLogic2CLK();
    int mergeLayouts();
    int mapInitialLogic2CLKNets();
    int stitchCLKrows();
    int mainCLKdistri();
    int mainCLKvertical();
    int postCleanUP();

    // aux functions
    int drawCLKnodeBasic();
    unsigned int createCLKnode(unsigned int CLKnode0,
                                unsigned int CLKnode1);
    unsigned int createCLKnodeAlone(unsigned int CLKnode);
    vector<unsigned int> create2CLKnode(unsigned int CLKnode);
    int stitch2Clk(unsigned int parentCLK, unsigned int childCLK);

  public:
    clkChip(){};
    ~clkChip(){};

    void fetchData(vector<BlifNode> inNodes,
                    vector<BlifNet> inNets,
                    vector<cellDis> inGateList,
                    string configFile,
                    unsigned int gateCnt,
                    vector<vector<unsigned int>> layout);

    int execute();

    vector<BlifNode> get_nodes(){return this->nodes;};
    vector<BlifNet> get_nets(){return this->nets;};
    vector<cellDis> get_GateList(){return gateList;};
    vector<vector<unsigned int>> getCellLayout(){return this->cellLayout;};

    void to_str();
    gdsSTR to_gds();
};

#endif