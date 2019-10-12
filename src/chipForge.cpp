/**
 * Author:      Jude de Villiers
 * Origin:      E&E Engineering - Stellenbosch University
 * For:         Supertools, Coldflux Project - IARPA
 * Created:     2019-10-04
 * Modified:
 * license:
 * Description: Creates the final GDS file
 * File:        chipForge.cpp
 */

#include "viper/chipForge.hpp"

/**
 * [chipForge::forgeChip - Creates the final GDS file]
 * @param  fileName [The file name of the to be created GDS file]
 * @return          [1 - All good, 0 - Error]
 */

int chipForge::forgeChip(string fileName){
  cout << "Creating the GDS file \"" << fileName << "\"" << endl;
  gdscpp gdsFile;

  this->Stuctures.resize(this->Stuctures.size()+1);
  this->Stuctures.back().name = fileExtensionRenamer(fileName, "");
  this->Stuctures.back().SREF.push_back(drawSREF("layout", 0, 0));
  this->Stuctures.back().SREF.push_back(drawSREF("NETs", 0, 0));
  this->Stuctures.back().SREF.push_back(drawSREF("clk", 0, 0));

  gdsFile.setSTR(Stuctures);
  gdsFile.write(fileName);

  return 1;
}