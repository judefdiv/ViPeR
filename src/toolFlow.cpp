/**
 * Author:      Jude de Villiers, Edrich Verburg
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-09-11
 * Modified:
 * license:
 * Description: All the ViPeR tools get executed here
 * File:        toolFlow.cpp
 */

#include "viper/toolFlow.hpp"
#include "viper/ForgeSFQblif.hpp"
#include <cstdio>

/**
 * [blif2gds - Convert a blif file to a GDS file]
 * @param  gdsFile  [The .gds output file]
 * @param  blifFile [The .blif input file]
 * @return          [1 - All good; 0 - Error]
 */

int blif2gds(string gdsFile, string blifFile, string configFName){
  // Logic conversion
  ForgeSFQBlif SFQcir;
  SFQcir.importBlif(blifFile);
  SFQcir.importParameters(configFName);
  SFQcir.to_jpg(fileExtensionRenamer(blifFile, "_cmos.jpg"));
  // SFQcir.to_str();
  SFQcir.toSFQ();
  // SFQcir.to_str();
  SFQcir.to_blif(fileExtensionRenamer(blifFile, "_sfq.blif"));
  SFQcir.to_jpgAdv(fileExtensionRenamer(blifFile, "_sfq.jpg"));

  // GDS file creation
  chipForge SFQchip;

  // Cell placement
  plek placedCir;
  placedCir.fetchSFQblif(SFQcir.get_nodes(),
                          SFQcir.get_nets(),
                          SFQcir.get_routes(),
                          SFQcir.get_routesWS(),
                          SFQcir.get_CLKlevels());
  placedCir.populate(configFName);
  SFQchip.appendSTR(placedCir.defSTR());

  // clk placement
  clkChip clockIt;
  clockIt.fetchData(placedCir.get_nodes(),
                    placedCir.get_nets(),
                    placedCir.get_GateList(),
                    configFName,
                    SFQcir.getGateCnt(),
                    placedCir.get_layout());
  clockIt.execute();

  placedCir.setLayout(clockIt.getCellLayout(), clockIt.get_nodes());
  SFQchip.addSTR(placedCir.gsdLayout());

  SFQcir.set_stuffs(clockIt.get_nodes(), clockIt.get_nets());
  // SFQcir.to_str();

  // Routing
  roete route;
  route.fetchData(placedCir.get_nodes(),
                  clockIt.get_nets(),
                  clockIt.get_GateList(),
                  configFName);
  route.straightRoute();
  route.qrouter(fileExtensionRenamer(blifFile, ".def"));
  runqRouter(fileExtensionRenamer(blifFile, ".def"), "./data/qrouter.cfg", SFQcir);

  SFQchip.addSTR(route.route2gds());

  SFQchip.forgeChip(gdsFile);

  return 1;
}

/**
 * [gdf2lef - Converting a gdf.toml file to a standard .lef file]
 * @param  gdfFile [The .toml file containing definitions of the cells]
 * @param  lefFile [The .lef file that is going to be created]
 * @return         [1 - All good; 0 - Error]
 */

int gdf2lef(const string &gdfFile, const string &lefFile){
  lef_file foo;
  foo.importGDF(gdfFile);
  foo.exportLef(lefFile);

  return 1;
}

/**
 * [runqRouter - Runs qRouter]
 * @param  defFile    [The location of the .def file
 * @param  configFile [the location of the qRouter config file]
 * @return            [0 - All good; 1 - Error]
 */

int runqRouter(const string &defFile, const string &configFile, ForgeSFQBlif sfqblif){
  cout << "Determining priority routes" << endl;

  const auto nodes = sfqblif.get_nodes();
  set<string> priority_nets;

  for (const auto& net : sfqblif.get_nets()){
    for (const auto nodeIndex : net.inNodes){
      if (nodes[nodeIndex].name.find("SC_") != string::npos){
        priority_nets.insert(net.name);
        break;
      }
    }
    for (const auto nodeIndex : net.outNodes){
      if (nodes[nodeIndex].name.find("SC_") != string::npos){
        priority_nets.insert(net.name);
        break;    
      }
    }
  }

  // populate configfile with priority routes

  ifstream filein(configFile);

  string strTemp;
  vector<string> lines;
  while(getline(filein, strTemp)){
    lines.push_back(strTemp);
    if (strTemp == "#start priority nets"){
      for (const auto& net : priority_nets){
        lines.push_back("route priority " + net);
      }
    }
  }

  filein.close();
  ofstream fileout(configFile + ".temp");

  for (const auto& line : lines)
    fileout << line << "\n";
  
  fileout.close();
  
  cout << "Executing qRouter." << endl;



  stringstream ss;
  ss << "time qrouter -nog -noc -v 0 -c " << configFile  + ".temp" << " " << defFile;
  string bashCmd = ss.str();

  cout << "Bash command: " <<  bashCmd << endl;

  if(system(bashCmd.c_str()) == -1){
    cout << "Bash command :\"" << bashCmd << "\" error." << endl;
  }

  // ss.str("");
  // ss << "exit";
  // bashCmd = ss.str();

  // if(system(bashCmd.c_str()) == -1){
  //   cout << "Bash command :\"" << bashCmd << "\" error." << endl;
  // }

  return 0;
}