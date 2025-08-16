# Metis
Dumb orbital mechanics simulator for spacecraft, that I hope to turn into an open-source mission planner/manouver planner.
Began its life in python, this represents my port to C++

Included is a trajectory_viewer python program to allow you to visualise any data that the code spits out, and an example bodies.cfg input file containing orbital parameters for the earth and the moon.

Only need to download the .py file, the .exe file and the INPUT/OUTPUT folders, or the latest release. If you are reading this on the dev branch, I won't always have a release bundled of the latest version, however just download the aforementioned files/folders and put them in a common directory. I will never intentionally push a completely non-unfunctional version to the dev branch (i.e. if I am backing up or saving my work, it will always be somewhat functional), however things happen so proceed at your own risk.

## How do I use this?
1) Open SIM_CONFIG.cfg and change the parameters
2) Either create or edit a bodies file containing the celestial bodies you wish to involve. I ship with a file with the Sun, Earth and the Moon with parameters from JPL Horizons
3) Create or modify a spacecraft file
4) Go back to SIM_CONFIG.cfg and ensure the file names are correct, and your files are in the INPUT folder
5) Run Metis.exe
6) Wait, get a cup of tea, etc etc
7) Plot the results with the packaged python program!

## How Do I define a Manouver?
In your spacecraft file, after the craft's intial conditions are specified, the software will look for specific event tags to parse. Currently the only one is 'MNV', which is short hand for 'Manouver'. For more information, check the regularly updated wiki.

A command is specific as follows -> `MNV : LABEL : PROGRADE : RADIAL : NORMAL : TRIGGER_TYPE : TRIGGER_PARAM`

`LABEL` = A string labelling the manouver for output tracking

`PROGRADE` = Magnitude of manouver along the direction of travel

`RADIAL` = Magnitude of manouver away ('outwards') from central body

`NORMAL` = Magnitude of manouver 'upwards' from the orbital plane formed by the prograde and radial directions

`TRIGGER_TYPE` = What type of trigger the manouver has (See below)

`TRIGGER_PARAM` = The parameter for the passed trigger type (See below)

You can specify as many manouvers as you'd like, in the sequence of execution. Any untriggerable manouvers won't cause errors, they just won't happen. Metis will provide a read-out at the end of a run informing you which manouvers/events were triggered and when.

Example:  `MNV:Earth Transfer:2500:150:0:TIME:15000`

You can also specify a simple zero impulse event-trigger set using `COAST`, which will simply progress the sim until the condition is triggered, before moving onto the next event or manouver. You do not need to specify any x-y-z impulse or zeroes
Example: `COAST:Waiting for Periapsis:ANOM:0` (Wait until the periapsis, AKA a true anomaly of 0 degrees)

### Triggers

- `TIME` -> Takes seconds as a parameter, will trigger this long after last event
- `CLA` -> Takes a detection threshold in M as a parameter, will trigger at the point of closest approach during the approach to a body
- `TURNBACK` -> Takes no parameter, will trigger at the point the craft starts falling back towards an object it is receeding from
- `ANOM` -> Takes degrees as a parameter, will trigger at a true anomaly equal to this value (or just past it). 0 is Periapsis, 180 is apoapsis
- `BDYSWP` -> Takes no parameter, slightly abstract detection of when the dominant gravitational body (the one which orbital elements are usually calculated of) changes. Is useful for sequencing and organisation, or timing/reporting. When transferring between bodies, it should be used before either an `ANOM` event or `CLA` event to ensure you are actually triggering relative to the desired body.

## NOTES AND CAVEATS
- Python requires Plotly for 3d plots, but there is a matplotlib version commented in the code
- Currently only single body reference frames work for plotting, as in, you select a body to be the center of the system
