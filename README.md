# ViPeR
Verilog to Placement and Routing

Die2Sim is a tool used to aid simulations of large scale superconducting circuits. The main usage of the tool is to create GDSII and JoSIM files from LEF/DEF files using an existing cell library.

Version: 0.9

## Tool Flow

All the possible tools flows:

### verilog2gds

Requirements: .genlib, .v

Outputs: .jpg (cmos & SFQ), GDS

Verilog (.v) -> std blif (.blif) -> SFQ blif (.blif) -> GDS



### blif2gds

Requirements: .blif

Outputs: .jpg (cmos & SFQ), GDS

cmos blif (.blif) -> SFQ blif (.blif) -> GDS



### runABC

Requirements: .genlib, .v

Outputs: .blif

Verilog (.v) -> std blif (.blif)



### CMOSblif2SFQblif

Requirements: .blif

Outputs: .jpg (cmos & SFQ), .blif(SFQ)

std blif (.blif) -> SFQ blif (.blif)

## Features

* LEF/DEF interpretation
* GDS to ASCII
* Easily create GDS file
* Importing GDS files
* Draw GDS file
* Create JoSIM files
* Convert LEF/DEF to GDS
* Convert LEF/DEF to JoSIM



## Required files

| **File**    | **Description**           | Required by                 |
| ----------- | ------------------------- | --------------------------- |
| .genlib     | logic gate description    | ABC(ViPeR)                  |
| .toml       | physical gate description | ViPeR                       |
| .cfg        | qRouter config            | qRouter                     |
| .lef        | standard gate description | qRouter, Die2Sim, chipSmith |
| config.toml | configuration for ViPeR   | ViPeR                       |



## Examples
Examples of how to execute ViPeR:

### Interpret Files
``` bash
./Die2Sim -i gdsExample.gdsii
./Die2Sim -i lefExample.lef
./Die2Sim -i defExample.def
```
### LEF/DEF to GDSII
``` bash
./Die2Sim -g lefExample.lef defExample.def -o gdsOutput.gds
./Die2Sim -g lefExample.lef defExample.def # automatically assigns output filename
```
### LEF/DEF to JoSIM
``` bash
./Die2Sim -j lefExample.lef defExample.def -o josimOutput.cir
./Die2Sim -j lefExample.lef defExample.def # automatically assigns output filename
```
### Run Using Parameters from the config file
``` bash
./Die2Sim -c
```

## Needed stuffs
``` bash
apt install build-essencials 	# for compiling
apt install libreadline-dev 	# for Berkeley ABC
# apt install gv graphviz				# to visualise ABC's logic flow
apt install dot 							# To replace ABC's logic flow visuals
```
## To Compile
``` bash
# Current directory: Die2Sim root
mkdir build && cd build
cmake ..
make
```

## ABC Modifications
``` cpp
// in "abc/src/opt/dau/dau.h" added:
extern void		Abc_TtVerifySmallTruth(word * pTruth, int nVars);
extern int 		shiftFunc(int ci);
```

## Assumption Made
All cells are of the same height due to the algorithm are using row cell placement

## Notes
Author - JF de Villiers (Stellenbosch University)

For IARPA contract SuperTools

LEF: Library Exchange Format

DEF: Design Exchange Format

GDSII: Graphic Database System

JoSIM: Superconductor Circuit Simulator

BLIF: The Berkeley Logic Interchange Format