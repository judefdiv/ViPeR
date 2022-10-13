/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-11-07
 * Modified:
 * license:
 * Description: Class for storing .lef
 * File:				ParserLef.hpp
 */

#ifndef ParserLef
#define ParserLef

#include <string>
#include <iostream>
#include <vector>
#include "toml/toml.hpp"
#include <ctime>
// #include <map>


#define unitScale 1

using namespace std;

/**
 * Class declaration/definitions
 */

class lef_macro;
class lef_layer;
class lef_via;
class lef_file;

/**
 * LEF file Class
 */

class lef_file{
	private:
		vector<lef_macro> macros;
		vector<lef_layer> layers;
		vector<lef_via> vias;

	public:
		lef_file(){};
		~lef_file(){};

		int importFile(const string &fileName);
		int importGDF(const string &fileName);
		int exportLef(const string &fileName);

		void to_str();
};


/**
 *  Macro Class
 */

struct macro_port{
	string layer = "\0";
	vector<double> ptsX;
	vector<double> ptsY;
};

struct macro_pin{
	string name = "\0";
	string direction = "\0";
	vector<macro_port> ports;

	// void clear(){
	// 	name = "\0";
	// 	direction = "\0";
	// 	ports.clear();
	// }
};

class lef_macro{
	private:

	public:
		lef_macro(){};
		~lef_macro(){};

		string name;

		double sizeX = 0;
		double sizeY = 0;
		double originX = 0;
		double originY = 0;

		vector<macro_pin> pins;
		vector<macro_port> obs;

		string get_varName(){return name;}
		double get_varSIZEX(){return sizeX;}
		double get_varSIZEY(){return sizeY;}
		double get_varORIGINX(){return originX;}
		double get_varORIGINY(){return originY;}
		void get_varPIN(vector<macro_pin> &VecOut){VecOut = pins;};
		int get_noPIN(){return pins.size();}
		void to_str();
};

/**
 * 	Layer Class
 */

class lef_layer{
	private:

  public:
    lef_layer(){};
    ~lef_layer(){};

    string name      = "\0";
    string type      = "\0";         // "ROUTING", "MASTERSLICE", "OVERLAP";
    string direction = "\0";         // {HORIZONTAL | VERTICAL} ;
    float pitch      = 0;          	 // {distance | xDistance yDistance} ;
    float width      = 0;            // defaultWidth ;
    float spacing    = 0;
    //float OFFSET[];         // {distance | xDistance yDistance} ;

    string get_varName() {return name;}
    string get_varType() {return type;}
    string get_varDirection() {return direction;}
    float get_varPitch() {return pitch;}
    float get_varWidth() {return width;}

    void to_str();
};

/**
 *  Via Class
 */

class lef_via{
	private:

	public:
		lef_via(){};
		~lef_via(){};

		string name;

		vector<string> layers;
		vector<double> ptsX;
		vector<double> ptsY;

		string get_varName(){return name;}
		// void get_varLayerNames(string arrayIn){arrayIn = *LayerNames;}
		// void get_varLayerDim(float *arrayIn){arrayIn = *layerDim;}

		void to_str();
};

/**
 *  Unit Class
 */

// class lef_units{
// 	private:
// 		string database;
// 		int dbCount;
// 		int classID;

// 		static int no_unit_classes;

// 	public:
// 		lef_units();
// 		void createAuto(vector<vector<string> > &inBlock);
// 		static int get_varNoUnits() {return no_unit_classes;}
// 		string get_varDB() {return database;}
// 		int get_varDBcount() {return dbCount;}
// 		void to_str();
// };

/**
 *  Spacing Class
 */

// class lef_spacing{
// 	private:
// 		string name;
// 		int classID;
// 		static int no_classes;

// 		int no_SAMENET = 0;
// 		vector<string> SAMENETlayer1;
// 		vector<string> SAMENETlayer2;
// 		vector<float> SAMENETdim;

// 	public:
// 		lef_spacing();
// 		void createAuto(vector<vector<string> > &inBlock);

// 		static int get_varNoCl() {return no_classes;}
// 		void to_str();
// };

/**
 *  Site Class
 */

// class lef_site{
// 	private:
// 		string name;
// 		int classID;
// 		static int no_classes;

// 		float SIZE[2];
// 		string CLASS;
// 		string SYMMETRY;

// 	public:
// 		lef_site(vector<vector<string> > &inBlock);

// 		static int get_varNoCl() {return no_classes;}
// 		void to_str();
// };

#endif