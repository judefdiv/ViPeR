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