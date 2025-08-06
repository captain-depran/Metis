# Metis
Dumb orbital mechanics simulator for spacecraft, that I hope to turn into a mission planner/manouver planner.
Began its life in python, this represents my port to C++

Included is a trajectory_viewer python program to allow you to visualise any data that the code spits out, and an example bodies.cfg input file containing orbital parameters for the earth and the moon.

Only need to download the .py file, the .exe file and the INPUT/OUTPUT folders.

##How do I use this?
1) Open SIM_CONFIG.cfg and change the parameters
2) Either create or edit a bodies file containing the celestial bodies you wish to involve. I ship with a file with the Sun, Earth and the Moon with parameters from JPL Horizons
3) Create or modify a spacecraft file
4) Go back to SIM_CONFIG.cfg and ensure the file names are correct, and your files are in the INPUT folder
5) Run Metis.exe
6) Wait, get a cup of tea, etc etc
7) Plot the results with the packaged python program!

##NOTES AND CAVEATS
- Currently, sim is hardcoded to recenter to a geocentric, ecliptic alligned frame after the run. The sun is also not plotted, but it is there.
- I would advise not adding or removing any bodies from the config file for now, due to underhood stop-gap measures
- Python requires Plotly for 3d plots, but there is a matplotlib version commented in the code
