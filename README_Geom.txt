V5
Geometry Layout

2181 Strings in an almost octagonal grid 
The file "geo_orca_octagon_2181strings_3mhorizontal_51f_3.0mvertical" is a GDML input geometry file
There the geometry is described. The materials are hard coded in the GEANT4 source application and are
not read from the GDML input. 
The layout is:
World Volume 300x300x300
Lower 25 meters of world volume is the crust (rock)
The detector consists of 51 slices (parallel to the x-z plane)
Each slice contains 23 to 51 strings
Each string contains 51 Storeys
Each storeys contains one OM
each OM contains 31 PMTs
So the geometry depth is 5

The file "PmtPositionsAndDirections_geo_orca_octagon_2181strings_3mhorizontal_51f_3.0mvertical"
contains the pmt positions and directions:
First line of this file contains the number of pmts and the maximum QE used in the simulation
then there are enties for each pmt, e.g.:
5   - Geometry depth
1   - # of slice (1-51, 0 is the crust volume)
1   - # of string in this slice (0-22 to 0-50)
12  - # of storey in this string (0-50)
0   - # of OM in this storey (always 0)
15  - # of PMT in this OM (0-30)
-3.016500e+03 -7.490400e+03 -3.905900e+03 -8.269880e-01 4.779931e-01 -2.959957e-01   ---- x(cm), y(cm), z(cm), dx,dy,dz of photocathod center

 
