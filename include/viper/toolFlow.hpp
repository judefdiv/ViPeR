/**
 * Author:      Jude de Villiers, Edrich Verburg
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-09-11
 * Modified:
 * license:
 * Description: All the ViPeR tools get executed here
 * File:        toolFlow.hpp
 */

#include <string>
#include <iostream>

// #include "viper/ParserBlif.hpp"
// #include "viper/ParserDot.hpp"
#include "viper/genFunc.hpp"
#include "viper/ForgeSFQblif.hpp"
#include "viper/placement.hpp"
#include "viper/routing.hpp"
#include "viper/placement.hpp"
#include "viper/chipForge.hpp"
#include "viper/clkPlacement.hpp"
#include "viper/ParserLef.hpp"


using namespace std;

int blif2gds(string gdsFile, string blifFile, string configFName);
int gdf2lef(const string &gdfFile, const string &lefFile);
int runqRouter(const string &defFile, const string &configFile, ForgeSFQBlif SFQcir);