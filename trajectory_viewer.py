import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import plotly.graph_objects as go
import plotly.io as pio

pio.renderers.default='browser'

host_data=np.fromfile("OUTPUT/4.bin",dtype=np.float64)
moon_data=np.fromfile("OUTPUT/9.bin",dtype=np.float64)
sat_data=np.fromfile("OUTPUT/8.bin",dtype=np.float64)

host_pos = host_data.reshape(-1, 3)
moon_pos = moon_data.reshape(-1, 3)
sat_pos = sat_data.reshape(-1, 3)
#roid_pos=roid_data.reshape(-1,3)

host_pos = pd.DataFrame(host_pos, columns=['x', 'y', 'z'])
moon_pos = pd.DataFrame(moon_pos, columns=['x', 'y', 'z'])
sat_pos = pd.DataFrame(sat_pos, columns=['x', 'y', 'z'])
#roid_pos = pd.DataFrame(roid_pos, columns=['x', 'y', 'z'])

#earth_pos=pd.read_csv("Source/4.txt",names=["x","y","z"])
#moon_pos=pd.read_csv("Source/7.txt",names=["x","y","z"])
#sat_pos=pd.read_csv("Source/8.txt",names=["x","y","z"])

#PLOTLY VERSION

fig = go.Figure()   
fig.add_trace(go.Scatter3d(x=[0], y=[0],z=[0]))
fig.add_trace(go.Scatter3d(x=moon_pos["x"][::10], y=moon_pos["y"][::10],z=moon_pos["z"][::10], mode='lines'))
fig.add_trace(go.Scatter3d(x=sat_pos["x"][::10], y=sat_pos["y"][::10],z=sat_pos["z"][::10], mode='lines'))
#fig.add_trace(go.Scatter3d(x=roid_pos["x"][::1], y=roid_pos["y"][::1],z=roid_pos["z"][::1], mode='lines'))

fig.update_layout(scene_aspectmode='data')
fig.show()
print(sat_pos)
#print(np.min(np.sqrt(((roid_pos["x"]-sat_pos["x"])**2)+((roid_pos["y"]-sat_pos["y"])**2)+((roid_pos["z"]-sat_pos["z"])**2))))

"""
#MATPLOTLIB VERSION

plt.scatter(0,0)
plt.plot(moon_pos["x"],moon_pos["y"])
plt.plot(sat_pos["x"],sat_pos["y"],color="r")
plt.axis("equal")
plt.show()
"""