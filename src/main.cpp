/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-03-20
 * Modified:
 * license: 
 * Description: Primary file for the program.
 * File:				main.cpp
 */

#include <iostream>				//stream
#include <string>					//string goodies
#include <set>
#include <map>

// #include "die2sim/ToGds.hpp"
// #include "die2sim/ToJosim.hpp"
// #include "die2sim/ToText.hpp"
#include "die2sim/genFunc.hpp"
#include "die2sim/ForgeSFQABC.hpp"
#include "toml/toml.hpp"

#define versionNo 0.92
#define configFile "config.toml"
#define outFolder "data/results/"
// #define outFolderGDS "data/results/gds/"
// #define outFolderJoSIM "data/results/josim/"
#define outFolderABC "data/results/abc/"
#define outFolderBlif "data/results/blif/"
using namespace std;

/** 
 * Declaring functions 
 */

void welcomeScreen();
void helpScreen();
string fileRenamer(string inName, string preFix, string suffix);
int RunTool(int argCount, char** argValues);
int RunToolFromConfig();

/**
 * Main loop
 */

int main(int argc, char* argv[]){
	// welcomeScreen();
	if(!RunTool(argc, argv)) return 0;

	return 0;
}

/**
 * [RunTool - Runs the appropriate tool]
 * @param  argCount  [Size of the input argument array]
 * @param  argValues [The input argument array]
 * @return           [1 - all good; 0 - error]
 */

