/**
 * Author:      Jude de Villiers, Edrich Verburg
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-04
 * Modified:
 * license:
 * Description: Creates the final GDS file
 * File:        chipForge.hpp
 */

#ifndef chipforge
#define chipforge

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

class chipForge{
  private:
    vector<BlifNode> nodes;
    vector<BlifNet> nets;
    vector<cellDis> gateList;

    vector<gdsSTR> Stuctures;

  public:
    chipForge(){};
    ~chipForge(){};

    void addSTR(gdsSTR inSTR){Stuctures.push_back(inSTR);};
    void appendSTR(vector<gdsSTR> inSTR){
      Stuctures.insert(Stuctures.end(), inSTR.begin(), inSTR.end());
    };

    int forgeChip(string fileName);
    void to_str();
};

#endif