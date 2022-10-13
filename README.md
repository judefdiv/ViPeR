# ViPeR

ViPeR(Verilog to Placement and Routing) is a Stellenbosch University homegrown/in-house tool which synthesises RSFQ circuits from a HDL(hardware Description Language) file. ABC is used to synthesis the sequential logic circuit from a HDL(Verilog) file. Then ViPeR creates the RSFQ circuit by inserting DFF and splitter gates where required. The gates are then placed into an optimal layout to minimise track lengths and via count. The gates are then clocked using an h-tree structure. After the layout is complete, qRouter generates optimal routing.


Verilog -> YoSYS -> .BLIF -> [CMOS to RSFQ] -> [Layout/Placement] -> .DEF -> [qRouter] -> .DEF(routed)

Version: 0.9

## Features
* qRouter integration
* ABC integration
* Create LEF file from custom, easy to read file
* view circuit flow
*

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


## Required files

| **File**    | **Description**           | Required by                 |
| ----------- | ------------------------- | --------------------------- |
| .genlib     | logic gate description    | ABC(ViPeR)                  |
| .toml       | physical gate description | ViPeR                       |
| .cfg        | qRouter config            | qRouter                     |
| .lef        | standard gate description | qRouter, chipSmith          |
| config.toml | configuration for ViPeR   | ViPeR                       |



## Examples
Examples of how to execute ViPeR:

### Run Using Parameters from the config file
``` bash
./Die2Sim -c
```

## Needed stuffs

``` bash
apt install build-essencials cmake 	# for compiling
apt install libreadline-dev 	      # for Berkeley ABC
apt install graphviz				        # logic flow visuals
```

## To Compile

``` bash
# Current directory: ViPeR root
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