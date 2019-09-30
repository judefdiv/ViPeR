/**
 * Author:      Jude de Villiers
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
#include "viper/ParserABC.hpp"
#include "viper/genFunc.hpp"
#include "viper/ForgeSFQblif.hpp"
#include "viper/placement.hpp"

using namespace std;

int verilog2gds(string gdsFile, string veriFile, string cellFile, string configFName);
int blif2gds(string gdsFile, string blifFile, string configFName);
int runABC(string blifFile, string veriFile, string cellFile);
// int CMOSblif2SFQblif(string blifInFileName, string blifOutFileName);