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

#include "viper/genFunc.hpp"
#include "viper/ForgeSFQABC.hpp"
#include "toml/toml.hpp"

#define versionNo 0.92
#define configFile "config.toml"
#define outFolder "data/results/"
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

	set<string> validCommands = {"-v", "-h", "-c", "-a", "-b", "-s"};

	string outFName = "\0";			// The output file, which is follow by the -o parameter
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

	// search for output filename
	for(int i = 0; i < argCount-1; i++){
	  if(!string(argValues[i]).compare("-o")){
	  	outFName = string(argValues[i+1]);
	  }
	}

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

	it_run_para = run_para.find("outFileName");
	if(it_run_para != run_para.end()){
		outFName = it_run_para->second;
	}

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
	cout << "  |\\" << endl;
	cout << "  | \\" << endl;
	cout << "  | |  " << endl;
	cout << "  \\  \\  __     __  _   ____         ____" << endl;
	cout << "   \\  \\ \\ \\   / / (_) |  _ \\  __   |  _ \\     ____ " << endl;
	cout << "   |  |  \\ \\ / /  | | | |_)| / _ \\ | |_) |   /   ^\\___ " << endl;
	cout << "   /  /   \\ V /   | | |  __/ | __/ |  _ <   /       `_\\" << endl;
	cout << "  /  /     \\_/    |_| |_|    \\___| |_| \\_\\ /   _______/" << endl;
	cout << " /  /     _____                          _/   / " << endl;
	cout << "|  |     /     \\       __              _/   _/" << endl;
	cout << "\\   \\___/       \\     /  \\            /   _/" << endl;
	cout << " \\         __    \\___/    \\__________/   /" << endl;
	cout << "  \\_______/  \\         __               /" << endl;
	cout << "              \\_______/  \\_____________/" << endl;
	cout << " " << endl;
	cout << "========================================================" << endl;
	cout << "           Verilog to Placement and Routing" << endl;
	cout << "                   for SFQ circuits." << endl;
	cout << "                 Author: JF de Villiers" << endl;
	cout << "                Stellenbosch University" << endl;
	cout << "              For IARPA, ColdFlux project" << endl;
	cout << "========================================================" << endl;
}