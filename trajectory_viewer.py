import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

earth_pos=pd.read_csv("Source/1.txt",names=["x","y","z"])
moon_pos=pd.read_csv("Source/2.txt",names=["x","y","z"])
sat_pos=pd.read_csv("Source/3.txt",names=["x","y","z"])
plt.scatter(0,0)
plt.plot(moon_pos["x"]-earth_pos["x"],moon_pos["y"]-earth_pos["y"])
plt.plot(sat_pos["x"]-earth_pos["x"],sat_pos["y"]-earth_pos["y"])
plt.axis("equal")
plt.show()