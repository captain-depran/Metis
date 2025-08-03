import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import plotly.graph_objects as go
import plotly.io as pio

pio.renderers.default='browser'

earth_data=np.fromfile("Source/4.bin",dtype=np.float64)
moon_data=np.fromfile("Source/5.bin",dtype=np.float64)
sat_data=np.fromfile("Source/6.bin",dtype=np.float64)

earth_pos = earth_data.reshape(-1, 3)
moon_pos = moon_data.reshape(-1, 3)
sat_pos = sat_data.reshape(-1, 3)

earth_pos = pd.DataFrame(earth_pos, columns=['x', 'y', 'z'])
moon_pos = pd.DataFrame(moon_pos, columns=['x', 'y', 'z'])
sat_pos = pd.DataFrame(sat_pos, columns=['x', 'y', 'z'])

#earth_pos=pd.read_csv("Source/4.txt",names=["x","y","z"])
#moon_pos=pd.read_csv("Source/7.txt",names=["x","y","z"])
#sat_pos=pd.read_csv("Source/8.txt",names=["x","y","z"])

#PLOTLY VERSION

fig = go.Figure()   
fig.add_trace(go.Scatter3d(x=[0], y=[0],z=[0]))
fig.add_trace(go.Scatter3d(x=moon_pos["x"][::5], y=moon_pos["y"][::5],z=moon_pos["z"][::5], mode='lines'))
fig.add_trace(go.Scatter3d(x=sat_pos["x"][::5], y=sat_pos["y"][::5],z=sat_pos["z"][::5], mode='lines'))

fig.update_layout(scene_aspectmode='data')
fig.show()

#print(np.min(np.sqrt(((moon_pos["x"]-sat_pos["x"])**2)+((moon_pos["y"]-sat_pos["y"])**2)+((moon_pos["z"]-sat_pos["z"])**2))))

"""
#MATPLOTLIB VERSION

plt.scatter(0,0)
plt.plot(moon_pos["x"],moon_pos["y"])
plt.plot(sat_pos["x"],sat_pos["y"],color="r")
plt.axis("equal")
plt.show()
"""