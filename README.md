# Metis
Dumb orbital mechanics simulator for spacecraft, that I hope to turn into a mission planner/manouver planner.
Began its life in python, this represents my port to C++

Included is a trajectory_viewer python program to allow you to visualise any data that the code spits out, and an example bodies.cfg input file containing orbital parameters for the earth and the moon.

Only need to download the .py file, the .exe file and the INPUT/OUTPUT folders.

## How do I use this?
1) Open SIM_CONFIG.cfg and change the parameters
2) Either create or edit a bodies file containing the celestial bodies you wish to involve. I ship with a file with the Sun, Earth and the Moon with parameters from JPL Horizons
3) Create or modify a spacecraft file
4) Go back to SIM_CONFIG.cfg and ensure the file names are correct, and your files are in the INPUT folder
5) Run Metis.exe
6) Wait, get a cup of tea, etc etc
7) Plot the results with the packaged python program!

## How Do I define a Manouver?
In your spacecraft file, after the craft's intial conditions are specified, the software will look for specific event tags to parse. Currently the only one is 'MNV', which is short hand for 'Manouver'.

A command is specific as follows -> `MNV : LABEL : PROGRADE : RADIAL : NORMAL : TRIGGER_TYPE : TRIGGER_PARAM`

`LABEL` = A string labelling the manouver for output tracking

`PROGRADE` = Magnitude of manouver along the direction of travel

`RADIAL` = Magnitude of manouver away ('outwards') from central body

`NORMAL` = Magnitude of manouver 'upwards' from the orbital plane formed by the prograde and radial directions

`TRIGGER_TYPE` = What type of trigger the manouver has (See below)

`TRIGGER_PARAM` = The parameter for the passed trigger type (See below)

You can specify as many manouvers as you'd like, and in any order. Any untriggerable manouvers won't cause errors, they just won't happen.

Example:  `MNV:Earth Transfer:2500:150:0:TIME:15000`

### Triggers

- `TIME` -> Takes seconds as a parameter, will trigger at simulation time equal to this value (0 being start)

## NOTES AND CAVEATS
- Python requires Plotly for 3d plots, but there is a matplotlib version commented in the code
- Currently only single body reference frames work for plotting, as in, you select a body to be the center of the system
