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

#include "viper/genFunc.hpp"

using namespace std;

// #define MaxNumberLevels 124

// struct BlifNode{      // includes input and outputs
//   string name = "\0";                   // Must be a unique ID
//   string GateType = "\0";

//   vector<unsigned int> inNets;
//   vector<unsigned int> outNets;
//   unsigned int clkNet = 0;

//   // Levels
//   unsigned int CLKlevel = 0;
//   unsigned int MaxLevel = 0;
//   unsigned int MinLevel = MaxNumberLevels;

// };

// struct BlifNet{
//   string name = "\0";                   // Must be a unique ID

//   vector<unsigned int> inNodes;
//   vector<unsigned int> outNodes;
// };

class plek{
  private:


  public:
    plek();
    ~plek(){};

    void to_str();
};


#endif