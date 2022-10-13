# ViPer - Notes

## ToDo

### Current

* CMOS to SFQ
* Die2Sim Documentation
* Die2Sim Clean up

### Future

* Basic placement and routing
* Advanced placement and routing

## CMOS to SFQ

### Inserting DFFs

Originally the plans was only to look at each gate's difference in levels and to then insert DFFs to make up the difference.

A problem accrued when the inputs were on different levels... The first solution was to looks for the first common intersection, lots of work... The second and better option was to look at each inputs longest route which would be the last common interesting between all the routes. Then look at the differences and insert DFFs at at the inputs. The longest route for each input always ends up at the same output.

### Inserting Splits

Looks for every gate(net) that has 2 outputs and then insert a splitter.

### Levels and Routes

Levels help with the clocking and DFF placement as well. Levels use a recursive function to looks through all the possible pass in the circuit and by doing so levels are assigned to each gate. When the circuit is unbalanced, it is possible for a gate to have different levels.

### The Sequence of Converting

1. Insert DFFs
2. insert Splitter
3. Calculate clock levels

If splitters are inserted first, its a huge mess. The circuit has to be cleaned of excess DFFs which is a pain.

## Placement

### Opitmiser

Stacking all possible routes including splitters.



#### Order of priority if sorting:

   *   predefined input order
   * predefined output order
   * longest route

#### Sorting

* Creating subsets per priority
* use insertion for sorting per subset

### Layout

#### Centre

Looks for longest/widest row

#### Left align



#### Right Align



#### Justify

Look for widest row, calculate each row length. divide by the number of gaps, add the gap size.

## qRouter

### Only Need DEF:
* Overheads
* DieArea?
* Tracks
* COMPONENTS
* Nets

### LEF
* Overheads
* tracks
* layers
* vias
* macros



### config file

**Route Segment Cost** *value*


Cost of a route of the length of one track pitch on any metal  layer in the preferred direction of the route layer.  This  should be the lowest of all the costs.  Default value 2.

**Route Via Cost** *value*


Cost to switch routing layers up or down using a via.  Default  value 10.

**Route Jog Cost** *value*


Cost of a route of the length of one track pitch on any metal  layer against the preferred direction of the route layer.  This cost should be relatively high;  however, it should be  less than (Route Segment Cost + 2 * Route Via Cost) to allow  a jog of a single track pitch to be preferred over switching  to another layer.  Default value 20.

**Route Crossover Cost** *value*


Cost of routing directly over or under an unrouted pin  connection to a cell.  This helps prevent pins from getting  boxed in before they are routed, making them unroutable.  Default value 8.

**Route Block Cost** *value*


Cost of routing directly over or under an unrouted pin  connection to a cell, when that connection is the only  available vertical connection to the pin.  Default value 25.

**Route Collision Cost** *value*


Cost of shorting another net.  This happens during the second  routing stage.  All nets that are shorted by this net will  have to be ripped up and rerouted.  The value should be large  to avoid a large number of existing routes needing to be ripped  up.  Default value 50.



### USC notes

[ TRACKS statement ]:
********************
[TRACKS
[{X | Y} start DO numtracks STEP space
 [LAYER layerName ...]
;] ...]

Defines the routing grid for a standard cell-based design.
The track spacing is the PITCH value for the layer defined in LEF.

-{X | Y} start
Specifies the location and direction of the first track defined. X indicates vertical lines; Y indicates horizontal lines.
start is the X or Y coordinate of the first line.
For example, X 3000 creates a set of vertical lines, with the first line going through (3000 0).

DO numTracks
Specifies the number of tracks to create for the grid. You cannot specify 0 numtracks.

-STEP space
Specifies the spacing between the tracks.

-LAYER layerName
Specifies the routing layer used for the tracks. You can specify more than one layer.