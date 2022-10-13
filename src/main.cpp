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
#include "viper/toolFlow.hpp"
#include "toml/toml.hpp"

#define versionNo 0.1
#define configFile "config.toml"

using namespace std;

/**
 * Declaring functions
 */

void welcomeScreen();
void helpScreen();
int RunTool(int argCount, char** argValues);
int RunToolFromConfig(string fileName);

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

	set<string> validCommands = {"-v", "-h", "-c"};

	string tomlFName = "\0";			// .toml
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

	// search for .toml
	for(int i = 0; i < argCount; i++){
		foo = string(argValues[i]);
	  if(foo.find(".toml")!=string::npos){
	  	tomlFName = foo;
	  }
	}

	if(!command.compare("-c")){
		if(tomlFName.compare("\0")){
			return RunToolFromConfig(tomlFName);
		}
		else {
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("-v")){
		if(argCount == 1 + 1){
			cout << setprecision(2);
			cout << "Version: " << versionNo << endl;
			return 1;
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
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
 * @param  fileName [The config toml file]
 * @return          [1 - all good; 0 - error]
 */

int RunToolFromConfig(string fileName){
	cout << "Importing execution parameters from config.toml" << endl;

	const auto mainConfig  = toml::parse(fileName);
	map<string, string> run_para = toml::get<map<string, string>>(mainConfig.at("run_parameters"));
	map<string, string> cir_Files = toml::get<map<string, string>>(mainConfig.at("Circuit_Files"));
	map<string, string> gen_Files = toml::get<map<string, string>>(mainConfig.at("General_Config_File"));

	map<string, string>::iterator it_run_para;

	string command  = "\0";
	string gdsFName = "\0";
	string bliFName = "\0";			// The standard blif file
	string verFName = "\0";			// verilog
	string glbFName = "\0";			// .genlib
	string clbFName = "\0";			// .toml
	string lefFName = "\0";			// .toml
	string workDir  = "";			  //

	it_run_para = run_para.find("Command");
	if(it_run_para != run_para.end()){
		command = it_run_para->second;
	}
	else{
		cout << "Invalid parameters." << endl;
		return 0;
	}

	it_run_para = cir_Files.find("work_dir");
	if(it_run_para != cir_Files.end()){
		workDir = it_run_para->second;
	}

	it_run_para = cir_Files.find("gds_file");
	if(it_run_para != cir_Files.end()){
		gdsFName = it_run_para->second;
		gdsFName = gdsFName.insert(0, workDir);
	}

	it_run_para = cir_Files.find("blif_file");
	if(it_run_para != cir_Files.end()){
		bliFName = it_run_para->second;
		bliFName = bliFName.insert(0, workDir);
	}

	it_run_para = cir_Files.find("veri_file");
	if(it_run_para != cir_Files.end()){
		verFName = it_run_para->second;
		verFName = verFName.insert(0, workDir);
	}

	it_run_para = gen_Files.find("cell_dis");
	if(it_run_para != gen_Files.end()){
		glbFName = it_run_para->second;
		// glbFName = glbFName.insert(0, workDir);
	}

	it_run_para = gen_Files.find("cell_dis_gds");
	if(it_run_para != gen_Files.end()){
		clbFName = it_run_para->second;
		// clbFName = clbFName.insert(0, workDir);
	}

	it_run_para = gen_Files.find("lef_file");
	if(it_run_para != gen_Files.end()){
		lefFName = it_run_para->second;
		// lefFName = lefFName.insert(0, workDir);
	}

	if(!command.compare("verilog2gds")){
		if(verFName.compare("\0") && gdsFName.compare("\0") && glbFName.compare("\0")){
			return verilog2gds(gdsFName, verFName, glbFName, fileName);
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("blif2gds")){
		if(bliFName.compare("\0") && gdsFName.compare("\0")){
			return blif2gds(gdsFName, bliFName, fileName);
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("gdf2lef")){
		if(lefFName.compare("\0") && clbFName.compare("\0")){
			return gdf2lef(clbFName, lefFName);
		}
		else{
			cout << "Input argument error." << endl;
			return 0;
		}
	}
	else if(!command.compare("runABC")){
		if(verFName.compare("\0") && bliFName.compare("\0") && glbFName.compare("\0")){
			return runABC(bliFName, verFName, glbFName);
		}
		else{
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
	cout << "=====================================================================" << endl;
	cout << "Usage: Die2Sim [ OPTION ] [ filenames ]" << endl;
	cout << "-c(onfig)     Runs the tools using the parameters in the toml file." << endl;
	cout << "                [.toml file]" << endl;
	cout << "-v(ersion)    Displays the version number." << endl;
	cout << "-h(elp)       Help screen." << endl;
	cout << "=====================================================================" << endl;
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