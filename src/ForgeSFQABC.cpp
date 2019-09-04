 /**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-11
 * Modified:
 * license:
 * Description: Creates SFQ circuits using ABC
 * File:				ForgeSFQABC.cpp
 */

#include "viper/ForgeSFQABC.hpp"

/**
 * [runForgeSFQABC - To create a .blif sfq file using ABC]
 * @param  blifFileName [The output file name for the SQF circuit created]
 * @param  veriFileName [The input file name of the verilog file that describes the circuit]
 * @param  cellFileName [The cell library that ABC uses to generate the circuit]
 * @return              [1 - All good; 0 - Error]
 */

int runForgeSFQABC(string blifFileName, string veriFileName, string cellFileName){
	cout << "Executing process for creating SFQ circuits." << endl;

	ABCconvertV2blif(string(binFolder) + "cmosABC.blif", veriFileName, cellFileName);
	runForgeSFQ(string(binFolder) + "cmosABC.blif", blifFileName);
	return 1;
}

/**
 * [runForgeABC - Uses ABC to generate a .blif file from .v]
 * @param  blifFileName [The .blif output file]
 * @param  veriFileName [The verilog input file that must be converted]
 * @param  cellFileName [The cell library file ABC uses to generate the .blif file]
 * @return              [1 - All good; 0 - Error]
 */

int runForgeABC(string blifFileName, string veriFileName, string cellFileName){
	cout << "Converting .v to .blif using ABC" << endl;

	ABCconvertV2blif(blifFileName, veriFileName, cellFileName);

	cout << "Done converting .v to .blif using ABC" << endl;

	return 1;
}

/**
 * [runForgeSFQ - Converts a generic CMOS .blif file to SFQ .blif file]
 * @param  blifInFileName  [The input CMOS .blif file]
 * @param  blifOutFileName [The output SFQ .blif file]
 * @return                 [1 - All good; 0 - Error]
 */

int runForgeSFQ(string blifInFileName, string blifOutFileName){
	cout << "Converting CMOS .blif to SFQ .blif" << endl;

	ForgeSFQBlif SFQcir;
	SFQcir.importBlif(blifInFileName);
	SFQcir.toSFQ();
	// SFQcir.to_str();
	SFQcir.to_blif(blifOutFileName);
	// SFQcir.to_jpg(fileRenamer(blifOutFileName, blifOutFolder, "_sfq.jpg"));
	SFQcir.to_jpgAdv(fileRenamer(blifOutFileName, blifOutFolder, "_sfq.jpg"));

	// showBlif(string blifFileName, string gvFileName, string jpgFileName)
	// runDot(blifInFileName, fileRenamer(blifInFileName, binFolder, "_cmos.gv"), fileRenamer(blifInFileName, blifOutFolder, "_cmos.jpg"));
	// runDot(blifOutFileName, fileRenamer(blifOutFileName, binFolder, "_sfq.gv"), fileRenamer(blifOutFileName, blifOutFolder, "_sfq.jpg"));

	return 1;
}
