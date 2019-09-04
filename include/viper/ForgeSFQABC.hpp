/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-11
 * Modified:
 * license:
 * Description: Creates SFQ circuits using ABC
 * File:				ForgeSFQABC.hpp
 */

#include <string>
#include <iostream>

#include "viper/ParserBlif.hpp"
#include "viper/ParserDot.hpp"
#include "viper/ParserABC.hpp"
#include "viper/genFunc.hpp"
#include "viper/ForgeSFQblif.hpp"

using namespace std;

#define binFolder "data/bin/"
#define blifOutFolder "data/results/blif/"

int runForgeSFQABC(string blifFileName, string veriFileName, string cellFileName);

int runForgeABC(string blifFileName, string veriFileName, string cellFileName);
int runForgeSFQ(string blifInFileName, string blifOutFileName);