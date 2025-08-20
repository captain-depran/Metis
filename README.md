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

## Where do I get started?
Read this: https://github.com/captain-depran/Metis/wiki/Mission-Sequences-(and-a-Quick-Tutorial)
(Its a walkthrough of a simple mission profile)

## NOTES AND CAVEATS
- Python requires Plotly for 3d plots, but there is a matplotlib version commented in the code
- Currently only single body reference frames work for plotting, as in, you select a body to be the center of the system
