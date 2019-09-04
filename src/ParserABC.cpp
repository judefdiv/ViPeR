/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-03
 * Modified:
 * license:
 * Description: Parser for ABC.
 * File:				ParserABC.cpp
 */

#include "viper/ParserABC.hpp"

Abc_Frame_t * pAbc;


/**
 * [runABC - Test script for running ABC]
 * @return [1 - All good; 0 - error]
 */

int runABC(){
	// Abc_Frame_t * pAbc;


	// start ABC framework
	Abc_Start();
	pAbc = Abc_FrameGetGlobalFrame();

	// runCmd("read rca2.v");
	// runCmd("strash");
	// runCmd("print_stats");

	runABCcmd("read data/abc/rca2.v");
	// runABCcmd("show");
	runABCcmd("strash");
	runABCcmd("dc2");
	runABCcmd("logic");
	runABCcmd("read_library data/abc/abc.genlib");
	runABCcmd("map");
	// runABCcmd("show -g");
	runABCcmd("print_gates");
	runABCcmd("write data/results/abc/rca2_abc.v");
	runABCcmd("write data/results/abc/rca2_abc.blif");

	// end ABC framework
	Abc_Stop();
	return 1;
}

/**
 * [runABCcmd - Executes an ABC command]
 * @param  inStr [The input string to be passed to ABC]
 * @return       [1 - All good; 0 - error]
 */

int runABCcmd(string inStr){
	if(Cmd_CommandExecute(pAbc, inStr.c_str())){
		cout << "Invalid command for ABC - \"" << inStr << "\"" << endl;
		return 0;
	}
	return 1;
}


/**
 * [showBlif - Displays a .blif file using ABC functions]
 * @param  blifFileName  [The file name for .blif]
 * @param  c_libFileName [The file name for the cell library (.genlib)]
 * @return               [1 - All good; 0 - error]
 */

int showBlif(string blifFileName, string c_libFileName){

	Abc_Start();
	pAbc = Abc_FrameGetGlobalFrame();

	runABCcmd("read_library data/abc/abc.genlib");
	runABCcmd("read data/abc/rca2.v");
	runABCcmd("show -g");
	// runABCcmd("print_gates");

	Abc_Stop();
	return 1;
}




/**
 * [ABCconvertV2blif - To create a .blif file using ABC]
 * @param  blifFileName [The output file name for the SQF circuit created]
 * @param  veriFileName [The input file name of the verilog file that describes the circuit]
 * @param  cellFileName [The cell library that ABC uses to generate the circuit]
 * @return              [1 - All good; 0 - Error]
 */

int ABCconvertV2blif(string blifFileName, string veriFileName, string cellFileName){
	cout << "Starting ABC" << endl;
	// start ABC framework
	Abc_Start();
	pAbc = Abc_FrameGetGlobalFrame();

	runABCcmd("read " + veriFileName);
	runABCcmd("strash");
	runABCcmd("dc2");
	runABCcmd("logic");
	runABCcmd("read_library " + cellFileName);
	runABCcmd("map");
	// runABCcmd("show -g");
	// runABCcmd("print_gates");
	runABCcmd("write " + blifFileName);
	cout << "ABC created \"" << blifFileName << "\"" << endl;

	// end ABC framework
	Abc_Stop();

	cout << "ABC is dead." << endl;

	return 1;
}