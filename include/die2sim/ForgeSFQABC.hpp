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

#include "die2sim/ParserBlif.hpp"
#include "die2sim/ParserDot.hpp"
#include "die2sim/ParserABC.hpp"
#include "die2sim/genFunc.hpp"
#include "die2sim/ForgeSFQblif.hpp"

using namespace std;

#define binFolder "data/bin/"
#define blifOutFolder "data/results/blif/"

int runForgeSFQABC(string blifFileName, string veriFileName, string cellFileName);

int runForgeABC(string blifFileName, string veriFileName, string cellFileName);
int runForgeSFQ(string blifInFileName, string blifOutFileName);