int RunTool(int argCount, char** argValues){
	welcomeScreen();

	if(argCount <= 1){		
		return 0; 
	}

	// set<string> validCommands = {"-g", "-j", "-i", "-v", "-h", "-c", "-a", "-b", "-s"};
	set<string> validCommands = {"-v", "-h", "-c", "-a", "-b", "-s"};

	string outFName = "\0";			// The output file, which is follow by the -o parameter
	// string lefFName = "\0";			// The LEF file
	// string defFName = "\0";			// The DEF file
	// string gdsFName = "\0";			// The GDS/GDS2 file
	// string decFName = "\0";			// The file to be deciphered 
	string bliFName = "\0";			// The non SFQ blif file
	string verFName = "\0";			// verilog
	string glbFName = "\0";			// .genlib
	string command  = "\0";			// The command to be executed

	string foo;

	// search for command
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
		if(validCommands.find(foo) != validCommands.end()){
			command = foo;
		}
	}
	if(!command.compare("\0")){
		cout << "Invalid." << endl;
		return 0;
	}

	// // search for .lef
	// for(int i = 0; i < argCount; i++){
	// 	foo = string(argValues[i]);
	//   if(foo.find(".lef")!=string::npos){
	//   	lefFName = foo;
	//   }
	// }

	// // search for .def
	// for(int i = 0; i < argCount; i++){
	// 	foo = string(argValues[i]);
	//   if(foo.find(".def")!=string::npos){
	//   	defFName = foo;
	//   }
	// }

	// search for .v
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
	  if(foo.find(".v")!=string::npos){
	  	verFName = foo;
	  }
	}

	// search for .genlib
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
	  if(foo.find(".genlib")!=string::npos){
	  	glbFName = foo;
	  }
	}

	// search for .blif
	for(int i = 1; i < argCount; i++){
		foo = string(argValues[i]);
	  if(foo.find(".blif")!=string::npos && string(argValues[i-1]).compare("-o")){
	  	bliFName = foo;
	  }
	}

	// // search for gds/gsd2
	// for(int i = 0; i < argCount; i++){
	// 	foo = string(argValues[i]);
	//   if(foo.find(".gds")!=string::npos || foo.find(".gds2")!=string::npos){
	//   	gdsFName = foo;
	//   }
	// }

	// search for output filename
	for(int i = 0; i < argCount-1; i++){
	  if(!string(argValues[i]).compare("-o")){
	  	outFName = string(argValues[i+1]);
	  }
	}
	// // auto assign output filename if non has been set
	// if(!outFName.compare("\0") && defFName.compare("\0")){
	// 	if(!command.compare("-g")){
	// 		outFName = fileRenamer(defFName, outFolderGDS, ".gds2");
	// 	}
	// 	else if(!command.compare("-j")){
	// 		outFName = fileRenamer(defFName, outFolderJoSIM, ".cir");
	// 	}
	// }
	if(!outFName.compare("\0") && verFName.compare("\0")){
		if(!command.compare("-a")){
			outFName = fileRenamer(verFName, outFolderABC, ".blif");
		}
	}
	else if(!outFName.compare("\0") && bliFName.compare("\0")){
		if(!command.compare("-b")){
			outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
		}
		else if(!command.compare("-s")){
			outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
		}
	}

	// if(!command.compare("-g")){
	// 	if(lefFName.compare("\0") && defFName.compare("\0") && outFName.compare("\0")){
	// 		runDie2Sim(lefFName, defFName, outFName);
	// 		return 1;
	// 	}
	// 	else{
	// 		cout << "Input argument error." << endl;
	// 		return 0;
	// 	}
	// }
	// else if(!command.compare("-j")){
	// 	// if(lefFName.compare("\0") && defFName.compare("\0") && outFName.compare("\0")){
	// 	if(defFName.compare("\0") && outFName.compare("\0")){
	// 		runJoSIM(lefFName, defFName, outFName);
	// 		return 1;
	// 	}
	// 	else {
	// 		cout << "Input argument error." << endl;
	// 		return 0;
	// 	}
	// }
	if(!command.compare("-a")){
		if(verFName.compare("\0") && outFName.compare("\0") && glbFName.compare("\0")){
			return runForgeABC(outFName, verFName, glbFName);
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-b")){
		if(bliFName.compare("\0") && outFName.compare("\0")){
			return runForgeSFQ(bliFName, outFName);;
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-s")){
		if(verFName.compare("\0") && outFName.compare("\0") && glbFName.compare("\0")){
			return runForgeSFQABC(outFName, verFName, glbFName);
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	// else if(!command.compare("-i")){
	// 	if(argCount == 1 + 2){
	// 		if(lefFName.compare("\0"))
	// 			decFName = lefFName;
	// 		else if(defFName.compare("\0"))
	// 			decFName = defFName;
	// 		else if(gdsFName.compare("\0"))
	// 			decFName = gdsFName;
	// 		else{
	// 			cout << "Input argument error." << endl;
	// 			return 0;
	// 		}
	// 	}
	// 	decipherFile(decFName);
	// 	return 1;		
	// }
	else if(!command.compare("-c")){
		if(argCount == 1 + 1){
			return RunToolFromConfig();
		}
		cout << "Input argument error." << endl;
		return 0;		
	}
	else if(!command.compare("-v")){
		if(argCount == 1 + 1){
			cout << setprecision(2);
			cout << "Version: " << versionNo << endl;
			return 1;
		}
		cout << "Input argument error." << endl;
		return 0;		
	}
	else if(!command.compare("-h")){
		helpScreen();
		return 1;
	}
	else{
		cout << "Quickly catch the smoke before it escapes." << endl;
		return 0;
	}

	cout << "I am smelling smoke." << endl;
	return 0;
}


/**
 * [RunToolFromConfig - Runs the tools using parameters from config.toml]
 * @return [1 - all good; 0 - error]
 */

int RunToolFromConfig(){
	cout << "Importing execution parameters from config.toml" << endl;

	const auto mainConfig  = toml::parse(configFile);
	map<string, string> run_para = toml::get<map<string, string>>(mainConfig.at("run_parameters"));

	map<string, string>::iterator it_run_para;

	string outFName = "\0";
	string lefFName = "\0";
	string defFName = "\0";
	string decFName = "\0";
	string command  = "\0";
	string bliFName = "\0";			// The non SFQ blif file
	string verFName = "\0";			// verilog
	string glbFName = "\0";			// .genlib

	it_run_para = run_para.find("Command");
	if(it_run_para != run_para.end()){
		command = it_run_para->second; 
	}
	else{
		cout << "Invalid parameters." << endl;
		return 0;
	}

	// it_run_para = run_para.find("lefFileName");
	// if(it_run_para != run_para.end()){
	// 	lefFName = it_run_para->second; 
	// }

	// it_run_para = run_para.find("defFileName");
	// if(it_run_para != run_para.end()){
	// 	defFName = it_run_para->second; 
	// }

	it_run_para = run_para.find("outFileName");
	if(it_run_para != run_para.end()){
		outFName = it_run_para->second; 
	}

	// it_run_para = run_para.find("deciFileName");
	// if(it_run_para != run_para.end()){
	// 	decFName = it_run_para->second; 
	// }

	it_run_para = run_para.find("blifFileName");
	if(it_run_para != run_para.end()){
		bliFName = it_run_para->second; 
	}

	it_run_para = run_para.find("veriFileName");
	if(it_run_para != run_para.end()){
		verFName = it_run_para->second; 
	}

	it_run_para = run_para.find("glibFileName");
	if(it_run_para != run_para.end()){
		glbFName = it_run_para->second; 
	}

	// auto assign output filename if non has been set
	// if(!outFName.compare("\0") && defFName.compare("\0")){
	// if(!outFName.compare("\0")){
	// 	if(!command.compare("gds")){
	// 		outFName = fileRenamer(defFName, outFolderGDS, ".gds2");
	// 	}
	// 	else if(!command.compare("josim")){
	// 		outFName = fileRenamer(defFName, outFolderJoSIM, ".cir");
	// 	}
	// 	else if(!command.compare("ABC")){
	// 		outFName = fileRenamer(verFName, outFolderABC, ".blif");
	// 	}
	// 	else if(!command.compare("SFQblif")){
	// 		outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
	// 	}
	// 	else if(!command.compare("SFQABC")){
	// 		outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
	// 	}
	// }

	// if(!outFName.compare("\0") && defFName.compare("\0")){
	// 	if(!command.compare("gds")){
	// 		outFName = fileRenamer(defFName, outFolderGDS, ".gds2");
	// 	}
	// 	else if(!command.compare("josim")){
	// 		outFName = fileRenamer(defFName, outFolderJoSIM, ".cir");
	// 	}
	// }
	if(!outFName.compare("\0") && verFName.compare("\0")){
		if(!command.compare("ABC")){
			outFName = fileRenamer(verFName, outFolderABC, ".blif");
		}
	}
	else if(!outFName.compare("\0") && bliFName.compare("\0")){
		if(!command.compare("SFQblif")){
			outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
		}
		else if(!command.compare("SFQABC")){
			outFName = fileRenamer(bliFName, outFolderBlif, ".blif");
		}
	}

	// if(!command.compare("gds")){
	// 	if(lefFName.compare("\0") && defFName.compare("\0") && outFName.compare("\0")){
	// 		runDie2Sim(lefFName, defFName, outFName);
	// 		return 1;
	// 	}
	// 	else{
	// 		cout << "Input argument error." << endl;
	// 		return 0;
	// 	}
	// }
	// else if(!command.compare("josim")){
	// 	if(defFName.compare("\0") && outFName.compare("\0")){
	// 	// if(lefFName.compare("\0") && defFName.compare("\0") && outFName.compare("\0")){
	// 		runJoSIM(lefFName, defFName, outFName);
	// 		return 1;
	// 	}
	// 	else {
	// 		cout << "Input argument error." << endl;
	// 		return 0;
	// 	}
	// }
	// else if(!command.compare("decipherer")){
	// 	if(decFName.compare("\0")){
	// 		decipherFile(decFName);
	// 		return 1;
	// 	}
	// 	decipherFile(decFName);
	// 	return 1;	
	// }
	if(!command.compare("ABC")){
		if(verFName.compare("\0") && outFName.compare("\0") && glbFName.compare("\0")){
			return runForgeABC(outFName, verFName, glbFName);
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("SFQblif")){
		if(bliFName.compare("\0") && outFName.compare("\0")){
			return runForgeSFQ(bliFName, outFName);;
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("SFQABC")){
		if(verFName.compare("\0") && outFName.compare("\0") && glbFName.compare("\0")){
			return runForgeSFQABC(outFName, verFName, glbFName);
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else{
		cout << "Invalid command." << endl;
		return 0;
	}

	return 0;
}


void helpScreen(){
	cout << "===============================================================================" << endl;
	cout << "Usage: Die2Sim [ OPTION ] [ filenames ]" << endl;
	cout << "-a(BC)        Creates a CMOS circuit from a verilog." << endl;
	cout << "                [.v file] [.genlib] -o [.blif]" << endl;
	cout << "-b(lif)       Creates a SFQ blif circuit from a standard blif file." << endl;
	cout << "                [.blif file] -o [.blif]" << endl;
	cout << "-s(FQABC)     Creates a SFQ circuit from a verilog." << endl;
	cout << "                [.v file] [.genlib] -o [.blif]" << endl;
	cout << "-c(onfig)     Runs the tools using the parameters in the config.toml file." << endl;
	cout << "-v(ersion)    Displays the version number." << endl;
	cout << "-h(elp)       Help screen." << endl;
	cout << "===============================================================================" << endl;
}

/**
 * Welcoming screen
 */

void welcomeScreen(){
	cout << "=====================================" << endl;
	cout << "              Die2Sim" << endl;
	cout << "       Author JF de Villiers" << endl;
	cout << "      Stellenbosch University" << endl;
	cout << "    For IARPA, ColdFlux project" << endl;
	cout << "=====================================" << endl;
}