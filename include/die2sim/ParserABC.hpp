/**
 * Author:  		Jude de Villiers
 * Origin:  		E&E Engineering - Stellenbosch University
 * For:					Supertools, Coldflux Project - IARPA
 * Created: 		2019-06-03
 * Modified:
 * license: 
 * Description: Parser for ABC
 * File:				ParserABC.cpp
 */

// #include <stdio.h>
// #include <time.h>
#include <string>
#include <iostream>
// #include "base/abc/abc.h"
// #include "include/abc/src/base/abc/abc.h"

using namespace std;

#define ABCdataLoc data/abc/

// -------------------------------------------------------------------------------
// ------- Modifications from Alan Mishchenko, UC Berkeley demo.c for ABC --------
// -------------------------------------------------------------------------------

#if defined(ABC_NAMESPACE)
namespace ABC_NAMESPACE {
	#elif defined(__cplusplus)
	extern "C" {
		#endif

		// procedures to start and stop the ABC framework
		// (should be called before and after the ABC procedures are called)
		void   Abc_Start();
		void   Abc_Stop();

		// procedures to get the ABC framework and execute commands in it
		typedef struct Abc_Frame_t_ Abc_Frame_t;

		Abc_Frame_t * Abc_FrameGetGlobalFrame();
		int    Cmd_CommandExecute( Abc_Frame_t * pAbc, const char * sCommand );

		// Own references

		// ioAbc.h
		// abcReadBlif.c
		// extern Abc_Ntk_t *        Io_ReadBlif( char * pFileName, int fCheck );
		// Abc_Ntk_t * Io_ReadBlif(char * pFileName, int fCheck);
		// bcWriteDot.c
		// extern void               Io_WriteDot( Abc_Ntk_t * pNtk, char * FileName );
		// void Io_WriteDot(Abc_Ntk_t * pNtk, char * FileName);

		#if defined(ABC_NAMESPACE)
	}
	using namespace ABC_NAMESPACE;
	#elif defined(__cplusplus)
}
#endif

// -------------------------------------------------------------------------------
// ----------------------------- Function Declaration ----------------------------
// -------------------------------------------------------------------------------

int runABC();
int runABCcmd(string inStr);
int ABCconvertV2blif(string blifFileName, string veriFileName, string cellFileName);
int showBlif(string blifFileName, string c_libFileName);