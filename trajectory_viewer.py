import matplotlib.pyplot as plt
import numpy as np
import pandas as pd

import plotly.graph_objects as go
import plotly.io as pio

pio.renderers.default='browser'

earth_pos=pd.read_csv("Source/4.txt",names=["x","y","z"])
moon_pos=pd.read_csv("Source/7.txt",names=["x","y","z"])
sat_pos=pd.read_csv("Source/8.txt",names=["x","y","z"])

#PLOTLY VERSION

fig = go.Figure()   
fig.add_trace(go.Scatter3d(x=[0], y=[0],z=[0]))
fig.add_trace(go.Scatter3d(x=moon_pos["x"], y=moon_pos["y"],z=moon_pos["z"], mode='lines'))
fig.add_trace(go.Scatter3d(x=sat_pos["x"], y=sat_pos["y"],z=sat_pos["z"], mode='lines'))

fig.update_layout(scene_aspectmode='data')
fig.show()

#MATPLOTLIB VERSION
"""

plt.scatter(0,0)
plt.plot(moon_pos["y"],moon_pos["z"])
plt.plot(sat_pos["y"],sat_pos["z"],color="r")
plt.axis("equal")
plt.show()
"""