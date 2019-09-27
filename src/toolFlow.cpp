/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-09-11
 * Modified:
 * license:
 * Description: All the ViPeR tools get executed here
 * File:        toolFlow.cpp
 */

#include "viper/toolFlow.hpp"

/**
 * [verilog2gds - Converts a verilog to a GDS file]
 * @param  gdsFile  [The .gds output file]
 * @param  veriFile [The .v input file]
 * @param  cellFile [The .genlib input file]
 * @return          [1 - All good; 0 - Error]
 */

int verilog2gds(string gdsFile, string veriFile, string cellFile, string configFName){
  string cmosBlif =  fileExtensionRenamer(veriFile, ".blif");
  ABCconvertV2blif(cmosBlif, veriFile, cellFile);

  blif2gds(gdsFile, cmosBlif, configFName);
  return 1;
}

/**
 * [blif2gds - Convert a blif file to a GDS file]
 * @param  gdsFile  [The .gds output file]
 * @param  blifFile [The .blif input file]
 * @return          [1 - All good; 0 - Error]
 */

int blif2gds(string gdsFile, string blifFile, string configFName){
  ForgeSFQBlif SFQcir;
  SFQcir.importBlif(blifFile);
  SFQcir.to_jpg(fileExtensionRenamer(blifFile, "_cmos.jpg"));
  SFQcir.toSFQ();
  SFQcir.to_str();
  SFQcir.to_blif(fileExtensionRenamer(blifFile, "_sfq.blif"));
  SFQcir.to_jpgAdv(fileExtensionRenamer(blifFile, "_sfq.jpg"));

  plek placedCir;
  placedCir.fetchSFQblif(SFQcir.get_nodes(), SFQcir.get_nets(), SFQcir.get_routes(), SFQcir.get_routesWS(), SFQcir.get_CLKlevels());
  placedCir.populate(configFName);
  placedCir.to_gds(gdsFile);

  return 1;
}

/**
 * [runABC - Uses ABC to generate a .blif file from .v]
 * @param  blifFile [The .blif output file]
 * @param  veriFile [The verilog input file that must be converted]
 * @param  cellFile [The cell library file ABC uses to generate the .blif file]
 * @return          [1 - All good; 0 - Error]
 */

int runABC(string blifFile, string veriFile, string cellFile){
  cout << "Converting .v to .blif using ABC" << endl;
  ABCconvertV2blif(blifFile, veriFile, cellFile);
  cout << "Done converting .v to .blif using ABC" << endl;

  return 1;
}

/**
 * [CMOSblif2SFQblif - Converts a generic CMOS .blif file to SFQ .blif file]
 * @param  blifInFile  [The input CMOS .blif file]
 * @param  blifOutFile [The output SFQ .blif file]
 * @return             [1 - All good; 0 - Error]
 */

// int CMOSblif2SFQblif(string blifInFile, string blifOutFile){
//   cout << "Converting CMOS .blif to SFQ .blif" << endl;

//   ForgeSFQBlif SFQcir;
//   SFQcir.importBlif(blifInFile);
//   SFQcir.to_jpg(fileRenamer(blifInFile, "", "_cmos.jpg"));
//   SFQcir.toSFQ();
//   // SFQcir.to_str();
//   SFQcir.to_blif(blifOutFile);
//   SFQcir.to_jpgAdv(fileRenamer(blifOutFile, "", "_sfq.jpg"));

//   return 1;
// }